/*
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright 2026 amigazen project
 *
 * ZTest.c - Standalone z.library smoke / regression harness.
 *
 * Run after "smake install" and "smake headers" in Source/lib_source.
 * Exercises every public z.library LVO from SDK/SFD/z_lib.sfd.
 * Uses library-default zalloc (NULL); app-supplied allocators are not tested
 * here because SAS/C small-data callbacks across the library boundary are
 * unsupported in production (see amihttp ht_zlib.c).
 */

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <stdio.h>
#include <string.h>

#include <libraries/z.h>
#include <clib/compiler-specific.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/z.h>

extern struct Library *ZBase;

#define ZT_GZIP_WINDOW  (15L + 16L)
#define ZT_RAW_WINDOW   (-15L)
#define ZT_IB_WINDOW    15L
#define ZT_IB_WINDOW_SZ (1UL << ZT_IB_WINDOW)

/* gzip(mtime=0) of "Hello, Amiga!" */
static const UBYTE zt_gzip_hello[] = {
    0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0xff, 0xf3, 0x48, 0xcd, 0xc9, 0xc9, 0xd7,
    0x51, 0x70, 0xcc, 0xcd, 0x4c, 0x4f, 0x54, 0x04,
    0x00, 0x0c, 0xba, 0x3f, 0xb5, 0x0d, 0x00, 0x00,
    0x00
};

static const STRPTR zt_plain_hello = (STRPTR)"Hello, Amiga!";

/* zlib wrapper of "Hello, Amiga!" */
static const UBYTE zt_zlib_hello[] = {
    0x78, 0xda, 0xf3, 0x48, 0xcd, 0xc9, 0xc9, 0xd7,
    0x51, 0x70, 0xcc, 0xcd, 0x4c, 0x4f, 0x54, 0x04,
    0x00, 0x1e, 0xd7, 0x04, 0x41
};

static const UBYTE zt_dict[] = "hello ";
static const UBYTE zt_dict_src[] =
    "hello there, hello there, hello there!";

static ULONG zt_pass;
static ULONG zt_fail;

static VOID
zt_note(STRPTR name, BOOL ok, STRPTR detail)
{
    if (ok) {
        zt_pass++;
        Printf("ZTest: PASS %s", name);
    } else {
        zt_fail++;
        Printf("ZTest: FAIL %s", name);
    }
    if (detail != NULL && detail[0] != '\0') {
        Printf(" (%s)", detail);
    }
    Printf("\n");
    Flush(Output());
}

static VOID
zt_note_zerr(STRPTR name, LONG rc)
{
    STRPTR msg;
    char buf[64];

    msg = (STRPTR)ZError(rc);
    if (msg == NULL) {
        msg = (STRPTR)"?";
    }
    sprintf(buf, "rc=%ld %s", (long)rc, (char *)msg);
    zt_note(name, FALSE, (STRPTR)buf);
}

static VOID
zt_stream_clear(z_stream *strm)
{
    memset(strm, 0, sizeof(*strm));
    /* zalloc/zfree left NULL: use z.library pooled malloc (amihttp pattern). */
}

static BOOL
zt_test_open(VOID)
{
    ZBase = OpenLibrary((STRPTR)"z.library", 2);
    if (ZBase == NULL) {
        zt_note("OpenLibrary(z.library,2)", FALSE, (STRPTR)"not found");
        return FALSE;
    }
    zt_note("OpenLibrary(z.library,2)", TRUE, NULL);
    return TRUE;
}

static VOID
zt_test_version(VOID)
{
    CONST_STRPTR ver;

    ver = ZlibVersion();
    if (ver == NULL || ver[0] == '\0') {
        zt_note("ZlibVersion", FALSE, (STRPTR)"NULL/empty");
        return;
    }
    zt_note("ZlibVersion", TRUE, (STRPTR)ver);
}

static VOID
zt_test_zerror(VOID)
{
    CONST_STRPTR msg;

    /* zlib maps Z_OK to an empty string, not an error description. */
    msg = ZError(Z_OK);
    if (msg == NULL) {
        zt_note("ZError", FALSE, (STRPTR)"Z_OK NULL");
        return;
    }
    msg = ZError(Z_DATA_ERROR);
    if (msg == NULL || msg[0] == '\0') {
        zt_note("ZError", FALSE, (STRPTR)"Z_DATA_ERROR");
        return;
    }
    zt_note("ZError", TRUE, NULL);
}

static VOID
zt_test_crc32(VOID)
{
    ULONG crc;
    ULONG init;

    init = CRC32(0, NULL, 0);
    crc = CRC32(init, (CONST_APTR)"123456789", 9);
    if (crc == 0xcbf43926UL) {
        zt_note("CRC32", TRUE, NULL);
    } else {
        zt_note("CRC32", FALSE, (STRPTR)"bad value");
    }
}

static VOID
zt_test_adler32(VOID)
{
    ULONG adler;
    ULONG init;

    init = Adler32(0, NULL, 0);
    adler = Adler32(init, (CONST_APTR)"Wikipedia", 9);
    if (adler == 0x11e60398UL) {
        zt_note("Adler32", TRUE, NULL);
    } else {
        zt_note("Adler32", FALSE, (STRPTR)"bad value");
    }
}

