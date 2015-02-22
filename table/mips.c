/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table/mips.h"

struct _mips_instr mips_r_table[] = {
  { "add", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x20, 3 },
  { "addu", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x21, 3 },
  { "and", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x24, 3 },
  { "break", { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x0d, 0 },
  { "div", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_NONE }, 0x1a, 2 },
  { "divu", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_NONE }, 0x1b, 2 },
  { "jalr", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_NONE }, 0x09, 2 },
  { "jr", { MIPS_OP_RS, MIPS_OP_NONE, MIPS_OP_NONE }, 0x08, 1 },
  { "mfhi", { MIPS_OP_RD, MIPS_OP_NONE, MIPS_OP_NONE }, 0x10, 1 },
  { "mflo", { MIPS_OP_RD, MIPS_OP_NONE, MIPS_OP_NONE }, 0x12, 1 },
  { "mthi", { MIPS_OP_RS, MIPS_OP_NONE, MIPS_OP_NONE }, 0x11, 1 },
  { "mtlo", { MIPS_OP_RS, MIPS_OP_NONE, MIPS_OP_NONE }, 0x13, 1 },
  { "mult", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_NONE }, 0x18, 2 },
  { "multu", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_NONE }, 0x19, 2 },
  { "nor", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x27, 3 },
  { "or", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x25, 3 },
  { "sll", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x00, 3 },
  { "sllv", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_RS }, 0x04, 3 },
  { "slt", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x2a, 3 },
  { "sltu", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x2b, 3 },
  { "sra", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x03, 3 },
  { "srav", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_RS }, 0x07, 3 },
  { "srl", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x02, 3 },
  { "srlv", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_RS }, 0x06, 3 },
  { "sub", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x22, 3 },
  { "subu", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x23, 3 },
  { "syscall", { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x0c, 0 },
  { "xor", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x26, 3 },
  { NULL, { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x00, 0 }
};

struct _mips_instr mips_i_table[] = {
  { "addi", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x08, 3 },
  { "addiu", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x09, 3 },
  { "andi", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0c, 3 },
  { "beq", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_LABEL }, 0x04, 3 },
  { "bgez", { MIPS_OP_RS, MIPS_OP_LABEL, MIPS_OP_RT_IS_1 }, 0x01, 2 },
  { "bgtz", { MIPS_OP_RS, MIPS_OP_LABEL, MIPS_OP_RT_IS_0 }, 0x07, 2 },
  { "blez", { MIPS_OP_RS, MIPS_OP_LABEL, MIPS_OP_RT_IS_0 }, 0x06, 2 },
  { "bltz", { MIPS_OP_RS, MIPS_OP_LABEL, MIPS_OP_RT_IS_0 }, 0x01, 2 },
  { "bne", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_LABEL }, 0x05, 3 },
  { "lb", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x20, 2 },
  { "lbu", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x24, 2 },
  { "lh", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x21, 2 },
  { "lhu", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x25, 2 },
  { "lui", { MIPS_OP_RT, MIPS_OP_IMMEDIATE, MIPS_OP_NONE }, 0x0f, 2 },
  { "lw", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x23, 2 },
  { "lwc1", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x31, 2 },
  { "ori", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0d, 3 },
  { "sb", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x28, 2 },
  { "slti", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0a, 3 },
  { "sltiu", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0b, 3 },
  { "sh", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x29, 2 },
  { "sw", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x2b, 2 },
  { "swc1", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x39, 2 },
  { "xori", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0e, 3 },
  { NULL, { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x00, 0 }
};

struct _mips_cop_instr mips_cop_table[] = {
  { "add.s", { MIPS_COP_FD, MIPS_COP_FS, MIPS_COP_FT }, 0x00, 0x10, 3 },
  { "cvt.s.w", { MIPS_COP_FD, MIPS_COP_FS, MIPS_COP_FT }, 0x20, 0x14, 3 },
  { "cvt.w.s", { MIPS_COP_FD, MIPS_COP_FS, MIPS_COP_FT }, 0x24, 0x10, 3 },
  { "div.s", { MIPS_COP_FD, MIPS_COP_FS, MIPS_COP_FT }, 0x03, 0x10, 3 },
  { "mfc1", { MIPS_COP_FT, MIPS_COP_FS, MIPS_OP_NONE }, 0x00, 0x00, 2 },
  { "mov.s", { MIPS_COP_FD, MIPS_COP_FS, MIPS_OP_NONE }, 0x06, 0x10, 2 },
  { "mtc1", { MIPS_COP_FT, MIPS_COP_FS, MIPS_OP_NONE }, 0x00, 0x04, 2 },
  { "mul.s", { MIPS_COP_FD, MIPS_COP_FS, MIPS_COP_FT }, 0x02, 0x10, 3 },
  { "sub.s", { MIPS_COP_FD, MIPS_COP_FS, MIPS_COP_FT }, 0x01, 0x10, 3 },
  { NULL, { MIPS_COP_NONE, MIPS_COP_NONE, MIPS_COP_NONE }, 0x00, 0x00, 0 }
};

