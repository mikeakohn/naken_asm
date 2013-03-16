#include <stdio.h>
#include <stdlib.h>
#include "table_z80.h"

// http://search.cpan.org/~pscust/Asm-Z80-Table-0.02/lib/Asm/Z80/Table.pm
// http://wikiti.brandonw.net/index.php?title=Z80_Instruction_Set
// http://nemesis.lonestar.org/computers/tandy/software/apps/m4/qd/opcodes.html

struct _table_z80 table_z80[] =
{
  { "adc", 0x88, 0xf8, OP_A_REG8, 4 },
  { "adc", 0xdd8c, 0xdffe, OP_A_REG_IHALF, 8 },
  { "adc", 0xce, 0xff, OP_A_NUMBER8, 7 },
  { "adc", 0x8e, 0xff, OP_A_INDEX_HL, 7 },
  { "adc", 0xdd8e, 0xdfff, OP_A_INDEX, 19 },
  { "adc", 0xed4a, 0xffcf, OP_HL_REG16_2, 15 },

  { "add", 0x80, 0xf8, OP_A_REG8, 4 },
  { "add", 0xdd84, 0xdffe, OP_A_REG_IHALF, 8 },
  { "add", 0xc6, 0xff, OP_A_NUMBER8, 7 },
  { "add", 0x86, 0xff, OP_A_INDEX_HL, 7 },
  { "add", 0xdd86, 0xdfff, OP_A_INDEX, 19 },
  { "add", 0x09, 0xcf, OP_HL_REG16_1, 11 },
  { "add", 0xdd09, 0xdfcf, OP_XY_REG16, 15 },

  { "and", 0xa0, 0xf8, OP_REG8, 4 },
  { "and", 0xdda4, 0xdffe, OP_REG_IHALF, 8 },
  { "and", 0xe6, 0xff, OP_NUMBER8, 7 },
  { "and", 0xc6, 0xff, OP_INDEX_HL, 7 },
  { "and", 0xdda6, 0xdfff, OP_INDEX, 19 },

  { "bit", 0xcb40, 0xffc0, OP_BIT_REG8, 8 },
  { "bit", 0xcb46, 0xffc7, OP_BIT_INDEX_HL, 12 },
  { "bit", 0xddcb, 0xdfff, OP_BIT_INDEX, 20 },

  { "call", 0xcd, 0xff, OP_ADDRESS, 17 },
  { "call", 0xc4, 0xc7, OP_COND_ADDRESS, 10, 7 },
  { "ccf", 0x3f, 0xff, OP_NONE, 4 },
  { "cp", 0xb8, 0xf8, OP_REG8, 4 },
  { "cp", 0xddbc, 0xdffe, OP_REG_IHALF, 8 },
  { "cp", 0xfe, 0xff, OP_NUMBER8, 7 },
  { "cp", 0xbe, 0xff, OP_INDEX_HL, 7 },
  { "cp", 0xddbe, 0xdfff, OP_INDEX, 19 },
  { "cpd", 0xeda9, 0xffff, OP_NONE16, 16 },
  { "cpdr", 0xedb9, 0xffff, OP_NONE16, 16, 5 },
  { "cpi", 0xeda1, 0xffff, OP_NONE16, 16 },
  { "cpir", 0xedb1, 0xffff, OP_NONE16, 16, 5 },
  { "cpl", 0x2f, 0xff, OP_NONE, 4 },

  { "daa", 0x27, 0xff, OP_NONE, 4 },
  { "dec", 0x05, 0xc7, OP_REG8_V2, 4 },
  { "dec", 0xdd25, 0xdff7, OP_REG_IHALF_V2, 8 },
  { "dec", 0xc6, 0xff, OP_INDEX_HL, 11 },
  { "dec", 0xdd35, 0xdfff, OP_INDEX, 19 },
  { "dec", 0x0b, 0xcf, OP_REG16, 6 },
  { "dec", 0xdd2b, 0xdfff, OP_XY, 10 },
  { "di", 0xf3, 0xff, OP_NONE, 4 },
  { "djnz", 0x10, 0xff, OP_NUMBER8, 8, 5 },

  { "ei", 0xfb, 0xff, OP_NONE, 4 },
  { "exx", 0xd9, 0xff, OP_NONE, 4 },

  { "halt", 0x76, 0xff, OP_NONE, 4 },

