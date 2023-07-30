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

#include "table/tms1000.h"

struct _table_tms1000 table_tms1000[] =
{
  { "knez",   0x09, 0x0e },
  { "tka",    0x08, 0x08 },
  { "setr",   0x0d, 0x0d },
  { "rstr",   0x0c, 0x0c },
  { "tdo",    0x0a, 0x0a },
  { "clo",    0x0b,   -1 },
  { "tam",    0x03, 0x27 },
  { "tmy",    0x22, 0x22 },
  { "tma",    0x21, 0x21 },
  { "xma",    0x2e, 0x03 },
  { "tamiy",  0x20,   -1 },
  { "tamiyc",   -1, 0x25 },
  { "tamdyn",   -1, 0x24 },
  { "tamza",  0x04, 0x26 },
  { "amaac",  0x25, 0x06 },
  { "saman",  0x27, 0x3c },
  { "imac",   0x28, 0x3e },
  { "dman",   0x2a, 0x07 },
  { "alem",   0x29, 0x01 },
  { "mnea",     -1, 0x00 },
  { "mnez",   0x26, 0x3f },
  //{ "sbit",     -1,   -1 },
  //{ "rbit",     -1,   -1 },
  //{ "tbit1",    -1,   -1 },
  //{ "tcy",      -1,   -1 },
  //{ "tcmiy",    -1,   -1 },
  //{ "ldx",      -1,   -1 },
  //{ "ldp",      -1,   -1 },
  //{ "alec",     -1,   -1 },
  //{ "ynec",     -1,   -1 },
  { "a2aac",    -1, 0x78 },
  { "a3aac",    -1, 0x74 },
  { "a4aac",    -1, 0x7c },
  { "a5aac",    -1, 0x72 },
  { "a6aac",  0x06, 0x7a },
  { "a7aac",    -1, 0x76 },
  { "a8aac",  0x01, 0x7e },
  { "a9aac",    -1, 0x71 },
  { "a10aac", 0x05, 0x79 },
  { "a11aac",   -1, 0x75 },
  { "a12aac",   -1, 0x7d },
  { "a13aac",   -1, 0x73 },
  { "a14aac",   -1, 0x7b },
  { "retn",   0x0f, 0x0f },
  //{ "br",       -1,   -1 },
  //{ "call",     -1,   -1 },
  { "tay",    0x24, 0x20 },
  { "tya",    0x23, 0x23 },
  { "ynea",   0x02, 0x02 },
  { "cla",    0x2f, 0x7f },
  { "ia",     0x0e,   -1 },
  { "iac",      -1, 0x70 },
  { "dan",    0x07, 0x77 },
  { "iyc",    0x2b, 0x05 },
  { "dyn",    0x2c, 0x04 },
  { "cpaiz",  0x2d, 0x3d },
  { "comx",   0x00,   -1 },
  { "comx",     -1, 0x09 },
  { "comc",     -1, 0x0b },
  { NULL,        0,    0 }
};

int tms1000_reverse_constant[] =
{
  0x0,
  0x8,
  0x4,
  0xc,
  0x2,
  0xa,
  0x6,
  0xe,
  0x1,
  0x9,
  0x5,
  0xd,
  0x3,
  0xb,
  0x7,
  0xf,
};

int tms1000_reverse_bit_address[] =
{
  0x0,
  0x2,
  0x1,
  0x3,
};

uint8_t tms1000_address_to_lsfr[] =
{
  0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x3e,
  0x3d, 0x3b, 0x37, 0x2f, 0x1e, 0x3c, 0x39, 0x33,
  0x27, 0x0e, 0x1d, 0x3a, 0x35, 0x2b, 0x16, 0x2c,
  0x18, 0x30, 0x21, 0x02, 0x05, 0x0b, 0x17, 0x2e,
  0x1c, 0x38, 0x31, 0x23, 0x06, 0x0d, 0x1b, 0x36,
  0x2d, 0x1a, 0x34, 0x29, 0x12, 0x24, 0x08, 0x11,
  0x22, 0x04, 0x09, 0x13, 0x26, 0x0c, 0x19, 0x32,
  0x25, 0x0a, 0x15, 0x2a, 0x14, 0x28, 0x10, 0x20,
};

uint8_t tms1000_lsfr_to_address[] =
{
  0x00, 0x01, 0x1b, 0x02, 0x31, 0x1c, 0x24, 0x03,
  0x2e, 0x32, 0x39, 0x1d, 0x35, 0x25, 0x11, 0x04,
  0x3e, 0x2f, 0x2c, 0x33, 0x3c, 0x3a, 0x16, 0x1e,
  0x18, 0x36, 0x29, 0x26, 0x20, 0x12, 0x0c, 0x05,
  0x3f, 0x1a, 0x30, 0x23, 0x2d, 0x38, 0x34, 0x10,
  0x3d, 0x2b, 0x3b, 0x15, 0x17, 0x28, 0x1f, 0x0b,
  0x19, 0x22, 0x37, 0x0f, 0x2a, 0x14, 0x27, 0x0a,
  0x21, 0x0e, 0x13, 0x09, 0x0d, 0x08, 0x07, 0x06,
};

