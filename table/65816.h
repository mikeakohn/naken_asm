/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn, Joe Davisson
 *
 * 65816 by Joe Davisson
 *
 */

#ifndef NAKEN_ASM_TABLE_65816_H
#define NAKEN_ASM_TABLE_65816_H

#include "common/assembler.h"

struct _table_65816
{
  char *name;
  uint8_t instr;
  uint8_t op;
};

struct _table_65816_opcodes
{
  uint8_t instr;
  uint8_t op;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

enum
{
  M65816_ADC,
  M65816_AND,
  M65816_ASL,
  M65816_BCC,
  M65816_BCS,
  M65816_BEQ,
  M65816_BIT,
  M65816_BMI,
  M65816_BNE,
  M65816_BPL,
  M65816_BRA,
  M65816_BRK,
  M65816_BRL,
  M65816_BVC,
  M65816_BVS,
  M65816_CLC,
  M65816_CLD,
  M65816_CLI,
  M65816_CLV,
  M65816_CMP,
  M65816_COP,
  M65816_CPX,
  M65816_CPY,
  M65816_DEC,
  M65816_DEX,
  M65816_DEY,
  M65816_EOR,
  M65816_INC,
  M65816_INX,
  M65816_INY,
  M65816_JMP,
  M65816_JSR,
  M65816_LDA,
  M65816_LDX,
  M65816_LDY,
  M65816_LSR,
  M65816_MVN,
  M65816_MVP,
  M65816_NOP,
  M65816_ORA,
  M65816_PEA,
  M65816_PEI,
  M65816_PER,
  M65816_PHA,
  M65816_PHB,
  M65816_PHD,
  M65816_PHK,
  M65816_PHP,
  M65816_PHX,
  M65816_PHY,
  M65816_PLA,
  M65816_PLB,
  M65816_PLD,
  M65816_PLP,
  M65816_PLX,
  M65816_PLY,
  M65816_REP,
  M65816_ROL,
  M65816_ROR,
  M65816_RTI,
  M65816_RTL,
  M65816_RTS,
  M65816_SBC,
  M65816_SEP,
  M65816_SEC,
  M65816_SED,
  M65816_SEI,
  M65816_STA,
  M65816_STP,
  M65816_STX,
  M65816_STY,
  M65816_STZ,
  M65816_TAX,
  M65816_TAY,
  M65816_TCD,
  M65816_TCS,
  M65816_TDC,
  M65816_TRB,
  M65816_TSB,
  M65816_TSC,
  M65816_TSX,
  M65816_TXA,
  M65816_TXS,
  M65816_TXY,
  M65816_TYA,
  M65816_TYX,
  M65816_WAI,
  M65816_WDM,
  M65816_XBA,
  M65816_XCE
};

enum
{
  OP_NONE,             //            A / s 

  OP_IMMEDIATE8,       // #$10       #
  OP_IMMEDIATE16,      // #$1000     #

  OP_ADDRESS8,         // $10        d
  OP_ADDRESS16,        // $1000      a
  OP_ADDRESS24,        // $100000    al

  OP_INDEXED8_X,       // $10,x      d,x
  OP_INDEXED8_Y,       // $10,y      d,y
  OP_INDEXED16_X,      // $1000,x    a,x
  OP_INDEXED16_Y,      // $1000,y    a,y
  OP_INDEXED24_X,      // $100000,x  al,x

  OP_INDIRECT8,        // ($10)      (d) 
  OP_INDIRECT8_LONG,   // [$10]      [d] 
  OP_INDIRECT16,       // ($1000)    (a) 
  OP_INDIRECT16_LONG,  // [$1000]    [d] 

  OP_X_INDIRECT8,      // ($10,x)    (d,x) 
  OP_X_INDIRECT16,     // ($1000,x)  (a,x) 

  OP_INDIRECT8_Y,      // ($10),y    (d),y 
  OP_INDIRECT8_Y_LONG, // [$10],y    [d],y 

  OP_BLOCK_MOVE,       // $10,$10    xyc

  OP_RELATIVE,         // $10        r
  OP_RELATIVE_LONG,    // $1000      rl

  OP_SP_RELATIVE,      // $10,s      d,s
  OP_SP_INDIRECT_Y,    // $(10,s),y  (d,s),y
};

#endif