static VOID
zt_test_combine(VOID)
{
    ULONG base;
    ULONG a1;
    ULONG a2;
    ULONG ac;
    ULONG ad;
    ULONG c1;
    ULONG c2;
    ULONG cc;
    ULONG cd;
    ULONG op;

    base = Adler32(0, NULL, 0);
    a1 = Adler32(base, (CONST_APTR)"abc", 3);
    a2 = Adler32(base, (CONST_APTR)"def", 3);
    ac = Adler32Combine(a1, a2, 3);
    ad = Adler32(base, (CONST_APTR)"abcdef", 6);
    if (ac != ad) {
        zt_note("Adler32Combine", FALSE, (STRPTR)"mismatch");
        return;
    }
    zt_note("Adler32Combine", TRUE, NULL);

    c1 = CRC32(0, (CONST_APTR)"abc", 3);
    c2 = CRC32(0, (CONST_APTR)"def", 3);
    cc = CRC32Combine(c1, c2, 3);
    cd = CRC32(0, (CONST_APTR)"abcdef", 6);
    if (cc != cd) {
        zt_note("CRC32Combine", FALSE, (STRPTR)"mismatch");
        return;
    }
    zt_note("CRC32Combine", TRUE, NULL);

    op = CRC32CombineGen(3);
    if (op == 0) {
        zt_note("CRC32CombineGen", FALSE, (STRPTR)"zero op");
        return;
    }
    if (CRC32CombineOp(c1, c2, op) != cd) {
        zt_note("CRC32CombineOp", FALSE, (STRPTR)"mismatch");
        return;
    }
    zt_note("CRC32CombineGen/CRC32CombineOp", TRUE, NULL);
}

static VOID
zt_test_compress(VOID)
{
    UBYTE comp[256];
    UBYTE plain[256];
    ULONG comp_len;
    ULONG plain_len;
    LONG rc;

    comp_len = sizeof(comp);
    rc = Compress(comp, &comp_len, (CONST_APTR)zt_plain_hello,
        (ULONG)strlen((char *)zt_plain_hello));
    if (rc != Z_OK || comp_len == 0) {
        zt_note("Compress", FALSE, (STRPTR)"compress failed");
        return;
    }

    plain_len = sizeof(plain);
    rc = Uncompress(plain, &plain_len, comp, comp_len);
    if (rc != Z_OK) {
        zt_note("Uncompress", FALSE, (STRPTR)"uncompress failed");
        return;
    }
    plain[plain_len] = '\0';
    if (strcmp((char *)plain, (char *)zt_plain_hello) != 0) {
        zt_note("Compress/Uncompress", FALSE, (STRPTR)"roundtrip mismatch");
        return;
    }
    zt_note("Compress/Uncompress", TRUE, NULL);
}

static VOID
zt_test_compress_extras(VOID)
{
    UBYTE comp[256];
    UBYTE plain[256];
    ULONG src_len;
    ULONG comp_len;
    ULONG plain_len;
    ULONG src_used;
    ULONG bound;
    LONG rc;

    src_len = (ULONG)strlen((char *)zt_plain_hello);
    bound = CompressBound(src_len);
    if (bound < src_len) {
        zt_note("CompressBound", FALSE, (STRPTR)"too small");
        return;
    }
    zt_note("CompressBound", TRUE, NULL);

    comp_len = sizeof(comp);
    rc = Compress2(comp, &comp_len, (CONST_APTR)zt_plain_hello, src_len,
        Z_BEST_COMPRESSION);
    if (rc != Z_OK || comp_len == 0) {
        zt_note("Compress2", FALSE, (STRPTR)"failed");
        return;
    }
    zt_note("Compress2", TRUE, NULL);

    plain_len = sizeof(plain);
    src_used = comp_len;
    rc = Uncompress2(plain, &plain_len, comp, &src_used);
    if (rc != Z_OK || src_used != comp_len) {
        zt_note("Uncompress2", FALSE, (STRPTR)"failed");
        return;
    }
    plain[plain_len] = '\0';
    if (strcmp((char *)plain, (char *)zt_plain_hello) != 0) {
        zt_note("Uncompress2", FALSE, (STRPTR)"mismatch");
        return;
    }
    zt_note("Uncompress2", TRUE, NULL);
}

static BOOL
zt_inflate_buffer(z_stream *strm, CONST_APTR in, ULONG in_len,
    APTR out, ULONG out_cap, ULONG *out_len)
{
    LONG rc;

    *out_len = 0;
    strm->next_in = (UBYTE *)in;
    strm->avail_in = in_len;
    strm->next_out = (UBYTE *)out;
    strm->avail_out = out_cap;

    rc = Inflate(strm, Z_FINISH);
    *out_len = out_cap - strm->avail_out;

    if (rc != Z_STREAM_END) {
        return FALSE;
    }
    return TRUE;
}

static BOOL
zt_inflate_chunked(z_stream *strm, CONST_APTR in, ULONG in_len,
    APTR out, ULONG out_cap, ULONG *out_len, ULONG chunk)
{
    ULONG in_pos;
    ULONG take;
    ULONG produced;
    LONG rc;
    LONG flush;

    in_pos = 0;
    strm->next_out = (UBYTE *)out;
    strm->avail_out = out_cap;
    flush = Z_NO_FLUSH;

    for (;;) {
        if (strm->avail_in == 0 && in_pos < in_len) {
            take = in_len - in_pos;
            if (take > chunk) {
                take = chunk;
            }
            strm->next_in = ((UBYTE *)in) + in_pos;
            strm->avail_in = take;
            in_pos += take;
        }
        if (in_pos >= in_len && strm->avail_in == 0) {
            flush = Z_FINISH;
        }

        rc = Inflate(strm, flush);
        produced = out_cap - strm->avail_out;

        if (rc == Z_STREAM_END) {
            *out_len = produced;
            return TRUE;
        }
        if (rc == Z_BUF_ERROR && flush == Z_FINISH) {
            if (produced > 0) {
                *out_len = produced;
                return TRUE;
            }
        }
        if (rc != Z_OK && rc != Z_BUF_ERROR) {
            return FALSE;
        }
        if (flush == Z_FINISH && produced == out_cap) {
            return FALSE;
        }
    }
}

