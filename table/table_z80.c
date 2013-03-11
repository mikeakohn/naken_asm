#include <stdio.h>
#include <stdlib.h>
#include "table_z80.h"

// http://search.cpan.org/~pscust/Asm-Z80-Table-0.02/lib/Asm/Z80/Table.pm
// http://wikiti.brandonw.net/index.php?title=Z80_Instruction_Set
// http://nemesis.lonestar.org/computers/tandy/software/apps/m4/qd/opcodes.html

struct _table_z80 table_z80[] =
{
  { "adc", 0x88, 0xf8, OP_A_REG8 },
  { "add", 0x80, 0xf8, OP_A_REG8 },
  { "ld", 0x78, 0xf8, OP_A_REG8 },
  { "sbc", 0x98, 0xf8, OP_A_REG8 },

  { "and", 0xa0, 0xf8, OP_REG8 },
  { "cp", 0xb8, 0xf8, OP_REG8 },
  { "or", 0xb0, 0xf8, OP_REG8 },
  { "sub", 0x90, 0xf8, OP_REG8 },
  { "xor", 0xa8, 0xf8, OP_REG8 },

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

  { NULL, 0x00, OP_NONE },
};


