/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _TABLE_DSPIC_H
#define _TABLE_DSPIC_H

#include "assembler.h"

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
  OP_BRA,        // branch calculated address
  OP_F,          // f ; flag
  OP_EXPR,       // Expr (n); Expression
  OP_LIT,        // literal (k) ; #lit<num>  (#bit<num> b)
  OP_ACC,        // Acc (A) ; Accumulator
  OP_WREG,       // WREG (D); W0
  OP_WN,         // Wn (d,s 4) ; address of working register?
  OP_WM,         // Wn (t 4) ; dividend?
  OP_WB,         // Wb (w 4) ; address of base register
  OP_WNS,        // Wns (s 4) ; source register
  OP_WND,        // Wnd (d 4) ; destination register
  OP_WD,         // Wd (d 4) ; source with inc/dec (q 3) addr mode
  OP_WS,         // Ws (s 4) ; dest with inc/dec (p 3) addr mode
  OP_W_INDEX,    // (d) [Wd] or [Ws]
  OP_WS_LIT,     // Ws (s) (k); [ Ws + LIT ]
  OP_WD_LIT,     // Wd (d) (k); [ Wd + LIT ]
  OP_WXD,        // (xx)
  OP_WYD,        // (yy)
  OP_MULT2,      // (mm) Wm*Wm W4*W4, W5*W5, W6*W6, W7*W7
  OP_MULT3,      // (mmm) Wm*Wn W4*(W5,W6*W7), W5*(W6*W7), W6*W7
  OP_PREFETCH_ED_X, // (iiii)
  OP_PREFETCH_ED_Y, // (jjjj)
  OP_PREFETCH_X, // (iiii)
  OP_PREFETCH_Y, // (jjjj)
  OP_ACC_WB,     // (aa)
  OP_EXPR_DO,    // (nnnnnnnnnnn)
  OP_EXPR_GOTO   // (nnnnnnnnnnn)
};

struct _dspic_operand
{
  char type;
  char bitlen;
  char bitpos;
  char optional;
  char attrlen;
  char attrpos;
};

struct _dspic_table
{
  char *name;
  int opcode;
  int opcode48;
  int mask;
  int mask48;
  char bitlen;
  char args;
  char cycles_min;
  char cycles_max;
  int flags;
  int default_flag;  // if FLAG_NONE then flag is not optional
  char flag_pos;
  struct _dspic_operand operands[5];
};

extern struct _dspic_table dspic_table[];

#endif


