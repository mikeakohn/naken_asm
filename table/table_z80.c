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

#include <stdio.h>
#include <stdlib.h>
#include "table_z80.h"

// http://search.cpan.org/~pscust/Asm-Z80-Table-0.02/lib/Asm/Z80/Table.pm
// http://wikiti.brandonw.net/index.php?title=Z80_Instruction_Set
// http://nemesis.lonestar.org/computers/tandy/software/apps/m4/qd/opcodes.html

struct _table_z80 table_z80[] =
{
  { "adc", 0x88, 0xf8, OP_A_REG8, Z80_ADC, 4 },
  { "adc", 0xdd8c, 0xdffe, OP_A_REG_IHALF, Z80_ADC, 8 },
  { "adc", 0xce, 0xff, OP_A_NUMBER8, Z80_ADC, 7 },
  { "adc", 0x8e, 0xff, OP_A_INDEX_HL, Z80_ADC, 7 },
  { "adc", 0xdd8e, 0xdfff, OP_A_INDEX, Z80_ADC, 19 },
  { "adc", 0xed4a, 0xffcf, OP_HL_REG16_2, Z80_ADC, 15 },

  { "add", 0x80, 0xf8, OP_A_REG8, Z80_ADD, 4 },
  { "add", 0xdd84, 0xdffe, OP_A_REG_IHALF, Z80_ADD, 8 },
  { "add", 0xc6, 0xff, OP_A_NUMBER8, Z80_ADD, 7 },
  { "add", 0x86, 0xff, OP_A_INDEX_HL, Z80_ADD, 7 },
  { "add", 0xdd86, 0xdfff, OP_A_INDEX, Z80_ADD, 19 },
  { "add", 0x09, 0xcf, OP_HL_REG16_1, Z80_ADD, 11 },
  { "add", 0xdd09, 0xdfcf, OP_XY_REG16, Z80_ADD, 15 },

  { "and", 0xa0, 0xf8, OP_REG8, Z80_AND, 4 },
  { "and", 0xdda4, 0xdffe, OP_REG_IHALF, Z80_AND, 8 },
  { "and", 0xe6, 0xff, OP_NUMBER8, Z80_AND, 7 },
  { "and", 0xc6, 0xff, OP_INDEX_HL, Z80_AND, 7 },
  { "and", 0xdda6, 0xdfff, OP_INDEX, Z80_AND, 19 },

  { "bit", 0xcb40, 0xffc0, OP_BIT_REG8, Z80_BIT, 8 },
  { "bit", 0xcb46, 0xffc7, OP_BIT_INDEX_HL, Z80_BIT, 12 },
  { "bit", 0xddcb, 0xdfff, OP_BIT_INDEX, Z80_BIT, 20 },

  { "call", 0xcd, 0xff, OP_ADDRESS, Z80_CALL, 17 },
  { "call", 0xc4, 0xc7, OP_COND_ADDRESS, Z80_CALL, 10, 7 },
  { "ccf", 0x3f, 0xff, OP_NONE, Z80_CCF, 4 },
  { "cp", 0xb8, 0xf8, OP_REG8, Z80_CP, 4 },
  { "cp", 0xddbc, 0xdffe, OP_REG_IHALF, Z80_CP, 8 },
  { "cp", 0xfe, 0xff, OP_NUMBER8, Z80_CP, 7 },
  { "cp", 0xbe, 0xff, OP_INDEX_HL, Z80_CP, 7 },
  { "cp", 0xddbe, 0xdfff, OP_INDEX, Z80_CP, 19 },
  { "cpd", 0xeda9, 0xffff, OP_NONE16, Z80_CPD, 16 },
  { "cpdr", 0xedb9, 0xffff, OP_NONE16, Z80_CPDR, 16, 5 },
  { "cpi", 0xeda1, 0xffff, OP_NONE16, Z80_CPI, 16 },
  { "cpir", 0xedb1, 0xffff, OP_NONE16, Z80_CPIR, 16, 5 },
  { "cpl", 0x2f, 0xff, OP_NONE, Z80_CPL, 4 },