static BOOL
zt_deflate_buffer(z_stream *strm, CONST_APTR in, ULONG in_len,
    APTR out, ULONG out_cap, ULONG *out_len, LONG flush)
{
    LONG rc;

    strm->next_in = (UBYTE *)in;
    strm->avail_in = in_len;
    strm->next_out = (UBYTE *)out;
    strm->avail_out = out_cap;
    rc = Deflate(strm, flush);
    *out_len = out_cap - strm->avail_out;
    if (rc != Z_STREAM_END && rc != Z_OK) {
        return FALSE;
    }
    return TRUE;
}

static VOID
zt_test_deflate_init(VOID)
{
    z_stream strm;
    UBYTE comp[128];
    UBYTE out[64];
    ULONG comp_len;
    ULONG out_len;
    LONG rc;

    zt_stream_clear(&strm);
    rc = DeflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (rc != Z_OK) {
        zt_note_zerr("DeflateInit", rc);
        return;
    }
    if (!zt_deflate_buffer(&strm, zt_plain_hello,
            (ULONG)strlen((char *)zt_plain_hello),
            comp, sizeof(comp), &comp_len, Z_FINISH)) {
        DeflateEnd(&strm);
        zt_note("DeflateInit/Deflate", FALSE, NULL);
        return;
    }
    DeflateEnd(&strm);

    zt_stream_clear(&strm);
    rc = InflateInit(&strm);
    if (rc != Z_OK) {
        zt_note_zerr("InflateInit(after DeflateInit)", rc);
        return;
    }
    if (!zt_inflate_buffer(&strm, comp, comp_len, out, sizeof(out) - 1, &out_len)) {
        InflateEnd(&strm);
        zt_note("DeflateInit roundtrip", FALSE, NULL);
        return;
    }
    InflateEnd(&strm);
    out[out_len] = '\0';
    if (strcmp((char *)out, (char *)zt_plain_hello) != 0) {
        zt_note("DeflateInit roundtrip", FALSE, (STRPTR)"mismatch");
        return;
    }
    zt_note("DeflateInit/Deflate/Inflate", TRUE, NULL);
}

static VOID
zt_test_deflate_stream_ops(VOID)
{
    z_stream strm;
    z_stream copy;
    UBYTE comp_a[128];
    UBYTE comp_b[128];
    ULONG src_len;
    ULONG len_a;
    ULONG len_b;
    ULONG bound;
    ULONG partial;
    LONG rc;

    src_len = (ULONG)strlen((char *)zt_plain_hello);
    zt_stream_clear(&strm);
    rc = DeflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (rc != Z_OK) {
        zt_note_zerr("DeflateReset setup", rc);
        return;
    }

    bound = DeflateBound(&strm, src_len);
    if (bound < src_len) {
        DeflateEnd(&strm);
        zt_note("DeflateBound", FALSE, (STRPTR)"too small");
        return;
    }
    zt_note("DeflateBound", TRUE, NULL);

    rc = DeflateTune(&strm, 8, 16, 32, 128);
    if (rc != Z_OK) {
        DeflateEnd(&strm);
        zt_note("DeflateTune", FALSE, NULL);
        return;
    }
    zt_note("DeflateTune", TRUE, NULL);

    strm.next_in = (UBYTE *)zt_plain_hello;
    strm.avail_in = src_len / 2;
    strm.next_out = comp_a;
    strm.avail_out = sizeof(comp_a);
    rc = Deflate(&strm, Z_SYNC_FLUSH);
    if (rc != Z_OK) {
        DeflateEnd(&strm);
        zt_note("DeflateReset", FALSE, (STRPTR)"partial deflate");
        return;
    }
    len_a = sizeof(comp_a) - strm.avail_out;

    rc = DeflateReset(&strm);
    if (rc != Z_OK) {
        DeflateEnd(&strm);
        zt_note("DeflateReset", FALSE, NULL);
        return;
    }
    if (!zt_deflate_buffer(&strm, zt_plain_hello, src_len,
            comp_a, sizeof(comp_a), &len_a, Z_FINISH)) {
        DeflateEnd(&strm);
        zt_note("DeflateReset", FALSE, (STRPTR)"after reset");
        return;
    }
    zt_note("DeflateReset", TRUE, NULL);

    zt_stream_clear(&strm);
    zt_stream_clear(&copy);
    rc = DeflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (rc != Z_OK) {
        zt_note_zerr("DeflateCopy setup", rc);
        return;
    }
    rc = DeflateInit(&copy, Z_DEFAULT_COMPRESSION);
    if (rc != Z_OK) {
        DeflateEnd(&strm);
        zt_note_zerr("DeflateCopy dest setup", rc);
        return;
    }
    strm.next_in = (UBYTE *)zt_plain_hello;
    strm.avail_in = src_len / 2;
    strm.next_out = comp_a;
    strm.avail_out = sizeof(comp_a);
    rc = Deflate(&strm, Z_NO_FLUSH);
    if (rc != Z_OK) {
        DeflateEnd(&strm);
        DeflateEnd(&copy);
        zt_note("DeflateCopy", FALSE, (STRPTR)"partial deflate");
        return;
    }
    rc = DeflateCopy(&copy, &strm);
    if (rc != Z_OK) {
        DeflateEnd(&strm);
        DeflateEnd(&copy);
        zt_note("DeflateCopy", FALSE, NULL);
        return;
    }
    partial = (ULONG)(strm.next_out - comp_a);
    memcpy(comp_b, comp_a, partial);
    strm.next_in = ((UBYTE *)zt_plain_hello) + (src_len / 2);
    strm.avail_in = src_len - (src_len / 2);
    rc = Deflate(&strm, Z_FINISH);
    len_a = sizeof(comp_a) - strm.avail_out;
    copy.next_out = comp_b + partial;
    copy.avail_out = sizeof(comp_b) - partial;
    copy.next_in = ((UBYTE *)zt_plain_hello) + (src_len / 2);
    copy.avail_in = src_len - (src_len / 2);
    rc = Deflate(&copy, Z_FINISH);
    len_b = sizeof(comp_b) - copy.avail_out;
    DeflateEnd(&strm);
    DeflateEnd(&copy);
    if (rc != Z_STREAM_END || len_a == 0 || len_a != len_b
        || memcmp(comp_a, comp_b, len_a) != 0) {
        zt_note("DeflateCopy", FALSE, (STRPTR)"output mismatch");
        return;
    }
    zt_note("DeflateCopy", TRUE, NULL);

    zt_stream_clear(&strm);
    rc = DeflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (rc != Z_OK) {
        zt_note_zerr("DeflateParams setup", rc);
        return;
    }
    strm.next_in = (UBYTE *)zt_plain_hello;
    strm.avail_in = src_len / 2;
    strm.next_out = comp_a;
    strm.avail_out = sizeof(comp_a);
    rc = Deflate(&strm, Z_NO_FLUSH);
    if (rc != Z_OK) {
        DeflateEnd(&strm);
        zt_note("DeflateParams", FALSE, (STRPTR)"first half");
        return;
    }
    rc = DeflateParams(&strm, Z_BEST_SPEED, Z_DEFAULT_STRATEGY);
    if (rc != Z_OK) {
        DeflateEnd(&strm);
        zt_note("DeflateParams", FALSE, NULL);
        return;
    }
    strm.next_in = ((UBYTE *)zt_plain_hello) + (src_len / 2);
    strm.avail_in = src_len - (src_len / 2);
    rc = Deflate(&strm, Z_FINISH);
    len_a = sizeof(comp_a) - strm.avail_out;
    DeflateEnd(&strm);
    if (rc != Z_STREAM_END || len_a == 0) {
        zt_note("DeflateParams", FALSE, (STRPTR)"second half");
        return;
    }
    zt_note("DeflateParams", TRUE, NULL);
}

