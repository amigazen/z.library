/*
 * Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Amiga z.library integration by amigazen project
 *
 * malloc.c - pooled malloc/free for zlib inside z.library
 *
 * zlib zutil.c calls malloc/free; this module routes those calls through an
 * Exec memory pool so allocations are reclaimed when the library expunges.
 */

#define __USE_SYSBASE

#include <exec/types.h>
#include <exec/semaphores.h>
#include <proto/exec.h>

#include <stddef.h>
#include <string.h>

struct SignalSemaphore ZLibPoolSemaphore;
static APTR ZLibPool;

int
z_malloc_init(void)
{
    InitSemaphore(&ZLibPoolSemaphore);
    ZLibPool = CreatePool(MEMF_ANY, 4096, 1024);
    if (ZLibPool == NULL) {
        return 0;
    }
    return 1;
}

void
z_malloc_exit(void)
{
    if (ZLibPool != NULL) {
        DeletePool(ZLibPool);
        ZLibPool = NULL;
    }
}

#define Z_BASE_PTR(ptr) ((void *)((size_t *)ptr - 1))
#define Z_ALLOC_SIZE(ptr) (*((size_t *)ptr - 1))

void *
malloc(size_t size)
{
    size_t *ptr;

    if (ZLibPool == NULL) {
        return NULL;
    }
    if (size > ((size_t)-1) - sizeof(size_t)) {
        return NULL;
    }
    ObtainSemaphore(&ZLibPoolSemaphore);
    ptr = (size_t *)AllocPooled(ZLibPool, sizeof(size_t) + size);
    ReleaseSemaphore(&ZLibPoolSemaphore);
    if (ptr != NULL) {
        *ptr = size;
        ptr++;
    }
    return (void *)ptr;
}

void *
calloc(size_t num, size_t es)
{
    size_t size;
    void *ptr;

    size = num * es;
    if (num != 0 && es != 0 && (size / es) != num) {
        return NULL;
    }
    ptr = malloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void
free(void *ptr)
{
    size_t size;

    if (ptr == NULL || ZLibPool == NULL) {
        return;
    }
    size = Z_ALLOC_SIZE(ptr);
    ObtainSemaphore(&ZLibPoolSemaphore);
    FreePooled(ZLibPool, Z_BASE_PTR(ptr), sizeof(size_t) + size);
    ReleaseSemaphore(&ZLibPoolSemaphore);
}