  { "daa", 0x27, 0xff, OP_NONE, Z80_DAA, 4 },
  { "dec", 0x05, 0xc7, OP_REG8_V2, Z80_DEC, 4 },
  { "dec", 0xdd25, 0xdff7, OP_REG_IHALF_V2, Z80_DEC, 8 },
  { "dec", 0xc6, 0xff, OP_INDEX_HL, Z80_DEC, 11 },
  { "dec", 0xdd35, 0xdfff, OP_INDEX, Z80_DEC, 19 },
  { "dec", 0x0b, 0xcf, OP_REG16, Z80_DEC, 6 },
  { "dec", 0xdd2b, 0xdfff, OP_XY, Z80_DEC, 10 },
  { "di", 0xf3, 0xff, OP_NONE, Z80_DI, 4 },
  { "djnz", 0x10, 0xff, OP_NUMBER8, Z80_DJNZ, 8, 5 },

  { "ei", 0xfb, 0xff, OP_NONE, Z80_EI, 4 },
  { "ex", 0xe3, 0xff, OP_INDEX_SP_HL, Z80_EX, 19 },
  { "ex", 0xdde3, 0xdfff, OP_INDEX_SP_XY, Z80_EX, 23 },
  { "ex", 0x08, 0xff, OP_AF_AF_TICK, Z80_EX, 4 },
  { "ex", 0xeb, 0xff, OP_DE_HL, Z80_EX, 4 },
  { "exx", 0xd9, 0xff, OP_NONE, Z80_EXX, 4 },

  { "halt", 0x76, 0xff, OP_NONE, Z80_HALT, 4 },

  { "im", 0xed46, 0xffc7, OP_IM_NUM, Z80_IM, 8 },
  { "in", 0xdb, 0xff, OP_A_INDEX_N, Z80_IN, 11 },
  { "in", 0xed40, 0xffc7, OP_REG8_INDEX_C, Z80_IN, 12 },
  { "in", 0xed70, 0xffff, OP_F_INDEX_C, Z80_IN, 12 },
  { "inc", 0x04, 0xc7, OP_REG8_V2, Z80_INC, 4 },
  { "inc", 0xdd24, 0xdff7, OP_REG_IHALF_V2, Z80_INC, 8 },
  { "inc", 0x34, 0xff, OP_INDEX_HL, Z80_INC, 11 },
  { "inc", 0xdd34, 0xdfff, OP_INDEX, Z80_INC, 23 },
  { "inc", 0x03, 0xcf, OP_REG16, Z80_INC, 6 },
  { "inc", 0xdd23, 0xdfff, OP_XY, Z80_INC, 10 },
  { "ind", 0xedaa, 0xffff, OP_NONE16, Z80_IND, 16 },
  { "indr", 0xedba, 0xffff, OP_NONE16, Z80_INDR, 16, 5 },
  { "ini", 0xeda2, 0xffff, OP_NONE16, Z80_INI, 16 },
  { "inir", 0xedb2, 0xffff, OP_NONE16, Z80_INIR, 16, 5 },

  { "jp", 0xc3, 0xff, OP_ADDRESS, Z80_JP, 10 },
  { "jp", 0xc3, 0xff, OP_INDEX_HL, Z80_JP, 4 },
  { "jp", 0xc3, 0xff, OP_INDEX_XY, Z80_JP, 8 },
  { "jp", 0xc2, 0xc7, OP_COND_ADDRESS, Z80_JP, 10 },
  { "jr", 0x18, 0xff, OP_NUMBER8, Z80_JR, 12 },
  { "jr", 0x20, 0xe7, OP_JR_COND_ADDRESS, Z80_JR, 7, 5 },

