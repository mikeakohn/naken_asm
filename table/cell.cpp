/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include "table/cell.h"

struct _table_cell table_cell[] =
{
  // Load / Store
  { "lqd",    0x34000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "lqx",    0x38800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "lqa",    0x30800000, 0xff800000, OP_RT_ADDRESS,   FLAG_NONE, 0, 0 },
  { "lqr",    0x33800000, 0xff800000, OP_RT_RELATIVE,  FLAG_NONE, 0, 0 },
  { "stqd",   0x24000000, 0xff000000, OP_RT_S10_RA,    FLAG_NONE, 0, 0 },
  { "stqx",   0x28800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "stqa",   0x20800000, 0xff800000, OP_RT_ADDRESS,   FLAG_NONE, 0, 0 },
  { "stqr",   0x23800000, 0xff800000, OP_RT_RELATIVE,  FLAG_NONE, 0, 0 },
  { "cbd",    0x3e800000, 0xffe00000, OP_RT_S7_RA,     FLAG_NONE, 0, 0 },
  { "cbx",    0x3a800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "chd",    0x3ea00000, 0xffe00000, OP_RT_S7_RA,     FLAG_NONE, 0, 0 },
  { "chx",    0x3aa00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cwd",    0x3ec00000, 0xffe00000, OP_RT_S7_RA,     FLAG_NONE, 0, 0 },
  { "cwx",    0x3ac00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cdd",    0x3ee00000, 0xffe00000, OP_RT_S7_RA,     FLAG_NONE, 0, 0 },
  { "cdx",    0x3ae00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  // Constant formation instructions
  { "ilh",    0x41800000, 0xff800000, OP_RT_S16,       FLAG_NONE, 0, 0 },
  { "ilhu",   0x41000000, 0xff800000, OP_RT_U16,       FLAG_NONE, 0, 0 },
  { "il",     0x40800000, 0xff800000, OP_RT_S16,       FLAG_NONE, 0, 0 },
  { "ila",    0x42000000, 0xfe000000, OP_RT_U18,       FLAG_NONE, 0, 0 },
  { "iohl",   0x60800000, 0xff800000, OP_RT_U16,       FLAG_NONE, 0, 0 },
  // Integer and logical instructions
  { "ah",     0x19000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "ahi",    0x1d000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "a",      0x18000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "ai",     0x1c000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "sfh",    0x09000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "sfhi",   0x0d000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "sf",     0x08000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "sfi",    0x0c000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "addx",   0x68000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cg",     0x18400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cgx",    0x68400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "sfx",    0x68200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "bg",     0x08400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "bgx",    0x68600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpy",    0x78800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpyu",   0x79800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpyi",   0x74000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "mpyui",  0x75000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "mpya",   0xc0000000, 0xf0000000, OP_RT_RA_RB_RC,  FLAG_NONE, 0, 0 },
  { "mpyh",   0x78a00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpys",   0x78e00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpyhh",  0x78c00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpyhha", 0x68c00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpyhhu", 0x79c00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "mpyhhau",0x69c00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "clz",    0x54a00000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "cntb",   0x56800000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "fsmb",   0x36c00000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "fsmh",   0x36a00000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "fsm",    0x36800000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "fsmbi",  0x32800000, 0xff800000, OP_RT_U16,       FLAG_NONE, 0, 0 },
  { "gbb",    0x36400000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "gbh",    0x36200000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "gb",     0x36000000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "avgb",   0x1a600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "absdb",  0x0a600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "sumb",   0x4a600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "xsbh",   0x56c00000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "xshw",   0x55c00000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "xswd",   0x54c00000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "and",    0x18200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "andc",   0x58200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "andbi",  0x16000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "andhi",  0x15000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "andi",   0x14000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "or",     0x08200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "orc",    0x59200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "orbi",   0x06000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "orhi",   0x05000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "ori",    0x04000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "orx",    0x3e000000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "xor",    0x48200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "xorbi",  0x46000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "xorhi",  0x45000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "xori",   0x44000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "nand",   0x19200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "nor",    0x09200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "eqv",    0x49200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "selb",   0x80000000, 0xf0000000, OP_RT_RA_RB_RC,  FLAG_NONE, 0, 0 },
  { "shufb",  0xb0000000, 0xf0000000, OP_RT_RA_RB_RC,  FLAG_NONE, 0, 0 },
  // Shift and rotate instructions
  { "shlh",   0x0be00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "shlhi",  0x0fe00000, 0xffe00000, OP_RT_RA_U7,     FLAG_NONE, 0, 0 },
  { "shl",    0x0b600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "shli",   0x0f600000, 0xffe00000, OP_RT_RA_U7,     FLAG_NONE, 0, 0 },
  { "shlqbi", 0x3b600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "shlqbii",0x3f600000, 0xffe00000, OP_RT_RA_U7,     FLAG_NONE, 0, 0 },
  { "shlqby", 0x3be00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "shlqbyi",0x3fe00000, 0xffe00000, OP_RT_RA_U7,     FLAG_NONE, 0, 0 },
  { "shlqbybi",0x39e00000, 0xffe00000,OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "roth",   0x0b800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rothi",  0x0f800000, 0xffe00000, OP_RT_RA_U7,     FLAG_NONE, 0, 0 },
  { "rot",    0x0b000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "roti",   0x0f000000, 0xffe00000, OP_RT_RA_U7,     FLAG_NONE, 0, 0 },
  { "rotqby", 0x3b800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rotqbyi",0x3f800000, 0xffe00000, OP_RT_RA_U7,     FLAG_NONE, 0, 0 },
  { "rotqbybi",0x39800000, 0xffe00000,OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rotqbi", 0x3b000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rotqbii",0x3f000000, 0xffe00000, OP_RT_RA_U7,     FLAG_NONE, 0, 0 },
  { "rothm",  0x0ba00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rothmi", 0x0fa00000, 0xffe00000, OP_RT_RA_S6,     FLAG_NONE, 0, 0 },
  { "rotm",   0x0b200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rotmi",  0x0f200000, 0xffe00000, OP_RT_RA_S6,     FLAG_NONE, 0, 0 },
  { "rotqmby",0x3ba00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rotqmbyi",0x3fa00000, 0xffe00000,OP_RT_RA_S6,     FLAG_NONE, 0, 0 },
  { "rotqmbybi",0x39a00000, 0xffe00000,OP_RT_RA_RB,    FLAG_NONE, 0, 0 },
  { "rotqmbi",0x3b200000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rotqmbii",0x3f200000, 0xffe00000,OP_RT_RA_S6,     FLAG_NONE, 0, 0 },
  { "rotmah", 0x0bc00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rotmahi",0x0fc00000, 0xffe00000, OP_RT_RA_S6,     FLAG_NONE, 0, 0 },
  { "rotma",  0x0b400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "rotmai", 0x0f400000, 0xffe00000, OP_RT_RA_S6,     FLAG_NONE, 0, 0 },
  // Compare, branch, and halt instructions
  { "heq",    0x7b000000, 0xffe00000, OP_RA_RB,        FLAG_NONE, 0, 0 },
  { "heqi",   0x7f000000, 0xff000000, OP_RA_S10,       FLAG_NONE, 0, 0 },
  { "hgt",    0x4b000000, 0xffe00000, OP_RA_RB,        FLAG_NONE, 0, 0 },
  { "hgti",   0x4f000000, 0xff000000, OP_RA_S10,       FLAG_NONE, 0, 0 },
  { "hlgt",   0x5b000000, 0xffe00000, OP_RA_RB,        FLAG_NONE, 0, 0 },
  { "hlgti",  0x5f000000, 0xff000000, OP_RA_S10,       FLAG_NONE, 0, 0 },
  { "ceqb",   0x7a000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "ceqbi",  0x7e000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "ceqh",   0x79000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "ceqhi",  0x7d000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "ceq",    0x78000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "ceqi",   0x7c000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "cgtb",   0x4a000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cgtbi",  0x4e000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "cgth",   0x49000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cgthi",  0x4d000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "cgt",    0x48000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "cgti",   0x4c000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "clgtb",  0x5a000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "clgtbi", 0x5e000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "clgth",  0x59000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "clgthi", 0x5d000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  { "clgt",   0x58000000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "clgti",  0x5c000000, 0xff000000, OP_RT_RA_S10,    FLAG_NONE, 0, 0 },
  // Branch instructions
  { "br",     0x32000000, 0xff800000, OP_BRANCH_RELATIVE, FLAG_NONE, 0, 0 },
  { "bra",    0x30000000, 0xff800000, OP_BRANCH_ABSOLUTE, FLAG_NONE, 0, 0 },
  { "brsl",   0x33000000, 0xff800000, OP_BRANCH_RELATIVE_RT,FLAG_NONE, 0, 0 },
  { "brasl",  0x31000000, 0xff800000, OP_BRANCH_RELATIVE_RT,FLAG_NONE, 0, 0 },
  { "bi",     0x35000000, 0xffe00000, OP_RA,           FLAG_NONE, 0, 0 },
  { "iret",   0x35400000, 0xffe00000, OP_RA,           FLAG_NONE, 0, 0 },
  { "bisled", 0x35600000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "bisl",   0x35200000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "brnz",   0x21000000, 0xff800000, OP_BRANCH_RELATIVE_RT,FLAG_NONE, 0, 0 },
  { "brz",    0x20000000, 0xff800000, OP_BRANCH_RELATIVE_RT,FLAG_NONE, 0, 0 },
  { "brhnz",  0x23000000, 0xff800000, OP_BRANCH_RELATIVE_RT,FLAG_NONE, 0, 0 },
  { "brhz",   0x22000000, 0xff800000, OP_BRANCH_RELATIVE_RT,FLAG_NONE, 0, 0 },
  { "biz",    0x25000000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "binz",   0x25200000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "bihz",   0x25400000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "bihnz",  0x25600000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  // Hint for branch instructions
  { "hbr",    0x35800000, 0xfff00000, OP_HINT_RELATIVE_RO_RA, FLAG_NONE, 0, 0 },
  { "hbrp",   0x35900000, 0xffffffff, OP_NONE,                FLAG_NONE, 0, 0 },
  { "hbra",   0x10000000, 0xfe000000, OP_HINT_ABSOLUTE_RO_I16,FLAG_NONE, 0, 0 },
  { "hbrr",   0x12000000, 0xfe000000, OP_HINT_RELATIVE_RO_I16,FLAG_NONE, 0, 0 },
  // Floating point
  { "fa",     0x58800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "dfa",    0x59800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fs",     0x58a00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "dfs",    0x59a00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fm",     0x58c00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "dfm",    0x59c00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fma",    0xe0000000, 0xf0000000, OP_RT_RA_RB_RC,  FLAG_NONE, 0, 0 },
  { "dfma",   0x6b800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fnms",   0xd0000000, 0xf0000000, OP_RT_RA_RB_RC,  FLAG_NONE, 0, 0 },
  { "dfnms",  0x6bc00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fms",    0xf0000000, 0xf0000000, OP_RT_RA_RB_RC,  FLAG_NONE, 0, 0 },
  { "dfms",   0x6ba00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "dfnma",  0x6be00000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "frest",  0x37000000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "frsqest",0x37200000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "fi",     0x7a800000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "csflt",  0x76800000, 0xffc00000, OP_RT_RA_SCALE155,FLAG_NONE, 0, 0 },
  { "cflts",  0x76000000, 0xffc00000, OP_RT_RA_SCALE173,FLAG_NONE, 0, 0 },
  { "cuflt",  0x76c00000, 0xffc00000, OP_RT_RA_SCALE155,FLAG_NONE, 0, 0 },
  { "cfltu",  0x76400000, 0xffc00000, OP_RT_RA_SCALE173,FLAG_NONE, 0, 0 },
  { "frds",   0x77200000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "fesd",   0x77000000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "dfceq",  0x78600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "dfcmeq", 0x79600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "dfcgt",  0x58600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "dfcmgt", 0x59600000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "dftsv",  0x77e00000, 0xffe00000, OP_RT_RA_U7,     FLAG_NONE, 0, 0 },
  { "fceq",   0x78400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fcmeq",  0x79400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fcgt",   0x58400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fcmgt",  0x59400000, 0xffe00000, OP_RT_RA_RB,     FLAG_NONE, 0, 0 },
  { "fscrwr", 0x77400000, 0xffe00000, OP_RT_RA,        FLAG_NONE, 0, 0 },
  { "fscrrd", 0x73000000, 0xffe00000, OP_RT,           FLAG_NONE, 0, 0 },
  // Stop and signal
  { "stop",   0x00000000, 0xffe00000, OP_U14,          FLAG_NONE, 0, 0 },
  { "stopd",  0x28000000, 0xffe00000, OP_RA_RB_RC,     FLAG_NONE, 0, 0 },
  { "lnop",   0x00200000, 0xffe00000, OP_NONE,         FLAG_NONE, 0, 0 },
  { "nop",    0x40200000, 0xffe00000, OP_RT,           FLAG_NONE, 0, 0 },
  { "sync",   0x00400000, 0xfff00000, OP_NONE,         FLAG_NONE, 0, 0 },
  { "syncc",  0x00500000, 0xfff00000, OP_NONE,         FLAG_NONE, 0, 0 },
  { "dsync",  0x00600000, 0xffe00000, OP_NONE,         FLAG_NONE, 0, 0 },
  { "mfspr",  0x01800000, 0xffe00000, OP_RT_SA,        FLAG_NONE, 0, 0 },
  { "mtspr",  0x21800000, 0xffe00000, OP_SA_RT,        FLAG_NONE, 0, 0 },
  // Read Channel
  { "rdch",   0x01a00000, 0xffe00000, OP_RT_CA,        FLAG_NONE, 0, 0 },
  { "rchcnt", 0x01e00000, 0xffe00000, OP_RT_CA,        FLAG_NONE, 0, 0 },
  { "wrch",   0x21a00000, 0xffe00000, OP_CA_RT,        FLAG_NONE, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

