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

#include "table/mips.h"

struct _mips_instr mips_r_table[] = {
  { "add", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x20, 3, MIPS_I },
  { "addu", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x21, 3, MIPS_I },
  { "and", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x24, 3, MIPS_I },
  { "break", { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x0d, 0, MIPS_I },
  { "dadd", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x2c, 3, MIPS_III },
  { "daddu", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x2d, 3, MIPS_III },
  { "div", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_NONE }, 0x1a, 2, MIPS_I },
  { "divu", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_NONE }, 0x1b, 2, MIPS_I },
  { "dsll", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0xc8, 3, MIPS_III },
  { "dsll32", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x3c, 3, MIPS_III },
  { "dsllv", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_RS }, 0x14, 3, MIPS_III },
  { "dsra", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x3b, 3, MIPS_III },
  { "dsra32", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x3f, 3, MIPS_III },
  { "dsrav", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_RS }, 0x17, 3, MIPS_III },
  { "dsrl", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x3a, 3, MIPS_III },
  { "dsrl32", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x3e, 3, MIPS_III },
  { "dsrlv", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_RS }, 0x16, 3, MIPS_III },
  { "dsub", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x2e, 3, MIPS_III },
  { "dsubu", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x2f, 3, MIPS_III },
  { "jalr", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_NONE }, 0x09, 2, MIPS_I },
  { "jr", { MIPS_OP_RS, MIPS_OP_NONE, MIPS_OP_NONE }, 0x08, 1, MIPS_I },
  { "mfhi", { MIPS_OP_RD, MIPS_OP_NONE, MIPS_OP_NONE }, 0x10, 1, MIPS_I },
  { "mflo", { MIPS_OP_RD, MIPS_OP_NONE, MIPS_OP_NONE }, 0x12, 1, MIPS_I },
  { "movn", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x0b, 3, MIPS_IV | MIPS_32 },
  { "movz", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x0a, 3, MIPS_IV | MIPS_32 },
  { "mthi", { MIPS_OP_RS, MIPS_OP_NONE, MIPS_OP_NONE }, 0x11, 1, MIPS_I },
  { "mtlo", { MIPS_OP_RS, MIPS_OP_NONE, MIPS_OP_NONE }, 0x13, 1, MIPS_I },
  { "mult", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_NONE }, 0x18, 2, MIPS_I },
  { "multu", { MIPS_OP_RS, MIPS_OP_RT, MIPS_OP_NONE }, 0x19, 2, MIPS_I },
  { "nor", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x27, 3, MIPS_I },
  { "or", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x25, 3, MIPS_I },
  { "sll", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x00, 3, MIPS_I },
  { "sllv", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_RS }, 0x04, 3, MIPS_I },
  { "slt", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x2a, 3, MIPS_I },
  { "sltu", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x2b, 3, MIPS_I },
  { "sra", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x03, 3, MIPS_I },
  { "srav", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_RS }, 0x07, 3, MIPS_I },
  { "srl", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_SA }, 0x02, 3, MIPS_I },
  { "srlv", { MIPS_OP_RD, MIPS_OP_RT, MIPS_OP_RS }, 0x06, 3, MIPS_I },
  { "sub", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x22, 3, MIPS_I },
  { "subu", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x23, 3, MIPS_I },
  { "syscall", { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x0c, 0, MIPS_I },
  { "xor", { MIPS_OP_RD, MIPS_OP_RS, MIPS_OP_RT }, 0x26, 3, MIPS_I },
  { NULL, { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x00, 0 }
};

struct _mips_instr mips_i_table[] = {
  { "addi", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE_SIGNED }, 0x08, 3, MIPS_I },
  { "addiu", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE_SIGNED }, 0x09, 3, MIPS_I },
  { "andi", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0c, 3, MIPS_I },
  { "daddi", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE_SIGNED }, 0x18, 3, MIPS_III },
  { "daddiu", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE_SIGNED }, 0x19, 3, MIPS_III },
  { "lb", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x20, 2, MIPS_I },
  { "lbu", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x24, 2, MIPS_I },
  { "ld", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x37, 2, MIPS_III },
  { "ldl", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x1a, 2, MIPS_III },
  { "ldr", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x1b, 2, MIPS_III },
  { "lh", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x21, 2, MIPS_I },
  { "lhu", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x25, 2, MIPS_I },
  { "lui", { MIPS_OP_RT, MIPS_OP_IMMEDIATE, MIPS_OP_NONE }, 0x0f, 2, MIPS_I },
  { "ll", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x30, 2 },
  { "lw", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x23, 2, MIPS_I },
  { "lwl", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x22, 2, MIPS_I },
  { "lwc1", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x31, 2, MIPS_FPU },
  { "lwr", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x26, 2, MIPS_I },
  { "lwu", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x27, 2, MIPS_III },
  { "ori", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0d, 3, MIPS_I },
  { "pref", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x28, 2, MIPS_IV },
  { "sb", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x28, 2, MIPS_I },
  { "sc", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x38, 2, MIPS_II },
  { "sd", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x3f, 2, MIPS_III },
  { "sdl", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x2c, 2, MIPS_III },
  { "sdr", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x2d, 2, MIPS_III },
  { "sh", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x29, 2, MIPS_I },
  { "slti", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0a, 3, MIPS_I },
  { "sltiu", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0b, 3, MIPS_I },
  { "sw", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x2b, 2, MIPS_I },
  { "swl", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x2a, 2, MIPS_I },
  { "swr", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x2e, 2, MIPS_I },
  { "swc1", { MIPS_OP_RT, MIPS_OP_IMMEDIATE_RS, MIPS_OP_NONE }, 0x39, 2, MIPS_FPU },
  { "xori", { MIPS_OP_RT, MIPS_OP_RS, MIPS_OP_IMMEDIATE }, 0x0e, 3, MIPS_I },
  { NULL, { MIPS_OP_NONE, MIPS_OP_NONE, MIPS_OP_NONE }, 0x00, 0 }
};