  { "ld", 0x40, 0xc0, OP_REG8_REG8, Z80_LD, 4 },
  // { "ld", 0xdd44, 0xdfc6, OP_REG8_REG_IHALF, Z80_LD, 8 },       WTF??
  // { "ld", 0xdd60, 0xdff0, OP_REG_IHALF_REG8, Z80_LD, 8 },       WTF??
  // { "ld", 0xdd64, 0xdf64, OP_REG_IHALF_REG_IHALF, Z80_LD, 8 },  WTF??
  { "ld", 0x06, 0xc7, OP_REG8_NUMBER8, Z80_LD, 7 },
  { "ld", 0x46, 0xc7, OP_REG8_INDEX_HL, Z80_LD, 7 },
  { "ld", 0xdd46, 0xdfc7, OP_REG8_INDEX, Z80_LD, 19 },
  { "ld", 0x70, 0xf8, OP_INDEX_HL_REG8, Z80_LD, 7 },
  { "ld", 0x36, 0xff, OP_INDEX_HL_NUMBER8, Z80_LD, 10 },
  { "ld", 0xdd70, 0xdff8, OP_INDEX_REG8, Z80_LD, 19 },
  { "ld", 0xdd36, 0xdfff, OP_INDEX_NUMBER8, Z80_LD, 19 },
  { "ld", 0x0a, 0xff, OP_A_INDEX_BC, Z80_LD, 7 },
  { "ld", 0x1a, 0xff, OP_A_INDEX_DE, Z80_LD, 7 },
  { "ld", 0x3a, 0xff, OP_A_INDEX_ADDRESS, Z80_LD, 13 },
  { "ld", 0x02, 0xff, OP_INDEX_BC_A, Z80_LD, 7 },
  { "ld", 0x12, 0xff, OP_INDEX_DE_A, Z80_LD, 7 },
  { "ld", 0x32, 0xff, OP_INDEX_ADDRESS_A, Z80_LD, 13 },
  { "ld", 0xed47, 0xfff7, OP_IR_A, Z80_LD, 9 },
  { "ld", 0xed57, 0xfff7, OP_A_IR, Z80_LD, 9 },
  { "ld", 0x01, 0xcf, OP_REG16_ADDRESS, Z80_LD, 10 },
  { "ld", 0xdd21, 0xdfff, OP_XY_ADDRESS, Z80_LD, 14 },
  { "ld", 0x2a, 0xff, OP_HL_INDEX_ADDRESS, Z80_LD, 16 },
  { "ld", 0xed4b, 0xffcf, OP_REG16_INDEX_ADDRESS, Z80_LD, 20 },
  { "ld", 0xdd2a, 0xdfff, OP_XY_INDEX_ADDRESS, Z80_LD, 20 },
  { "ld", 0x22, 0xff, OP_INDEX_ADDRESS_HL, Z80_LD, 16 },
  { "ld", 0xed43, 0xffcf, OP_INDEX_ADDRESS_REG16, Z80_LD, 20 },
  { "ld", 0xdd22, 0xdfff, OP_INDEX_ADDRESS_XY, Z80_LD, 20 },
  { "ld", 0xf9, 0xff, OP_SP_HL, Z80_LD, 6 },
  { "ld", 0xddf9, 0xdfff, OP_SP_XY, Z80_LD, 10 },
  { "ldd", 0xeda8, 0xffff, OP_NONE16, Z80_LDD, 16 },
  { "lddr", 0xedb8, 0xffff, OP_NONE16, Z80_LDDR, 16, 5 },
  { "ldi", 0xeda0, 0xffff, OP_NONE16, Z80_LDI, 16 },
  { "ldir", 0xed80, 0xffff, OP_NONE16, Z80_LDIR, 16, 5 },

  { "neg", 0xed44, 0xffff, OP_NONE16, Z80_NEG, 8 },
  { "nop", 0x00, 0xff, OP_NONE, Z80_NOP, 4 },

  { "or", 0xb0, 0xf8, OP_REG8, Z80_OR, 4 },
  { "or", 0xddb4, 0xdffe, OP_REG_IHALF, Z80_OR, 8 },
  { "or", 0xf6, 0xff, OP_NUMBER8, Z80_OR, 7 },
  { "or", 0xb6, 0xff, OP_INDEX_HL, Z80_OR, 7 },
  { "or", 0xddb6, 0xdfff, OP_INDEX, Z80_OR, 19 },
  { "out", 0xd3, 0xff, OP_INDEX_ADDRESS8_A, Z80_OUT, 11 },
  { "out", 0xed71, 0xffff, OP_INDEX_C_ZERO, Z80_OUT, 12 },
  { "out", 0xed41, 0xffc7, OP_INDEX_C_REG8, Z80_OUT, 12 },
  { "outd", 0xedab, 0xffff, OP_NONE16, Z80_OUTD, 16 },
  { "otdr", 0xedbb, 0xffff, OP_NONE16, Z80_OTDR, 16, 5 },
  { "outi", 0xeda3, 0xffff, OP_NONE16, Z80_OUTI, 16 },
  { "otir", 0xedb3, 0xffff, OP_NONE16, Z80_OTIR, 16, 5 },

