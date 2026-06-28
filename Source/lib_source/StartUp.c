/*
 * Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Amiga z.library integration by amigazen project
 *
 * StartUp.c - LVO trap, function vector table, and library lifecycle
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
#include "z_funcs.h"

extern ULONG L_OpenLibs(struct ZBase *ZLibBase);
extern void L_CloseLibs(void);
extern int z_malloc_init(void);
extern void z_malloc_exit(void);

extern struct Resident ROMTag;
extern const char Z_LibName[];
extern const char Z_LibID[];
extern struct MyDataInit DataTab;

struct ZBase *ZBase = NULL;

struct ExecBase *SysBase = NULL;

LONG __ASM__ LibStart(void);
struct ZBase * __ASM__ __SAVE_DS__ InitLib(
    __REG__(a6, struct ExecBase *sysbase),
    __REG__(a0, BPTR seglist),
    __REG__(d0, struct ZBase *base));
struct ZBase * __ASM__ __SAVE_DS__ OpenLib(
    __REG__(a6, struct ZBase *base));
BPTR __ASM__ __SAVE_DS__ CloseLib(
    __REG__(a6, struct ZBase *base));
BPTR __ASM__ __SAVE_DS__ ExpungeLib(
    __REG__(a6, struct ZBase *base));
ULONG __ASM__ ExtFuncLib(void);

APTR FuncTab[];

/*
 * FuncTab[] order MUST match SDK/SFD/z_lib.sfd and SDK/Include_H/pragmas/z_pragmas.h
 * (bias 0x1e, +6 per LVO).  Index 0-3 are Open/Close/Expunge/Reserved; index 4 is
 * ZlibVersion (libcall 0x1e).
 */

struct InitTable InitTab = {
    (ULONG)sizeof(struct ZBase),
    (APTR *)FuncTab,
    (APTR)&DataTab,
    (APTR)InitLib
};

APTR FuncTab[] = {
    (APTR)OpenLib,
    (APTR)CloseLib,
    (APTR)ExpungeLib,
    (APTR)ExtFuncLib,
    (APTR)ZlibVersion,
    (APTR)DeflateInit,
    (APTR)Deflate,
    (APTR)DeflateEnd,
    (APTR)InflateInit,
    (APTR)Inflate,
    (APTR)InflateEnd,
    (APTR)DeflateInit2,
    (APTR)DeflateSetDictionary,
    (APTR)DeflateCopy,
    (APTR)DeflateReset,
    (APTR)DeflateParams,
    (APTR)InflateInit2,
    (APTR)InflateSetDictionary,
    (APTR)InflateReset,
    (APTR)Compress,
    (APTR)Uncompress,
    (APTR)Adler32,
    (APTR)CRC32,
    (APTR)InflateSync,
    (APTR)DeflateTune,
    (APTR)DeflateBound,
    (APTR)DeflatePrime,
    (APTR)DeflateSetHeader,
    (APTR)InflateCopy,
    (APTR)InflatePrime,
    (APTR)InflateGetHeader,
    (APTR)InflateBackInit,
    (APTR)InflateBack,
    (APTR)InflateBackEnd,
    (APTR)Adler32Combine,
    (APTR)CRC32Combine,
    (APTR)Compress2,
    (APTR)CompressBound,
    (APTR)InflateGetDictionary,
    (APTR)ZError,
    (APTR)Uncompress2,
    (APTR)InflateReset2,
    (APTR)InflateValidate,
    (APTR)CRC32CombineGen,
    (APTR)CRC32CombineOp,
    (APTR)((LONG)-1)
};

LONG
__ASM__ LibStart(void)
{
    return -1;
}

struct ZBase *
__ASM__ __SAVE_DS__ InitLib(
    __REG__(a6, struct ExecBase *sysbase),
    __REG__(a0, BPTR seglist),
    __REG__(d0, struct ZBase *base))
{
    ZBase = base;

    base->zlb_LibNode.lib_Node.ln_Type = NT_LIBRARY;
    base->zlb_LibNode.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
    base->zlb_LibNode.lib_Version = Z_LIB_VERSION;
    base->zlb_LibNode.lib_Revision = Z_LIB_REVISION;
    base->zlb_LibNode.lib_IdString = (STRPTR)Z_LibID;

    base->zlb_SegList = seglist;
    base->zlb_Pad = 0;

    if (L_OpenLibs(base) != 0) {
        return (struct ZBase *)NULL;
    }

    return base;
}

struct ZBase *
__ASM__ __SAVE_DS__ OpenLib(__REG__(a6, struct ZBase *base))
{
    base->zlb_LibNode.lib_OpenCnt++;
    base->zlb_LibNode.lib_Flags &= ~LIBF_DELEXP;
    return base;
}

BPTR
__ASM__ __SAVE_DS__ CloseLib(__REG__(a6, struct ZBase *base))
{
    base->zlb_LibNode.lib_OpenCnt--;

    if (base->zlb_LibNode.lib_OpenCnt == 0) {
        if (base->zlb_LibNode.lib_Flags & LIBF_DELEXP) {
            return ExpungeLib(base);
        }
    }

    return 0;
}

BPTR
__ASM__ __SAVE_DS__ ExpungeLib(__REG__(a6, struct ZBase *base))
{
    BPTR seg;

    if (base->zlb_LibNode.lib_OpenCnt != 0) {
        base->zlb_LibNode.lib_Flags |= LIBF_DELEXP;
        return 0;
    }

    seg = base->zlb_SegList;

    L_CloseLibs();

    Remove(&base->zlb_LibNode.lib_Node);
    FreeMem((APTR)((BYTE *)base - base->zlb_LibNode.lib_NegSize),
        base->zlb_LibNode.lib_NegSize + base->zlb_LibNode.lib_PosSize);

    ZBase = NULL;

    return seg;
}

ULONG
__ASM__ ExtFuncLib(void)
{
    return 0;
}
