/* zglue.c -- gzip deflate/inflate through z.library zlib (USE_ZLIB)
 * Copyright (C) 2026 amigazen project
 */

#include "tailor.h"

#ifdef USE_ZLIB
#include "zlib.h"
#endif

#include "gzip.h"

#ifdef USE_ZLIB

/* gzip extra-flags byte (algorithm.doc, deflate.c FAST/SLOW) */
#define GZ_EXTRA_FAST 4
#define GZ_EXTRA_SLOW 2

uch gz_zlib_extra_flags(void)
{
    uch flags;

    flags = 0;
    if (level == 1) {
        flags = (uch)GZ_EXTRA_FAST;
    } else if (level >= 9) {
        flags = (uch)GZ_EXTRA_SLOW;
    }
    return flags;
}

int gz_zlib_compress(void)
{
    z_stream strm;
    int err;
    unsigned nread;

    read_buf = file_read;

    strm.zalloc = (alloc_func)Z_NULL;
    strm.zfree = (free_func)Z_NULL;
    strm.opaque = (voidpf)Z_NULL;

    err = deflateInit2(&strm, level, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
    if (err == Z_MEM_ERROR) {
        return 3;
    }
    if (err != Z_OK) {
        return 1;
    }

    strm.next_out = (Bytef *)outbuf;
    strm.avail_out = OUTBUFSIZ;
    strm.avail_in = 0;

    for (;;) {
        if (strm.avail_in == 0) {
            nread = (unsigned)file_read((char *)inbuf, INBUFSIZ);
            if (nread == (unsigned)(-1)) {
                deflateEnd(&strm);
                return 1;
            }
            strm.avail_in = nread;
            strm.next_in = (Bytef *)inbuf;
        }

        err = deflate(&strm, strm.avail_in == 0 ? Z_FINISH : Z_NO_FLUSH);
        if (err == Z_STREAM_END) {
            break;
        }
        if (err != Z_OK) {
            deflateEnd(&strm);
            return 1;
        }

        while (strm.avail_out == 0) {
            flush_outbuf();
            strm.next_out = (Bytef *)outbuf;
            strm.avail_out = OUTBUFSIZ;
        }
    }

    if (outcnt > 0) {
        flush_outbuf();
    }

    err = deflateEnd(&strm);
    if (err != Z_OK) {
        return 1;
    }
    return 0;
}

int gz_zlib_inflate(void)
{
    z_stream strm;
    int err;

    outcnt = 0;

    strm.zalloc = (alloc_func)Z_NULL;
    strm.zfree = (free_func)Z_NULL;
    strm.opaque = (voidpf)Z_NULL;

    err = inflateInit2(&strm, -15);
    if (err == Z_MEM_ERROR) {
        return 3;
    }
    if (err != Z_OK) {
        return 1;
    }

    strm.next_out = (Bytef *)window;
    strm.avail_out = WSIZE;

    for (;;) {
        if (inptr >= insize) {
            if (fill_inbuf(1) == EOF) {
                inflateEnd(&strm);
                return 1;
            }
        }

        strm.next_in = (Bytef *)(inbuf + inptr);
        strm.avail_in = insize - inptr;

        err = inflate(&strm, Z_NO_FLUSH);
        inptr = (unsigned)(strm.next_in - (Bytef *)inbuf);

        if (strm.avail_out == 0) {
            flush_window();
            strm.next_out = (Bytef *)window;
            strm.avail_out = WSIZE;
            outcnt = 0;
        } else {
            outcnt = WSIZE - strm.avail_out;
        }

        if (err == Z_STREAM_END) {
            break;
        }
        if (err != Z_OK) {
            inflateEnd(&strm);
            return 1;
        }
    }

    inflateEnd(&strm);
    if (outcnt > 0) {
        flush_window();
    }
    return 0;
}

#endif /* USE_ZLIB */