static VOID
zt_test_dictionary(VOID)
{
    z_stream dstrm;
    z_stream istrm;
    UBYTE comp[256];
    UBYTE plain[256];
    ULONG dict_len;
    ULONG src_len;
    ULONG comp_len;
    ULONG plain_len;
    LONG rc;

    dict_len = (ULONG)strlen((char *)zt_dict);
    src_len = (ULONG)strlen((char *)zt_dict_src);

    zt_stream_clear(&dstrm);
    rc = DeflateInit(&dstrm, Z_DEFAULT_COMPRESSION);
    if (rc != Z_OK) {
        zt_note_zerr("DeflateSetDictionary setup", rc);
        return;
    }
    rc = DeflateSetDictionary(&dstrm, zt_dict, dict_len);
    if (rc != Z_OK) {
        DeflateEnd(&dstrm);
        zt_note("DeflateSetDictionary", FALSE, NULL);
        return;
    }
    zt_note("DeflateSetDictionary", TRUE, NULL);
    if (!zt_deflate_buffer(&dstrm, zt_dict_src, src_len,
            comp, sizeof(comp), &comp_len, Z_FINISH)) {
        DeflateEnd(&dstrm);
        zt_note("DeflateSetDictionary", FALSE, (STRPTR)"deflate");
        return;
    }
    DeflateEnd(&dstrm);

    zt_stream_clear(&istrm);
    rc = InflateInit(&istrm);
    if (rc != Z_OK) {
        zt_note_zerr("InflateSetDictionary setup", rc);
        return;
    }
    istrm.next_in = comp;
    istrm.avail_in = comp_len;
    istrm.next_out = plain;
    istrm.avail_out = sizeof(plain);
    rc = Inflate(&istrm, Z_FINISH);
    if (rc == Z_NEED_DICT) {
        rc = InflateSetDictionary(&istrm, zt_dict, dict_len);
        if (rc != Z_OK) {
            InflateEnd(&istrm);
            zt_note("InflateSetDictionary", FALSE, NULL);
            return;
        }
        zt_note("InflateSetDictionary", TRUE, NULL);
        rc = Inflate(&istrm, Z_FINISH);
    } else {
        zt_note("InflateSetDictionary", FALSE, (STRPTR)"no Z_NEED_DICT");
        InflateEnd(&istrm);
        return;
    }
    plain_len = sizeof(plain) - istrm.avail_out;
    InflateEnd(&istrm);
    if (rc != Z_STREAM_END) {
        zt_note("DeflateSetDictionary roundtrip", FALSE, (STRPTR)"inflate");
        return;
    }
    plain[plain_len] = '\0';
    if (strcmp((char *)plain, (char *)zt_dict_src) != 0) {
        zt_note("DeflateSetDictionary roundtrip", FALSE, (STRPTR)"mismatch");
        return;
    }
    zt_note("DeflateSetDictionary roundtrip", TRUE, NULL);
}

