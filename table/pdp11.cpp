/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "table/pdp11.h"

struct _table_pdp11 table_pdp11[] =
{
  // [opcode] [src:reg] [dst:reg]
  { "mov" ,  0x1000, 0xf000, OP_DOUBLE },
  { "movb",  0x9000, 0xf000, OP_DOUBLE },
  { "cmp",   0x2000, 0xf000, OP_DOUBLE },
  { "cmpb",  0xa000, 0xf000, OP_DOUBLE },
  { "bit",   0x3000, 0xf000, OP_DOUBLE },
  { "bitb",  0xb000, 0xf000, OP_DOUBLE },
  { "bic",   0x4000, 0xf000, OP_DOUBLE },
  { "bicb",  0xc000, 0xf000, OP_DOUBLE },
  { "bis",   0x5000, 0xf000, OP_DOUBLE },
  { "bisb",  0xd000, 0xf000, OP_DOUBLE },
  { "add",   0x6000, 0xf000, OP_DOUBLE },
  { "sub",   0xe000, 0xf000, OP_DOUBLE },

  // [opcode 15:9] [reg] [src/dst] [reg]
  { "mul",   0x7000, 0xfe00, OP_REG_S },
  { "div",   0x7200, 0xfe00, OP_REG_S },
  { "ash",   0x7400, 0xfe00, OP_REG_S },
  { "ashc",  0x7600, 0xfe00, OP_REG_S },
  { "xor",   0x7800, 0xfe00, OP_REG_D },

  // [opcode 15:6] [src/dst] [reg]
  { "jmp",   0x0040, 0xffc0, OP_SINGLE },
  { "swab",  0x00c0, 0xffc0, OP_SINGLE },
  { "clr",   0x0a00, 0xffc0, OP_SINGLE },
  { "clrb",  0x8a00, 0xffc0, OP_SINGLE },
  { "com",   0x0a40, 0xffc0, OP_SINGLE },
  { "comb",  0x8a40, 0xffc0, OP_SINGLE },
  { "inc",   0x0a80, 0xffc0, OP_SINGLE },
  { "incb",  0x8a80, 0xffc0, OP_SINGLE },
  { "dec",   0x0ac0, 0xffc0, OP_SINGLE },
  { "decb",  0x8ac0, 0xffc0, OP_SINGLE },
  { "neg",   0x0b00, 0xffc0, OP_SINGLE },
  { "negb",  0x8b00, 0xffc0, OP_SINGLE },
  { "adc",   0x0b40, 0xffc0, OP_SINGLE },
  { "adcb",  0x8b40, 0xffc0, OP_SINGLE },
  { "sbc",   0x0b80, 0xffc0, OP_SINGLE },
  { "sbcb",  0x8b80, 0xffc0, OP_SINGLE },
  { "tst",   0x0bc0, 0xffc0, OP_SINGLE },
  { "tstb",  0x8bc0, 0xffc0, OP_SINGLE },
  { "ror",   0x0c00, 0xffc0, OP_SINGLE },
  { "rorb",  0x8c00, 0xffc0, OP_SINGLE },
  { "rol",   0x0c40, 0xffc0, OP_SINGLE },
  { "rolb",  0x8c40, 0xffc0, OP_SINGLE },
  { "asr",   0x0c80, 0xffc0, OP_SINGLE },
  { "asrb",  0x8c80, 0xffc0, OP_SINGLE },
  { "asl",   0x0cc0, 0xffc0, OP_SINGLE },
  { "aslb",  0x8cc0, 0xffc0, OP_SINGLE },
  { "mtps",  0x8d00, 0xffc0, OP_SINGLE },
  { "mfpi",  0x0d40, 0xffc0, OP_SINGLE },
  { "mfpd",  0x8d40, 0xffc0, OP_SINGLE },
  { "mtpi",  0x0d80, 0xffc0, OP_SINGLE },
  { "mtpd",  0x8d80, 0xffc0, OP_SINGLE },
  { "sxt",   0x0dc0, 0xffc0, OP_SINGLE },
  { "mfps",  0x8dc0, 0xffc0, OP_SINGLE },

