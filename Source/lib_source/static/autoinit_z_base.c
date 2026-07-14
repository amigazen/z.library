/*
 * Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Amiga z.library integration by amigazen project
 *
 * autoinit_z_base.c - ZBase, SAS/C _INIT/_EXIT, lazy OpenLibrary for stubs
 */

#define __USE_SYSBASE

#include <exec/types.h>
#include <proto/exec.h>

#ifndef __NOLIBBASE__
struct Library *ZBase = NULL;
#endif

static struct Library *ZBaseAuto;

int z_stub_ensure_open(void)
{
    if (ZBase != NULL) {
        return 1;
    }
    ZBaseAuto = OpenLibrary((STRPTR)"z.library", 2);
    ZBase = ZBaseAuto;
    return (ZBase != NULL);
}

void _INIT_5_ZBase(void)
{
    (void)z_stub_ensure_open();
}

void _EXIT_5_ZBase(void)
{
    if (ZBaseAuto != NULL) {
        CloseLibrary(ZBaseAuto);
        ZBaseAuto = NULL;
        ZBase = NULL;
    }
}
