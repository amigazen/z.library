# z.library build

SAS/C + smake project for AmigaOS 3.2 / 4.x. Layout follows `amihttp.library`
and `CLib39x`: `Source/lib_source` builds the product, `SDK/` ships headers and
the SFD.

## Targets (`Source/lib_source`)

| Command | Output | Purpose |
|---------|--------|---------|
| `smake` | `z.library` | Shared library, full zlib + 41 public LVOs |
| `smake static` | `zlib.lib` | Static deflate/inflate core (`Z_SOLO`, AWeb-style) |
| `smake stubs` | `zlibstubs.lib` | `autoinit_z_base.o` + `z_stubs.o` → LVOs |
| `smake install` | `LIBS:z.library` | Install shared library |
| `smake installstatic` | `LIBS:zlib.lib` | Install static archive |
| `smake headers` | `include:` + `SDK/Include_H` | Publish developer headers |

Requires NDK 3.2+ on `include:` (`clib/compiler-specific.h` is bundled under
`SDK/Include_H/clib/`).

## ABI

LVO order and register conventions match `SDK/SFD/z_lib.sfd` (Fredrik Wikstrom
z.library v2, bias 30). Public types live in `libraries/z.h`; do not include the
internal `Source/lib_source/zlib/zlib.h` from application code unless you link
`zlib.lib` directly.

## zlib source tree

Core imported from `github/AWeb3/Source/zlib` (1.2.11). Supplemental objects
for the full LVO set (`compress.c`, `uncompr.c`, `infback.c`, `crc32.c` with
`crc32_combine_gen` / `crc32_combine_op`) come from zlib 1.2.13.

Internal zlib compiles with `DEF=AMIGA`. The shared library routes allocator
calls through `malloc.c` (Exec pooled memory freed on expunge).

**Do not** `#include <libraries/z.h>` and `zlib/zlib.h` in the same `.c` file.
LVO wrappers (`z_lvo.c`) use internal `zlib.h` only; `StartUp.c` uses
`z_funcs.h` → public `libraries/z.h` for the functable declarations.

## Linking choices

- **Shared:** `LIBS:z.library` + `#include <proto/z.h>` — preferred for system
  software and amihttp gzip decode. Leave `z_stream.zalloc`/`zfree` NULL so
  inflate/deflate use the library memory pool (`malloc.c`).
- **Static:** `LIBS:zlib.lib` + `zlib/zlib.h` — link into a single executable
  (AWeb `z.lib` pattern); no `z.library` at runtime.
- **Stubs:** `LIBS:zlibstubs.lib` + stock zlib headers — legacy sources that
  call `deflate()`; opens `z.library` on first stub call (and at `_INIT`).

## ZTest (`SDK/Examples`)

Standalone smoke harness — build with `smake` in `SDK/Examples` after
`smake install` and `smake headers`. Redirect output to a file on Amiga
(`ZTest >RAM:ZTest.out`) and inspect PASS/FAIL lines before integrating gzip
into amihttp.

## Version

Library identity: version 2, revision 2 (`private/z_build.h`). Reported zlib
release string comes from internal `zlibVersion()` (currently 1.2.11).
