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

#ifndef _TABLE_STM8_H
#define _TABLE_STM8_H

#include "assembler.h"

#define ST7_NO 0
#define ST7_YES 1

struct _stm8_single
{
  char *instr;
  unsigned char opcode;
  char cycles;
  char st7_support;
};

struct _stm8_x_y
{
  char *instr;
  unsigned char opcode;
  char cycles;
  char st7_support;
};

struct _stm8_r_r
{
  char *instr;
  unsigned char opcode;
  char cycles_min;
  char cycles_max;
  char st7_support;
};

struct _stm8_jumps
{  char *instr;
   uint8_t opcode;
   uint8_t prefix;
};

struct _table_stm8
{
  char *instr;
  uint8_t id;
  uint8_t opcode;
};

struct _table_stm8_opcodes
{
  uint8_t id;
  uint8_t type;
  uint8_t prefix;
  uint8_t opcode;
  uint8_t is_stm7;
};

enum
{
  STM8_ADC,
  STM8_ADD,
  STM8_ADDW,
  STM8_AND,
  STM8_BCCM,
  STM8_BCP,
  STM8_BCPL,
  STM8_BREAK,
  STM8_BRES,
  STM8_BSET,
  STM8_BTJF,
  STM8_BTJT,
  STM8_CALL,
  STM8_CALLF,
  STM8_CALLR,
  STM8_CCF,
  STM8_CLR,
  STM8_CLRW,
  STM8_CP,
  STM8_CPW,
  STM8_CPLW,
  STM8_DEC,
  STM8_DECW,
  STM8_DIV,
  STM8_DIVW,
  STM8_EXG,
  STM8_EXGW,
  STM8_HALT,
  STM8_INC,
  STM8_INCW,
  STM8_INT,
  STM8_IRET,
  STM8_JP,
  STM8_JPF,
  STM8_JRA,
  STM8_JRXX,
  STM8_LD,
  STM8_LDF,
  STM8_LDW,
  STM8_MOV,
  STM8_MUL,
  STM8_NEG,
  STM8_NEGW,
  STM8_NOP,
  STM8_OR,
  STM8_POP,
  STM8_POPW,
  STM8_PUSH,
  STM8_PUSHW,
  STM8_RCF,
  STM8_RET,
  STM8_RETF,
  STM8_RIM,
  STM8_RLC,
  STM8_RLCW,
  STM8_RLWA,
  STM8_RRC,
  STM8_RRCW,
  STM8_RRCA,
  STM8_RVF,
  STM8_SBC,
  STM8_SIM,
  STM8_SLL,
  STM8_SLA,
  STM8_SLLW,
  STM8_SLAW,
  STM8_SRA,
  STM8_SRAW,
  STM8_SRL,
  STM8_SRLW,
  STM8_SUB,
  STM8_SUBW,
  STM8_SWAP,
  STM8_SWAPW,
  STM8_TNZ,
  STM8_TNZW,
  STM8_TRAP,
  STM8_WFE,
  STM8_WFI,
  STM8_XOR,
};

extern struct _table_stm8 table_stm8[];
extern struct _table_stm8_opcodes table_stm8_opcodes[];

extern struct _stm8_single stm8_single[];
extern struct _stm8_x_y stm8_x_y[];
extern char *stm8_type1[];
extern char *stm8_type2[];
extern char *stm8_bit_oper[];
extern struct _stm8_r_r stm8_r_r[];
extern struct _stm8_jumps stm8_jumps[];

#endif


