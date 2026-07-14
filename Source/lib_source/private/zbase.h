/*
 * Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Amiga z.library integration by amigazen project
 *
 * zbase.h - z.library private base structure (lib_source/private only)
 */

#ifndef Z_ZBASE_H
#define Z_ZBASE_H

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif
#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

struct ZBase {
    struct Library zlb_LibNode;
    UWORD          zlb_Pad;
    BPTR           zlb_SegList;
};

#define ZLIBNAME "z.library"

#endif /* Z_ZBASE_H */
