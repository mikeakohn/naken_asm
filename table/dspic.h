/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_DSPIC_H
#define NAKEN_ASM_TABLE_DSPIC_H

#include <stdint.h>

#include "common/assembler.h"

#define FLAG_NONE 0x0000
#define FLAG_B 0x0001        // byte
#define FLAG_W 0x0002        // word
#define FLAG_C 0x0004        // carry
#define FLAG_Z 0x0008        // zero
#define FLAG_N 0x0010        // negative
#define FLAG_D 0x0020        // double (32 bit)
#define FLAG_U 0x0040        // unsigned
#define FLAG_SW 0x0080       // signed 16 bit
#define FLAG_SD 0x0100       // signed 32 bit
#define FLAG_UW 0x0200       // unsigned 16 bit
#define FLAG_UD 0x0400       // unsigned 32 bit
#define FLAG_S 0x0800        // signed 16 bit
#define FLAG_SS 0x1000       // 16x16 signed   signed
#define FLAG_SU 0x2000       // 16x16 signed   unsigned
#define FLAG_US 0x4000       // 16x16 unsigned signed
#define FLAG_UU 0x8000       // 16x16 unsigned unsigned
#define FLAG_R 0x10000       // rounded

enum
{
  OP_NONE,
  OP_F,
  OP_F_WREG,
  OP_F_OR_WREG,
  OP_WREG_F,
  OP_ACC,
  OP_ACC_LIT4_WD,
  OP_ACC_LIT6,
  OP_ACC_WB,
  OP_BRA,
  OP_CP0_WS,
  OP_CP_F,
  OP_D_WNS_WND_1,
  OP_D_WNS_WND_2,
  OP_F_BIT4,
  OP_F_BIT4_2,
  OP_F_WND,
  OP_GOTO,
  OP_LIT1,
  OP_LIT10_WN,
  OP_LIT14,
  OP_LIT14_EXPR,
  OP_LIT16_WND,
  OP_LIT8_WND,
  OP_LNK_LIT14,
  OP_POP_D_WND,
  OP_POP_S,
  OP_POP_WD,
  OP_PUSH_D_WNS,
  OP_SS_WB_WS_WND,
  OP_SU_WB_WS_WND,
  OP_US_WB_WS_WND,
  OP_UU_WB_WS_WND,
  OP_SU_WB_LIT5_WND,
  OP_UU_WB_LIT5_WND,
  OP_S_WM_WN,
  OP_U_WM_WN,
  OP_WB_LIT4_WND,
  OP_WB_LIT5,
  OP_WB_LIT5_WD,
  OP_WB_WN,
  OP_WB_WNS_WND,
  OP_WB_WS,
  OP_WB_WS_WD,
  OP_WD,
  OP_WN,
  OP_B_WN,
  OP_DAW_B_WN,
  OP_WN_EXPR,
  OP_WNS_F,
  OP_WNS_WD_LIT10,
  OP_WNS_WND,
  OP_WS_BIT4,
  OP_WS_BIT4_2,
  OP_WS_LIT10_WND,
  OP_WS_LIT4_ACC,
  OP_WS_PLUS_WB,
  OP_WS_WB,
  OP_WS_WB_WD_WB,
  OP_WS_WD,
  OP_WS_WND,
  OP_WM_WN,
  OP_A_WX_WY_AWB,
  OP_N_WM_WN_ACC_WX_WY,
  OP_WM_WM_ACC_WX_WY,
  OP_WM_WN_ACC_WX_WY,
  OP_WM_WM_ACC_WX_WY_WXD,
  OP_WM_WN_ACC_WX_WY_AWB,
};

struct _table_dspic
{
  char *name;
  uint32_t opcode;
  //uint32_t opcode48;
  uint32_t mask;
  //uint32_t mask48;
  uint8_t type;
  uint8_t cycles_min;
  uint8_t cycles_max;
  uint8_t dspic;
};

extern struct _table_dspic table_dspic[];

#endif