static VOID
zt_test_gzip_header(VOID)
{
    z_stream dstrm;
    z_stream istrm;
    gz_header dh;
    gz_header ih;
    UBYTE comp[256];
    UBYTE plain[256];
    ULONG src_len;
    ULONG comp_len;
    ULONG plain_len;
    LONG rc;

    src_len = (ULONG)strlen((char *)zt_plain_hello);
    memset(&dh, 0, sizeof(dh));
    memset(&ih, 0, sizeof(ih));
    dh.text = 1;
    dh.time = 12345;
    dh.os = 3;
    ih.text = -1;

    zt_stream_clear(&dstrm);
    rc = DeflateInit2(&dstrm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
        ZT_GZIP_WINDOW, 8, Z_DEFAULT_STRATEGY);
    if (rc != Z_OK) {
        zt_note_zerr("DeflateSetHeader setup", rc);
        return;
    }
    rc = DeflateSetHeader(&dstrm, &dh);
    if (rc != Z_OK) {
        DeflateEnd(&dstrm);
        zt_note("DeflateSetHeader", FALSE, NULL);
        return;
    }
    zt_note("DeflateSetHeader", TRUE, NULL);
    if (!zt_deflate_buffer(&dstrm, zt_plain_hello, src_len,
            comp, sizeof(comp), &comp_len, Z_FINISH)) {
        DeflateEnd(&dstrm);
        zt_note("DeflateSetHeader", FALSE, (STRPTR)"deflate");
        return;
    }
    DeflateEnd(&dstrm);

    zt_stream_clear(&istrm);
    rc = InflateInit2(&istrm, ZT_GZIP_WINDOW);
    if (rc != Z_OK) {
        zt_note_zerr("InflateGetHeader setup", rc);
        return;
    }
    rc = InflateGetHeader(&istrm, &ih);
    if (rc != Z_OK) {
        InflateEnd(&istrm);
        zt_note("InflateGetHeader", FALSE, (STRPTR)"before inflate");
        return;
    }
    if (!zt_inflate_buffer(&istrm, comp, comp_len,
            plain, sizeof(plain) - 1, &plain_len)) {
        InflateEnd(&istrm);
        zt_note("InflateGetHeader", FALSE, (STRPTR)"inflate");
        return;
    }
    InflateEnd(&istrm);
    if (ih.done != 1 || ih.time != 12345) {
        zt_note("InflateGetHeader", FALSE, (STRPTR)"header fields");
        return;
    }
    plain[plain_len] = '\0';
    if (strcmp((char *)plain, (char *)zt_plain_hello) != 0) {
        zt_note("InflateGetHeader", FALSE, (STRPTR)"plain mismatch");
        return;
    }
    zt_note("InflateGetHeader", TRUE, NULL);
}

static VOID
zt_test_raw_prime(VOID)
{
    z_stream dstrm;
    z_stream istrm;
    UBYTE comp[64];
    UBYTE plain[64];
    ULONG src_len;
    ULONG comp_len;
    ULONG plain_len;
    LONG rc;

    src_len = (ULONG)strlen((char *)zt_plain_hello);

    zt_stream_clear(&dstrm);
    rc = DeflateInit2(&dstrm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
        ZT_RAW_WINDOW, 8, Z_DEFAULT_STRATEGY);
    if (rc != Z_OK) {
        zt_note_zerr("DeflatePrime setup", rc);
        return;
    }
    rc = DeflatePrime(&dstrm, 0, 0);
    if (rc != Z_OK) {
        DeflateEnd(&dstrm);
        zt_note("DeflatePrime", FALSE, NULL);
        return;
    }
    zt_note("DeflatePrime", TRUE, NULL);
    if (!zt_deflate_buffer(&dstrm, zt_plain_hello, src_len,
            comp, sizeof(comp), &comp_len, Z_FINISH)) {
        DeflateEnd(&dstrm);
        zt_note("DeflatePrime", FALSE, (STRPTR)"deflate");
        return;
    }
    DeflateEnd(&dstrm);

    zt_stream_clear(&istrm);
    rc = InflateInit2(&istrm, ZT_RAW_WINDOW);
    if (rc != Z_OK) {
        zt_note_zerr("InflatePrime setup", rc);
        return;
    }
    rc = InflatePrime(&istrm, 0, 0);
    if (rc != Z_OK) {
        InflateEnd(&istrm);
        zt_note("InflatePrime", FALSE, NULL);
        return;
    }
    zt_note("InflatePrime", TRUE, NULL);
    if (!zt_inflate_buffer(&istrm, comp, comp_len,
            plain, sizeof(plain) - 1, &plain_len)) {
        InflateEnd(&istrm);
        zt_note("InflatePrime", FALSE, (STRPTR)"inflate");
        return;
    }
    InflateEnd(&istrm);
    plain[plain_len] = '\0';
    if (strcmp((char *)plain, (char *)zt_plain_hello) != 0) {
        zt_note("DeflatePrime roundtrip", FALSE, (STRPTR)"mismatch");
        return;
    }
    zt_note("DeflatePrime roundtrip", TRUE, NULL);
}

