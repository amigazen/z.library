/*
 * Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Amiga z.library integration by amigazen project
 *
 * z_funcs.h - LVO forward declarations for z.library
 */

#ifndef Z_FUNCS_H
#define Z_FUNCS_H

#include <exec/types.h>
#include <libraries/z.h>

CONST_STRPTR __ASM__ __SAVE_DS__ ZlibVersion(void);
LONG __ASM__ __SAVE_DS__ DeflateInit(__REG__(a0, z_streamp strm), __REG__(d0, LONG level));
LONG __ASM__ __SAVE_DS__ Deflate(__REG__(a0, z_streamp strm), __REG__(d0, LONG flush));
LONG __ASM__ __SAVE_DS__ DeflateEnd(__REG__(a0, z_streamp strm));
LONG __ASM__ __SAVE_DS__ InflateInit(__REG__(a0, z_streamp strm));
LONG __ASM__ __SAVE_DS__ Inflate(__REG__(a0, z_streamp strm), __REG__(d0, LONG flush));
LONG __ASM__ __SAVE_DS__ InflateEnd(__REG__(a0, z_streamp strm));
LONG __ASM__ __SAVE_DS__ DeflateInit2(__REG__(a0, z_streamp strm), __REG__(d0, LONG level),
    __REG__(d1, LONG method), __REG__(d2, LONG windowBits), __REG__(d3, LONG memLevel),
    __REG__(d4, LONG strategy));
LONG __ASM__ __SAVE_DS__ DeflateSetDictionary(__REG__(a0, z_streamp strm),
    __REG__(a1, CONST_APTR dictionary), __REG__(d0, ULONG dictLength));
LONG __ASM__ __SAVE_DS__ DeflateCopy(__REG__(a0, z_streamp dest), __REG__(a1, z_streamp source));
LONG __ASM__ __SAVE_DS__ DeflateReset(__REG__(a0, z_streamp strm));
LONG __ASM__ __SAVE_DS__ DeflateParams(__REG__(a0, z_streamp strm), __REG__(d0, LONG level),
    __REG__(d1, LONG strategy));
LONG __ASM__ __SAVE_DS__ InflateInit2(__REG__(a0, z_streamp strm), __REG__(d0, LONG windowBits));
LONG __ASM__ __SAVE_DS__ InflateSetDictionary(__REG__(a0, z_streamp strm),
    __REG__(a1, CONST_APTR dictionary), __REG__(d0, ULONG dictLength));
LONG __ASM__ __SAVE_DS__ InflateReset(__REG__(a0, z_streamp strm));
LONG __ASM__ __SAVE_DS__ Compress(__REG__(a0, APTR dest), __REG__(a1, ULONG *destLen),
    __REG__(a2, CONST_APTR source), __REG__(d0, ULONG sourceLen));
LONG __ASM__ __SAVE_DS__ Uncompress(__REG__(a0, APTR dest), __REG__(a1, ULONG *destLen),
    __REG__(a2, CONST_APTR source), __REG__(d0, ULONG sourceLen));
ULONG __ASM__ __SAVE_DS__ Adler32(__REG__(d0, ULONG adler), __REG__(a0, CONST_APTR buf),
    __REG__(d1, ULONG len));
ULONG __ASM__ __SAVE_DS__ CRC32(__REG__(d0, ULONG crc), __REG__(a0, CONST_APTR buf),
    __REG__(d1, ULONG len));
LONG __ASM__ __SAVE_DS__ InflateSync(__REG__(a0, z_streamp strm));
LONG __ASM__ __SAVE_DS__ DeflateTune(__REG__(a0, z_streamp strm), __REG__(d0, LONG good_length),
    __REG__(d1, LONG max_lazy), __REG__(d2, LONG nice_length), __REG__(d3, LONG max_chain));
ULONG __ASM__ __SAVE_DS__ DeflateBound(__REG__(a0, z_streamp strm), __REG__(d0, ULONG sourceLen));
LONG __ASM__ __SAVE_DS__ DeflatePrime(__REG__(a0, z_streamp strm), __REG__(d0, LONG bits),
    __REG__(d1, LONG value));
LONG __ASM__ __SAVE_DS__ DeflateSetHeader(__REG__(a0, z_streamp strm), __REG__(a1, gz_headerp head));
LONG __ASM__ __SAVE_DS__ InflateCopy(__REG__(a0, z_streamp dest), __REG__(a1, z_streamp source));
LONG __ASM__ __SAVE_DS__ InflatePrime(__REG__(a0, z_streamp strm), __REG__(d0, LONG bits),
    __REG__(d1, LONG value));
LONG __ASM__ __SAVE_DS__ InflateGetHeader(__REG__(a0, z_streamp strm), __REG__(a1, gz_headerp head));
LONG __ASM__ __SAVE_DS__ InflateBackInit(__REG__(a0, z_streamp strm), __REG__(d0, LONG windowBits),
    __REG__(a1, UBYTE *window));
LONG __ASM__ __SAVE_DS__ InflateBack(__REG__(a0, z_streamp strm), __REG__(a1, in_func in),
    __REG__(d0, APTR in_desc), __REG__(a2, out_func out), __REG__(d1, APTR out_desc));
LONG __ASM__ __SAVE_DS__ InflateBackEnd(__REG__(a0, z_streamp strm));
ULONG __ASM__ __SAVE_DS__ Adler32Combine(__REG__(d0, ULONG adler1), __REG__(d1, ULONG adler2),
    __REG__(d2, LONG len2));
ULONG __ASM__ __SAVE_DS__ CRC32Combine(__REG__(d0, ULONG crc1), __REG__(d1, ULONG crc2),
    __REG__(d2, LONG len2));
LONG __ASM__ __SAVE_DS__ Compress2(__REG__(a0, APTR dest), __REG__(a1, ULONG *destLen),
    __REG__(a2, CONST_APTR source), __REG__(d0, ULONG sourceLen), __REG__(d1, LONG level));
ULONG __ASM__ __SAVE_DS__ CompressBound(__REG__(d0, ULONG sourceLen));
LONG __ASM__ __SAVE_DS__ InflateGetDictionary(__REG__(a0, z_streamp strm),
    __REG__(a1, UBYTE *dictionary), __REG__(a2, ULONG *dictLength));
CONST_STRPTR __ASM__ __SAVE_DS__ ZError(__REG__(d0, LONG err));
LONG __ASM__ __SAVE_DS__ Uncompress2(__REG__(a0, APTR dest), __REG__(a1, ULONG *destLen),
    __REG__(a2, CONST_APTR source), __REG__(a3, ULONG *sourceLen));
LONG __ASM__ __SAVE_DS__ InflateReset2(__REG__(a0, z_streamp strm), __REG__(d0, LONG windowBits));
LONG __ASM__ __SAVE_DS__ InflateValidate(__REG__(a0, z_streamp strm), __REG__(d0, LONG check));
ULONG __ASM__ __SAVE_DS__ CRC32CombineGen(__REG__(d0, LONG len2));
ULONG __ASM__ __SAVE_DS__ CRC32CombineOp(__REG__(d0, ULONG crc1), __REG__(d1, ULONG crc2),
    __REG__(d2, ULONG op));

#endif /* Z_FUNCS_H */
