/*
 * Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Amiga z.library integration by amigazen project
 *
 * compiler.h - cross-compiler attributes for z.library
 */

#ifndef Z_COMPILER_H
#define Z_COMPILER_H

#include <exec/types.h>
#include <clib/compiler-specific.h>
#include <proto/exec.h>

#ifndef Z_INITTABLE_DEFINED
#define Z_INITTABLE_DEFINED 1
struct InitTable
{
    ULONG it_LibSize;
    APTR *it_FuncTable;
    APTR  it_DataTable;
    APTR  it_InitFunc;
};
#endif

struct MyDataInit
{
    ULONG md_Init[19];
};

#endif /* Z_COMPILER_H */
