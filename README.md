# z.library

This is **z.library**, an AmigaOS shared library wrapping [zlib](https://zlib.net/)
behind a compact, Amiga-native C API.  It exposes the full zlib API so applications 
can compress and decompress in memory — without linking a private copy of zlib into every
executable.

The same project also builds **zlib.lib** (static deflate/inflate core) and
**zlibstubs.lib** (legacy `deflate()`/`inflate()` sources routed to the shared library).

## [amigazen project](http://www.amigazen.com)

*A web, suddenly*

*Forty years meditation*

*Minds awaken, free*

**amigazen project** is using modern software development tools and methods to update and rerelease classic Amiga open source software. Projects include a new AWeb, a new Amiga Python 2, and the ToolKit project — a universal SDK for Amiga development. All *amigazen project* releases are guaranteed to build against the ToolKit standard so that anyone can download and begin contributing straightaway without having to tailor the toolchain for their own setup.

The amigazen project philosophy is based on openness:

*Open* to anyone and everyone — *Open* source and free for all — *Open* your mind and create!

PRs for all projects are gratefully received at [GitHub](https://github.com/amigazen/). While the focus now is on classic 68k software, it is intended that all amigazen project releases can be ported to other Amiga-like systems including AROS and MorphOS where feasible.

## About z.library

z.library is a **standalone compression library** for Amiga.  It
performs in-memory deflate/inflate, one-shot `Compress`/`Uncompress`, and
Adler-32 / CRC-32 checksums.  It does **not** provide gzip file I/O (`gzopen`
and friends are not shared library functions) — callers own input and output buffers.

Bootstrap is deliberately minimal:

```c
ZBase = OpenLibrary("z.library", 2L);
```

Internal zlib state uses a custom pooled Exec allocator (`malloc.c`); leave `z_stream.zalloc` and
`z_stream.zfree` NULL so the library routes allocations through its own memory pool management.

## Compression features

### Format support

| Feature | Support | Notes |
|---------|---------|-------|
| Deflate (RFC 1951) | ✅ Full | `DeflateInit` / `DeflateInit2`, all flush modes |
| Zlib wrapper (RFC 1950) | ✅ Full | Default `windowBits` 15 |
| Gzip wrapper (RFC 1952) | ✅ Full | `InflateInit2` with `windowBits = 15+16` |
| Raw deflate | ✅ Full | Negative `windowBits` (e.g. HTTP deflate) |
| Preset dictionary | ✅ Full | `DeflateSetDictionary` / `InflateSetDictionary` |
| Backwards inflate | ✅ Full | `InflateBackInit` / `InflateBack` / `InflateBackEnd` |
| zlib-ng / minizip | ❌ | Classic zlib 1.2.x only |

### API coverage

| Feature | Support | Notes |
|---------|---------|-------|
| Stream deflate/inflate | ✅ Full | 41 public LVOs per `SDK/SFD/z_lib.sfd` |
| One-shot Compress/Uncompress | ✅ Full | Including `Compress2`, `Uncompress2` |
| Checksums | ✅ Full | `Adler32`, `CRC32`, combine helpers |
| gzip header fields | ✅ Full | `DeflateSetHeader`, `InflateGetHeader` |
| Error strings | ✅ Full | `ZError()` |
| Version query | ✅ Full | `ZlibVersion()` → internal 1.2.11 string |

### Memory and concurrency

| Feature | Support | Notes |
|---------|---------|-------|
| Library pooled allocator | ✅ Full | Default when `zalloc`/`zfree` are NULL |
| Caller-supplied allocators | ⚠️ Partial | Supported in API; SAS/C small-data callbacks across the library boundary are not recommended |
| Per-stream state | ✅ Full | Each `z_stream` is independent |
| Thread safety | ⚠️ Partial | Safe when each task owns its streams; no internal locking |

### Error model

zlib return codes apply directly from LVOs:

| Code | Meaning |
|------|---------|
| `Z_OK` (0) | Success, continue |
| `Z_STREAM_END` (1) | End of stream |
| `Z_NEED_DICT` (2) | Preset dictionary required |
| `Z_BUF_ERROR` (-5) | Output buffer full or need more input |
| `Z_DATA_ERROR` (-3) | Corrupt or invalid compressed data |
| `Z_MEM_ERROR` (-4) | Allocation failure |

Use `ZError(rc)` for a human-readable English string.

## Build

From `Source/lib_source/` on Amiga:

```
cd Source/lib_source
smake
smake static
smake install
smake headers
```

Targets:

| Target | Action |
|--------|--------|
| `smake` | Build `z.library` (shared, full zlib + 41 LVOs) |
| `smake static` | Build `zlib.lib` (static deflate/inflate core, `Z_SOLO`) |
| `smake stubs` | Build `zlibstubs.lib` (`autoinit_z_base.o` + `z_stubs.o`) |
| `smake install` | Copy to `LIBS:z.library` and flush |
| `smake installstatic` | Copy to `LIB:zlib.lib` |
| `smake headers` | Install SDK headers to `SDK/Include_H/` and `include:` |
| `smake rebuild` | Clean wrapper + relink |

Building z.library uses **SAS/C** + smake + slink (ToolKit standard).

Example smoke harness:

```
cd SDK/Examples
smake
ZTest >RAM:ZTest.out
```

`ZTest` opens `z.library` and checks CRC/Adler, `Compress`/`Uncompress`, zlib
and gzip `InflateInit2` (whole buffer and chunked), with library-default
allocators. 

## Prerequisites / dependencies

Building z.library requires:

- NDK 3.2 headers on `include:`
- SAS/C + smake + slink + oml (ToolKit standard)

Runtime requirements for applications (Tier 0):

- `z.library` version 2 in `LIBS:`
- Developer headers from `SDK/Include_H/` or `smake headers`

## Contact

- At GitHub https://github.com/amigazen/z.library/
- On the web at http://www.amigazen.com/ (Amiga browser compatible)
- Or email aweb@amigazen.com

## Acknowledgements

zlib is Copyright Jean-loup Gailly and Mark Adler; see [LICENSE.md](LICENSE.md) and
`Source/lib_source/zlib/zlib.h`.

*Amiga* is a trademark of **Amiga Inc**.
