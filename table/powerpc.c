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

#include "table/powerpc.h"

struct _table_powerpc table_powerpc[] =
{
  { "add",    0x7c000214, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addo",   0x7c000614, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addc",   0x7c000014, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addco",  0x7c000414, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "adde",   0x7c000114, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addeo",  0x7c000514, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addi",   0x38000000, 0xfc000000, OP_R_R_SIMM, FLAG_NONE, 0, 0 },
  { "addic",  0x30000000, 0xfc000000, OP_R_R_SIMM, FLAG_NONE, 0, 0 },
  { "addic",  0x34000000, 0xfc000000, OP_R_R_SIMM, FLAG_REQUIRE_DOT, 0, 0 },
  { "addis",  0x3c000000, 0xfc000000, OP_R_R_SIMM, FLAG_NONE, 0, 0 },
  { "addme",  0x7c0001d4, 0xfc0007fe, OP_R_R, FLAG_DOT, 0, 0 },
  { "addmeo", 0x7c0005d4, 0xfc00fffe, OP_R_R, FLAG_DOT, 0, 0 },
  //{ "blcr", 0x4c000000, 0xfc00f801, OP_BRANCH, 0, 0 },
  //{ "blcrl", 0x4c000001, 0xfc00f801, OP_BRANCH, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

