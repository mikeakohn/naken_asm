#include <stdio.h>
#include <stdlib.h>
#include "disasm_stm8.h"

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
Load and Transfer
LD
LDF
CLR
MOV
EXG
LDW
EXGW

Stack operation
PUSH
POP

Increment/ Decrement
INC
DEC

Compare and Tests
CP
TNZ
BCP
CPW

Logical operations
AND
OR
XOR
CPL

Bit Operation
BSET
BRES
BCPL
BCCM

Conditional Bit Test and Branch
BTJT
BTJF

Arithmetic operations
NEG
ADC
ADD
SUB
SBC
MUL
DIV
DIVW
ADDW
SUBW

Shift and Rotates
SLL
SRL
SRA
RLC
RRC
SWAP

Unconditional Jump or Call
JRA
JRT
JRF
JP
JPF
CALL
CALLR
CALLF

Conditional Branch/ Execution
JRxx
WFE

*/


