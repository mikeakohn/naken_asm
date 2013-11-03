/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include "table_arm.h"

struct _table_arm table_arm[] =
{
  //{ "lsl", 0x02000000, 0x0e000000, OP_ALU, 3, 2 },
  { "and", 0x02000000, 0x0fe00000, OP_ALU, 3, 2 },
  { "eor", 0x02200000, 0x0fe00000, OP_ALU, 3, 2 },
  { "sub", 0x02400000, 0x0fe00000, OP_ALU, 3, 2 },
  { "rsb", 0x02600000, 0x0fe00000, OP_ALU, 3, 2 },
  { "add", 0x02800000, 0x0fe00000, OP_ALU, 3, 2 },
  { "adc", 0x02a00000, 0x0fe00000, OP_ALU, 3, 2 },
  { "sbc", 0x02c00000, 0x0fe00000, OP_ALU, 3, 2 },
  { "rsc", 0x02e00000, 0x0fe00000, OP_ALU, 3, 2 },
  { "tst", 0x03000000, 0x0fe00000, OP_ALU, 3, 2 },
  { "teq", 0x03200000, 0x0fe00000, OP_ALU, 3, 2 },
  { "cmp", 0x03400000, 0x0fe00000, OP_ALU, 3, 2 },
  { "cmn", 0x03600000, 0x0fe00000, OP_ALU, 3, 2 },
  { "orr", 0x03800000, 0x0fe00000, OP_ALU, 3, 2 },
  { "mov", 0x03a00000, 0x0fe00000, OP_ALU, 3, 2 },
  { "bic", 0x03c00000, 0x0fe00000, OP_ALU, 3, 2 },
  { "mvn", 0x03e00000, 0x0fe00000, OP_ALU, 3, 2 },
  { "mul", 0x00000090, 0x0fc000f0, OP_MULTIPLY, 3, -1 },
  { "mla", 0x00200090, 0x0fc000f0, OP_MULTIPLY, 3, -1 },
  { "swap", 0x01000090, 0x0fb00ff0, OP_SWAP, 4, -1 },
  { "mrs", 0x010f0000, 0x0fbf0fff, OP_MRS, 3, -1 },
  { "msr", 0x0129f000, 0x0fbffff0, OP_MSR_ALL, 3, -1 },
  { "msr", 0x0329f000, 0x0fbff000, OP_MSR_FLAG, 3, -1 },
  { "ldr", 0x06100000, 0x0e100000, OP_LDR_STR, 3, -1 },  // LS = 1 load
  { "str", 0x06000000, 0x0e100000, OP_LDR_STR, 3, -1 },  // LS = 0 store
  { "???", 0x06000000, 0x0e000000, OP_UNDEFINED, 3, -1 },  // is this needed?
  { "ldm", 0x08100000, 0x0e100000, OP_LDM_STM, 3, -1 },  // LS = 1 load
  { "stm", 0x08000000, 0x0e100000, OP_LDM_STM, 3, -1 },  // LS = 0 store
  { "b", 0x0a000000, 0x0e000000, OP_BRANCH, 1, -1 },
  { "bx", 0x012fff10, 0x0ffffff0, OP_BRANCH_EXCHANGE, 2, -1 },
  { "swi", 0x0f000000, 0x0f000000, OP_SWI, 3, -1 },
  { NULL, 0, 0, 0, 0, 0 }
};


