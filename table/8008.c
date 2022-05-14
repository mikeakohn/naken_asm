/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include "table/8008.h"

struct _table_8008 table_8008[] =
{
  { "mov",  0xc0, 0xc0, OP_REG_REG },
  { "mov",  0xf8, 0xf8, OP_M_REG },
  { "mov",  0xc7, 0xc7, OP_REG_M },
  { "mvi",  0x3e, 0xff, OP_M_IMMEDIATE },
  { "mvi",  0x06, 0xc7, OP_REG_IMMEDIATE },
  { "inr",  0x00, 0xc7, OP_DREG_NOT_A },
  { "dcr",  0x01, 0xc7, OP_DREG_NOT_A },
  { "add",  0x87, 0xff, OP_M },
  { "add",  0x80, 0xf8, OP_SREG },
  { "adi",  0x04, 0xff, OP_IMMEDIATE },
  { "adc",  0x8f, 0xff, OP_M },
  { "adc",  0x88, 0xf8, OP_SREG },
  { "aci",  0x0c, 0xff, OP_IMMEDIATE },
  { "sub",  0x97, 0xff, OP_M },
  { "sub",  0x90, 0xf8, OP_SREG },
  { "sui",  0x14, 0xff, OP_IMMEDIATE },
  { "sbb",  0x9f, 0xff, OP_M },
  { "sbb",  0x98, 0xf8, OP_SREG },
  { "sbi",  0x1c, 0xff, OP_IMMEDIATE },
  { "ana",  0xa7, 0xff, OP_M },
  { "ana",  0xa0, 0xf8, OP_SREG },
  { "ani",  0x24, 0xff, OP_IMMEDIATE },
  { "xra",  0xaf, 0xff, OP_M },
  { "xra",  0xa8, 0xf8, OP_SREG },
  { "xri",  0x2c, 0xff, OP_IMMEDIATE },
  { "ora",  0xb7, 0xff, OP_M },
  { "ora",  0xb0, 0xf8, OP_SREG },
  { "ori",  0x34, 0xff, OP_IMMEDIATE },
  { "cmp",  0xbf, 0xff, OP_M },
  { "cmp",  0xb8, 0xf8, OP_SREG },
  { "cpi",  0x3c, 0xff, OP_IMMEDIATE },
  { "rlc",  0x02, 0xff, OP_NONE },
  { "rrc",  0x0a, 0xff, OP_NONE },
  { "ral",  0x12, 0xff, OP_NONE },
  { "rar",  0x1a, 0xff, OP_NONE },
  { "jmp",  0x44, 0xc7, OP_ADDRESS },
  { "jnc",  0x40, 0xff, OP_ADDRESS },
  { "jnz",  0x48, 0xff, OP_ADDRESS },
  { "jp",   0x50, 0xff, OP_ADDRESS },
  { "jpo",  0x58, 0xff, OP_ADDRESS },
  { "jc",   0x60, 0xff, OP_ADDRESS },
  { "jz",   0x68, 0xff, OP_ADDRESS },
  { "jm",   0x70, 0xff, OP_ADDRESS },
  { "jpe",  0x78, 0xff, OP_ADDRESS },
  { "call", 0x46, 0xc7, OP_ADDRESS },
  { "cnc",  0x42, 0xff, OP_ADDRESS },
  { "cnz",  0x4a, 0xff, OP_ADDRESS },
  { "cp",   0x52, 0xff, OP_ADDRESS },
  { "cpo",  0x5a, 0xff, OP_ADDRESS },
  { "cc",   0x62, 0xff, OP_ADDRESS },
  { "cz",   0x6a, 0xff, OP_ADDRESS },
  { "cm",   0x72, 0xff, OP_ADDRESS },
  { "cpe",  0x7a, 0xff, OP_ADDRESS },
  { "ret",  0x07, 0xff, OP_NONE },
  { "rnc",  0x03, 0xff, OP_NONE },
  { "rnz",  0x0b, 0xff, OP_NONE },
  { "rp",   0x13, 0xff, OP_NONE },
  { "rpo",  0x1b, 0xff, OP_NONE },
  { "rc",   0x23, 0xff, OP_NONE },
  { "rz",   0x2b, 0xff, OP_NONE },
  { "rm",   0x33, 0xff, OP_NONE },
  { "rpe",  0x3b, 0xff, OP_NONE },
  { "rst",  0x05, 0xc7, OP_SUB },
  { "in",   0x41, 0xf1, OP_PORT_MMM },
  { "out",  0x41, 0xc1, OP_PORT_MMM_NOT_0 },
  { "hlt", 0x00, 0xff, OP_NONE },
  { "hlt", 0xff, 0xff, OP_NONE },
  { NULL, 0, 0 }
};

