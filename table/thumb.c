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

#include "table/thumb.h"

struct _table_thumb table_thumb[] =
{
  { "lsl",   0x0000, 0xf800, OP_SHIFT, VERSION_THUMB, 2 },
  { "lsr",   0x0800, 0xf800, OP_SHIFT, VERSION_THUMB, 2 },
  { "asr",   0x1000, 0xf800, OP_SHIFT, VERSION_THUMB, 2 },
  { "add",   0x1800, 0xfa00, OP_ADD_SUB, VERSION_THUMB, 2 },
  { "sub",   0x1a00, 0xfa00, OP_ADD_SUB, VERSION_THUMB, 2 },
  { "mov",   0x2000, 0xf800, OP_REG_IMM, VERSION_THUMB, 2 },
  { "cmp",   0x2800, 0xf800, OP_REG_IMM, VERSION_THUMB, 2 },
  { "add",   0x3000, 0xf800, OP_REG_IMM, VERSION_THUMB, 2 },
  { "sub",   0x3800, 0xf800, OP_REG_IMM, VERSION_THUMB, 2 },
  { "and",   0x4000, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "eor",   0x4040, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "lsl",   0x4080, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "lsr",   0x40c0, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "asr",   0x4100, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "adc",   0x4140, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "sbc",   0x4180, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "ror",   0x41c0, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "tst",   0x4200, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "neg",   0x4240, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "cmp",   0x4280, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "cmn",   0x42c0, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "orr",   0x4300, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "mul",   0x4340, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "bic",   0x4380, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "mvn",   0x43c0, 0xffc0, OP_ALU, VERSION_THUMB, 2 },
  { "add",   0x4400, 0xff00, OP_HI, VERSION_THUMB, 2 },
  { "cmp",   0x4500, 0xff00, OP_HI, VERSION_THUMB, 2 },
  { "mov",   0x4600, 0xff00, OP_HI, VERSION_THUMB, 2 },
  { "bx",    0x4700, 0xff80, OP_HI_BX, VERSION_THUMB, -1 },  // This is odd.
  { "ldr",   0x4800, 0xf800, OP_PC_RELATIVE_LOAD, VERSION_THUMB, -1 },
  { "str",   0x5000, 0xfe00, OP_LOAD_STORE, VERSION_THUMB, -1 },
  { "strb",  0x5400, 0xfe00, OP_LOAD_STORE, VERSION_THUMB, -1 },
  { "ldr",   0x5800, 0xfe00, OP_LOAD_STORE, VERSION_THUMB, -1 },
  { "ldrb",  0x5c00, 0xfe00, OP_LOAD_STORE, VERSION_THUMB, -1 },
  { "strh",  0x5200, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, VERSION_THUMB, -1 },
  { "ldrh",  0x5a00, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, VERSION_THUMB, -1 },
  { "ldsb",  0x5600, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, VERSION_THUMB, -1 },
  { "ldsh",  0x5e00, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, VERSION_THUMB, -1 },
  { "str",   0x6000, 0xf800, OP_LOAD_STORE_IMM_OFFSET_WORD, VERSION_THUMB, -1 },
  { "ldr",   0x6800, 0xf800, OP_LOAD_STORE_IMM_OFFSET_WORD, VERSION_THUMB, -1 },
  { "strb",  0x7000, 0xf800, OP_LOAD_STORE_IMM_OFFSET, VERSION_THUMB, -1 },
  { "ldrb",  0x7800, 0xf800, OP_LOAD_STORE_IMM_OFFSET, VERSION_THUMB, -1 },
  { "strh",  0x8000, 0xf800, OP_LOAD_STORE_IMM_OFFSET_HALF_WORD, VERSION_THUMB, -1 },
  { "ldrh",  0x8800, 0xf800, OP_LOAD_STORE_IMM_OFFSET_HALF_WORD, VERSION_THUMB, -1 },
  { "str",   0x9000, 0xf800, OP_LOAD_STORE_SP_RELATIVE, VERSION_THUMB, -1 },
  { "ldr",   0x9800, 0xf800, OP_LOAD_STORE_SP_RELATIVE, VERSION_THUMB, -1 },
  { "add",   0xa000, 0xf000, OP_LOAD_ADDRESS, VERSION_THUMB, -1 },
  { "add",   0xb000, 0xfe00, OP_ADD_OFFSET_TO_SP, VERSION_THUMB, -1 },
  { "push",  0xb400, 0xfe00, OP_PUSH_POP_REGISTERS, VERSION_THUMB, -1 },
  { "pop",   0xbc00, 0xfe00, OP_PUSH_POP_REGISTERS, VERSION_THUMB, -1 },
  { "stmia", 0xc000, 0xf800, OP_MULTIPLE_LOAD_STORE, VERSION_THUMB, -1 },
  { "ldmia", 0xc800, 0xf800, OP_MULTIPLE_LOAD_STORE, VERSION_THUMB, -1 },
  { "beq",   0xd000, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bne",   0xd100, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bcs",   0xd200, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bcc",   0xd300, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bmi",   0xd400, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bpl",   0xd500, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bvs",   0xd600, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bvc",   0xd700, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bhi",   0xd800, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bls",   0xd900, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bge",   0xda00, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "blt",   0xdb00, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bgt",   0xdc00, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "ble",   0xdd00, 0xff00, OP_CONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "swi",   0xdf00, 0xff00, OP_SOFTWARE_INTERRUPT, VERSION_THUMB, -1 },
  { "b",     0xe000, 0xf800, OP_UNCONDITIONAL_BRANCH, VERSION_THUMB, -1 },
  { "bl",    0xf000, 0xf800, OP_LONG_BRANCH_WITH_LINK, VERSION_THUMB, -1 },
  // These are the same as ldsh and ldsb
  { "ldrsb", 0x5600, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, VERSION_THUMB, -1 },
  { "ldrsh", 0x5e00, 0xfe00, OP_LOAD_STORE_SIGN_EXT_HALF_WORD, VERSION_THUMB, -1 },
  // These two are the same as add SP, #imm
  { "add",   0xb000, 0xff80, OP_SP_SP_IMM, VERSION_THUMB, -1 },
  { "sub",   0xb080, 0xff80, OP_SP_SP_IMM, VERSION_THUMB, -1 },
  // Thumb 2:
  { "blx",   0x4780, 0xff80, OP_HI_BX, VERSION_THUMB_2, -1 },
  { "sxth",  0xb200, 0xffc0, OP_REG_REG, VERSION_THUMB_2, -1 },
  { "sxtb",  0xb240, 0xffc0, OP_REG_REG, VERSION_THUMB_2, -1 },
  { "uxth",  0xb280, 0xffc0, OP_REG_REG, VERSION_THUMB_2, -1 },
  { "uxtb",  0xb2c0, 0xffc0, OP_REG_REG, VERSION_THUMB_2, -1 },
  { "cpsie", 0xb660, 0xffe0, OP_CPS, VERSION_THUMB_2, -1 },
  { "cpsid", 0xb670, 0xffe0, OP_CPS, VERSION_THUMB_2, -1 },
  { "rev",   0xba00, 0xffc0, OP_REG_REG, VERSION_THUMB_2, -1 },
  { "rev16", 0xba40, 0xffc0, OP_REG_REG, VERSION_THUMB_2, -1 },
  { "revsh", 0xbac0, 0xffc0, OP_REG_REG, VERSION_THUMB_2, -1 },
  { "bkpt",  0xbe00, 0xff00, OP_UINT8, VERSION_THUMB_2, -1 },
  { "nop",   0xbf00, 0xffff, OP_NONE, VERSION_THUMB_2, -1 },
  { "yield", 0xbf10, 0xffff, OP_NONE, VERSION_THUMB_2, -1 },
  { "wfe",   0xbf20, 0xffff, OP_NONE, VERSION_THUMB_2, -1 },
  { "wfi",   0xbf30, 0xffff, OP_NONE, VERSION_THUMB_2, -1 },
  { "sev",   0xbf40, 0xffff, OP_NONE, VERSION_THUMB_2, -1 },
  { "udf",   0xde00, 0xff00, OP_UINT8, VERSION_THUMB_2, -1 },
  { "svc",   0xdf00, 0xff00, OP_UINT8, VERSION_THUMB_2, -1 },
  { "adr",   0xa000, 0xf800, OP_REGISTER_ADDRESS, VERSION_THUMB, -1 },
  { "mov",   0x1c00, 0xf800, OP_REG_LOW, VERSION_THUMB, -1 },
  { "mrs",   0xf3ef, 0xffff, OP_MRS, VERSION_THUMB, -1 },
  { "msr",   0xf380, 0xfff0, OP_MSR, VERSION_THUMB, -1 },
  // dsb
  // dmb
  // isb
 
  { NULL, 0, 0, 0, 0, 0 }
};

struct _special_reg_thumb special_reg_thumb[] =
{
  { "apsr",       0 },
  { "iapsr",      1 },
  { "eapsr",      2 },
  { "xpsr",       3 },
  { "ipsr",       5 },
  { "epsr",       6 },
  { "iepsr",      7 },
  { "msp",        8 },
  { "psp",        9 },
  { "primask",   16 },
  { "basepri",   17 },
  { "faultmask", 19 },
  { "control",   20 },
  { NULL,         0 }
};