struct _mips_branch mips_branch_table[] = {
  { "beq", 0x04, -1, MIPS_I },
  { "beql", 0x14, -1, MIPS_II },
  { "bgez", 0x01, 0x01, MIPS_I },
  { "bgezal", 0x01, 0x11, MIPS_I },
  { "bgezall", 0x01, 0x13, MIPS_II },
  { "bgezl", 0x01, 0x03, MIPS_II },
  { "bgtz", 0x07, 0x00, MIPS_I },
  { "bgtzl", 0x17, 0x00, MIPS_II },
  { "blez", 0x06, 0x00, MIPS_I },
  { "blezl", 0x16, 0x00, MIPS_II },
  { "bltz", 0x01, 0x00, MIPS_I },
  { "bltzal", 0x01, 0x10, MIPS_I },
  { "bltzall", 0x01, 0x12, MIPS_II },
  { "bltzl", 0x01, 0x02, MIPS_II },
  { "bne", 0x05, -1, MIPS_I },
  { "bnel", 0x15, -1, MIPS_II },
  { NULL, 0, 0, 0 }
};

struct _mips_special_instr mips_special_table[] = {
  { "clo", { 1, 0, 0, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x21, SPECIAL_TYPE_REGS },
  { "clz", { 1, 0, 0, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x20, SPECIAL_TYPE_REGS },
  { "ext", { 1, 0, 3, 2 }, 4, FORMAT_SPECIAL3, 0x00, 0x00, SPECIAL_TYPE_BITS },
  { "ins", { 1, 0, 3, 2 }, 4, FORMAT_SPECIAL3, 0x00, 0x04, SPECIAL_TYPE_BITS2 },
  { "madd", { 0, 1, -1, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x00, SPECIAL_TYPE_REGS },
  { "maddu", { 0, 1, -1, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x01, SPECIAL_TYPE_REGS },
  { "msub", { 0, 1, -1, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x04, SPECIAL_TYPE_REGS },
  { "msubu", { 0, 1, -1, -1 }, 2, FORMAT_SPECIAL2, 0x00, 0x05, SPECIAL_TYPE_REGS },
  //{ "movn", { 1, 2, 0, -1 }, 3, FORMAT_SPECIAL0, 0x00, 0x0b, SPECIAL_TYPE_REGS },
  //{ "movz", { 1, 2, 0, -1 }, 3, FORMAT_SPECIAL0, 0x00, 0x0a, SPECIAL_TYPE_REGS },
  { "mul", { 1, 2, 0, -1 }, 3, FORMAT_SPECIAL2, 0x00, 0x02, SPECIAL_TYPE_REGS },
  { "seb", { -1, 1, 0, -1 }, 2, FORMAT_SPECIAL3, 0x10, 0x20, SPECIAL_TYPE_REGS },
  { "seh", { -1, 1, 0, -1 }, 2, FORMAT_SPECIAL3, 0x18, 0x20, SPECIAL_TYPE_REGS },
  { "rotr", { 1, 0, 2, -1 }, 3, FORMAT_SPECIAL0, 0x01, 0x02, SPECIAL_TYPE_SA },
  { "rotrv", { 2, 1, 0, -1 }, 3, FORMAT_SPECIAL0, 0x01, 0x06, SPECIAL_TYPE_REGS },
  { "wsbh", { -1, 1, 0, -1 }, 2, FORMAT_SPECIAL3, 0x02, 0x20, SPECIAL_TYPE_REGS },
  { NULL, { 0, 0, 0, 0 }, 0, 0, 0x00 }
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

struct _mips_no_operands mips_no_operands[] = {
  { "sync.l", 0x0000000f, MIPS_II },
  { "sync", 0x0000000f, MIPS_II },
  { "sync.p", 0x0000040f, MIPS_II },
  { NULL, 0, 0 }
};