  // [opcode 15:9] [c] [offset]
  { "br",    0x0100, 0xff00, OP_BRANCH },
  { "bne",   0x0200, 0xff00, OP_BRANCH },
  { "beq",   0x0300, 0xff00, OP_BRANCH },
  { "bge",   0x0400, 0xff00, OP_BRANCH },
  { "blt",   0x0500, 0xff00, OP_BRANCH },
  { "bgt",   0x0600, 0xff00, OP_BRANCH },
  { "ble",   0x0700, 0xff00, OP_BRANCH },
  { "bpl",   0x8000, 0xff00, OP_BRANCH },
  { "bmi",   0x8100, 0xff00, OP_BRANCH },
  { "bhi",   0x8200, 0xff00, OP_BRANCH },
  { "blos",  0x8300, 0xff00, OP_BRANCH },
  { "bvc",   0x8400, 0xff00, OP_BRANCH },
  { "bvs",   0x8500, 0xff00, OP_BRANCH },
  { "bcc",   0x8600, 0xff00, OP_BRANCH },
  { "bhis",  0x8600, 0xff00, OP_BRANCH },
  { "bcs",   0x8700, 0xff00, OP_BRANCH },
  { "blo",   0x8700, 0xff00, OP_BRANCH },

  // [opcode 15:9] [reg] [offset]
  { "sob",   0x7e00, 0xfe00, OP_SUB_BR },

  // [opcode 15:9] [reg] [src] [reg]
  { "jsr",   0x0800, 0xfe00, OP_JSR },

  // [opcode 15:3] [reg]
  { "rts",   0x0080, 0xfff8, OP_REG },

  { "fadd",  0x3d00, 0xfff8, OP_REG },
  { "fsub",  0x3d40, 0xfff8, OP_REG },
  { "fmul",  0x3d80, 0xfff8, OP_REG },
  { "fdiv",  0x3dc0, 0xfff8, OP_REG },

  // [opcode 15:6] [nn]
  { "mark",  0x0d00, 0xffc0, OP_NN },

  // [opcode 15:9] [S] [Operation Code]
  { "emt",   0x8800, 0xff00, OP_S_OPER },
  { "trap",  0x8c00, 0xff00, OP_S_OPER },

  // [opcode 15:0]
  { "rti",   0x0002, 0xffff, OP_NONE },
  { "bpt",   0x0003, 0xffff, OP_NONE },
  { "iot",   0x0004, 0xffff, OP_NONE },
  { "rtt",   0x0006, 0xffff, OP_NONE },

  { "halt",  0x0000, 0xffff, OP_NONE },
  { "wait",  0x0001, 0xffff, OP_NONE },
  { "reset", 0x0005, 0xffff, OP_NONE },
  { "nop",   0x00a0, 0xffff, OP_NONE },
  { "nop",   0x00b0, 0xffff, OP_NONE },

  { "clc",   0x00a1, 0xffff, OP_NONE },
  { "sec",   0x00b1, 0xffff, OP_NONE },
  { "clv",   0x00a2, 0xffff, OP_NONE },
  { "sev",   0x00b2, 0xffff, OP_NONE },
  { "clz",   0x00a4, 0xffff, OP_NONE },
  { "sez",   0x00b4, 0xffff, OP_NONE },
  { "cln",   0x00a8, 0xffff, OP_NONE },
  { "sen",   0x00b8, 0xffff, OP_NONE },
  { "ccc",   0x00af, 0xffff, OP_NONE },
  { "scc",   0x00bf, 0xffff, OP_NONE },

  // [opcode 15:6] [1] [S] [NZVC]
  { "c",     0x00a0, 0xffe0, OP_NZVC },
  { "s",     0x00b0, 0xffe0, OP_NZVC },

  { NULL, 0, 0, OP_NONE },
};

