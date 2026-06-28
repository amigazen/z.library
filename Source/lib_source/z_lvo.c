/*
 * Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Amiga z.library integration by amigazen project
 *
 * z_lvo.c - z.library LVO wrappers around internal zlib
 *
 * Public ABI uses PascalCase LVO names from z_lib.sfd; internal zlib keeps
 * standard lowercase C symbols compiled from Source/lib_source/zlib/.
 */

#define __USE_SYSBASE

#include <exec/types.h>

#include "compiler.h"

/*
 * LVO implementations call internal zlib (deflate/inflate/...).  Public
 * <libraries/z.h> and zlib/zlib.h both typedef z_stream, in_func, out_func,
 * etc. — include only one per translation unit.  StartUp.c uses z_funcs.h
 * (public ABI); this file uses internal zlib.h only (SDK z.library pattern).
 */
#define LIBRARIES_Z_H
#include "zlib.h"

CONST_STRPTR
__ASM__ __SAVE_DS__ ZlibVersion(void)
{
    return (CONST_STRPTR)zlibVersion();
}

LONG
__ASM__ __SAVE_DS__ DeflateInit(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG level))
{
    return (LONG)deflateInit((z_streamp)strm, (int)level);
}

LONG
__ASM__ __SAVE_DS__ Deflate(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG flush))
{
    return (LONG)deflate((z_streamp)strm, (int)flush);
}

LONG
__ASM__ __SAVE_DS__ DeflateEnd(
    __REG__(a0, z_streamp strm))
{
    return (LONG)deflateEnd((z_streamp)strm);
}

LONG
__ASM__ __SAVE_DS__ InflateInit(
    __REG__(a0, z_streamp strm))
{
    return (LONG)inflateInit((z_streamp)strm);
}

LONG
__ASM__ __SAVE_DS__ Inflate(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG flush))
{
    return (LONG)inflate((z_streamp)strm, (int)flush);
}

LONG
__ASM__ __SAVE_DS__ InflateEnd(
    __REG__(a0, z_streamp strm))
{
    return (LONG)inflateEnd((z_streamp)strm);
}

LONG
__ASM__ __SAVE_DS__ DeflateInit2(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG level),
    __REG__(d1, LONG method),
    __REG__(d2, LONG windowBits),
    __REG__(d3, LONG memLevel),
    __REG__(d4, LONG strategy))
{
    return (LONG)deflateInit2((z_streamp)strm, (int)level, (int)method,
        (int)windowBits, (int)memLevel, (int)strategy);
}

LONG
__ASM__ __SAVE_DS__ DeflateSetDictionary(
    __REG__(a0, z_streamp strm),
    __REG__(a1, CONST_APTR dictionary),
    __REG__(d0, ULONG dictLength))
{
    return (LONG)deflateSetDictionary((z_streamp)strm,
        (const Bytef *)dictionary, (uInt)dictLength);
}

LONG
__ASM__ __SAVE_DS__ DeflateCopy(
    __REG__(a0, z_streamp dest),
    __REG__(a1, z_streamp source))
{
    return (LONG)deflateCopy((z_streamp)dest, (z_streamp)source);
}

LONG
__ASM__ __SAVE_DS__ DeflateReset(
    __REG__(a0, z_streamp strm))
{
    return (LONG)deflateReset((z_streamp)strm);
}

LONG
__ASM__ __SAVE_DS__ DeflateParams(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG level),
    __REG__(d1, LONG strategy))
{
    return (LONG)deflateParams((z_streamp)strm, (int)level, (int)strategy);
}

LONG
__ASM__ __SAVE_DS__ InflateInit2(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG windowBits))
{
    return (LONG)inflateInit2((z_streamp)strm, (int)windowBits);
}

LONG
__ASM__ __SAVE_DS__ InflateSetDictionary(
    __REG__(a0, z_streamp strm),
    __REG__(a1, CONST_APTR dictionary),
    __REG__(d0, ULONG dictLength))
{
    return (LONG)inflateSetDictionary((z_streamp)strm,
        (const Bytef *)dictionary, (uInt)dictLength);
}

