/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#include "table/sparc.h"

struct _table_sparc table_sparc[] =
{
  // Instructions
  { "add",                 0x80000000, OP_REG_REG_REG, 0 },
  { "add",                 0x80002000, OP_REG_SIMM13_REG, 0 },
  { "addcc",               0x80800000, OP_REG_REG_REG, 0 },
  { "addcc",               0x80802000, OP_REG_SIMM13_REG, 0 },
  { "addc",                0x80400000, OP_REG_REG_REG, 0 },
  { "addc",                0x80402000, OP_REG_SIMM13_REG, 0 },
  { "addccc",              0x80c00000, OP_REG_REG_REG, 0 },
  { "addccc",              0x80c02000, OP_REG_SIMM13_REG, 0 },
  { "addxc",               0x80300220, OP_REG_REG_REG, 0 },
  { "addxccc",             0x80300260, OP_REG_REG_REG, 0 },
  { "aes_eround01",        0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_eround23",        0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_dround01",        0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_dround23",        0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_eround01_last",   0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_eround23_last",   0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_dround01_last",   0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_dround23_last",   0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "aes_kexpand1",        0x00000000, OP_FREG_FREG_IMM5_FREG, 0 },
  { "aes_kexpand0",        0x00000000, OP_FREG_FREG_FREG, 0 },
  { "aes_kexpand2",        0x00000000, OP_FREG_FREG_FREG, 0 },
  { "alignaddress",        0x81b00300, OP_REG_REG_REG, 0 },
  { "alignaddress_little", 0x81b00300, OP_REG_REG_REG, 0 },
  { "allclean",            0x85880000, OP_NONE, 0 },
  { "and",                 0x80080000, OP_REG_REG_REG, 0 },
  { "and",                 0x80082000, OP_REG_SIMM13_REG, 0 },
  { "andcc",               0x80880000, OP_REG_REG_REG, 0 },
  { "andcc",               0x80882000, OP_REG_SIMM13_REG, 0 },
  { "andn",                0x80280000, OP_REG_REG_REG, 0 },
  { "andn",                0x80282000, OP_REG_SIMM13_REG, 0 },
  { "andncc",              0x80a80000, OP_REG_REG_REG, 0 },
  { "andncc",              0x80a82000, OP_REG_SIMM13_REG, 0 },
  { "array8",              0x801b0200, OP_REG_REG_REG, 0 },
  { "array16",             0x801b0240, OP_REG_REG_REG, 0 },
  { "array32",             0x801b0260, OP_REG_REG_REG, 0 },
  { "ba",                  0x10800000, OP_BRANCH, 0 },
  { "bn",                  0x00800000, OP_BRANCH, 0 },
  { "bne",                 0x12800000, OP_BRANCH, 0 },
  { "be",                  0x02800000, OP_BRANCH, 0 },
  { "bg",                  0x14800000, OP_BRANCH, 0 },
  { "ble",                 0x04800000, OP_BRANCH, 0 },
  { "bge",                 0x16800000, OP_BRANCH, 0 },
  { "bl",                  0x06800000, OP_BRANCH, 0 },
  { "bgu",                 0x18800000, OP_BRANCH, 0 },
  { "bleu",                0x08800000, OP_BRANCH, 0 },
  { "bcc",                 0x1a800000, OP_BRANCH, 0 },
  { "bcs",                 0x0a800000, OP_BRANCH, 0 },
  { "bpos",                0x1c800000, OP_BRANCH, 0 },
  { "bneg",                0x0c800000, OP_BRANCH, 0 },
  { "bvc",                 0x1e800000, OP_BRANCH, 0 },
  { "bvs",                 0x0e800000, OP_BRANCH, 0 },
  { "bmask",               0x81b00320, OP_REG_REG_REG, 0 },
  { "bmask",               0x81b00980, OP_REG_REG_REG, 0 },
  { "bpa",                 0x10400000, OP_BRANCH_P, 0 },
  { "bpn",                 0x00400000, OP_BRANCH_P, 0 },
  { "bpne",                0x12400000, OP_BRANCH_P, 0 },
  { "bpe",                 0x02400000, OP_BRANCH_P, 0 },
  { "bpg",                 0x14400000, OP_BRANCH_P, 0 },
  { "bple",                0x04400000, OP_BRANCH_P, 0 },
  { "bpge",                0x16400000, OP_BRANCH_P, 0 },
  { "bpl",                 0x06400000, OP_BRANCH_P, 0 },
  { "bpgu",                0x18400000, OP_BRANCH_P, 0 },
  { "bpleu",               0x08400000, OP_BRANCH_P, 0 },
  { "bpcc",                0x1a400000, OP_BRANCH_P, 0 },
  { "bpcs",                0x0a400000, OP_BRANCH_P, 0 },
  { "bppos",               0x1c400000, OP_BRANCH_P, 0 },
  { "bpneg",               0x0c400000, OP_BRANCH_P, 0 },
  { "bpvc",                0x1e400000, OP_BRANCH_P, 0 },
  { "bpvs",                0x0e800000, OP_BRANCH_P, 0 },
  { "brz",                 0x02c00000, OP_BRANCH_P_REG, 0 },
  { "brlez",               0x04c00000, OP_BRANCH_P_REG, 0 },
  { "brlz",                0x06c00000, OP_BRANCH_P_REG, 0 },
  { "brnz",                0x0ac00000, OP_BRANCH_P_REG, 0 },
  { "brgz",                0x0cc00000, OP_BRANCH_P_REG, 0 },
  { "brgez",               0x0ec00000, OP_BRANCH_P_REG, 0 },
  { "call",                0x40000000, OP_CALL, 0 },
  { "camellia_f",          0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "camellia_fl",         0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "camellia_fli",        0x00000000, OP_FREG_FREG_FREG_FREG, 0 },
  { "casa",                0xc0000000, OP_IMM_ASI_REG_REG, 0 },
  { "casxa",               0xc0000000, OP_ASI_REG_REG, 0 },
  { NULL, 0, 0, 0 }
};

uint32_t mask_sparc[] =
{
  0xffffffff,                                         // OP_NONE
  (3 << 30) | (0x3f << 19) | (1 << 13) | (0xff << 5), // OP_REG_REG_REG
  (3 << 30) | (0x3f << 19) | (1 << 13),               // OP_REG_SIMM13_REG
  (3 << 30) | (0x3f << 19) | (0xf << 5),              // OP_FREG_FREG_FREG_FREG
  (3 << 30) | (0x3f << 19) | (0xf << 5),              // OP_FREG_FREG_IMM5_FREG
  (3 << 30) | (0x3f << 19) | (0x1ff << 5),            // OP_FREG_FREG_FREG
  (3 << 30) | (0xf << 25) | (0x7 << 22),              // OP_BRANCH
  (3 << 30) | (0xf << 25) | (0x7 << 22),              // OP_BRANCH_P
  (3 << 30) | (0xf << 25) | (0x7 << 22),              // OP_BRANCH_P_REG
  (1 << 30),                                          // OP_CALL
  (3 << 30) | (1 << 13),                              // OP_IMM_ASI_REG_REG
  (3 << 30) | (1 << 13) | (0xff << 5),                // OP_ASI_REG_REG
};

