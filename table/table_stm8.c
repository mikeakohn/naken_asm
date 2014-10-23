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
  { "jrxx", STM8_JRXX, 0 },
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
};

// October 21, 2014
// teqo: please call the table bigHonkinTable
struct _table_stm8_opcodes table_stm8_opcodes[] = {
  { STM8_ADC, OP_NUMBER, 0, 0xa9, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_ADDRESS8, 0, 0xb9, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_ADDRESS16, 0, 0xc9, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_INDEX_X, 0, 0xf9, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_OFFSET8_INDEX_X, 0, 0xe9, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_OFFSET16_INDEX_X, 0, 0xd9, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_INDEX_Y, 0x90, 0xf9, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_OFFSET8_INDEX_Y, 0x90, 0xe9, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_OFFSET16_INDEX_Y, 0x90, 0xd9, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_OFFSET8_INDEX_SP, 0, 0x19, REG_A, REG_NONE, 1, ST7_YES },
  { STM8_ADC, OP_INDIRECT8, 0x92, 0xe9, REG_A, REG_NONE, 4, ST7_YES },
  { STM8_ADC, OP_INDIRECT16, 0x72, 0xe9, REG_A, REG_NONE, 4, ST7_YES },
  { STM8_ADC, OP_INDIRECT8_X, 0x92, 0xe9, REG_A, REG_NONE, 4, ST7_YES },
  { STM8_ADC, OP_INDIRECT16_X, 0x72, 0xe9, REG_A, REG_NONE, 4, ST7_YES },
  { STM8_ADC, OP_INDIRECT8_Y, 0x91, 0xe9, REG_A, REG_NONE, 4, ST7_YES },
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





