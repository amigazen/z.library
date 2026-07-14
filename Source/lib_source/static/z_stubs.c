/*
 * Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Amiga z.library integration by amigazen project
 *
 * z_stubs.c - standard zlib C names routed through z.library LVOs.
 * ZBase and OpenLibrary live in autoinit_z_base.c (one copy per archive).
 */

#define __USE_SYSBASE

#include <exec/types.h>
#include <proto/exec.h>

#define LIBRARIES_Z_H
#include "zlib.h"
#include <proto/z.h>
#include "z_stub_open.h"

/*
 * zlib.h defines Init functions as version-check macros. Undef before
 * implementing the stub entry points with the same names.
 */
#undef deflateInit
#undef deflateInit2
#undef inflateInit
#undef inflateInit2
#undef inflateBackInit

#define ZSTUB_GUARD_INT() \
    do { if (!z_stub_ensure_open()) return (int)Z_STREAM_ERROR; } while (0)

#define ZSTUB_GUARD_ULONG() \
    do { if (!z_stub_ensure_open()) return (uLong)0; } while (0)

#define ZSTUB_GUARD_STR() \
    do { if (!z_stub_ensure_open()) return (const char *)0; } while (0)

const char *zlibVersion(void)
{
    ZSTUB_GUARD_STR();
    return (const char *)ZlibVersion();
}

int deflateInit(z_streamp strm, int level)
{
    ZSTUB_GUARD_INT();
    return (int)DeflateInit((z_streamp)strm, (LONG)level);
}

int deflateInit_(z_streamp strm, int level, const char *version, int stream_size)
{
    ZSTUB_GUARD_INT();
    (void)version;
    (void)stream_size;
    return (int)DeflateInit((z_streamp)strm, (LONG)level);
}

int deflateInit2_(z_streamp strm, int level, int method, int windowBits,
    int memLevel, int strategy, const char *version, int stream_size)
{
    ZSTUB_GUARD_INT();
    (void)version;
    (void)stream_size;
    return (int)DeflateInit2((z_streamp)strm, (LONG)level, (LONG)method,
        (LONG)windowBits, (LONG)memLevel, (LONG)strategy);
}

int deflate(z_streamp strm, int flush)
{
    ZSTUB_GUARD_INT();
    return (int)Deflate((z_streamp)strm, (LONG)flush);
}

int deflateEnd(z_streamp strm)
{
    ZSTUB_GUARD_INT();
    return (int)DeflateEnd((z_streamp)strm);
}

int inflateInit(z_streamp strm)
{
    ZSTUB_GUARD_INT();
    return (int)InflateInit((z_streamp)strm);
}

int inflateInit_(z_streamp strm, const char *version, int stream_size)
{
    ZSTUB_GUARD_INT();
    (void)version;
    (void)stream_size;
    return (int)InflateInit((z_streamp)strm);
}

int inflateInit2_(z_streamp strm, int windowBits, const char *version, int stream_size)
{
    ZSTUB_GUARD_INT();
    (void)version;
    (void)stream_size;
    return (int)InflateInit2((z_streamp)strm, (LONG)windowBits);
}

int inflate(z_streamp strm, int flush)
{
    ZSTUB_GUARD_INT();
    return (int)Inflate((z_streamp)strm, (LONG)flush);
}

int inflateEnd(z_streamp strm)
{
    ZSTUB_GUARD_INT();
    return (int)InflateEnd((z_streamp)strm);
}

int deflateInit2(z_streamp strm, int level, int method, int windowBits,
    int memLevel, int strategy)
{
    ZSTUB_GUARD_INT();
    return (int)DeflateInit2((z_streamp)strm, (LONG)level, (LONG)method,
        (LONG)windowBits, (LONG)memLevel, (LONG)strategy);
}

int deflateSetDictionary(z_streamp strm, const Bytef *dictionary, uInt dictLength)
{
    ZSTUB_GUARD_INT();
    return (int)DeflateSetDictionary((z_streamp)strm, (CONST_APTR)dictionary,
        (ULONG)dictLength);
}

int deflateCopy(z_streamp dest, z_streamp source)
{
    ZSTUB_GUARD_INT();
    return (int)DeflateCopy((z_streamp)dest, (z_streamp)source);
}

int deflateReset(z_streamp strm)
{
    ZSTUB_GUARD_INT();
    return (int)DeflateReset((z_streamp)strm);
}

int deflateParams(z_streamp strm, int level, int strategy)
{
    ZSTUB_GUARD_INT();
    return (int)DeflateParams((z_streamp)strm, (LONG)level, (LONG)strategy);
}

int inflateInit2(z_streamp strm, int windowBits)
{
    ZSTUB_GUARD_INT();
    return (int)InflateInit2((z_streamp)strm, (LONG)windowBits);
}

int inflateSetDictionary(z_streamp strm, const Bytef *dictionary, uInt dictLength)
{
    ZSTUB_GUARD_INT();
    return (int)InflateSetDictionary((z_streamp)strm, (CONST_APTR)dictionary,
        (ULONG)dictLength);
}

int inflateReset(z_streamp strm)
{
    ZSTUB_GUARD_INT();
    return (int)InflateReset((z_streamp)strm);
}

int compress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    ZSTUB_GUARD_INT();
    return (int)Compress((APTR)dest, (ULONG *)destLen, (CONST_APTR)source,
        (ULONG)sourceLen);
}

