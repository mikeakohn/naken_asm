/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table/ps2_ee_vu.h"

struct _table_ps2_ee_vu table_ps2_ee_vu_upper[] =
{
  { "abs", { EE_VU_OP_FT, EE_VU_OP_FS, EE_VU_OP_NONE }, 2, 0x000001fd, 0x060007ff, FLAG_DEST },
  { "add", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000028, 0x0600003f, FLAG_DEST },
  { "addi", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_I }, 3, 0x00000022, 0x061f003f, FLAG_DEST },
  { "addq", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_Q }, 3, 0x00000020, 0x061f003f, FLAG_DEST },
  { "addx", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000000, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "addy", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000001, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "addz", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000002, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "addw", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000003, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "adda", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x000002bc, 0x060007ff, FLAG_DEST },
  { "addai", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_I }, 3, 0x0000023e, 0x061f07ff, FLAG_DEST },
  { "addaq", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_Q }, 3, 0x0000023c, 0x061f07ff, FLAG_DEST },
  { "addax", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000003c, 0x060007ff, FLAG_DEST },
  { "adday", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000003d, 0x060007ff, FLAG_DEST },
  { "addaz", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000003e, 0x060007ff, FLAG_DEST },
  { "addaw", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000003f, 0x060007ff, FLAG_DEST },
  { "clipw", { EE_VU_OP_FS, EE_VU_OP_FT, EE_VU_OP_NONE }, 2, 0x000001ff, 0x060003ff, FLAG_DEST },
  { "ftoi0", { EE_VU_OP_FT, EE_VU_OP_FS, EE_VU_OP_NONE }, 2, 0x0000017c, 0x060003ff, FLAG_DEST },
  { "ftoi4", { EE_VU_OP_FT, EE_VU_OP_FS, EE_VU_OP_NONE }, 2, 0x0000017d, 0x060003ff, FLAG_DEST },
  { "ftoi12", { EE_VU_OP_FT, EE_VU_OP_FS, EE_VU_OP_NONE }, 2, 0x0000017e, 0x060003ff, FLAG_DEST },
  { "ftoi15", { EE_VU_OP_FT, EE_VU_OP_FS, EE_VU_OP_NONE }, 2, 0x0000017f, 0x060003ff, FLAG_DEST },
  { "itof0", { EE_VU_OP_FT, EE_VU_OP_FS, EE_VU_OP_NONE }, 2, 0x0000013c, 0x060003ff, FLAG_DEST },
  { "itof4", { EE_VU_OP_FT, EE_VU_OP_FS, EE_VU_OP_NONE }, 2, 0x0000013d, 0x060003ff, FLAG_DEST },
  { "itof12", { EE_VU_OP_FT, EE_VU_OP_FS, EE_VU_OP_NONE }, 2, 0x0000013e, 0x060003ff, FLAG_DEST },
  { "itof15", { EE_VU_OP_FT, EE_VU_OP_FS, EE_VU_OP_NONE }, 2, 0x0000013f, 0x060003ff, FLAG_DEST },
  { "madd", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000029, 0x0600003f, FLAG_DEST },
  { "maddi", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_I }, 3, 0x00000023, 0x061f003f, FLAG_DEST },
  { "maddq", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_Q }, 3, 0x00000021, 0x061f003f, FLAG_DEST },
  { "maddx", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000008, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "maddy", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000009, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "maddz", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000000a, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "maddw", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000000b, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "madda", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x000002bd, 0x060007ff, FLAG_DEST },
  { "maddai", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_I }, 3, 0x0000023f, 0x061f07ff, FLAG_DEST },
  { "maddaq", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_Q }, 3, 0x0000023d, 0x061f07ff, FLAG_DEST },
  { "maddaq", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_Q }, 3, 0x0000023c, 0x061f07ff, FLAG_DEST },
  { "maddax", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000023c, 0x060007ff, FLAG_DEST },
  { "madday", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000023d, 0x060007ff, FLAG_DEST },
  { "maddaz", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000023e, 0x060007ff, FLAG_DEST },
  { "maddaw", { EE_VU_OP_ACC, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000023f, 0x060007ff, FLAG_DEST },
  { "max", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000002b, 0x0600003f, FLAG_DEST },
  { "maxi", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_I }, 3, 0x0000001d, 0x061f003f, FLAG_DEST },
  { "maxx", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000010, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "maxy", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000011, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "maxz", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000012, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "maxw", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000013, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "mini", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x0000002f, 0x0600003f, FLAG_DEST },
  { "minii", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_I }, 3, 0x0000001f, 0x061f003f, FLAG_DEST },
  { "minix", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000014, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "miniy", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000015, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "miniz", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000016, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "miniw", { EE_VU_OP_FD, EE_VU_OP_FS, EE_VU_OP_FT }, 3, 0x00000017, 0x0600003f, FLAG_DEST | FLAG_BC },
  { "nop", { EE_VU_OP_NONE, EE_VU_OP_NONE, EE_VU_OP_NONE }, 0, 0x000002ff, 0x07ffffff, FLAG_NONE },
  { NULL, { 0, 0, 0 }, 0, 0, 0, 0 }
};

struct _table_ps2_ee_vu table_ps2_ee_vu_lower[] =
{
  { "nop", { EE_VU_OP_NONE, EE_VU_OP_NONE, EE_VU_OP_NONE }, 0, 0x8000033c, 0xffffffff, FLAG_NONE },
  { NULL, { 0, 0, 0 }, 0, 0, 0, 0 }
};

