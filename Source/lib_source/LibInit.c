/*
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright 2026 amigazen project
 *
 * LibInit.c - ROMTag, DataTab, and pooled allocator init for z.library
 */

#define __USE_SYSBASE

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>

#include <proto/exec.h>

#include <z/zbase.h>
#include "private/z_build.h"
#include "compiler.h"
#include "zlib.h"

#define ZLIBNAME "z"
#define ZLIBVER  " 2.2 (23.6.2026)"

const char Z_LibName[] = ZLIBNAME ".library";
const char Z_LibID[]   = ZLIBNAME ZLIBVER;
const char Z_VerString[] = "\0$VER: " ZLIBNAME ZLIBVER;

extern struct ExecBase *SysBase;
extern struct ZBase *ZBase;

extern int z_malloc_init(void);
extern void z_malloc_exit(void);

ULONG __SAVE_DS__
L_OpenLibs(struct ZBase *base)
{
    (void)base;
    SysBase = *((struct ExecBase **)4);

    if (z_malloc_init() == 0) {
        return 1;
    }

    (void)get_crc_table();

    return 0;
}

void __SAVE_DS__
L_CloseLibs(void)
{
    z_malloc_exit();
}

extern struct InitTable InitTab;
extern APTR EndResident;

struct Resident ROMTag = {
    RTC_MATCHWORD,
    &ROMTag,
    &EndResident,
    RTF_AUTOINIT,
    Z_LIB_VERSION,
    NT_LIBRARY,
    0,
    (APTR)Z_LibName,
    (APTR)Z_LibID,
    (APTR)&InitTab
};

APTR EndResident;

struct MyDataInit DataTab = {
    0xE000, 8,  NT_LIBRARY,
    0x80,   10, (ULONG)Z_LibName,
    0xE000, 14, LIBF_SUMUSED | LIBF_CHANGED,
    0xE000, 20, Z_LIB_VERSION,
    0xE000, 22, Z_LIB_REVISION,
    0x80,   24, (ULONG)Z_LibID,
    (ULONG)0
};

#ifdef __SASC
void __regargs __chkabort(void) { }
void __regargs _CXBRK(void)     { }
#endif