LONG
__ASM__ __SAVE_DS__ InflateReset(
    __REG__(a0, z_streamp strm))
{
    return (LONG)inflateReset((z_streamp)strm);
}

LONG
__ASM__ __SAVE_DS__ Compress(
    __REG__(a0, APTR dest),
    __REG__(a1, ULONG *destLen),
    __REG__(a2, CONST_APTR source),
    __REG__(d0, ULONG sourceLen))
{
    return (LONG)compress((Bytef *)dest, (uLongf *)destLen,
        (const Bytef *)source, (uLong)sourceLen);
}

LONG
__ASM__ __SAVE_DS__ Uncompress(
    __REG__(a0, APTR dest),
    __REG__(a1, ULONG *destLen),
    __REG__(a2, CONST_APTR source),
    __REG__(d0, ULONG sourceLen))
{
    return (LONG)uncompress((Bytef *)dest, (uLongf *)destLen,
        (const Bytef *)source, (uLong)sourceLen);
}

ULONG
__ASM__ __SAVE_DS__ Adler32(
    __REG__(d0, ULONG adler),
    __REG__(a0, CONST_APTR buf),
    __REG__(d1, ULONG len))
{
    return (ULONG)adler32((uLong)adler, (const Bytef *)buf, (uInt)len);
}

ULONG
__ASM__ __SAVE_DS__ CRC32(
    __REG__(d0, ULONG crc),
    __REG__(a0, CONST_APTR buf),
    __REG__(d1, ULONG len))
{
    return (ULONG)crc32((uLong)crc, (const Bytef *)buf, (uInt)len);
}

LONG
__ASM__ __SAVE_DS__ InflateSync(
    __REG__(a0, z_streamp strm))
{
    return (LONG)inflateSync((z_streamp)strm);
}

LONG
__ASM__ __SAVE_DS__ DeflateTune(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG good_length),
    __REG__(d1, LONG max_lazy),
    __REG__(d2, LONG nice_length),
    __REG__(d3, LONG max_chain))
{
    return (LONG)deflateTune((z_streamp)strm, (int)good_length, (int)max_lazy,
        (int)nice_length, (int)max_chain);
}

ULONG
__ASM__ __SAVE_DS__ DeflateBound(
    __REG__(a0, z_streamp strm),
    __REG__(d0, ULONG sourceLen))
{
    return (ULONG)deflateBound((z_streamp)strm, (uLong)sourceLen);
}

LONG
__ASM__ __SAVE_DS__ DeflatePrime(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG bits),
    __REG__(d1, LONG value))
{
    return (LONG)deflatePrime((z_streamp)strm, (int)bits, (int)value);
}

LONG
__ASM__ __SAVE_DS__ DeflateSetHeader(
    __REG__(a0, z_streamp strm),
    __REG__(a1, gz_headerp head))
{
    return (LONG)deflateSetHeader((z_streamp)strm, (gz_headerp)head);
}

LONG
__ASM__ __SAVE_DS__ InflateCopy(
    __REG__(a0, z_streamp dest),
    __REG__(a1, z_streamp source))
{
    return (LONG)inflateCopy((z_streamp)dest, (z_streamp)source);
}

LONG
__ASM__ __SAVE_DS__ InflatePrime(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG bits),
    __REG__(d1, LONG value))
{
    return (LONG)inflatePrime((z_streamp)strm, (int)bits, (int)value);
}

LONG
__ASM__ __SAVE_DS__ InflateGetHeader(
    __REG__(a0, z_streamp strm),
    __REG__(a1, gz_headerp head))
{
    return (LONG)inflateGetHeader((z_streamp)strm, (gz_headerp)head);
}

LONG
__ASM__ __SAVE_DS__ InflateBackInit(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG windowBits),
    __REG__(a1, UBYTE *window))
{
    return (LONG)inflateBackInit((z_streamp)strm, (int)windowBits, (unsigned char *)window);
}

