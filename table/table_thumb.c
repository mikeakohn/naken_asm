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

#include "table_thumb.h"

struct _table_thumb table_thumb[] =
{
  { "lsl", 0x0000, 0xf800, OP_SHIFT, 2 },
  { "lsr", 0x0800, 0xf800, OP_SHIFT, 2 },
  { "asr", 0x1000, 0xf800, OP_SHIFT, 2 },
  { "add", 0x1800, 0xfa00, OP_ADD_SUB, 2 },
  { "sub", 0x1a00, 0xfa00, OP_ADD_SUB, 2 },
  { "mov", 0x2000, 0xf800, OP_IMM, 2 },
  { "cmp", 0x2800, 0xf800, OP_IMM, 2 },
  { "add", 0x3000, 0xf800, OP_IMM, 2 },
  { "sub", 0x3800, 0xf800, OP_IMM, 2 },
  { "and", 0x4000, 0xffc0, OP_ALU, 2 },
  { "eor", 0x4040, 0xffc0, OP_ALU, 2 },
  { "lsl", 0x4080, 0xffc0, OP_ALU, 2 },
  { "lsr", 0x40c0, 0xffc0, OP_ALU, 2 },
  { "asr", 0x4100, 0xffc0, OP_ALU, 2 },
  { "adc", 0x4140, 0xffc0, OP_ALU, 2 },
  { "sbc", 0x4180, 0xffc0, OP_ALU, 2 },
  { "ror", 0x41c0, 0xffc0, OP_ALU, 2 },
  { "tst", 0x4200, 0xffc0, OP_ALU, 2 },
  { "neg", 0x4240, 0xffc0, OP_ALU, 2 },
  { "cmp", 0x4280, 0xffc0, OP_ALU, 2 },
  { "cmn", 0x42c0, 0xffc0, OP_ALU, 2 },
  { "orr", 0x4300, 0xffc0, OP_ALU, 2 },
  { "mul", 0x4340, 0xffc0, OP_ALU, 2 },
  { "bic", 0x4380, 0xffc0, OP_ALU, 2 },
  { "mvn", 0x43c0, 0xffc0, OP_ALU, 2 },
  { "add", 0x4400, 0xff00, OP_HI, 2 },
  { "cmp", 0x4500, 0xff00, OP_HI, 2 },
  { "mov", 0x4600, 0xff00, OP_HI, 2 },
  { "bx", 0x4700, 0xff00, OP_HI_BX, -1 },  // This is odd.
  { "ldr", 0x4800, 0xf800, OP_PC_RELATIVE_LOAD, -1 },
  { "str", 0x5000, 0xfe00, OP_LOAD_STORE, -1 },
  { "strb", 0x5400, 0xfe00, OP_LOAD_STORE, -1 },
  { "ldr", 0x5800, 0xfe00, OP_LOAD_STORE, -1 },
  { "ldrb", 0x5c00, 0xfe00, OP_LOAD_STORE, -1 },
  { "strh", 0x5200, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, -1 },
  { "ldrh", 0x5a00, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, -1 },
  { "ldsb", 0x5600, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, -1 },
  { "ldsh", 0x5e00, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, -1 },
  { "str", 0x6000, 0xf800, OP_LOAD_STORE_IMM_OFFSET_WORD, -1 },
  { "ldr", 0x6800, 0xf800, OP_LOAD_STORE_IMM_OFFSET_WORD, -1 },
  { "strb", 0x7000, 0xf800, OP_LOAD_STORE_IMM_OFFSET, -1 },
  { "ldrb", 0x7800, 0xf800, OP_LOAD_STORE_IMM_OFFSET, -1 },
  { "strh", 0x8000, 0xf800, OP_LOAD_STORE_IMM_OFFSET_HALF_WORD, -1 },
  { "ldrh", 0x8800, 0xf800, OP_LOAD_STORE_IMM_OFFSET_HALF_WORD, -1 },
  { "str", 0x9000, 0xf800, OP_LOAD_STORE_SP_RELATIVE, -1 },
  { "ldr", 0x9800, 0xf800, OP_LOAD_STORE_SP_RELATIVE, -1 },
  { "add", 0xa000, 0xf000, OP_LOAD_ADDRESS, -1 },
  { "add", 0xb000, 0xfe00, OP_ADD_OFFSET_TO_SP, -1 },
  { "push", 0xb400, 0xfe00, OP_PUSH_POP_REGISTERS, -1 },
  { "pop", 0xbc00, 0xfe00, OP_PUSH_POP_REGISTERS, -1 },
  { "stmia", 0xc000, 0xf800, OP_MULTIPLE_LOAD_STORE, -1 },
  { "ldmia", 0xc800, 0xf800, OP_MULTIPLE_LOAD_STORE, -1 },
  { "beq", 0xd000, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bne", 0xd100, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bcs", 0xd200, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bcc", 0xd300, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bmi", 0xd400, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bpl", 0xd500, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bvs", 0xd600, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bvc", 0xd700, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bhi", 0xd800, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bls", 0xd900, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bge", 0xda00, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "blt", 0xdb00, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "bgt", 0xdc00, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "ble", 0xdd00, 0xff00, OP_CONDITIONAL_BRANCH, -1 },
  { "swi", 0xdf00, 0xff00, OP_SOFTWARE_INTERRUPT, -1 },
  { "b", 0xe000, 0xf800, OP_UNCONDITIONAL_BRANCH, -1 },
  { "bl", 0xf000, 0xf800, OP_LONG_BRANCH_WITH_LINK, -1 },
  { NULL, 0, 0, 0, 0 }
};