  { "pop", 0xc1, 0xcf, OP_REG16P, Z80_POP, 10 },
  { "pop", 0xdde1, 0xdfff, OP_XY, Z80_POP, 14 },
  { "push", 0xc5, 0xcf, OP_REG16P, Z80_PUSH, 11 },
  { "push", 0xdde5, 0xdfff, OP_XY, Z80_PUSH, 15 },

  { "res", 0xcb80, 0xffc0, OP_BIT_REG8, Z80_RES, 8 },
  { "res", 0xcb86, 0xffc7, OP_BIT_INDEX_HL, Z80_RES, 15 },

  { "ret", 0xc9, 0xff, OP_NONE, Z80_RET, 10 },
  { "ret", 0xc0, 0xc7, OP_COND, Z80_RET, 5, 6 },
  { "reti", 0xed4d, 0xffff, OP_NONE16, Z80_RETI, 14 },
  { "retn", 0xed45, 0xffff, OP_NONE16, Z80_RETN, 14 },
  { "rla", 0x17, 0xff, OP_NONE, Z80_RLA, 4 },
  { "rl", 0xcb16, 0xffff, OP_INDEX_HL_CB, Z80_RL, 15 },
  { "rl", 0xcb10, 0xfff8, OP_REG8_CB, Z80_RL, 8 },
  { "rlca", 0x07, 0xff, OP_NONE, Z80_RLCA, 4 },
  { "rlc", 0xcb06, 0xffff, OP_INDEX_HL_CB, Z80_RLC, 15 },
  { "rlc", 0xcb00, 0xfff8, OP_REG8_CB, Z80_RLC, 8 },
  { "rld", 0xed6f, 0xffff, OP_NONE16, Z80_RLD, 18 },
  { "rra", 0x1f, 0xff, OP_NONE, Z80_RRA, 4 },
  { "rr", 0xcb1e, 0xffff, OP_INDEX_HL_CB, Z80_RR, 15 },
  { "rr", 0xcb18, 0xfff8, OP_REG8_CB, Z80_RR,8 },
  { "rrca", 0x0f, 0xff, OP_NONE, Z80_RRCA, 4 },
  { "rrc", 0xcb0e, 0xffff, OP_INDEX_HL_CB, Z80_RRC, 15 },
  { "rrc", 0xcb08, 0xfff8, OP_REG8_CB, Z80_RRC, 8 },
  { "rrd", 0xed67, 0xffff, OP_NONE16, Z80_RRD, 18 },
  { "rst", 0xc7, 0xc7, OP_RESTART_ADDRESS, Z80_RST, 11 },

  { "sbc", 0x98, 0xf8, OP_A_REG8, Z80_SBC, 4 },
  { "sbc", 0xdd9c, 0xdffe, OP_A_REG_IHALF, Z80_SBC, 8 },
  { "sbc", 0xde, 0xff, OP_A_NUMBER8, Z80_SBC, 7 },
  { "sbc", 0x9e, 0xff, OP_A_INDEX_HL, Z80_SBC, 7 },
  { "sbc", 0xdd9e, 0xdfff, OP_A_INDEX, Z80_SBC, 19 },
  { "sbc", 0xed42, 0xffcf, OP_HL_REG16_2, Z80_SBC, 15 },
  { "scf", 0x37, 0xff, OP_NONE, Z80_SCF, 4 },
  { "set", 0xcbc6, 0xffc7, OP_BIT_INDEX_HL, Z80_SET, 15 },
  { "set", 0xcbc0, 0xffc0, OP_BIT_REG8, Z80_SET, 8 },
  { "sla", 0xcb26, 0xffff, OP_INDEX_HL_CB, Z80_SLA, 15 },
  { "sla", 0xcb20, 0xfff8, OP_REG8_CB, Z80_SLA, 8 },
  { "sra", 0xcb2e, 0xffff, OP_INDEX_HL_CB, Z80_SRA, 15 },
  { "sra", 0xcb28, 0xfff8, OP_REG8_CB, Z80_SRA, 8 },
  { "sll", 0xcb36, 0xffff, OP_INDEX_HL_CB, Z80_SLL, 15 },
  { "sll", 0xcb30, 0xfff8, OP_REG8_CB, Z80_SLL, 8 },
  { "srl", 0xcb3e, 0xffff, OP_INDEX_HL_CB, Z80_SRL, 15 },
  { "srl", 0xcb38, 0xfff8, OP_REG8_CB, Z80_SRL, 8 },
  { "stop", 0xdddd, 0xffff, OP_NONE24, Z80_STOP, 0 },  // WTF?
  { "sub", 0x90, 0xf8, OP_REG8, Z80_SUB, 4 },
  { "sub", 0xdd94, 0xdffe, OP_REG_IHALF, Z80_SUB, 8 },
  { "sub", 0xd6, 0xff, OP_NUMBER8, Z80_SUB, 7 },
  { "sub", 0x96, 0xff, OP_INDEX_HL, Z80_SUB, 7 },
  { "sub", 0xdd96, 0xdfff, OP_INDEX, Z80_SUB, 19 },