LONG
__ASM__ __SAVE_DS__ InflateBack(
    __REG__(a0, z_streamp strm),
    __REG__(a1, in_func in),
    __REG__(d0, APTR in_desc),
    __REG__(a2, out_func out),
    __REG__(d1, APTR out_desc))
{
    return (LONG)inflateBack((z_streamp)strm, (in_func)in, (void *)in_desc,
        (out_func)out, (void *)out_desc);
}

LONG
__ASM__ __SAVE_DS__ InflateBackEnd(
    __REG__(a0, z_streamp strm))
{
    return (LONG)inflateBackEnd((z_streamp)strm);
}

ULONG
__ASM__ __SAVE_DS__ Adler32Combine(
    __REG__(d0, ULONG adler1),
    __REG__(d1, ULONG adler2),
    __REG__(d2, LONG len2))
{
    return (ULONG)adler32_combine((uLong)adler1, (uLong)adler2, (z_off_t)len2);
}

ULONG
__ASM__ __SAVE_DS__ CRC32Combine(
    __REG__(d0, ULONG crc1),
    __REG__(d1, ULONG crc2),
    __REG__(d2, LONG len2))
{
    return (ULONG)crc32_combine((uLong)crc1, (uLong)crc2, (z_off_t)len2);
}

LONG
__ASM__ __SAVE_DS__ Compress2(
    __REG__(a0, APTR dest),
    __REG__(a1, ULONG *destLen),
    __REG__(a2, CONST_APTR source),
    __REG__(d0, ULONG sourceLen),
    __REG__(d1, LONG level))
{
    return (LONG)compress2((Bytef *)dest, (uLongf *)destLen,
        (const Bytef *)source, (uLong)sourceLen, (int)level);
}

ULONG
__ASM__ __SAVE_DS__ CompressBound(
    __REG__(d0, ULONG sourceLen))
{
    return (ULONG)compressBound((uLong)sourceLen);
}

LONG
__ASM__ __SAVE_DS__ InflateGetDictionary(
    __REG__(a0, z_streamp strm),
    __REG__(a1, UBYTE *dictionary),
    __REG__(a2, ULONG *dictLength))
{
    return (LONG)inflateGetDictionary((z_streamp)strm, (unsigned char *)dictionary,
        (uInt *)dictLength);
}

CONST_STRPTR
__ASM__ __SAVE_DS__ ZError(
    __REG__(d0, LONG err))
{
    return (CONST_STRPTR)zError((int)err);
}

LONG
__ASM__ __SAVE_DS__ Uncompress2(
    __REG__(a0, APTR dest),
    __REG__(a1, ULONG *destLen),
    __REG__(a2, CONST_APTR source),
    __REG__(a3, ULONG *sourceLen))
{
    return (LONG)uncompress2((Bytef *)dest, (uLongf *)destLen,
        (const Bytef *)source, (uLongf *)sourceLen);
}

LONG
__ASM__ __SAVE_DS__ InflateReset2(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG windowBits))
{
    return (LONG)inflateReset2((z_streamp)strm, (int)windowBits);
}

LONG
__ASM__ __SAVE_DS__ InflateValidate(
    __REG__(a0, z_streamp strm),
    __REG__(d0, LONG check))
{
    return (LONG)inflateValidate((z_streamp)strm, (int)check);
}

ULONG
__ASM__ __SAVE_DS__ CRC32CombineGen(
    __REG__(d0, LONG len2))
{
    return (ULONG)crc32_combine_gen((z_off_t)len2);
}

ULONG
__ASM__ __SAVE_DS__ CRC32CombineOp(
    __REG__(d0, ULONG crc1),
    __REG__(d1, ULONG crc2),
    __REG__(d2, ULONG op))
{
    return (ULONG)crc32_combine_op((uLong)crc1, (uLong)crc2, (uLong)op);
}
