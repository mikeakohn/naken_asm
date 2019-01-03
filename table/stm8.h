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

#ifndef NAKEN_ASM_TABLE_STM8_H
#define NAKEN_ASM_TABLE_STM8_H

#include "common/assembler.h"

#define ST7_NO 0
#define ST7_YES 1

struct _table_stm8
{
  char *instr;
  uint8_t instr_enum;
};

struct _table_stm8_opcodes
{
  uint8_t instr_enum;
  uint8_t type;
  uint8_t prefix;
  uint8_t opcode;
  uint8_t dest;
  uint8_t src;
  uint8_t cycles_min;
  uint8_t cycles_max;
  uint8_t is_stm7;
};

enum
{
  STM8_NONE,
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
  STM8_CPL,
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
  STM8_JRC,
  STM8_JREQ,
  STM8_JRF,
  STM8_JRH,
  STM8_JRIH,
  STM8_JRIL,
  STM8_JRM,
  STM8_JRMI,
  STM8_JRNC,
  STM8_JRNE,
  STM8_JRNH,
  STM8_JRNM,
  STM8_JRNV,
  STM8_JRPL,
  STM8_JRSGE,
  STM8_JRSGT,
  STM8_JRSLE,
  STM8_JRSLT,
  STM8_JRT,
  STM8_JRUGE,
  STM8_JRUGT,
  STM8_JRULE,
  //STM8_JRC,
  STM8_JRULT,
  STM8_JRV,
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
  STM8_RRWA,
  STM8_RVF,
  STM8_SBC,
  STM8_SCF,
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

enum
{
  OP_NONE,
  OP_REG_A,
  OP_REG_X,
  OP_REG_Y,
  OP_REG_XL,
  OP_REG_YL,
  OP_REG_XH,
  OP_REG_YH,
  OP_REG_CC,
  OP_SP,
  OP_NUMBER8,            // #$10
  OP_NUMBER16,           // #$1000
  OP_ADDRESS8,           // $10
  OP_ADDRESS16,          // $1000
  OP_ADDRESS24,          // $35aa00
  OP_INDEX_X,            // (X)
  OP_OFFSET8_INDEX_X,    // ($10,X)
  OP_OFFSET16_INDEX_X,   // ($1000,X)
  OP_OFFSET24_INDEX_X,   // ($500000,X)
  OP_INDEX_Y,            // (Y)
  OP_OFFSET8_INDEX_Y,    // ($10,Y)
  OP_OFFSET16_INDEX_Y,   // ($1000,Y)
  OP_OFFSET24_INDEX_Y,   // ($500000,Y)
  OP_OFFSET8_INDEX_SP,   // ($10,SP)
  OP_INDIRECT8,          // [$10.w]
  OP_INDIRECT16,         // [$1000.w]
  OP_INDIRECT16_E,       // [$1000.e]
  OP_INDIRECT8_X,        // ([$10.w],X) 
  OP_INDIRECT16_X,       // ([$1000.w],X)
  OP_INDIRECT16_E_X,     // ([$1000.e],X)
  OP_INDIRECT8_Y,        // ([$10.w],Y)
  OP_INDIRECT16_Y,       // ([$1000.w],Y)
  OP_INDIRECT16_E_Y,     // ([$1000.e],Y)
  OP_ADDRESS_BIT,        // $1000, #2
  OP_ADDRESS_BIT_LOOP,   // $1000, #2, loop
  OP_RELATIVE,
  OP_SINGLE_REGISTER,
  OP_TWO_REGISTERS,
  OP_ADDRESS16_NUMBER8,
  OP_ADDRESS8_ADDRESS8,
  OP_ADDRESS16_ADDRESS16,
  //OP_ADDRESS16,
};

extern struct _table_stm8 table_stm8[];
extern struct _table_stm8_opcodes table_stm8_opcodes[];

#endif


