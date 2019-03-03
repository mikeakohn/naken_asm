/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include "table/arm.h"

struct _table_arm table_arm[] =
{
  //{ "lsl", 0x02000000, 0x0e000000, OP_ALU, 3, 2 },
  { "bx",    0x012fff10, 0x0ffffff0, OP_BRANCH_EXCHANGE, 2, -1 },
  { "and",   0x00000000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "eor",   0x00200000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "sub",   0x00400000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "rsb",   0x00600000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "add",   0x00800000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "adc",   0x00a00000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "sbc",   0x00c00000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "rsc",   0x00e00000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "tst",   0x01000000, 0x0de00000, OP_ALU_2_N, 3, 2 },
  { "teq",   0x01200000, 0x0de00000, OP_ALU_2_N, 3, 2 },
  { "cmp",   0x01400000, 0x0de00000, OP_ALU_2_N, 3, 2 },
  { "cmn",   0x01600000, 0x0de00000, OP_ALU_2_N, 3, 2 },
  { "orr",   0x01800000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "mov",   0x01a00000, 0x0de00000, OP_ALU_2_D, 3, 2 },
  { "bic",   0x01c00000, 0x0de00000, OP_ALU_3, 3, 2 },
  { "mvn",   0x01e00000, 0x0de00000, OP_ALU_2_D, 3, 2 },
  { "mul",   0x00000090, 0x0fd000f0, OP_MULTIPLY, 3, -1 },
  { "mla",   0x00200090, 0x0fd000f0, OP_MULTIPLY, 3, -1 },
  { "swp",   0x01000090, 0x0fb00ff0, OP_SWAP, 3, -1 },
  { "mrs",   0x010f0000, 0x0fbf0fff, OP_MRS, 3, -1 },
  { "msr",   0x0129f000, 0x0fbffff0, OP_MSR_ALL, 3, -1 },
  { "msr",   0x0329f000, 0x0fbff000, OP_MSR_FLAG, 3, -1 },
  { "ldr",   0x04100000, 0x0c100000, OP_LDR_STR, 3, -1 },  // LS = 1 load
  { "ldrh",  0x04100000, 0x0c100000, OP_LDR_STR, 3, -1 },  // LS = 1 load
  { "ldrsb", 0x00100090, 0x0e100f90, OP_LDR_STR_HB, 3, -1 },
  { "ldrsh", 0x00100090, 0x0e100f90, OP_LDR_STR_HB, 3, -1 },
  { "str",   0x04000000, 0x0c100000, OP_LDR_STR, 3, -1 },  // LS = 0 store
  { "strsb", 0x00000090, 0x0e100f90, OP_LDR_STR_HB, 3, -1 },
  { "strsh", 0x00000090, 0x0e100f90, OP_LDR_STR_HB, 3, -1 },
  { "???",   0x06000000, 0x0e000000, OP_UNDEFINED, 3, -1 },  // is this needed?
  { "ldm",   0x08100000, 0x0e100000, OP_LDM_STM, 3, -1 },  // LS = 1 load
  { "stm",   0x08000000, 0x0e100000, OP_LDM_STM, 3, -1 },  // LS = 0 store
  { "b",     0x0a000000, 0x0f000000, OP_BRANCH, 1, -1 },
  { "bl",    0x0b000000, 0x0f000000, OP_BRANCH, 2, -1 },
  { "swi",   0x0f000000, 0x0f000000, OP_SWI, 3, -1 },
  { NULL, 0, 0, 0, 0, 0 }
};