static VOID
zt_test_inflate_stream_ops(VOID)
{
    z_stream dstrm;
    z_stream strm;
    z_stream copy;
    UBYTE comp[256];
    UBYTE out[128];
    UBYTE dict[32768];
    ULONG out_len;
    ULONG dict_len;
    ULONG src_len;
    ULONG comp_len;
    LONG rc;

    src_len = (ULONG)strlen((char *)zt_dict_src);
    zt_stream_clear(&dstrm);
    rc = DeflateInit(&dstrm, Z_DEFAULT_COMPRESSION);
    if (rc != Z_OK) {
        zt_note_zerr("InflateGetDictionary setup", rc);
        return;
    }
    if (!zt_deflate_buffer(&dstrm, zt_dict_src, src_len,
            comp, sizeof(comp), &comp_len, Z_FINISH)) {
        DeflateEnd(&dstrm);
        zt_note("InflateGetDictionary", FALSE, (STRPTR)"deflate setup");
        return;
    }
    DeflateEnd(&dstrm);

    zt_stream_clear(&strm);
    rc = InflateInit2(&strm, ZT_GZIP_WINDOW);
    if (rc != Z_OK) {
        zt_note_zerr("InflateReset setup", rc);
        return;
    }
    if (!zt_inflate_buffer(&strm, zt_gzip_hello, sizeof(zt_gzip_hello),
            out, sizeof(out) - 1, &out_len)) {
        InflateEnd(&strm);
        zt_note("InflateReset", FALSE, (STRPTR)"first pass");
        return;
    }
    rc = InflateReset(&strm);
    if (rc != Z_OK) {
        InflateEnd(&strm);
        zt_note("InflateReset", FALSE, NULL);
        return;
    }
    if (!zt_inflate_buffer(&strm, zt_gzip_hello, sizeof(zt_gzip_hello),
            out, sizeof(out) - 1, &out_len)) {
        InflateEnd(&strm);
        zt_note("InflateReset", FALSE, (STRPTR)"second pass");
        return;
    }
    InflateEnd(&strm);
    zt_note("InflateReset", TRUE, NULL);

    zt_stream_clear(&strm);
    rc = InflateInit(&strm);
    if (rc != Z_OK) {
        zt_note_zerr("InflateReset2 setup", rc);
        return;
    }
    rc = InflateReset2(&strm, 15);
    if (rc != Z_OK) {
        InflateEnd(&strm);
        zt_note("InflateReset2", FALSE, NULL);
        return;
    }
    if (!zt_inflate_buffer(&strm, zt_zlib_hello, sizeof(zt_zlib_hello),
            out, sizeof(out) - 1, &out_len)) {
        InflateEnd(&strm);
        zt_note("InflateReset2", FALSE, (STRPTR)"inflate");
        return;
    }
    InflateEnd(&strm);
    zt_note("InflateReset2", TRUE, NULL);

    zt_stream_clear(&strm);
    rc = InflateInit2(&strm, ZT_GZIP_WINDOW);
    if (rc != Z_OK) {
        zt_note_zerr("InflateCopy setup", rc);
        return;
    }
    zt_stream_clear(&copy);
    rc = InflateInit2(&copy, ZT_GZIP_WINDOW);
    if (rc != Z_OK) {
        InflateEnd(&strm);
        zt_note_zerr("InflateCopy dest setup", rc);
        return;
    }
    if (!zt_inflate_chunked(&strm, zt_gzip_hello, sizeof(zt_gzip_hello),
            out, sizeof(out) - 1, &out_len, 8)) {
        InflateEnd(&strm);
        InflateEnd(&copy);
        zt_note("InflateCopy", FALSE, (STRPTR)"source inflate");
        return;
    }
    rc = InflateCopy(&copy, &strm);
    if (rc != Z_OK) {
        InflateEnd(&strm);
        InflateEnd(&copy);
        zt_note("InflateCopy", FALSE, NULL);
        return;
    }
    InflateEnd(&strm);
    InflateEnd(&copy);
    zt_note("InflateCopy", TRUE, NULL);

    zt_stream_clear(&strm);
    rc = InflateInit(&strm);
    if (rc != Z_OK) {
        zt_note_zerr("InflateGetDictionary setup", rc);
        return;
    }
    strm.next_in = comp;
    strm.avail_in = comp_len;
    strm.next_out = out;
    strm.avail_out = 8;
    rc = Inflate(&strm, Z_NO_FLUSH);
    if (rc != Z_OK && rc != Z_BUF_ERROR) {
        InflateEnd(&strm);
        zt_note("InflateGetDictionary", FALSE, (STRPTR)"partial inflate");
        return;
    }
    dict_len = sizeof(dict);
    rc = InflateGetDictionary(&strm, dict, &dict_len);
    if (rc != Z_OK) {
        InflateEnd(&strm);
        zt_note("InflateGetDictionary", FALSE, NULL);
        return;
    }
    InflateEnd(&strm);
    zt_note("InflateGetDictionary", TRUE, NULL);

    zt_stream_clear(&strm);
    rc = InflateInit(&strm);
    if (rc != Z_OK) {
        zt_note_zerr("InflateValidate setup", rc);
        return;
    }
    rc = InflateValidate(&strm, 1);
    if (rc != Z_OK) {
        InflateEnd(&strm);
        zt_note("InflateValidate", FALSE, (STRPTR)"enable");
        return;
    }
    rc = InflateValidate(&strm, 0);
    InflateEnd(&strm);
    if (rc != Z_OK) {
        zt_note("InflateValidate", FALSE, (STRPTR)"disable");
        return;
    }
    zt_note("InflateValidate", TRUE, NULL);
}

