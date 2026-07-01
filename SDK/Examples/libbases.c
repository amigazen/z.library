/*
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright 2026 amigazen project
 *
 * libbases.c - ZBase and SysBase for SAS/C #pragma libcall dispatch.
 */

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/execbase.h>

struct Library *ZBase;
struct Library *LowLevelBase;
extern struct ExecBase *SysBase;
