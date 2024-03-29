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

#include <stdlib.h>

#include "table/1802.h"

struct _table_1802 table_1802[] = {
  { "idl",  0x00, 0xff, RCA1802_OP_NONE,   2 },
  { "ldn",  0x00, 0xf0, RCA1802_OP_REG,    2 },
  { "inc",  0x10, 0xf0, RCA1802_OP_REG,    2 },
  { "dec",  0x20, 0xf0, RCA1802_OP_REG,    2 },
  { "br",   0x30, 0xff, RCA1802_OP_BRANCH, 2 },
  { "bq",   0x31, 0xff, RCA1802_OP_BRANCH, 2 },
  { "bz",   0x32, 0xff, RCA1802_OP_BRANCH, 2 },
  { "bdf",  0x33, 0xff, RCA1802_OP_BRANCH, 2 },
  { "b1",   0x34, 0xff, RCA1802_OP_BRANCH, 2 },
  { "b2",   0x35, 0xff, RCA1802_OP_BRANCH, 2 },
  { "b3",   0x36, 0xff, RCA1802_OP_BRANCH, 2 },
  { "b4",   0x37, 0xff, RCA1802_OP_BRANCH, 2 },
  { "skp",  0x38, 0xff, RCA1802_OP_NONE,   2 },
  { "bnq",  0x39, 0xff, RCA1802_OP_BRANCH, 2 },
  { "bnz",  0x3a, 0xff, RCA1802_OP_BRANCH, 2 },
  { "bnf",  0x3b, 0xff, RCA1802_OP_BRANCH, 2 },
  { "bn1",  0x3c, 0xff, RCA1802_OP_BRANCH, 2 },
  { "bn2",  0x3d, 0xff, RCA1802_OP_BRANCH, 2 },
  { "bn3",  0x3e, 0xff, RCA1802_OP_BRANCH, 2 },
  { "bn4",  0x3f, 0xff, RCA1802_OP_BRANCH, 2 },
  { "lda",  0x40, 0xf0, RCA1802_OP_REG,    2 },
  { "str",  0x50, 0xf0, RCA1802_OP_REG,    2 },
  { "irx",  0x60, 0xff, RCA1802_OP_NONE,   2 },
  { "out",  0x60, 0xf8, RCA1802_OP_NUM_1_TO_7, 2 },
  { "inp",  0x68, 0xf8, RCA1802_OP_NUM_1_TO_7, 2 },
  { "ret",  0x70, 0xff, RCA1802_OP_NONE,   2 },
  { "dis",  0x71, 0xff, RCA1802_OP_NONE,   2 },
  { "ldxa", 0x72, 0xff, RCA1802_OP_NONE,   2 },
  { "stxd", 0x73, 0xff, RCA1802_OP_NONE,   2 },
  { "adc",  0x74, 0xff, RCA1802_OP_NONE,   2 },
  { "sdb",  0x75, 0xff, RCA1802_OP_NONE,   2 },
  { "shrc", 0x76, 0xff, RCA1802_OP_NONE,   2 },
  { "smb",  0x77, 0xff, RCA1802_OP_NONE,   2 },
  { "sav",  0x78, 0xff, RCA1802_OP_NONE,   2 },
  { "mark", 0x79, 0xff, RCA1802_OP_NONE,   2 },
  { "req",  0x7a, 0xff, RCA1802_OP_NONE,   2 },
  { "seq",  0x7b, 0xff, RCA1802_OP_NONE,   2 },
  { "adci", 0x7c, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { "sdbi", 0x7d, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { "shlc", 0x7e, 0xff, RCA1802_OP_NONE,   2 },
  { "smbi", 0x7f, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { "glo",  0x80, 0xf0, RCA1802_OP_REG,    2 },
  { "ghi",  0x90, 0xf0, RCA1802_OP_REG,    2 },
  { "plo",  0xa0, 0xf0, RCA1802_OP_REG,    2 },
  { "phi",  0xb0, 0xf0, RCA1802_OP_REG,    2 },
  { "lbr",  0xc0, 0xff, RCA1802_OP_LONG_BRANCH, 3 },
  { "lbq",  0xc1, 0xff, RCA1802_OP_LONG_BRANCH, 3 },
  { "lbz",  0xc2, 0xff, RCA1802_OP_LONG_BRANCH, 3 },
  { "lbdf", 0xc3, 0xff, RCA1802_OP_LONG_BRANCH, 3 },
  { "nop",  0xc4, 0xff, RCA1802_OP_NONE,   3 },
  { "lsnq", 0xc5, 0xff, RCA1802_OP_NONE,   3 },
  { "lsnz", 0xc6, 0xff, RCA1802_OP_NONE,   3 },
  { "lsnf", 0xc7, 0xff, RCA1802_OP_NONE,   3 },
  { "lskp", 0xc8, 0xff, RCA1802_OP_NONE,   2 },
  { "lbnq", 0xc9, 0xff, RCA1802_OP_LONG_BRANCH, 3 },
  { "lbnz", 0xca, 0xff, RCA1802_OP_LONG_BRANCH, 3 },
  { "lbnf", 0xcb, 0xff, RCA1802_OP_LONG_BRANCH, 3 },
  { "lsie", 0xcc, 0xff, RCA1802_OP_NONE,   3 },
  { "lsq",  0xcd, 0xff, RCA1802_OP_NONE,   3 },
  { "lsz",  0xce, 0xff, RCA1802_OP_NONE,   3 },
  { "lsdf", 0xcf, 0xff, RCA1802_OP_NONE,   3 },
  { "sep",  0xd0, 0xf0, RCA1802_OP_REG,    2 },
  { "sex",  0xe0, 0xf0, RCA1802_OP_REG,    2 },
  { "ldx",  0xf0, 0xff, RCA1802_OP_NONE,   2 },
  { "or",   0xf1, 0xff, RCA1802_OP_NONE,   2 },
  { "and",  0xf2, 0xff, RCA1802_OP_NONE,   2 },
  { "xor",  0xf3, 0xff, RCA1802_OP_NONE,   2 },
  { "add",  0xf4, 0xff, RCA1802_OP_NONE,   2 },
  { "sd",   0xf5, 0xff, RCA1802_OP_NONE,   2 },
  { "shr",  0xf6, 0xff, RCA1802_OP_NONE,   2 },
  { "sm",   0xf7, 0xff, RCA1802_OP_NONE,   2 },
  { "ldi",  0xf8, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { "ori",  0xf9, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { "ani",  0xfa, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { "xri",  0xfb, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { "adi",  0xfc, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { "sdi",  0xfd, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { "shl",  0xfe, 0xff, RCA1802_OP_NONE,   2 },
  { "smi",  0xff, 0xff, RCA1802_OP_IMMEDIATE, 2 },
  { NULL,   0x00, 0xff, RCA1802_OP_ILLEGAL, 0 },
};

struct _table_1802 table_1802_16[] = {
  { "rlxa", 0x60, 0xf0, RCA1802_OP_REG,        5 },
  { "rsxd", 0xa0, 0xf0, RCA1802_OP_REG,        5 },
  { "dbnz", 0x20, 0xf0, RCA1802_OP_REG_BRANCH, 5 },
  { "rnx" , 0xb0, 0xf0, RCA1802_OP_REG,        5 },
  { "dadd", 0xf4, 0xff, RCA1802_OP_NONE,       4 },
  { "dadi", 0xfc, 0xff, RCA1802_OP_IMMEDIATE,  4 },
  { "dadc", 0x74, 0xff, RCA1802_OP_NONE,       4 },
  { "daci", 0x7c, 0xff, RCA1802_OP_IMMEDIATE,  4 },
  { "dsm",  0xf7, 0xff, RCA1802_OP_NONE,       4 },
  { "dsmi", 0xff, 0xff, RCA1802_OP_IMMEDIATE,  4 },
  { "dsmb", 0xf4, 0xff, RCA1802_OP_NONE,       4 },
  { "dsbi", 0x7f, 0xff, RCA1802_OP_IMMEDIATE,  4 },
  { "bci",  0x3e, 0xff, RCA1802_OP_BRANCH,     3 },
  { "bxi",  0x3e, 0xff, RCA1802_OP_BRANCH,     3 },
  { "ldc",  0x06, 0xff, RCA1802_OP_NONE,       3 },
  { "gec",  0x08, 0xff, RCA1802_OP_NONE,       3 },
  { "stpc", 0x00, 0xff, RCA1802_OP_NONE,       3 },
  { "dtc",  0x01, 0xff, RCA1802_OP_NONE,       3 },
  { "stm",  0x07, 0xff, RCA1802_OP_NONE,       3 },
  { "scm1", 0x05, 0xff, RCA1802_OP_NONE,       3 },
  { "scm2", 0x03, 0xff, RCA1802_OP_NONE,       3 },
  { "spm1", 0x04, 0xff, RCA1802_OP_NONE,       3 },
  { "spm2", 0x02, 0xff, RCA1802_OP_NONE,       3 },
  { "etq",  0x09, 0xff, RCA1802_OP_NONE,       3 },
  { "xie",  0x0a, 0xff, RCA1802_OP_NONE,       3 },
  { "xid",  0x0b, 0xff, RCA1802_OP_NONE,       3 },
  { "cie",  0x0c, 0xff, RCA1802_OP_NONE,       3 },
  { "cid",  0x0d, 0xff, RCA1802_OP_NONE,       3 },
  { "dsav", 0x76, 0xff, RCA1802_OP_NONE,       6 },
  { "scal", 0x80, 0xf0, RCA1802_OP_REG,       10 },
  { "sret", 0x90, 0xf0, RCA1802_OP_REG,        8 },
  { NULL,   0x00, 0xff, RCA1802_OP_ILLEGAL, 0 },
};