static VOID
zt_test_inflate_sync(VOID)
{
    z_stream dstrm;
    z_stream istrm;
    UBYTE comp[256];
    UBYTE corrupt[256];
    ULONG src_len;
    ULONG comp_len;
    LONG rc;

    src_len = (ULONG)strlen((char *)zt_dict_src);
    zt_stream_clear(&dstrm);
    rc = DeflateInit(&dstrm, Z_DEFAULT_COMPRESSION);
    if (rc != Z_OK) {
        zt_note_zerr("InflateSync setup", rc);
        return;
    }
    dstrm.next_in = (UBYTE *)zt_dict_src;
    dstrm.avail_in = src_len / 2;
    dstrm.next_out = comp;
    dstrm.avail_out = sizeof(comp);
    rc = Deflate(&dstrm, Z_FULL_FLUSH);
    if (rc != Z_OK) {
        DeflateEnd(&dstrm);
        zt_note("InflateSync", FALSE, (STRPTR)"deflate flush");
        return;
    }
    dstrm.next_in = ((UBYTE *)zt_dict_src) + (src_len / 2);
    dstrm.avail_in = src_len - (src_len / 2);
    rc = Deflate(&dstrm, Z_FINISH);
    comp_len = sizeof(comp) - dstrm.avail_out;
    DeflateEnd(&dstrm);
    if (rc != Z_STREAM_END || comp_len < 8) {
        zt_note("InflateSync", FALSE, (STRPTR)"deflate finish");
        return;
    }

    memcpy(corrupt, comp, comp_len);
    if (comp_len > 4) {
        corrupt[comp_len / 2] ^= 0x01;
    }

    zt_stream_clear(&istrm);
    rc = InflateInit(&istrm);
    if (rc != Z_OK) {
        zt_note_zerr("InflateSync inflate setup", rc);
        return;
    }
    istrm.next_in = corrupt;
    istrm.avail_in = comp_len;
    istrm.next_out = comp;
    istrm.avail_out = sizeof(comp);
    (void)Inflate(&istrm, Z_NO_FLUSH);
    rc = InflateSync(&istrm);
    InflateEnd(&istrm);
    if (rc != Z_OK && rc != Z_DATA_ERROR && rc != Z_BUF_ERROR) {
        zt_note("InflateSync", FALSE, (STRPTR)"unexpected rc");
        return;
    }
    zt_note("InflateSync", TRUE, NULL);
}

static ULONG __SAVE_DS__ __STDARGS__
zt_ib_in(APTR desc, UBYTE **buf)
{
    /* Input is preset on strm->next_in; return 0 if inflateBack calls in(). */
    (void)desc;
    (void)buf;
    return 0;
}

static LONG __SAVE_DS__ __STDARGS__
zt_ib_out(APTR desc, UBYTE *buf, ULONG len)
{
    /* Accept flushes from infback; output is verified in the caller window. */
    (void)desc;
    (void)buf;
    (void)len;
    return 0;
}

static VOID
zt_test_inflate_back(VOID)
{
    z_stream dstrm;
    z_stream bstrm;
    z_stream vstrm;
    UBYTE *window;
    UBYTE raw[128];
    UBYTE verify[64];
    ULONG src_len;
    ULONG raw_len;
    ULONG verify_len;
    LONG rc;

    src_len = (ULONG)strlen((char *)zt_plain_hello);
    zt_stream_clear(&dstrm);
    rc = DeflateInit2(&dstrm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
        ZT_RAW_WINDOW, 8, Z_DEFAULT_STRATEGY);
    if (rc != Z_OK) {
        zt_note_zerr("InflateBack setup", rc);
        return;
    }
    if (!zt_deflate_buffer(&dstrm, zt_plain_hello, src_len,
            raw, sizeof(raw), &raw_len, Z_FINISH)) {
        DeflateEnd(&dstrm);
        zt_note("InflateBack", FALSE, (STRPTR)"raw deflate");
        return;
    }
    DeflateEnd(&dstrm);

    zt_stream_clear(&vstrm);
    rc = InflateInit2(&vstrm, ZT_RAW_WINDOW);
    if (rc != Z_OK) {
        zt_note_zerr("InflateBack raw verify setup", rc);
        return;
    }
    if (!zt_inflate_buffer(&vstrm, raw, raw_len, verify, sizeof(verify) - 1,
            &verify_len) || verify_len != src_len
        || memcmp(verify, zt_plain_hello, src_len) != 0) {
        InflateEnd(&vstrm);
        zt_note("InflateBack", FALSE, (STRPTR)"raw verify");
        return;
    }
    InflateEnd(&vstrm);

    window = (UBYTE *)AllocMem(ZT_IB_WINDOW_SZ, MEMF_ANY);
    if (window == NULL) {
        zt_note("InflateBackInit", FALSE, (STRPTR)"no window mem");
        return;
    }

    zt_stream_clear(&bstrm);
    rc = InflateBackInit(&bstrm, ZT_IB_WINDOW, window);
    if (rc != Z_OK) {
        FreeMem(window, ZT_IB_WINDOW_SZ);
        zt_note("InflateBackInit", FALSE, NULL);
        return;
    }
    zt_note("InflateBackInit", TRUE, NULL);

    bstrm.next_in = raw;
    bstrm.avail_in = raw_len;

    rc = InflateBack(&bstrm, zt_ib_in, NULL, zt_ib_out, NULL);
    if (rc != Z_STREAM_END) {
        InflateBackEnd(&bstrm);
        FreeMem(window, ZT_IB_WINDOW_SZ);
        zt_note_zerr("InflateBack", rc);
        return;
    }
    zt_note("InflateBack", TRUE, NULL);

    rc = InflateBackEnd(&bstrm);
    if (rc != Z_OK) {
        FreeMem(window, ZT_IB_WINDOW_SZ);
        zt_note("InflateBackEnd", FALSE, NULL);
        return;
    }
    zt_note("InflateBackEnd", TRUE, NULL);

    /* infback writes decompressed bytes into the caller-supplied window. */
    if (memcmp(window, zt_plain_hello, src_len) != 0) {
        FreeMem(window, ZT_IB_WINDOW_SZ);
        zt_note("InflateBack roundtrip", FALSE, (STRPTR)"window mismatch");
        return;
    }
    zt_note("InflateBack roundtrip", TRUE, NULL);

    FreeMem(window, ZT_IB_WINDOW_SZ);
}