  //{ "inc", 0x38, OP_REG8 },
  { "ind", 0xedaa, 0xffff, OP_NONE16, 16 },
  { "indr", 0xedba, 0xffff, OP_NONE16, 16, 5 },
  { "ini", 0xeda2, 0xffff, OP_NONE16 },
  { "inir", 0xedb2, 0xffff, OP_NONE16 },

  { "ld", 0x78, 0xf8, OP_A_REG8 },
  { "ld", 0xdd5c, 0xdffe, OP_A_REG_IHALF },
  { "ld", 0xdd7e, 0xdfff, OP_A_INDEX },
  { "ld", 0x3d, 0xff, OP_A_NUMBER8 },
  { "ldir", 0xed80, 0xffff, OP_NONE16 },
  { "lddr", 0xedb8, 0xffff, OP_NONE16 },
  { "ldi", 0xeda0, 0xffff, OP_NONE16 },
  { "ldd", 0xeda8, 0xffff, OP_NONE16 },

  { "neg", 0xed44, 0xffff, OP_NONE16, 8 },
  { "nop", 0x00, 0xff, OP_NONE, 4 },

  { "or", 0xb0, 0xf8, OP_REG8 },
  { "otdr", 0xedbb, 0xffff, OP_NONE16 },
  { "otir", 0xedb3, 0xffff, OP_NONE16 },
  { "outd", 0xedab, 0xffff, OP_NONE16 },
  { "outi", 0xeda3, 0xffff, OP_NONE16 },

  { "ret", 0xc9, 0xff, OP_NONE },
  { "rla", 0x17, 0xff, OP_NONE },
  { "rra", 0x1f, 0xff, OP_NONE },
  { "rlca", 0x07, 0xff, OP_NONE },
  { "rrca", 0x0f, 0xff, OP_NONE },
  { "reti", 0xed4d, 0xffff, OP_NONE16 },
  { "retn", 0xed45, 0xffff, OP_NONE16 },
  { "rld", 0xed6f, 0xffff, OP_NONE16 },
  { "rrd", 0xed67, 0xffff, OP_NONE16 },
  { "rl", 0xcb10, 0xfff8, OP_REG8_CB },
  { "rlc", 0xcb00, 0xfff8, OP_REG8_CB },
  { "rr", 0xcb18, 0xfff8, OP_REG8_CB },
  { "rrc", 0xcb08, 0xfff8, OP_REG8_CB },

  { "stop", 0xdddd, 0xffff, OP_NONE24 },
  { "sla", 0xcb20, 0xfff8, OP_REG8_CB },
  //{ "sli", 0x30, OP_REG8_CB },
  { "sll", 0xcb30, 0xfff8, OP_REG8_CB },
  { "sra", 0xcb28, 0xfff8, OP_REG8_CB },
  { "srl", 0xcb38, 0xfff8, OP_REG8_CB },
  { "sbc", 0x98, 0xf8, OP_A_REG8, 4 },
  { "sbc", 0xdd9c, 0xdffe, OP_A_REG_IHALF, 8 },
  { "sbc", 0xde, 0xff, OP_A_NUMBER8, 7 },
  { "sbc", 0x9e, 0xff, OP_A_INDEX_HL, 7 },
  { "sbc", 0xdd9e, 0xdfff, OP_A_INDEX, 19 },
  { "sbc", 0xed42, 0xffcf, OP_HL_REG16_2, 15 },
  { "scf", 0x37, 0xff, OP_NONE, 4 },

  { "sub", 0x90, 0xf8, OP_REG8, 4 },
  { "sub", 0xdd94, 0xdffe, OP_REG_IHALF, 8 },
  { "sub", 0xd6, 0xff, OP_NUMBER8, 7 },
  { "sub", 0x96, 0xff, OP_INDEX_HL, 7 },
  { "sub", 0xdd96, 0xdfff, OP_INDEX, 19 },

  { "xor", 0xa8, 0xf8, OP_REG8, 4 },
  { "xor", 0xddac, 0xdffe, OP_REG_IHALF, 8 },
  { "xor", 0xee, 0xff, OP_NUMBER8, 7 },
  { "xor", 0xae, 0xff, OP_INDEX_HL, 7 },
  { "xor", 0xddae, 0xdfff, OP_INDEX, 19 },

  { NULL, 0x00, OP_NONE },
};


