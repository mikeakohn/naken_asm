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

#include <stdio.h>
#include <stdlib.h>
#include "disasm_stm8.h"
#include "table_stm8.h"

struct _table_stm8 table_stm8[] = {
  { "adc", STM8_ADC, 0 },
  { "add", STM8_ADD, 0 },
  { "addw", STM8_ADDW, 0 },
  { "and", STM8_AND, 0 },
  { "bccm", STM8_BCCM, 0 },
  { "bcp", STM8_BCP, 0 },
  { "bcpl", STM8_BCPL, 0 },
  { "break", STM8_BREAK, 0 },
  { "bres", STM8_BRES, 0 },
  { "bset", STM8_BSET, 0 },
  { "btjf", STM8_BTJF, 0 },
  { "btjt", STM8_BTJT, 0 },
  { "call", STM8_CALL, 0 },
  { "callf", STM8_CALLF, 0 },
  { "callr", STM8_CALLR, 0 },
  { "ccf", STM8_CCF, 0 },
  { "clr", STM8_CLR, 0 },
  { "clrw", STM8_CLRW, 0 },
  { "cp", STM8_CP, 0 },
  { "cpw", STM8_CPW, 0 },
  { "cpl", STM8_CPL, 0 },
  { "cplw", STM8_CPLW, 0 },
  { "dec", STM8_DEC, 0 },
  { "decw", STM8_DECW, 0 },
  { "div", STM8_DIV, 0 },
  { "divw", STM8_DIVW, 0 },
  { "exg", STM8_EXG, 0 },
  { "exgw", STM8_EXGW, 0 },
  { "halt", STM8_HALT, 0 },
  { "inc", STM8_INC, 0 },
  { "incw", STM8_INCW, 0 },
  { "int", STM8_INT, 0 },
  { "iret", STM8_IRET, 0 },
  { "jp", STM8_JP, 0 },
  { "jpf", STM8_JPF, 0 },
  { "jra", STM8_JRA, 0 },
  { "jrc", STM8_JRC, 0 },
  { "jreq", STM8_JREQ, 0 },
  { "jrf", STM8_JRF, 0 },
  { "jrh", STM8_JRH, 0 },
  { "jrih", STM8_JRIH, 0 },
  { "jril", STM8_JRIL, 0 },
  { "jrm", STM8_JRM, 0 },
  { "jrmi", STM8_JRMI, 0 },
  { "jrnc", STM8_JRNC, 0 },
  { "jrne", STM8_JRNE, 0 },
  { "jrnh", STM8_JRNH, 0 },
  { "jrnm", STM8_JRNM, 0 },
  { "jrnv", STM8_JRNV, 0 },
  { "jrpl", STM8_JRPL, 0 },
  { "jrsge", STM8_JRSGE, 0 },
  { "jrsgt", STM8_JRSGT, 0 },
  { "jrsle", STM8_JRSLE, 0 },
  { "jrslt", STM8_JRSLT, 0 },
  { "jrt", STM8_JRT, 0 },
  { "jruge", STM8_JRUGE, 0 },
  { "jrugt", STM8_JRUGT, 0 },
  { "jrule", STM8_JRULE, 0 },
  //{ "jrc", STM8_JRC, 0 },
  { "jrult", STM8_JRULT, 0 },
  { "jrv", STM8_JRV, 0 },
  { "ld", STM8_LD, 0 },
  { "ldf", STM8_LDF, 0 },
  { "ldw", STM8_LDW, 0 },
  { "mov", STM8_MOV, 0 },
  { "mul", STM8_MUL, 0 },
  { "neg", STM8_NEG, 0 },
  { "negw", STM8_NEGW, 0 },
  { "nop", STM8_NOP, 0 },
  { "or", STM8_OR, 0 },
  { "pop", STM8_POP, 0 },
  { "popw", STM8_POPW, 0 },
  { "push", STM8_PUSH, 0 },
  { "pushw", STM8_PUSHW, 0 },
  { "rcf", STM8_RCF, 0 },
  { "ret", STM8_RET, 0 },
  { "retf", STM8_RETF, 0 },
  { "rim", STM8_RIM, 0 },
  { "rlc", STM8_RLC, 0 },
  { "rlcw", STM8_RLCW, 0 },
  { "rlwa", STM8_RLWA, 0 },
  { "rrc", STM8_RRC, 0 },
  { "rrcw", STM8_RRCW, 0 },
  { "rrca", STM8_RRCA, 0 },
  { "rvf", STM8_RVF, 0 },
  { "sbc", STM8_SBC, 0 },
  { "sim", STM8_SIM, 0 },
  { "sll", STM8_SLL, 0 },
  { "sla", STM8_SLA, 0 },
  { "sllw", STM8_SLLW, 0 },
  { "slaw", STM8_SLAW, 0 },
  { "sra", STM8_SRA, 0 },
  { "sraw", STM8_SRAW, 0 },
  { "srl", STM8_SRL, 0 },
  { "srlw", STM8_SRLW, 0 },
  { "sub", STM8_SUB, 0 },
  { "subw", STM8_SUBW, 0 },
  { "swap", STM8_SWAP, 0 },
  { "swapw", STM8_SWAPW, 0 },
  { "tnz", STM8_TNZ, 0 },
  { "tnzw", STM8_TNZW, 0 },
  { "trap", STM8_TRAP, 0 },
  { "wfe", STM8_WFE, 0 },
  { "wfi", STM8_WFI, 0 },
  { "xor", STM8_XOR, 0 },
  { NULL, 0, 0}
};

