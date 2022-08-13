/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table/unsp.h"

// http://vtech.pulkomandy.tk/doku.php?id=instruction_encoding

// Note: In the documentation used here, some opcodes had ? in their
// bitfields. These are 0'd with a mask set to 1.

struct _table_unsp table_unsp[] = {
  { "int off",     0xf140, 0xffff, UNSP_OP_NONE },
  { "int irq",     0xf141, 0xffff, UNSP_OP_NONE },
  { "int fiq",     0xf142, 0xffff, UNSP_OP_NONE },
  { "int irq,fiq", 0xf143, 0xffff, UNSP_OP_NONE },
  { "fir_mov on",  0xf144, 0xffff, UNSP_OP_NONE },
  { "fir_mov off", 0xf145, 0xffff, UNSP_OP_NONE },
  { "irq off",     0xf148, 0xffff, UNSP_OP_NONE },
  { "irq on",      0xf149, 0xffff, UNSP_OP_NONE },
  { "fiq off",     0xf14c, 0xffff, UNSP_OP_NONE },
  { "fiq on",      0xf14e, 0xffff, UNSP_OP_NONE },
  { "break",       0xf160, 0xffff, UNSP_OP_NONE },
  //{ "nop",         0xf165, 0xffff, UNSP_OP_NONE },
  { "call",        0xf040, 0xffc0, UNSP_OP_GOTO },
  { "goto",        0xfe80, 0xffc0, UNSP_OP_GOTO },
  { "mul.ss",      0xf108, 0xf1f8, UNSP_OP_MUL },
  { "mul.us",      0xf008, 0xf1f8, UNSP_OP_MUL },
  //{ "mul.uu",      0xe008, 0xf1f8, UNSP_OP_MUL },
  { "mac.ss",      0xf180, 0xf180, UNSP_OP_MAC },
  { "mac.us",      0xf080, 0xf180, UNSP_OP_MAC },
  //{ "mac.uu",      0xe080, 0xf180, UNSP_OP_MAC },
  { "jb",          0x0e00, 0xff80, UNSP_OP_JMP },
  { "jcc",         0x0e00, 0xff80, UNSP_OP_JMP },
  { "jnae",        0x0e00, 0xff80, UNSP_OP_JMP },
  { "jae",         0x1e00, 0xff80, UNSP_OP_JMP },
  { "jcs",         0x1e00, 0xff80, UNSP_OP_JMP },
  { "jnb",         0x1e00, 0xff80, UNSP_OP_JMP },
  { "jge",         0x2e00, 0xff80, UNSP_OP_JMP },
  { "jsc",         0x2e00, 0xff80, UNSP_OP_JMP },
  { "jnl",         0x2e00, 0xff80, UNSP_OP_JMP },
  { "jl",          0x3e00, 0xff80, UNSP_OP_JMP },
  { "jss",         0x3e00, 0xff80, UNSP_OP_JMP },
  { "jnge",        0x3e00, 0xff80, UNSP_OP_JMP },
  { "jne",         0x4e00, 0xff80, UNSP_OP_JMP },
  { "jnz",         0x4e00, 0xff80, UNSP_OP_JMP },
  { "je",          0x5e00, 0xff80, UNSP_OP_JMP },
  { "jz",          0x5e00, 0xff80, UNSP_OP_JMP },
  { "jpl",         0x6e00, 0xff80, UNSP_OP_JMP },
  { "jmi",         0x7e00, 0xff80, UNSP_OP_JMP },
  { "jbe",         0x8e00, 0xff80, UNSP_OP_JMP },
  { "jna",         0x8e00, 0xff80, UNSP_OP_JMP },
  { "ja",          0x9e00, 0xff80, UNSP_OP_JMP },
  { "jnbe",        0x9e00, 0xff80, UNSP_OP_JMP },
  { "jle",         0xae00, 0xff80, UNSP_OP_JMP },
  { "jng",         0xae00, 0xff80, UNSP_OP_JMP },
  { "jg",          0xbe00, 0xff80, UNSP_OP_JMP },
  { "jnle",        0xbe00, 0xff80, UNSP_OP_JMP },
  { "jvc",         0xce00, 0xff80, UNSP_OP_JMP },
  { "jvs",         0xde00, 0xff80, UNSP_OP_JMP },
  { "jmp",         0xee00, 0xff80, UNSP_OP_JMP },
  { "retf",        0x9a90, 0xffff, UNSP_OP_NONE },
  { "reti",        0x9a98, 0xffff, UNSP_OP_NONE },
  { "pop",         0x9080, 0xf1c0, UNSP_OP_POP },
  { "push",        0xd080, 0xf1c0, UNSP_OP_PUSH },
  { "add",         0x0000, 0xf000, UNSP_OP_ALU },
  { "adc",         0x1000, 0xf000, UNSP_OP_ALU },
  { "sub",         0x2000, 0xf000, UNSP_OP_ALU },
  { "sbc",         0x3000, 0xf000, UNSP_OP_ALU },
  { "cmp",         0x4000, 0xf000, UNSP_OP_ALU },
  { "neg",         0x6000, 0xf000, UNSP_OP_ALU_2 },
  { "xor",         0x8000, 0xf000, UNSP_OP_ALU },
  { "ld",          0x9000, 0xf000, UNSP_OP_ALU_2 },
  { "or",          0xa000, 0xf000, UNSP_OP_ALU },
  { "and",         0xb000, 0xf000, UNSP_OP_ALU },
  { "test",        0xc000, 0xf000, UNSP_OP_ALU },
  { "st",          0xd000, 0xf000, UNSP_OP_ALU_2 },
  { NULL,          0x0000, 0xffff, UNSP_OP_NONE },
};

