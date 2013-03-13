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

  { "ccf", 0x3f, 0xff, OP_NONE, 4 },

  { "ld", 0x78, 0xf8, OP_A_REG8 },
  { "ld", 0xdd5c, 0xdffe, OP_A_REG_IHALF },
  { "ld", 0xdd7e, 0xdfff, OP_A_INDEX },
  { "ld", 0x3d, 0xff, OP_A_NUMBER8 },

  { "cp", 0xb8, 0xf8, OP_REG8 },
  { "or", 0xb0, 0xf8, OP_REG8 },


  { "nop", 0x00, 0xff, OP_NONE },
  { "exx", 0xd9, 0xff, OP_NONE },
  { "halt", 0x76, 0xff, OP_NONE },
  { "ei", 0xfb, 0xff, OP_NONE },
  { "di", 0xf3, 0xff, OP_NONE },
  { "daa", 0x27, 0xff, OP_NONE },
  { "cpl", 0x2f, 0xff, OP_NONE },
  { "ret", 0xc9, 0xff, OP_NONE },
  { "rla", 0x17, 0xff, OP_NONE },
  { "rra", 0x1f, 0xff, OP_NONE },
  { "rlca", 0x07, 0xff, OP_NONE },
  { "rrca", 0x0f, 0xff, OP_NONE },
  { "ldir", 0xed80, 0xffff, OP_NONE16 },
  { "lddr", 0xedb8, 0xffff, OP_NONE16 },
  { "ldi", 0xeda0, 0xffff, OP_NONE16 },
  { "ldd", 0xeda8, 0xffff, OP_NONE16 },
  { "neg", 0xed44, 0xffff, OP_NONE16 },
  { "ind", 0xedaa, 0xffff, OP_NONE16 },
  { "indr", 0xedba, 0xffff, OP_NONE16 },
  { "ini", 0xeda2, 0xffff, OP_NONE16 },
  { "inir", 0xedb2, 0xffff, OP_NONE16 },
  { "cpd", 0xeda9, 0xffff, OP_NONE16 },
  { "cpdr", 0xedb9, 0xffff, OP_NONE16 },
  { "cpi", 0xeda1, 0xffff, OP_NONE16 },
  { "cpir", 0xedb1, 0xffff, OP_NONE16 },
  { "otdr", 0xedbb, 0xffff, OP_NONE16 },
  { "otir", 0xedb3, 0xffff, OP_NONE16 },
  { "outd", 0xedab, 0xffff, OP_NONE16 },
  { "outi", 0xeda3, 0xffff, OP_NONE16 },
  { "reti", 0xed4d, 0xffff, OP_NONE16 },
  { "retn", 0xed45, 0xffff, OP_NONE16 },
  { "rld", 0xed6f, 0xffff, OP_NONE16 },
  { "rrd", 0xed67, 0xffff, OP_NONE16 },
  { "stop", 0xdddd, 0xffff, OP_NONE24 },

  { "rl", 0xcb10, 0xfff8, OP_REG8_CB },
  { "rlc", 0xcb00, 0xfff8, OP_REG8_CB },
  { "rr", 0xcb18, 0xfff8, OP_REG8_CB },
  { "rrc", 0xcb08, 0xfff8, OP_REG8_CB },
  { "sla", 0xcb20, 0xfff8, OP_REG8_CB },
  //{ "sli", 0x30, OP_REG8_CB },
  { "sll", 0xcb30, 0xfff8, OP_REG8_CB },
  { "sra", 0xcb28, 0xfff8, OP_REG8_CB },
  { "srl", 0xcb38, 0xfff8, OP_REG8_CB },

  //{ "dec", 0x38, OP_REG8 },
  //{ "inc", 0x38, OP_REG8 },

  { "sbc", 0x98, 0xf8, OP_A_REG8, 4 },
  { "sbc", 0xdd9c, 0xdffe, OP_A_REG_IHALF, 8 },
  { "sbc", 0xde, 0xff, OP_A_NUMBER8, 7 },
  { "sbc", 0x9e, 0xff, OP_A_INDEX_HL, 7 },
  { "sbc", 0xdd9e, 0xdfff, OP_A_INDEX, 19 },
  { "sbc", 0xed42, 0xffcf, OP_HL_REG16_2, 15 },
  { "scf", 0x37, 0xff, OP_NONE, 4 },

  { "sub", 0x96, 0xff, OP_INDEX_HL },
  { "sub", 0xd6, 0xff, OP_NUMBER8 },
  { "sub", 0x90, 0xf8, OP_REG8 },

  { "xor", 0xa8, 0xf8, OP_REG8 },
  { NULL, 0x00, OP_NONE },
};