// October 21, 2014
// teqo: please call the table bigHonkinTable
struct _table_stm8_opcodes table_stm8_opcodes[] = {
  // ADC
  { STM8_ADC, OP_NUMBER8, 0, 0xa9, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADC, OP_ADDRESS8, 0, 0xb9, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADC, OP_ADDRESS16, 0, 0xc9, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADC, OP_INDEX_X, 0, 0xf9, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADC, OP_OFFSET8_INDEX_X, 0, 0xe9, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADC, OP_OFFSET16_INDEX_X, 0, 0xd9, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADC, OP_INDEX_Y, 0x90, 0xf9, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADC, OP_OFFSET8_INDEX_Y, 0x90, 0xe9, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADC, OP_OFFSET16_INDEX_Y, 0x90, 0xd9, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADC, OP_OFFSET8_INDEX_SP, 0, 0x19, OP_REG_A, OP_NONE, 1,1, ST7_NO },
  { STM8_ADC, OP_INDIRECT8, 0x92, 0xc9, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_ADC, OP_INDIRECT16, 0x72, 0xc9, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_ADC, OP_INDIRECT8_X, 0x92, 0xd9, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_ADC, OP_INDIRECT16_X, 0x72, 0xd9, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_ADC, OP_INDIRECT8_Y, 0x91, 0xd9, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  // ADD
  { STM8_ADD, OP_NUMBER8, 0, 0xab, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADD, OP_ADDRESS8, 0, 0xbb, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADD, OP_ADDRESS16, 0, 0xcb, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADD, OP_INDEX_X, 0, 0xfb, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADD, OP_OFFSET8_INDEX_X, 0, 0xeb, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADD, OP_OFFSET16_INDEX_X, 0, 0xdb, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADD, OP_INDEX_Y, 0x90, 0xfb, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADD, OP_OFFSET8_INDEX_Y, 0x90, 0xeb, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADD, OP_OFFSET16_INDEX_Y, 0x90, 0xdb, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_ADD, OP_OFFSET8_INDEX_SP, 0, 0x1b, OP_REG_A, OP_NONE, 1,1, ST7_NO },
  { STM8_ADD, OP_INDIRECT8, 0x92, 0xcb, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_ADD, OP_INDIRECT16, 0x72, 0xcb, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_ADD, OP_INDIRECT8_X, 0x92, 0xdb, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_ADD, OP_INDIRECT16_X, 0x72, 0xdb, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_ADD, OP_INDIRECT8_Y, 0x91, 0xdb, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  // ADDW
  { STM8_ADDW, OP_NUMBER16, 0, 0x1c, OP_REG_X, OP_NONE, 2,2, ST7_NO },
  { STM8_ADDW, OP_ADDRESS16, 0x72, 0xbb, OP_REG_X, OP_NONE, 2,2, ST7_NO },
  { STM8_ADDW, OP_OFFSET8_INDEX_SP, 0x72, 0xfb, OP_REG_X, OP_NONE, 2,2, ST7_NO },
  { STM8_ADDW, OP_NUMBER16, 0x72, 0xa9, OP_REG_Y, OP_NONE, 2,2, ST7_NO },
  { STM8_ADDW, OP_ADDRESS16, 0x72, 0xb9, OP_REG_Y, OP_NONE, 2,2, ST7_NO },
  { STM8_ADDW, OP_OFFSET8_INDEX_SP, 0x72, 0xf9, OP_REG_Y, OP_NONE, 2,2, ST7_NO },
  { STM8_ADDW, OP_NUMBER8, 0, 0x5b, OP_SP, OP_NONE, 2,2, ST7_NO },
  // AND
  { STM8_AND, OP_NUMBER8, 0, 0xa4, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_AND, OP_ADDRESS8, 0, 0xb4, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_AND, OP_ADDRESS16, 0, 0xc4, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_AND, OP_INDEX_X, 0, 0xf4, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_AND, OP_OFFSET8_INDEX_X, 0, 0xe4, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_AND, OP_OFFSET16_INDEX_X, 0, 0xd4, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_AND, OP_INDEX_Y, 0x90, 0xf4, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_AND, OP_OFFSET8_INDEX_Y, 0x90, 0xe4, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_AND, OP_OFFSET16_INDEX_Y, 0x90, 0xd4, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_AND, OP_OFFSET8_INDEX_SP, 0, 0x14, OP_REG_A, OP_NONE, 1,1, ST7_NO },
  { STM8_AND, OP_INDIRECT8, 0x92, 0xc4, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_AND, OP_INDIRECT16, 0x72, 0xc4, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_AND, OP_INDIRECT8_X, 0x92, 0xd4, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_AND, OP_INDIRECT16_X, 0x72, 0xd4, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_AND, OP_INDIRECT8_Y, 0x91, 0xd4, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  // BCCM
  { STM8_BCCM, OP_ADDRESS_BIT, 0x90, 0x11, OP_NONE, OP_NONE, 1,1, ST7_NO },
  // BCP
  { STM8_BCP, OP_NUMBER8, 0, 0xa5, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_BCP, OP_ADDRESS8, 0, 0xb5, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_BCP, OP_ADDRESS16, 0, 0xc5, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_BCP, OP_INDEX_X, 0, 0xf5, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_BCP, OP_OFFSET8_INDEX_X, 0, 0xe5, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_BCP, OP_OFFSET16_INDEX_X, 0, 0xd5, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_BCP, OP_INDEX_Y, 0x90, 0xf5, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_BCP, OP_OFFSET8_INDEX_Y, 0x90, 0xe5, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_BCP, OP_OFFSET16_INDEX_Y, 0x90, 0xd5, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_BCP, OP_OFFSET8_INDEX_SP, 0, 0x15, OP_REG_A, OP_NONE, 1,1, ST7_NO },
  { STM8_BCP, OP_INDIRECT8, 0x92, 0xc5, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_BCP, OP_INDIRECT16, 0x72, 0xc5, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_BCP, OP_INDIRECT8_X, 0x92, 0xd5, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_BCP, OP_INDIRECT16_X, 0x72, 0xd5, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_BCP, OP_INDIRECT8_Y, 0x91, 0xd5, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  // BCPL
  { STM8_BCPL, OP_ADDRESS_BIT, 0x90, 0x10, OP_NONE, OP_NONE, 1,1, ST7_NO },
  // BREAK
  { STM8_BREAK, OP_NONE, 0, 0x8B, OP_NONE, OP_NONE, 1,1, ST7_YES },
  // BRES
  { STM8_BRES, OP_ADDRESS_BIT, 0x72, 0x11, OP_NONE, OP_NONE, 1,1, ST7_NO },
  // BSET
  { STM8_BSET, OP_ADDRESS_BIT, 0x72, 0x10, OP_NONE, OP_NONE, 1,1, ST7_NO },
  // BTJF
  { STM8_BTJF, OP_ADDRESS_BIT_LOOP, 0x72, 0x01, OP_NONE, OP_NONE, 2,3, ST7_NO },
  // BTJT
  { STM8_BTJT, OP_ADDRESS_BIT_LOOP, 0x72, 0x00, OP_NONE, OP_NONE, 2,3, ST7_NO },
  // CALL
  { STM8_CALL, OP_ADDRESS16, 0, 0xcd, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CALL, OP_INDEX_X, 0, 0xfd, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CALL, OP_OFFSET8_INDEX_X, 0, 0xed, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CALL, OP_OFFSET16_INDEX_X, 0, 0xdd, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CALL, OP_INDEX_Y, 0x90, 0xfd, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CALL, OP_OFFSET8_INDEX_Y, 0x90, 0xed, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CALL, OP_OFFSET16_INDEX_Y, 0x90, 0xdd, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CALL, OP_INDIRECT8, 0x92, 0xcd, OP_NONE, OP_NONE, 6,6, ST7_YES },
  { STM8_CALL, OP_INDIRECT16, 0x72, 0xcd, OP_NONE, OP_NONE, 6,6, ST7_NO },
  { STM8_CALL, OP_INDIRECT8_X, 0x92, 0xdd, OP_NONE, OP_NONE, 6,6, ST7_YES },
  { STM8_CALL, OP_INDIRECT16_X, 0x72, 0xdd, OP_NONE, OP_NONE, 6,6, ST7_NO },
  { STM8_CALL, OP_INDIRECT8_Y, 0x91, 0xdd, OP_NONE, OP_NONE, 6,6, ST7_YES },
  // CALLF
  { STM8_CALLF, OP_ADDRESS24, 0, 0x8d, OP_NONE, OP_NONE, 5,5, ST7_NO },
  { STM8_CALLF, OP_INDIRECT16_E, 0x92, 0x8d, OP_NONE, OP_NONE, 8,8, ST7_NO },
  // CALLR
  { STM8_CALLR, OP_RELATIVE, 0, 0xad, OP_NONE, OP_NONE, 4,4, ST7_YES },
  // CCF
  { STM8_CCF, OP_NONE, 0, 0x8C, OP_NONE, OP_NONE, 1,1, ST7_NO },
  // CLR
  { STM8_CLR, OP_SINGLE_REGISTER, 0, 0x4f, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CLR, OP_ADDRESS8, 0, 0x3f, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CLR, OP_ADDRESS16, 0x72, 0x5f, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_CLR, OP_INDEX_X, 0, 0x7f, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CLR, OP_OFFSET8_INDEX_X, 0, 0x6f, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CLR, OP_OFFSET16_INDEX_X, 0x72, 0x4f, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_CLR, OP_INDEX_Y, 0x90, 0x7f, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CLR, OP_OFFSET8_INDEX_Y, 0x90, 0x6f, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CLR, OP_OFFSET16_INDEX_Y, 0x90, 0x4f, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_CLR, OP_OFFSET8_INDEX_SP, 0, 0x0f, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_CLR, OP_INDIRECT8, 0x92, 0x3f, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CLR, OP_INDIRECT16, 0x72, 0x3f, OP_NONE, OP_NONE, 4,4, ST7_NO },
  { STM8_CLR, OP_INDIRECT8_X, 0x92, 0x6f, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CLR, OP_INDIRECT16_X, 0x72, 0x6f, OP_NONE, OP_NONE, 4,4, ST7_NO },
  { STM8_CLR, OP_INDIRECT8_Y, 0x91, 0x6f, OP_NONE, OP_NONE, 4,4, ST7_YES },
  // CLRW
  { STM8_CLRW, OP_SINGLE_REGISTER, 0, 0x5f, OP_REG_X, OP_NONE, 1,1, ST7_NO },
  { STM8_CLRW, OP_SINGLE_REGISTER, 0x90, 0x5f, OP_REG_Y, OP_NONE, 1,1, ST7_NO },
  // CP
  { STM8_CP, OP_NUMBER8, 0, 0xa1, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CP, OP_ADDRESS8, 0, 0xb1, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CP, OP_ADDRESS16, 0, 0xc1, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CP, OP_INDEX_X, 0, 0xf1, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CP, OP_OFFSET8_INDEX_X, 0, 0xe1, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CP, OP_OFFSET16_INDEX_X, 0, 0xd1, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CP, OP_INDEX_Y, 0x90, 0xf1, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CP, OP_OFFSET8_INDEX_Y, 0x90, 0xe1, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CP, OP_OFFSET16_INDEX_Y, 0x90, 0xd1, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CP, OP_OFFSET8_INDEX_SP, 0, 0x11, OP_REG_A, OP_NONE, 1,1, ST7_NO },
  { STM8_CP, OP_INDIRECT8, 0x92, 0xc1, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_CP, OP_INDIRECT16, 0x72, 0xc1, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_CP, OP_INDIRECT8_X, 0x92, 0xd1, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_CP, OP_INDIRECT16_X, 0x72, 0xd1, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_CP, OP_INDIRECT8_Y, 0x91, 0xd1, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  // CPW
  { STM8_CPW, OP_NUMBER16, 0, 0xa3, OP_REG_X, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_ADDRESS8, 0, 0xb3, OP_REG_X, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_ADDRESS16, 0, 0xc3, OP_REG_X, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_INDEX_Y, 0x90, 0xf3, OP_REG_X, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_OFFSET8_INDEX_Y, 0x90, 0xe3, OP_REG_X, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_OFFSET16_INDEX_Y, 0x90, 0xd3, OP_REG_X, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_OFFSET8_INDEX_SP, 0, 0x13, OP_REG_X, OP_NONE, 2,2, ST7_NO },
  { STM8_CPW, OP_INDIRECT8, 0x92, 0xc3, OP_REG_X, OP_NONE, 5,5, ST7_YES },
  { STM8_CPW, OP_INDIRECT16, 0x72, 0xc3, OP_REG_X, OP_NONE, 5,5, ST7_NO },
  { STM8_CPW, OP_INDIRECT8_Y, 0x91, 0xd3, OP_REG_X, OP_NONE, 5,5, ST7_YES },

  { STM8_CPW, OP_NUMBER16, 0x90, 0xa3, OP_REG_Y, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_ADDRESS8, 0x90, 0xb3, OP_REG_Y, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_ADDRESS16, 0x90, 0xc3, OP_REG_Y, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_INDEX_X, 0, 0xf3, OP_REG_Y, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_OFFSET8_INDEX_X, 0, 0xe3, OP_REG_Y, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_OFFSET16_INDEX_X, 0, 0xd3, OP_REG_Y, OP_NONE, 2,2, ST7_YES },
  { STM8_CPW, OP_INDIRECT8, 0x91, 0xc3, OP_REG_Y, OP_NONE, 5,5, ST7_YES },
  { STM8_CPW, OP_INDIRECT8_X, 0x92, 0xd3, OP_REG_Y, OP_NONE, 5,5, ST7_NO },
  { STM8_CPW, OP_INDIRECT16_X, 0x72, 0xd3, OP_REG_Y, OP_NONE, 5,5, ST7_YES },
  // CPL
  { STM8_CPL, OP_SINGLE_REGISTER, 0, 0x43, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_CPL, OP_ADDRESS8, 0, 0x33, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CPL, OP_ADDRESS16, 0x72, 0x53, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_CPL, OP_INDEX_X, 0, 0x73, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CPL, OP_OFFSET8_INDEX_X, 0, 0x63, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CPL, OP_OFFSET16_INDEX_X, 0x72, 0x43, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_CPL, OP_INDEX_Y, 0x90, 0x73, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CPL, OP_OFFSET8_INDEX_Y, 0x90, 0x63, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CPL, OP_OFFSET16_INDEX_Y, 0x90, 0x43, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_CPL, OP_OFFSET8_INDEX_SP, 0, 0x03, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_CPL, OP_INDIRECT8, 0x92, 0x33, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CPL, OP_INDIRECT16, 0x72, 0x33, OP_NONE, OP_NONE, 4,4, ST7_NO },
  { STM8_CPL, OP_INDIRECT8_X, 0x92, 0x63, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_CPL, OP_INDIRECT16_X, 0x72, 0x63, OP_NONE, OP_NONE, 4,4, ST7_NO },
  { STM8_CPL, OP_INDIRECT8_Y, 0x91, 0x63, OP_NONE, OP_NONE, 4,4, ST7_YES },
  // CPLW
  { STM8_CPLW, OP_SINGLE_REGISTER, 0, 0x53, OP_REG_X, OP_NONE, 2,2, ST7_YES },
  { STM8_CPLW, OP_SINGLE_REGISTER, 0x90, 0x53, OP_REG_Y, OP_NONE, 2,2, ST7_YES },
  // DEC
  { STM8_DEC, OP_SINGLE_REGISTER, 0, 0x4a, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_DEC, OP_ADDRESS8, 0, 0x3a, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_DEC, OP_ADDRESS16, 0x72, 0x5a, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_DEC, OP_INDEX_X, 0, 0x7a, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_DEC, OP_OFFSET8_INDEX_X, 0, 0x6a, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_DEC, OP_OFFSET16_INDEX_X, 0x72, 0x4a, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_DEC, OP_INDEX_Y, 0x90, 0x7a, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_DEC, OP_OFFSET8_INDEX_Y, 0x90, 0x6a, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_DEC, OP_OFFSET16_INDEX_Y, 0x90, 0x4a, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_DEC, OP_OFFSET8_INDEX_SP, 0, 0x0a, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_DEC, OP_INDIRECT8, 0x92, 0x3a, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_DEC, OP_INDIRECT16, 0x72, 0x3a, OP_NONE, OP_NONE, 4,4, ST7_NO },
  { STM8_DEC, OP_INDIRECT8_X, 0x92, 0x6a, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_DEC, OP_INDIRECT16_X, 0x72, 0x6a, OP_NONE, OP_NONE, 4,4, ST7_NO },
  { STM8_DEC, OP_INDIRECT8_Y, 0x91, 0x6a, OP_NONE, OP_NONE, 4,4, ST7_YES },
  // DECW
  { STM8_DECW, OP_SINGLE_REGISTER, 0, 0x5a, OP_REG_X, OP_NONE, 1,1, ST7_NO },
  { STM8_DECW, OP_SINGLE_REGISTER, 0x90, 0x5a, OP_REG_Y, OP_NONE, 1,1, ST7_NO },
  // DIV
  { STM8_DIV, OP_TWO_REGISTERS, 0, 0x62, OP_REG_X, OP_REG_A, 2,17, ST7_NO },
  { STM8_DIV, OP_TWO_REGISTERS, 0x90, 0x62, OP_REG_Y, OP_REG_A, 2,17, ST7_NO },
  // DIVW
  { STM8_DIVW, OP_TWO_REGISTERS, 0, 0x65, OP_REG_X, OP_REG_Y, 2,17, ST7_NO },
  // EXG
  { STM8_EXG, OP_TWO_REGISTERS, 0, 0x41, OP_REG_A, OP_REG_XL, 1,1, ST7_NO },
  { STM8_EXG, OP_TWO_REGISTERS, 0, 0x61, OP_REG_A, OP_REG_YL, 1,1, ST7_NO },
  { STM8_EXG, OP_ADDRESS16, 0, 0x31, OP_REG_A, OP_NONE, 3,3, ST7_NO },
  // EXGW
  { STM8_EXGW, OP_TWO_REGISTERS, 0, 0x51, OP_REG_X, OP_REG_Y, 1,1, ST7_NO },
  // HALT
  { STM8_HALT, OP_NONE, 0, 0x8e, OP_NONE, OP_NONE, 1,1, ST7_YES },
  // INC
  { STM8_INC, OP_SINGLE_REGISTER, 0, 0x4c, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_INC, OP_ADDRESS8, 0, 0x3c, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_INC, OP_ADDRESS16, 0x72, 0x5c, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_INC, OP_INDEX_X, 0, 0x7c, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_INC, OP_OFFSET8_INDEX_X, 0, 0x6c, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_INC, OP_OFFSET16_INDEX_X, 0x72, 0x4c, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_INC, OP_INDEX_Y, 0x90, 0x7c, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_INC, OP_OFFSET8_INDEX_Y, 0x90, 0x6c, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_INC, OP_OFFSET16_INDEX_Y, 0x90, 0x4c, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_INC, OP_OFFSET8_INDEX_SP, 0, 0x0c, OP_NONE, OP_NONE, 1,1, ST7_NO },
  { STM8_INC, OP_INDIRECT8, 0x92, 0x3c, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_INC, OP_INDIRECT16, 0x72, 0x3c, OP_NONE, OP_NONE, 4,4, ST7_NO },
  { STM8_INC, OP_INDIRECT8_X, 0x92, 0x6c, OP_NONE, OP_NONE, 4,4, ST7_YES },
  { STM8_INC, OP_INDIRECT16_X, 0x72, 0x6c, OP_NONE, OP_NONE, 4,4, ST7_NO },
  { STM8_INC, OP_INDIRECT8_Y, 0x91, 0x6c, OP_NONE, OP_NONE, 4,4, ST7_YES },
  // INCW
  { STM8_INCW, OP_SINGLE_REGISTER, 0, 0x5c, OP_REG_X, OP_NONE, 1,1, ST7_NO },
  { STM8_INCW, OP_SINGLE_REGISTER, 0x90, 0x5c, OP_REG_Y, OP_NONE, 1,1, ST7_NO },
  // INT
  { STM8_INT, OP_ADDRESS24, 0, 0x82, OP_NONE, OP_NONE, 2,2, ST7_NO },
  // IRET
  { STM8_IRET, OP_NONE, 0, 0x80, OP_NONE, OP_NONE, 11,11, ST7_YES },
  // JP
  { STM8_JP, OP_ADDRESS16, 0, 0xcc, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_JP, OP_INDEX_X, 0, 0xfc, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_JP, OP_OFFSET8_INDEX_X, 0, 0xec, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_JP, OP_OFFSET16_INDEX_X, 0, 0xdc, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_JP, OP_INDEX_Y, 0x90, 0xfc, OP_NONE, OP_NONE, 1,1, ST7_YES },
  { STM8_JP, OP_OFFSET8_INDEX_Y, 0x90, 0xec, OP_NONE, OP_NONE, 2,2, ST7_YES },
  { STM8_JP, OP_OFFSET16_INDEX_Y, 0x90, 0xdc, OP_NONE, OP_NONE, 2,2, ST7_YES },
  { STM8_JP, OP_INDIRECT8, 0x92, 0xcc, OP_NONE, OP_NONE, 5,5, ST7_YES },
  { STM8_JP, OP_INDIRECT16, 0x72, 0xcc, OP_NONE, OP_NONE, 5,5, ST7_NO },
  { STM8_JP, OP_INDIRECT8_X, 0x92, 0xdc, OP_NONE, OP_NONE, 5,5, ST7_YES },
  { STM8_JP, OP_INDIRECT16_X, 0x72, 0xdc, OP_NONE, OP_NONE, 5,5, ST7_NO },
  { STM8_JP, OP_INDIRECT8_Y, 0x91, 0xdc, OP_NONE, OP_NONE, 5,5, ST7_YES },
  // JPF
  { STM8_JPF, OP_ADDRESS24, 0, 0xac, OP_NONE, OP_NONE, 2,2, ST7_NO },
  { STM8_JPF, OP_INDIRECT16_E, 0x92, 0xac, OP_NONE, OP_NONE, 6,6, ST7_NO },
  // JRA
  { STM8_JRA, OP_RELATIVE, 0, 0x20, OP_NONE, OP_NONE, 2,2, ST7_YES },
  // JRxx
  { STM8_JRC, OP_RELATIVE, 0, 0x25, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JREQ, OP_RELATIVE, 0, 0x27, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRF, OP_RELATIVE, 0, 0x21, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRH, OP_RELATIVE, 0x90, 0x29, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRIH, OP_RELATIVE, 0x90, 0x2f, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRIL, OP_RELATIVE, 0x90, 0x2e, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRM, OP_RELATIVE, 0x90, 0x2d, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRMI, OP_RELATIVE, 0, 0x2b, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRNC, OP_RELATIVE, 0, 0x24, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRNE, OP_RELATIVE, 0, 0x26, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRNH, OP_RELATIVE, 0x90, 0x28, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRNM, OP_RELATIVE, 0x90, 0x2c, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRNV, OP_RELATIVE, 0, 0x28, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRPL, OP_RELATIVE, 0, 0x2a, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRSGE, OP_RELATIVE, 0, 0x2e, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRSGT, OP_RELATIVE, 0, 0x2c, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRSLE, OP_RELATIVE, 0, 0x2d, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRSLT, OP_RELATIVE, 0, 0x2f, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRT, OP_RELATIVE, 0, 0x20, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRUGE, OP_RELATIVE, 0, 0x24, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRUGT, OP_RELATIVE, 0, 0x22, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRULE, OP_RELATIVE, 0, 0x23, OP_NONE, OP_NONE, 1,2, ST7_YES },
  //{ STM8_JRC, OP_RELATIVE, 0, 0x25, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRULT, OP_RELATIVE, 0, 0x25, OP_NONE, OP_NONE, 1,2, ST7_YES },
  { STM8_JRV, OP_RELATIVE, 0, 0x29, OP_NONE, OP_NONE, 1,2, ST7_YES },
  // LD A, <xxx>
  { STM8_LD, OP_NUMBER8, 0, 0xa6, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_LD, OP_ADDRESS8, 0, 0xb6, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_LD, OP_ADDRESS16, 0, 0xc6, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_LD, OP_INDEX_X, 0, 0xf6, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET8_INDEX_X, 0, 0xe6, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET16_INDEX_X, 0, 0xd6, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_LD, OP_INDEX_Y, 0x90, 0xf6, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET8_INDEX_Y, 0x90, 0xe6, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET16_INDEX_Y, 0x90, 0xd6, OP_REG_A, OP_NONE, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET8_INDEX_SP, 0, 0x7b, OP_REG_A, OP_NONE, 1,1, ST7_NO },
  { STM8_LD, OP_INDIRECT8, 0x92, 0xc6, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_LD, OP_INDIRECT16, 0x72, 0xc6, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_LD, OP_INDIRECT8_X, 0x92, 0xd6, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  { STM8_LD, OP_INDIRECT16_X, 0x72, 0xd6, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_LD, OP_INDIRECT8_Y, 0x91, 0xd6, OP_REG_A, OP_NONE, 4,4, ST7_YES },
  // LD <xxx>, A
  { STM8_LD, OP_ADDRESS8, 0, 0xb7, OP_NONE, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_ADDRESS16, 0, 0xc7, OP_NONE, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_INDEX_X, 0, 0xf7, OP_NONE, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET8_INDEX_X, 0, 0xe7, OP_NONE, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET16_INDEX_X, 0, 0xd7, OP_NONE, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_INDEX_Y, 0x90, 0xf7, OP_NONE, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET8_INDEX_Y, 0x90, 0xe7, OP_NONE, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET16_INDEX_Y, 0x90, 0xd7, OP_NONE, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_OFFSET8_INDEX_SP, 0, 0x6b, OP_NONE, OP_REG_A, 1,1, ST7_NO },
  { STM8_LD, OP_INDIRECT8, 0x92, 0xc7, OP_NONE, OP_REG_A, 4,4, ST7_YES },
  { STM8_LD, OP_INDIRECT16, 0x72, 0xc7, OP_NONE, OP_REG_A, 4,4, ST7_NO },
  { STM8_LD, OP_INDIRECT8_X, 0x92, 0xd7, OP_NONE, OP_REG_A, 4,4, ST7_YES },
  { STM8_LD, OP_INDIRECT16_X, 0x72, 0xd7, OP_NONE, OP_REG_A, 4,4, ST7_NO },
  { STM8_LD, OP_INDIRECT8_Y, 0x91, 0xd7, OP_NONE, OP_REG_A, 4,4, ST7_YES },
  // LD REG, REG
  { STM8_LD, OP_TWO_REGISTERS, 0, 0x97, OP_REG_XL, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_TWO_REGISTERS, 0, 0x9f, OP_REG_A, OP_REG_XL, 1,1, ST7_YES },
  { STM8_LD, OP_TWO_REGISTERS, 0x90, 0x97, OP_REG_YL, OP_REG_A, 1,1, ST7_YES },
  { STM8_LD, OP_TWO_REGISTERS, 0x90, 0x9f, OP_REG_A, OP_REG_YL, 1,1, ST7_YES },
  { STM8_LD, OP_TWO_REGISTERS, 0, 0x95, OP_REG_XH, OP_REG_A, 1,1, ST7_NO },
  { STM8_LD, OP_TWO_REGISTERS, 0, 0x9e, OP_REG_A, OP_REG_XH, 1,1, ST7_NO },
  { STM8_LD, OP_TWO_REGISTERS, 0x90, 0x95, OP_REG_YH, OP_REG_A, 1,1, ST7_NO },
  { STM8_LD, OP_TWO_REGISTERS, 0x90, 0x9e, OP_REG_A, OP_REG_YH, 1,1, ST7_NO },
  // LDF A, <xxx>
  { STM8_LDF, OP_ADDRESS24, 0, 0xbc, OP_REG_A, OP_NONE, 1,1, ST7_NO },
  { STM8_LDF, OP_OFFSET24_INDEX_X, 0, 0xaf, OP_REG_A, OP_NONE, 1,1, ST7_NO },
  { STM8_LDF, OP_OFFSET24_INDEX_Y, 0x90, 0xaf, OP_REG_A, OP_NONE, 1,1, ST7_NO },
  { STM8_LDF, OP_INDIRECT16_E_X, 0x92, 0xaf, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_LDF, OP_INDIRECT16_E_Y, 0x91, 0xaf, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  { STM8_LDF, OP_INDIRECT16_E, 0x92, 0xbc, OP_REG_A, OP_NONE, 4,4, ST7_NO },
  // LDF <xxx>, A
  { STM8_LDF, OP_ADDRESS24, 0, 0xbd, OP_NONE, OP_REG_A, 1,1, ST7_NO },
  { STM8_LDF, OP_OFFSET24_INDEX_X, 0, 0xa7, OP_NONE, OP_REG_A, 1,1, ST7_NO },
  { STM8_LDF, OP_OFFSET24_INDEX_Y, 0x90, 0xa7, OP_NONE, OP_REG_A, 1,1, ST7_NO },
  { STM8_LDF, OP_INDIRECT16_E_X, 0x92, 0xa7, OP_NONE, OP_REG_A, 4,4, ST7_NO },
  { STM8_LDF, OP_INDIRECT16_E_Y, 0x91, 0xa7, OP_NONE, OP_REG_A, 4,4, ST7_NO },
  { STM8_LDF, OP_INDIRECT16_E, 0x92, 0xbd, OP_NONE, OP_REG_A, 4,4, ST7_NO },

  { STM8_NONE, 0, 0, 0, 0, 0, 0, 0 },
};

struct _stm8_single stm8_single[] = {
  { "break", 0x8b, 1, ST7_YES },
  { "ccf", 0x8c, 1, ST7_NO },
  { "halt", 0x8e, 10, ST7_YES },
  { "iret", 0x80, 11, ST7_YES },
  { "nop", 0x9d, 1, ST7_YES },
  { "rcf", 0x98, 1, ST7_YES },
  { "ret", 0x81, 4, ST7_YES },
  { "retf", 0x87, 5, ST7_NO },
  { "rim", 0x9a, 1, ST7_YES },
  { "rvf", 0x9c, 1, ST7_YES },
  { "scf", 0x99, 1, ST7_YES },
  { "sim", 0x9b, 1, ST7_YES },
  { "trap", 0x83, 9, ST7_YES },
  { "wfi", 0x8f, 10, ST7_YES },
  { NULL, 0x00, 0, 0 },
};

struct _stm8_x_y stm8_x_y[] = {
  { "clrw", 0x5f, 1, ST7_NO },
  { "cplw", 0x53, 2, ST7_YES },
  { "decw", 0x5a, 1, ST7_NO },
  { "incw", 0x5c, 1, ST7_NO },
  { "negw", 0x50, 2, ST7_NO },
  { "popw", 0x85, 2, ST7_YES },
  { "pushw", 0x89, 2, ST7_YES },
  { "rlcw", 0x59, 2, ST7_YES },
  { "rlwa", 0x02, 1, ST7_NO },
  { "rrcw", 0x56, 2, ST7_YES },
  { "rrwa", 0x01, 1, ST7_NO },
  { "sllw", 0x58, 2, ST7_NO },
  { "slaw", 0x58, 2, ST7_NO },   // Same as sllw
  { "sraw", 0x57, 2, ST7_NO },
  { "srlw", 0x54, 2, ST7_NO },
  { "swapw", 0x5e, 1, ST7_YES },
  { "tnzw", 0x5d, 2, ST7_NO },
  { NULL, 0x00, 0, 0 },
};

/*
SUB    0   // plus SUB SP, #byte
CP     1
SBC    2
AND    4
BCP    5
LD     6
LD     7   // reverse
XOR    8
ADC    9
OR     a
ADD    b
JP     c   // no a, b, 1
CALL   d   // no a and b
*/

//struct _stm8_instr stm8_type1[] = {
char *stm8_type1[] = {
  "sub",   //0   // plus SUB SP, #byte
  "cp",    //1
  "sbc",   //2
  "cpw",   //3  // X,
  "and",   //4
  "bcp" ,  //5
  "ld",    //6
  "ld",    //7   // reverse
  "xor",   //8
  "adc",   //9
  "or",    //a
  "add",   //b
  "jp",    //c   // no a, b, 1
  "call",  //d   // no a and b
  "ldw",   //e
  NULL,    //f
};

/*
NEG   0
CPL   3
SRL   4
RRC   6
SRA   7
SLL   8   (SLA also)
RLC   9
DEC   a
INC   c
TNZ   d
SWAP  e
CLR   f
*/

//struct _stm8_instr stm8_type2[] = {
char *stm8_type2[] = {
  "neg",  // 0
  NULL,   // 1
  NULL,   // 2
  "cpl",  // 3
  "srl",  // 4
  NULL,   // 5
  "rrc",  // 6
  "sra",  // 7
  "sll",  // 8   (SLA also)
  "rlc",  // 9
  "dec",  // a
  NULL,   // b
  "inc",  // c
  "tnz",  // d
  "swap", // e
  "clr",  // f
  "sla",  // 10 (fake.. translate to 8)
};

char *stm8_bit_oper[] = {
  "bset",
  "bres",
  "bcpl",
  "bccm",
  "btjt",
  "btjf",
};

struct _stm8_r_r stm8_r_r[] = {
  { "mul", 0x42, 4, 4, ST7_NO },   // X/Y, A
  { "div", 0x62, 2, 17, ST7_NO },  // X/Y, A
  { "divw", 0x65, 2, 17, ST7_NO }, // X,Y
  { "exgw", 0x51, 1, 1, ST7_NO },  // X,Y
  { NULL, 0x00, 0, 0, ST7_NO },
};

struct _stm8_jumps stm8_jumps[] = {
  { "jra", 0x20, 0 },
  { "jrt", 0x20, 0 },
  { "jrf", 0x21, 0 },
  { "jrugt", 0x22, 0 },
  { "jrule", 0x23, 0 },
  { "jruge", 0x24, 0 },
  { "jrnc", 0x24, 0 },
  { "jrc", 0x25, 0 },
  { "jrult", 0x25, 0 },
  { "jrne", 0x26, 0 },
  { "jreq", 0x27, 0 },
  { "jrnv", 0x28, 0 },
  { "jrv", 0x29, 0 },
  { "jrpl", 0x2a, 0 },
  { "jrmi", 0x2b, 0 },
  { "jrsgt", 0x2c, 0 },
  { "jrsle", 0x2d, 0 },
  { "jrsge", 0x2e, 0 },
  { "jrslt", 0x2f, 0 },
  { "jrnh", 0x28, 0x90 },
  { "jrh", 0x29, 0x90 },
  { "jrnm", 0x2c, 0x90 },
  { "jrm", 0x2d, 0x90 },
  { "jril", 0x2e, 0x90 },
  { "jrih", 0x2f, 0x90 },
  { NULL, 0, 0 },
};

/*
Load and Transfer
(done) LD
(done) EXG
(done) MOV
(done) LDF
LDW

Stack operation
(done) PUSH
(done) POP

Arithmetic operations
(done) ADDW
(done) SUBW

Unconditional Jump or Call
(done) JPF
(done) CALLR
(done) CALLF
JP

Conditional Branch/ Execution
(done) JRxx
(done) WFE

*/