static VOID
zt_test_zlib_inflate(VOID)
{
    z_stream strm;
    UBYTE out[64];
    ULONG out_len;
    LONG rc;

    zt_stream_clear(&strm);
    rc = InflateInit(&strm);
    if (rc != Z_OK) {
        zt_note_zerr("InflateInit(zlib)", rc);
        return;
    }
    if (!zt_inflate_buffer(&strm, zt_zlib_hello, sizeof(zt_zlib_hello),
            out, sizeof(out) - 1, &out_len)) {
        InflateEnd(&strm);
        zt_note("Inflate(zlib)", FALSE, NULL);
        return;
    }
    InflateEnd(&strm);
    out[out_len] = '\0';
    if (strcmp((char *)out, (char *)zt_plain_hello) != 0) {
        zt_note("Inflate(zlib)", FALSE, NULL);
        return;
    }
    zt_note("Inflate(zlib)", TRUE, NULL);
}

static VOID
zt_test_gzip_whole(VOID)
{
    z_stream strm;
    UBYTE out[64];
    ULONG out_len;
    LONG rc;

    zt_stream_clear(&strm);
    rc = InflateInit2(&strm, ZT_GZIP_WINDOW);
    if (rc != Z_OK) {
        zt_note_zerr("InflateInit2(gzip)", rc);
        return;
    }
    if (!zt_inflate_buffer(&strm, zt_gzip_hello, sizeof(zt_gzip_hello),
            out, sizeof(out) - 1, &out_len)) {
        InflateEnd(&strm);
        zt_note("Inflate(gzip whole)", FALSE, NULL);
        return;
    }
    InflateEnd(&strm);
    out[out_len] = '\0';
    if (strcmp((char *)out, (char *)zt_plain_hello) != 0) {
        zt_note("Inflate(gzip whole)", FALSE, NULL);
        return;
    }
    zt_note("Inflate(gzip whole)", TRUE, NULL);
}

static VOID
zt_test_gzip_chunked(VOID)
{
    z_stream strm;
    UBYTE out[64];
    ULONG out_len;
    LONG rc;

    zt_stream_clear(&strm);
    rc = InflateInit2(&strm, ZT_GZIP_WINDOW);
    if (rc != Z_OK) {
        zt_note_zerr("InflateInit2(gzip chunked)", rc);
        return;
    }
    if (!zt_inflate_chunked(&strm, zt_gzip_hello, sizeof(zt_gzip_hello),
            out, sizeof(out) - 1, &out_len, 8)) {
        InflateEnd(&strm);
        zt_note("Inflate(gzip chunked)", FALSE, NULL);
        return;
    }
    InflateEnd(&strm);
    out[out_len] = '\0';
    if (strcmp((char *)out, (char *)zt_plain_hello) != 0) {
        zt_note("Inflate(gzip chunked)", FALSE, NULL);
        return;
    }
    zt_note("Inflate(gzip chunked)", TRUE, NULL);
}

static VOID
zt_test_deflate_gzip_roundtrip(VOID)
{
    z_stream strm;
    UBYTE comp[128];
    UBYTE out[64];
    ULONG src_len;
    ULONG comp_len;
    ULONG out_len;
    LONG rc;

    src_len = (ULONG)strlen((char *)zt_plain_hello);

    zt_stream_clear(&strm);
    rc = DeflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
        ZT_GZIP_WINDOW, 8, Z_DEFAULT_STRATEGY);
    if (rc != Z_OK) {
        zt_note_zerr("DeflateInit2(gzip)", rc);
        return;
    }
    strm.next_in = (UBYTE *)zt_plain_hello;
    strm.avail_in = src_len;
    strm.next_out = comp;
    strm.avail_out = sizeof(comp);
    rc = Deflate(&strm, Z_FINISH);
    comp_len = sizeof(comp) - strm.avail_out;
    DeflateEnd(&strm);
    if (rc != Z_STREAM_END || comp_len < 10) {
        zt_note("Deflate(gzip)", FALSE, NULL);
        return;
    }

    zt_stream_clear(&strm);
    rc = InflateInit2(&strm, ZT_GZIP_WINDOW);
    if (rc != Z_OK) {
        zt_note_zerr("InflateInit2(after deflate)", rc);
        return;
    }
    if (!zt_inflate_buffer(&strm, comp, comp_len, out, sizeof(out) - 1, &out_len)) {
        InflateEnd(&strm);
        zt_note("Deflate/Inflate(gzip)", FALSE, NULL);
        return;
    }
    InflateEnd(&strm);
    out[out_len] = '\0';
    if (strcmp((char *)out, (char *)zt_plain_hello) != 0) {
        zt_note("Deflate/Inflate(gzip)", FALSE, NULL);
        return;
    }
    zt_note("Deflate/Inflate(gzip)", TRUE, NULL);
}

int
main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    zt_pass = 0;
    zt_fail = 0;

    Printf("ZTest: z.library full API harness\n");
    Printf("ZTest: sizeof(z_stream)=%lu\n", (unsigned long)sizeof(z_stream));
    Flush(Output());

    if (!zt_test_open()) {
        Printf("ZTest: %lu passed, %lu failed (library missing)\n",
            zt_pass, zt_fail);
        return 20;
    }

    zt_test_version();
    zt_test_zerror();
    zt_test_crc32();
    zt_test_adler32();
    zt_test_combine();
    zt_test_compress();
    zt_test_compress_extras();
    zt_test_deflate_init();
    zt_test_deflate_stream_ops();
    zt_test_dictionary();
    zt_test_gzip_header();
    zt_test_raw_prime();
    zt_test_inflate_stream_ops();
    zt_test_inflate_sync();
    zt_test_inflate_back();

    zt_test_zlib_inflate();
    zt_test_gzip_whole();
    zt_test_gzip_chunked();
    zt_test_deflate_gzip_roundtrip();

    CloseLibrary(ZBase);
    ZBase = NULL;

    Printf("ZTest: %lu passed, %lu failed\n", zt_pass, zt_fail);
    Flush(Output());

    if (zt_fail > 0) {
        return 10;
    }
    return 0;
}
