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
  { "add",   0xec000214, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addo",  0xec000614, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addc",  0xec000414, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addco", 0xec000414, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "adde",  0xec000514, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addeo", 0xec000514, 0xfc0007fe, OP_R_R_R, FLAG_DOT, 0, 0 },
  { "addi",  0x38000000, 0xfc000000, OP_R_R_SIMM, FLAG_NONE, 0, 0 },
  { "addic", 0x30000000, 0xfc000000, OP_R_R_SIMM, FLAG_NONE, 0, 0 },
  { "addic", 0x34000000, 0xfc000000, OP_R_R_SIMM, FLAG_REQUIRE_DOT, 0, 0 },
  //{ "blcr", 0x4c000000, 0xfc00f801, OP_BRANCH, 0, 0 },
  //{ "blcrl", 0x4c000001, 0xfc00f801, OP_BRANCH, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