  { "xor", 0xa8, 0xf8, OP_REG8, Z80_XOR, 4 },
  { "xor", 0xddac, 0xdffe, OP_REG_IHALF, Z80_XOR, 8 },
  { "xor", 0xee, 0xff, OP_NUMBER8, Z80_XOR, 7 },
  { "xor", 0xae, 0xff, OP_INDEX_HL, Z80_XOR, 7 },
  { "xor", 0xddae, 0xdfff, OP_INDEX, Z80_XOR, 19 },

  { NULL, 0x00, OP_NONE, 0, 0, 0 },
};


struct _table_z80 table_z80_4_byte[] =
{
  { "res", 0x86, 0xc7, OP_BIT_INDEX_V2, Z80_RES, 23 },
  { "res", 0x80, 0xc0, OP_BIT_INDEX_REG8, Z80_RES, 23 },
  { "rl", 0x16, 0xff, OP_BIT_INDEX_V2, Z80_RL, 23 },
  { "rl", 0x10, 0xf8, OP_BIT_INDEX_REG8, Z80_RL, 23 },
  { "rlc", 0x06, 0xff, OP_BIT_INDEX_V2, Z80_RLC, 23 },
  { "rlc", 0x00, 0xf8, OP_BIT_INDEX_REG8, Z80_RLC, 23 },
  { "rr", 0x1e, 0xff, OP_BIT_INDEX_V2, Z80_RR, 23 },
  { "rr", 0x18, 0xf8, OP_BIT_INDEX_REG8, Z80_RR, 23 },
  { "rrc", 0x0e, 0xff, OP_BIT_INDEX_V2, Z80_RRC, 23 },
  { "rrc", 0x08, 0xf8, OP_BIT_INDEX_REG8, Z80_RRC, 23 },
  { "set", 0xc6, 0xc7, OP_BIT_INDEX_V2, Z80_SET, 23 },
  { "set", 0xc0, 0xc0, OP_BIT_INDEX_REG8, Z80_SET, 23 },
  { "sla", 0x26, 0xff, OP_BIT_INDEX_V2, Z80_SLA, 23 },
  { "sla", 0x20, 0xf8, OP_BIT_INDEX_REG8, Z80_SLA, 23 },
  { "sra", 0x2e, 0xff, OP_BIT_INDEX_V2, Z80_SRA, 23 },
  { "sra", 0x28, 0xf8, OP_BIT_INDEX_REG8, Z80_SRA, 23 },
  { "sll", 0x36, 0xff, OP_BIT_INDEX_V2, Z80_SLL, 23 },
  { "sll", 0x30, 0xf8, OP_BIT_INDEX_REG8, Z80_SLL, 23 },
  { "srl", 0x3e, 0xff, OP_BIT_INDEX_V2, Z80_SRL, 23 },
  { "srl", 0x38, 0xf8, OP_BIT_INDEX_REG8, Z80_SRL, 23 },
  { NULL, 0x00, OP_NONE },
};

