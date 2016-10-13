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

#include "table/riscv.h"

struct _table_riscv table_riscv[] =
{
  { "lui",     0x00000037, 0x0000007f, OP_U_TYPE,  -1, -1 },
  { "auipic",  0x00000017, 0x0000007f, OP_U_TYPE,  -1, -1 },
  { "jal",     0x0000006f, 0x0000007f, OP_UJ_TYPE, -1, -1 },
  { "jalr",    0x00000067, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "beq",     0x00000063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "bne",     0x00001063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "blt",     0x00004063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "bge",     0x00005063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "bltu",    0x00006063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "bgeu",    0x00007063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "lb",      0x00000003, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "lh",      0x00001003, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "lw",      0x00002003, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "lbu",     0x00004003, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "lhu",     0x00005003, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "sb",      0x00000003, 0x0000707f, OP_S_TYPE,  -1, -1 },
  { "sh",      0x00001003, 0x0000707f, OP_S_TYPE,  -1, -1 },
  { "sw",      0x00002003, 0x0000707f, OP_S_TYPE,  -1, -1 },
  { "addi",    0x00000013, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "slti",    0x00002013, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "sltiu",   0x00003013, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "xori",    0x00004013, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "ori",     0x00006013, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "andi",    0x00007013, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "slli",    0x00001013, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "srli",    0x00005013, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "srai",    0x40005013, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "add",     0x00000033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sub",     0x40000033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sll",     0x00001033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "slt",     0x00002033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sltu",    0x00003033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "xor",     0x00004033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "srl",     0x00005033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sra",     0x40005033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "or",      0x00006033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "and",     0x00007033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "fence",   0x0000000f, 0xf00fffff, OP_R_TYPE,  -1, -1 },
  { "fence.i", 0x0000100f, 0xffffffff, OP_FFFF,    -1, -1 },
  { "scall",   0x00000073, 0xffffffff, OP_FFFF,    -1, -1 },
  { "sbreak",  0x00100073, 0xffffffff, OP_FFFF,    -1, -1 },
  { "rdcycle",   0xc0002073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdcycleh",  0xc8002073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdtime",    0xc0102073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdtimeh",   0xc8102073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdinstret", 0xc0202073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdinstreth",0xc8202073, 0xfffff07f, OP_READ,    -1, -1 },
  { NULL, 0, 0, 0, 0 }
};

