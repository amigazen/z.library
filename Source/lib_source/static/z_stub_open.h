/*
 * Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 * Amiga z.library integration by amigazen project
 *
 * z_stub_open.h - ensure z.library is open before stub LVO dispatch
 */

#ifndef Z_STUB_OPEN_H
#define Z_STUB_OPEN_H

int z_stub_ensure_open(void);

#endif /* Z_STUB_OPEN_H */
