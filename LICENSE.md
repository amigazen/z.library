# z.library — licensing

This repository combines **original amigazen project code** with **vendored
zlib sources**.  The whole tree is distributed under a single licence: the
**zlib License**.  The combined `z.library` binary, static archives, and all
published source incorporate both parts under those terms.

## Summary

| Component | Location | Licence |
|-----------|----------|---------|
| Entire repository | All files in this tree | **zlib License** |

That includes:

- Amiga shared library wrapper (`StartUp.c`, `LibInit.c`, `z_lvo.c`, `malloc.c`, …)
- Public headers and SDK (`SDK/Include_H/`, `SDK/SFD/`, Examples)
- Static and stub build support (`static/`, `zlib.lib`, `zlibstubs.lib`)
- Vendored zlib core (`Source/lib_source/zlib/`)

---

## Copyright holders

- **zlib core:** Copyright (C) 1995–2017 Jean-loup Gailly and Mark Adler
- **amigazen project additions:** Copyright (c) 2026 amigazen project

amigazen project additions are offered under the same zlib License as the upstream
library so that applications, system software, and derivative works can treat
the project as one coherent zlib-licensed package.

A copy of the licence also appears in `Source/lib_source/zlib/zlib.h`.

---

## zlib License

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the
use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

Jean-loup Gailly — jloup@gzip.org  
Mark Adler — madler@alumni.caltech.edu