int uncompress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
    ZSTUB_GUARD_INT();
    return (int)Uncompress((APTR)dest, (ULONG *)destLen, (CONST_APTR)source,
        (ULONG)sourceLen);
}

uLong adler32(uLong adler, const Bytef *buf, uInt len)
{
    ZSTUB_GUARD_ULONG();
    return (uLong)Adler32((ULONG)adler, (CONST_APTR)buf, (ULONG)len);
}

uLong crc32(uLong crc, const Bytef *buf, uInt len)
{
    ZSTUB_GUARD_ULONG();
    return (uLong)CRC32((ULONG)crc, (CONST_APTR)buf, (ULONG)len);
}

int inflateSync(z_streamp strm)
{
    ZSTUB_GUARD_INT();
    return (int)InflateSync((z_streamp)strm);
}

int deflateTune(z_streamp strm, int good_length, int max_lazy, int nice_length,
    int max_chain)
{
    ZSTUB_GUARD_INT();
    return (int)DeflateTune((z_streamp)strm, (LONG)good_length, (LONG)max_lazy,
        (LONG)nice_length, (LONG)max_chain);
}

uLong deflateBound(z_streamp strm, uLong sourceLen)
{
    ZSTUB_GUARD_ULONG();
    return (uLong)DeflateBound((z_streamp)strm, (ULONG)sourceLen);
}

int deflatePrime(z_streamp strm, int bits, int value)
{
    ZSTUB_GUARD_INT();
    return (int)DeflatePrime((z_streamp)strm, (LONG)bits, (LONG)value);
}

int deflateSetHeader(z_streamp strm, gz_headerp head)
{
    ZSTUB_GUARD_INT();
    return (int)DeflateSetHeader((z_streamp)strm, (gz_headerp)head);
}

int inflateCopy(z_streamp dest, z_streamp source)
{
    ZSTUB_GUARD_INT();
    return (int)InflateCopy((z_streamp)dest, (z_streamp)source);
}

int inflatePrime(z_streamp strm, int bits, int value)
{
    ZSTUB_GUARD_INT();
    return (int)InflatePrime((z_streamp)strm, (LONG)bits, (LONG)value);
}

int inflateGetHeader(z_streamp strm, gz_headerp head)
{
    ZSTUB_GUARD_INT();
    return (int)InflateGetHeader((z_streamp)strm, (gz_headerp)head);
}

int inflateBackInit(z_streamp strm, int windowBits, unsigned char *window)
{
    ZSTUB_GUARD_INT();
    return (int)InflateBackInit((z_streamp)strm, (LONG)windowBits, (UBYTE *)window);
}

int inflateBack(z_streamp strm, in_func in, void *in_desc, out_func out, void *out_desc)
{
    ZSTUB_GUARD_INT();
    return (int)InflateBack((z_streamp)strm, (in_func)in, (APTR)in_desc,
        (out_func)out, (APTR)out_desc);
}

int inflateBackEnd(z_streamp strm)
{
    ZSTUB_GUARD_INT();
    return (int)InflateBackEnd((z_streamp)strm);
}

uLong adler32_combine(uLong adler1, uLong adler2, z_off_t len2)
{
    ZSTUB_GUARD_ULONG();
    return (uLong)Adler32Combine((ULONG)adler1, (ULONG)adler2, (LONG)len2);
}

uLong crc32_combine(uLong crc1, uLong crc2, z_off_t len2)
{
    ZSTUB_GUARD_ULONG();
    return (uLong)CRC32Combine((ULONG)crc1, (ULONG)crc2, (LONG)len2);
}

int compress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen,
    int level)
{
    ZSTUB_GUARD_INT();
    return (int)Compress2((APTR)dest, (ULONG *)destLen, (CONST_APTR)source,
        (ULONG)sourceLen, (LONG)level);
}

uLong compressBound(uLong sourceLen)
{
    ZSTUB_GUARD_ULONG();
    return (uLong)CompressBound((ULONG)sourceLen);
}

int inflateGetDictionary(z_streamp strm, unsigned char *dictionary, uInt *dictLength)
{
    ZSTUB_GUARD_INT();
    return (int)InflateGetDictionary((z_streamp)strm, (UBYTE *)dictionary,
        (ULONG *)dictLength);
}

const char *zError(int err)
{
    ZSTUB_GUARD_STR();
    return (const char *)ZError((LONG)err);
}

int uncompress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLongf *sourceLen)
{
    ZSTUB_GUARD_INT();
    return (int)Uncompress2((APTR)dest, (ULONG *)destLen, (CONST_APTR)source,
        (ULONG *)sourceLen);
}

int inflateReset2(z_streamp strm, int windowBits)
{
    ZSTUB_GUARD_INT();
    return (int)InflateReset2((z_streamp)strm, (LONG)windowBits);
}

int inflateValidate(z_streamp strm, int check)
{
    ZSTUB_GUARD_INT();
    return (int)InflateValidate((z_streamp)strm, (LONG)check);
}

uLong crc32_combine_gen(z_off_t len2)
{
    ZSTUB_GUARD_ULONG();
    return (uLong)CRC32CombineGen((LONG)len2);
}

uLong crc32_combine_op(uLong crc1, uLong crc2, uLong op)
{
    ZSTUB_GUARD_ULONG();
    return (uLong)CRC32CombineOp((ULONG)crc1, (ULONG)crc2, (ULONG)op);
}
