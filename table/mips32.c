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

#include "table/mips32.h"

struct _mips32_instr mips32_r_table[] = {
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

struct _mips32_instr mips32_i_table[] = {
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
  { "ll", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x30, 2 },
  { "lw", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x23, 2 },
  { "lwl", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x22, 2 },
  { "lwc1", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x31, 2 },
  { "lwr", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x26, 2 },
  { "ori", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0d, 3 },
  { "sb", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x28, 2 },
  { "sc", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x38, 2 },
  { "slti", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0a, 3 },
  { "sltiu", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0b, 3 },
  { "sh", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x29, 2 },
  { "sw", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x2b, 2 },
  { "swl", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x2a, 2 },
  { "swr", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x2e, 2 },
  { "swc1", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x39, 2 },
  { "xori", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0e, 3 },
  { NULL, { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x00, 0 }
};

struct _mips32_special_instr mips32_special_table[] = {
  { "clo", { 1, 0, 0, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x21, SPECIAL_TYPE_REGS },
  { "clz", { 1, 0, 0, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x20, SPECIAL_TYPE_REGS },
  { "ext", { 1, 0, 3, 2 }, 4, FORMAT_SPECIAL3, 0x00, 0x00, SPECIAL_TYPE_BITS },
  { "ins", { 1, 0, 3, 2 }, 4, FORMAT_SPECIAL3, 0x00, 0x04, SPECIAL_TYPE_BITS2 },
  { "madd", { 0, 1, -1, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x00, SPECIAL_TYPE_REGS },
  { "maddu", { 0, 1, -1, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x01, SPECIAL_TYPE_REGS },
  { "msub", { 0, 1, -1, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x04, SPECIAL_TYPE_REGS },
  { "msubu", { 0, 1, -1, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x05, SPECIAL_TYPE_REGS },
  { "movn", { 1, 2, 0, -1 }, 3, FORMAT_SPECIAL0, 0x00, 0x0b, SPECIAL_TYPE_REGS },
  { "movz", { 1, 2, 0, -1 }, 3, FORMAT_SPECIAL0, 0x00, 0x0a, SPECIAL_TYPE_REGS },
  { "mul", { 1, 2, 0, -1 }, 3, FORMAT_SPECIAL2, 0x00, 0x02, SPECIAL_TYPE_REGS },
  { "seb", { -1, 1, 0, -1 }, 2, FORMAT_SPECIAL3, 0x10, 0x20, SPECIAL_TYPE_REGS },
  { "seh", { -1, 1, 0, -1 }, 2, FORMAT_SPECIAL3, 0x18, 0x20, SPECIAL_TYPE_REGS },
  { "rotr", { 1, 0, 2, -1 }, 3, FORMAT_SPECIAL0, 0x01, 0x02, SPECIAL_TYPE_SA },
  { "rotrv", { 2, 1, 0, -1 }, 3, FORMAT_SPECIAL0, 0x01, 0x06, SPECIAL_TYPE_REGS },
  { "wsbh", { -1, 1, 0, -1 }, 2, FORMAT_SPECIAL3, 0x02, 0x20, SPECIAL_TYPE_REGS },
  { NULL, { 0, 0, 0, 0 }, 0, 0, 0x00 }
};

struct _mips32_cop_instr mips32_cop_table[] = {
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

