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

#include <stdio.h>
#include <stdlib.h>
#include "table/8051.h"

struct _table_8051 table_8051[] =
{
  { "nop", { OP_NONE, OP_NONE, OP_NONE }, -1 }, // 0x00
  { "ajmp", { OP_PAGE, OP_NONE, OP_NONE }, 0 }, // 0x01
  { "ljmp", { OP_CODE_ADDR, OP_NONE, OP_NONE }, -1 }, // 0x02
  { "rr", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0x03
  { "inc", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0x04
  { "inc", { OP_IRAM_ADDR, OP_NONE, OP_NONE }, -1 }, // 0x05
  { "inc", { OP_AT_REG, OP_NONE, OP_NONE }, 0 }, // 0x06
  { "inc", { OP_AT_REG, OP_NONE, OP_NONE }, 1 }, // 0x07
  { "inc", { OP_REG, OP_NONE, OP_NONE }, 0 }, // 0x08
  { "inc", { OP_REG, OP_NONE, OP_NONE }, 1 }, // 0x09
  { "inc", { OP_REG, OP_NONE, OP_NONE }, 2 }, // 0x0A
  { "inc", { OP_REG, OP_NONE, OP_NONE }, 3 }, // 0x0B
  { "inc", { OP_REG, OP_NONE, OP_NONE }, 4 }, // 0x0C
  { "inc", { OP_REG, OP_NONE, OP_NONE }, 5 }, // 0x0D
  { "inc", { OP_REG, OP_NONE, OP_NONE }, 6 }, // 0x0E
  { "inc", { OP_REG, OP_NONE, OP_NONE }, 7 }, // 0x0F
  { "jbc", { OP_BIT_ADDR, OP_RELADDR, OP_NONE }, -1 }, // 0x10
  { "acall", { OP_PAGE, OP_NONE, OP_NONE }, 0 }, // 0x11
  { "lcall", { OP_CODE_ADDR, OP_NONE, OP_NONE }, -1 }, // 0x12
  { "rrc", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0x13
  { "dec", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0x14
  { "dec", { OP_IRAM_ADDR, OP_NONE, OP_NONE }, -1 }, // 0x15
  { "dec", { OP_AT_REG, OP_NONE, OP_NONE }, 0 }, // 0x16
  { "dec", { OP_AT_REG, OP_NONE, OP_NONE }, 1 }, // 0x17
  { "dec", { OP_REG, OP_NONE, OP_NONE }, 0 }, // 0x18
  { "dec", { OP_REG, OP_NONE, OP_NONE }, 1 }, // 0x19
  { "dec", { OP_REG, OP_NONE, OP_NONE }, 2 }, // 0x1A
  { "dec", { OP_REG, OP_NONE, OP_NONE }, 3 }, // 0x1B
  { "dec", { OP_REG, OP_NONE, OP_NONE }, 4 }, // 0x1C
  { "dec", { OP_REG, OP_NONE, OP_NONE }, 5 }, // 0x1D
  { "dec", { OP_REG, OP_NONE, OP_NONE }, 6 }, // 0x1E
  { "dec", { OP_REG, OP_NONE, OP_NONE }, 7 }, // 0x1F
  { "jb", { OP_BIT_ADDR, OP_RELADDR, OP_NONE }, -1 }, // 0x20
  { "ajmp", { OP_PAGE, OP_NONE, OP_NONE }, 1 }, // 0x21
  { "ret", { OP_NONE, OP_NONE, OP_NONE }, -1 }, // 0x22
  { "rl", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0x23
  { "add", { OP_A, OP_DATA, OP_NONE }, -1 }, // 0x24
  { "add", { OP_A, OP_IRAM_ADDR, OP_NONE }, -1 }, // 0x25
  { "add", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0x26
  { "add", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0x27
  { "add", { OP_A, OP_REG, OP_NONE }, 0 }, // 0x28
  { "add", { OP_A, OP_REG, OP_NONE }, 1 }, // 0x29
  { "add", { OP_A, OP_REG, OP_NONE }, 2 }, // 0x2A
  { "add", { OP_A, OP_REG, OP_NONE }, 3 }, // 0x2B
  { "add", { OP_A, OP_REG, OP_NONE }, 4 }, // 0x2C
  { "add", { OP_A, OP_REG, OP_NONE }, 5 }, // 0x2D
  { "add", { OP_A, OP_REG, OP_NONE }, 6 }, // 0x2E
  { "add", { OP_A, OP_REG, OP_NONE }, 7 }, // 0x2F
  { "jnb", { OP_BIT_ADDR, OP_RELADDR, OP_NONE }, -1 }, // 0x30
  { "acall", { OP_PAGE, OP_NONE, OP_NONE }, 1 }, // 0x31
  { "reti", { OP_NONE, OP_NONE, OP_NONE }, -1 }, // 0x32
  { "rlc", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0x33
  { "addc", { OP_A, OP_DATA, OP_NONE }, -1 }, // 0x34
  { "addc", { OP_A, OP_IRAM_ADDR, OP_NONE }, -1 }, // 0x35
  { "addc", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0x36
  { "addc", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0x37
  { "addc", { OP_A, OP_REG, OP_NONE }, 0 }, // 0x38
  { "addc", { OP_A, OP_REG, OP_NONE }, 1 }, // 0x39
  { "addc", { OP_A, OP_REG, OP_NONE }, 2 }, // 0x3A
  { "addc", { OP_A, OP_REG, OP_NONE }, 3 }, // 0x3B
  { "addc", { OP_A, OP_REG, OP_NONE }, 4 }, // 0x3C
  { "addc", { OP_A, OP_REG, OP_NONE }, 5 }, // 0x3D
  { "addc", { OP_A, OP_REG, OP_NONE }, 6 }, // 0x3E
  { "addc", { OP_A, OP_REG, OP_NONE }, 7 }, // 0x3F
  { "jc", { OP_RELADDR, OP_NONE, OP_NONE }, -1 }, // 0x40
  { "ajmp", { OP_PAGE, OP_NONE, OP_NONE }, 2 }, // 0x41
  { "orl", { OP_IRAM_ADDR, OP_A, OP_NONE }, -1 }, // 0x42
  { "orl", { OP_IRAM_ADDR, OP_DATA, OP_NONE }, -1 }, // 0x43
  { "orl", { OP_A, OP_DATA, OP_NONE }, -1 }, // 0x44
  { "orl", { OP_A, OP_IRAM_ADDR, OP_NONE }, -1 }, // 0x45
  { "orl", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0x46
  { "orl", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0x47
  { "orl", { OP_A, OP_REG, OP_NONE }, 0 }, // 0x48
  { "orl", { OP_A, OP_REG, OP_NONE }, 1 }, // 0x49
  { "orl", { OP_A, OP_REG, OP_NONE }, 2 }, // 0x4A
  { "orl", { OP_A, OP_REG, OP_NONE }, 3 }, // 0x4B
  { "orl", { OP_A, OP_REG, OP_NONE }, 4 }, // 0x4C
  { "orl", { OP_A, OP_REG, OP_NONE }, 5 }, // 0x4D
  { "orl", { OP_A, OP_REG, OP_NONE }, 6 }, // 0x4E
  { "orl", { OP_A, OP_REG, OP_NONE }, 7 }, // 0x4F
  { "jnc", { OP_RELADDR, OP_NONE, OP_NONE }, -1 }, // 0x50
  { "acall", { OP_PAGE, OP_NONE, OP_NONE }, 2 }, // 0x51
  { "anl", { OP_IRAM_ADDR, OP_A, OP_NONE }, -1 }, // 0x52
  { "anl", { OP_IRAM_ADDR, OP_DATA, OP_NONE }, -1 }, // 0x53
  { "anl", { OP_A, OP_DATA, OP_NONE }, -1 }, // 0x54
  { "anl", { OP_A, OP_IRAM_ADDR, OP_NONE }, -1 }, // 0x55
  { "anl", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0x56
  { "anl", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0x57
  { "anl", { OP_A, OP_REG, OP_NONE }, 0 }, // 0x58
  { "anl", { OP_A, OP_REG, OP_NONE }, 1 }, // 0x59
  { "anl", { OP_A, OP_REG, OP_NONE }, 2 }, // 0x5A
  { "anl", { OP_A, OP_REG, OP_NONE }, 3 }, // 0x5B
  { "anl", { OP_A, OP_REG, OP_NONE }, 4 }, // 0x5C
  { "anl", { OP_A, OP_REG, OP_NONE }, 5 }, // 0x5D
  { "anl", { OP_A, OP_REG, OP_NONE }, 6 }, // 0x5E
  { "anl", { OP_A, OP_REG, OP_NONE }, 7 }, // 0x5F
  { "jz", { OP_RELADDR, OP_NONE, OP_NONE }, -1 }, // 0x60
  { "ajmp", { OP_PAGE, OP_NONE, OP_NONE }, 3 }, // 0x61
  { "xrl", { OP_IRAM_ADDR, OP_A, OP_NONE }, -1 }, // 0x62
  { "xrl", { OP_IRAM_ADDR, OP_DATA, OP_NONE }, -1 }, // 0x63
  { "xrl", { OP_A, OP_DATA, OP_NONE }, -1 }, // 0x64
  { "xrl", { OP_A, OP_IRAM_ADDR, OP_NONE }, -1 }, // 0x65
  { "xrl", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0x66
  { "xrl", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0x67
  { "xrl", { OP_A, OP_REG, OP_NONE }, 0 }, // 0x68
  { "xrl", { OP_A, OP_REG, OP_NONE }, 1 }, // 0x69
  { "xrl", { OP_A, OP_REG, OP_NONE }, 2 }, // 0x6A
  { "xrl", { OP_A, OP_REG, OP_NONE }, 3 }, // 0x6B
  { "xrl", { OP_A, OP_REG, OP_NONE }, 4 }, // 0x6C
  { "xrl", { OP_A, OP_REG, OP_NONE }, 5 }, // 0x6D
  { "xrl", { OP_A, OP_REG, OP_NONE }, 6 }, // 0x6E
  { "xrl", { OP_A, OP_REG, OP_NONE }, 7 }, // 0x6F
  { "jnz", { OP_RELADDR, OP_NONE, OP_NONE }, -1 }, // 0x70
  { "acall", { OP_PAGE, OP_NONE, OP_NONE }, 3 }, // 0x71
  { "orl", { OP_C, OP_BIT_ADDR, OP_NONE }, -1 }, // 0x72
  { "jmp", { OP_AT_A_PLUS_DPTR, OP_NONE, OP_NONE }, -1 }, // 0x73
  { "mov", { OP_A, OP_DATA, OP_NONE }, -1 }, // 0x74
  { "mov", { OP_IRAM_ADDR, OP_DATA, OP_NONE }, -1 }, // 0x75
  { "mov", { OP_AT_REG, OP_DATA, OP_NONE }, 0 }, // 0x76
  { "mov", { OP_AT_REG, OP_DATA, OP_NONE }, 1 }, // 0x77
  { "mov", { OP_REG, OP_DATA, OP_NONE }, 0 }, // 0x78
  { "mov", { OP_REG, OP_DATA, OP_NONE }, 1 }, // 0x79
  { "mov", { OP_REG, OP_DATA, OP_NONE }, 2 }, // 0x7A
  { "mov", { OP_REG, OP_DATA, OP_NONE }, 3 }, // 0x7B
  { "mov", { OP_REG, OP_DATA, OP_NONE }, 4 }, // 0x7C
  { "mov", { OP_REG, OP_DATA, OP_NONE }, 5 }, // 0x7D
  { "mov", { OP_REG, OP_DATA, OP_NONE }, 6 }, // 0x7E
  { "mov", { OP_REG, OP_DATA, OP_NONE }, 7 }, // 0x7F
  { "sjmp", { OP_RELADDR, OP_NONE, OP_NONE }, -1 }, // 0x80
  { "ajmp", { OP_PAGE, OP_NONE, OP_NONE }, 4 }, // 0x81
  { "anl", { OP_C, OP_BIT_ADDR, OP_NONE }, -1 }, // 0x82
  { "movc", { OP_A, OP_AT_A_PLUS_PC, OP_NONE }, -1 }, // 0x83
  { "div", { OP_AB, OP_NONE, OP_NONE }, -1 }, // 0x84
  { "mov", { OP_IRAM_ADDR, OP_IRAM_ADDR, OP_NONE }, -1 }, // 0x85
  { "mov", { OP_IRAM_ADDR, OP_AT_REG, OP_NONE }, 0 }, // 0x86
  { "mov", { OP_IRAM_ADDR, OP_AT_REG, OP_NONE }, 1 }, // 0x87
  { "mov", { OP_IRAM_ADDR, OP_REG, OP_NONE }, 0 }, // 0x88
  { "mov", { OP_IRAM_ADDR, OP_REG, OP_NONE }, 1 }, // 0x89
  { "mov", { OP_IRAM_ADDR, OP_REG, OP_NONE }, 2 }, // 0x8A
  { "mov", { OP_IRAM_ADDR, OP_REG, OP_NONE }, 3 }, // 0x8B
  { "mov", { OP_IRAM_ADDR, OP_REG, OP_NONE }, 4 }, // 0x8C
  { "mov", { OP_IRAM_ADDR, OP_REG, OP_NONE }, 5 }, // 0x8D
  { "mov", { OP_IRAM_ADDR, OP_REG, OP_NONE }, 6 }, // 0x8E
  { "mov", { OP_IRAM_ADDR, OP_REG, OP_NONE }, 7 }, // 0x8F
  { "mov", { OP_DPTR, OP_DATA_16, OP_NONE }, -1 }, // 0x90
  { "acall", { OP_PAGE, OP_NONE, OP_NONE }, 4 }, // 0x91
  { "mov", { OP_BIT_ADDR, OP_C, OP_NONE }, -1 }, // 0x92
  { "movc", { OP_A, OP_AT_A_PLUS_DPTR, OP_NONE }, -1 }, // 0x93
  { "subb", { OP_A, OP_DATA, OP_NONE }, -1 }, // 0x94
  { "subb", { OP_A, OP_IRAM_ADDR, OP_NONE }, -1 }, // 0x95
  { "subb", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0x96
  { "subb", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0x97
  { "subb", { OP_A, OP_REG, OP_NONE }, 0 }, // 0x98
  { "subb", { OP_A, OP_REG, OP_NONE }, 1 }, // 0x99
  { "subb", { OP_A, OP_REG, OP_NONE }, 2 }, // 0x9A
  { "subb", { OP_A, OP_REG, OP_NONE }, 3 }, // 0x9B
  { "subb", { OP_A, OP_REG, OP_NONE }, 4 }, // 0x9C
  { "subb", { OP_A, OP_REG, OP_NONE }, 5 }, // 0x9D
  { "subb", { OP_A, OP_REG, OP_NONE }, 6 }, // 0x9E
  { "subb", { OP_A, OP_REG, OP_NONE }, 7 }, // 0x9F
  { "orl", { OP_C, OP_SLASH_BIT_ADDR, OP_NONE }, -1 }, // 0xA0
  { "ajmp", { OP_PAGE, OP_NONE, OP_NONE }, 5 }, // 0xA1
  { "mov", { OP_C, OP_BIT_ADDR, OP_NONE }, -1 }, // 0xA2
  { "inc", { OP_DPTR, OP_NONE, OP_NONE }, -1 }, // 0xA3
  { "mul", { OP_AB, OP_NONE, OP_NONE }, -1 }, // 0xA4
  { "???", { OP_NONE, OP_NONE, OP_NONE }, -1 }, // 0xA5
  { "mov", { OP_AT_REG, OP_IRAM_ADDR, OP_NONE }, 0 }, // 0xA6
  { "mov", { OP_AT_REG, OP_IRAM_ADDR, OP_NONE }, 1 }, // 0xA7
  { "mov", { OP_REG, OP_IRAM_ADDR, OP_NONE }, 0 }, // 0xA8
  { "mov", { OP_REG, OP_IRAM_ADDR, OP_NONE }, 1 }, // 0xA9
  { "mov", { OP_REG, OP_IRAM_ADDR, OP_NONE }, 2 }, // 0xAA
  { "mov", { OP_REG, OP_IRAM_ADDR, OP_NONE }, 3 }, // 0xAB
  { "mov", { OP_REG, OP_IRAM_ADDR, OP_NONE }, 4 }, // 0xAC
  { "mov", { OP_REG, OP_IRAM_ADDR, OP_NONE }, 5 }, // 0xAD
  { "mov", { OP_REG, OP_IRAM_ADDR, OP_NONE }, 6 }, // 0xAE
  { "mov", { OP_REG, OP_IRAM_ADDR, OP_NONE }, 7 }, // 0xAF
  { "anl", { OP_C, OP_SLASH_BIT_ADDR, OP_NONE }, -1 }, // 0xB0
  { "acall", { OP_PAGE, OP_NONE, OP_NONE }, 5 }, // 0xB1
  { "cpl", { OP_BIT_ADDR, OP_NONE, OP_NONE }, -1 }, // 0xB2
  { "cpl", { OP_C, OP_NONE, OP_NONE }, -1 }, // 0xB3
  { "cjne", { OP_A, OP_DATA, OP_RELADDR }, -1 }, // 0xB4
  { "cjne", { OP_A, OP_IRAM_ADDR, OP_RELADDR }, -1 }, // 0xB5
  { "cjne", { OP_AT_REG, OP_DATA, OP_RELADDR }, 0 }, // 0xB6
  { "cjne", { OP_AT_REG, OP_DATA, OP_RELADDR }, 1 }, // 0xB7
  { "cjne", { OP_REG, OP_DATA, OP_RELADDR }, 0 }, // 0xB8
  { "cjne", { OP_REG, OP_DATA, OP_RELADDR }, 1 }, // 0xB9
  { "cjne", { OP_REG, OP_DATA, OP_RELADDR }, 2 }, // 0xBA
  { "cjne", { OP_REG, OP_DATA, OP_RELADDR }, 3 }, // 0xBB
  { "cjne", { OP_REG, OP_DATA, OP_RELADDR }, 4 }, // 0xBC
  { "cjne", { OP_REG, OP_DATA, OP_RELADDR }, 5 }, // 0xBD
  { "cjne", { OP_REG, OP_DATA, OP_RELADDR }, 6 }, // 0xBE
  { "cjne", { OP_REG, OP_DATA, OP_RELADDR }, 7 }, // 0xBF
  { "push", { OP_IRAM_ADDR, OP_NONE, OP_NONE }, -1 }, // 0xC0
  { "ajmp", { OP_PAGE, OP_NONE, OP_NONE }, 6 }, // 0xC1
  { "clr", { OP_BIT_ADDR, OP_NONE, OP_NONE }, -1 }, // 0xC2
  { "clr", { OP_C, OP_NONE, OP_NONE }, -1 }, // 0xC3
  { "swap", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0xC4
  { "xch", { OP_A, OP_IRAM_ADDR, OP_NONE }, -1 }, // 0xC5
  { "xch", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0xC6
  { "xch", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0xC7
  { "xch", { OP_A, OP_REG, OP_NONE }, 0 }, // 0xC8
  { "xch", { OP_A, OP_REG, OP_NONE }, 1 }, // 0xC9
  { "xch", { OP_A, OP_REG, OP_NONE }, 2 }, // 0xCA
  { "xch", { OP_A, OP_REG, OP_NONE }, 3 }, // 0xCB
  { "xch", { OP_A, OP_REG, OP_NONE }, 4 }, // 0xCC
  { "xch", { OP_A, OP_REG, OP_NONE }, 5 }, // 0xCD
  { "xch", { OP_A, OP_REG, OP_NONE }, 6 }, // 0xCE
  { "xch", { OP_A, OP_REG, OP_NONE }, 7 }, // 0xCF
  { "pop", { OP_IRAM_ADDR, OP_NONE, OP_NONE }, -1 }, // 0xD0
  { "acall", { OP_PAGE, OP_NONE, OP_NONE }, 6 }, // 0xD1
  { "setb", { OP_BIT_ADDR, OP_NONE, OP_NONE }, -1 }, // 0xD2
  { "setb", { OP_C, OP_NONE, OP_NONE }, -1 }, // 0xD3
  { "da", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0xD4
  { "djnz", { OP_IRAM_ADDR, OP_RELADDR, OP_NONE }, -1 }, // 0xD5
  { "xchd", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0xD6
  { "xchd", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0xD7
  { "djnz", { OP_REG, OP_RELADDR, OP_NONE }, 0 }, // 0xD8
  { "djnz", { OP_REG, OP_RELADDR, OP_NONE }, 1 }, // 0xD9
  { "djnz", { OP_REG, OP_RELADDR, OP_NONE }, 2 }, // 0xDA
  { "djnz", { OP_REG, OP_RELADDR, OP_NONE }, 3 }, // 0xDB
  { "djnz", { OP_REG, OP_RELADDR, OP_NONE }, 4 }, // 0xDC
  { "djnz", { OP_REG, OP_RELADDR, OP_NONE }, 5 }, // 0xDD
  { "djnz", { OP_REG, OP_RELADDR, OP_NONE }, 6 }, // 0xDE
  { "djnz", { OP_REG, OP_RELADDR, OP_NONE }, 7 }, // 0xDF
  { "movx", { OP_A, OP_AT_DPTR, OP_NONE }, -1 }, // 0xE0
  { "ajmp", { OP_PAGE, OP_NONE, OP_NONE }, 7 }, // 0xE1
  { "movx", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0xE2
  { "movx", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0xE3
  { "clr", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0xE4
  { "mov", { OP_A, OP_IRAM_ADDR, OP_NONE }, -1 }, // 0xE5
  { "mov", { OP_A, OP_AT_REG, OP_NONE }, 0 }, // 0xE6
  { "mov", { OP_A, OP_AT_REG, OP_NONE }, 1 }, // 0xE7
  { "mov", { OP_A, OP_REG, OP_NONE }, 0 }, // 0xE8
  { "mov", { OP_A, OP_REG, OP_NONE }, 1 }, // 0xE9
  { "mov", { OP_A, OP_REG, OP_NONE }, 2 }, // 0xEA
  { "mov", { OP_A, OP_REG, OP_NONE }, 3 }, // 0xEB
  { "mov", { OP_A, OP_REG, OP_NONE }, 4 }, // 0xEC
  { "mov", { OP_A, OP_REG, OP_NONE }, 5 }, // 0xED
  { "mov", { OP_A, OP_REG, OP_NONE }, 6 }, // 0xEE
  { "mov", { OP_A, OP_REG, OP_NONE }, 7 }, // 0xEF
  { "movx", { OP_AT_DPTR, OP_A, OP_NONE }, -1 }, // 0xF0
  { "acall", { OP_PAGE, OP_NONE, OP_NONE }, 7 }, // 0xF1
  { "movx", { OP_AT_REG, OP_A, OP_NONE }, 0 }, // 0xF2
  { "movx", { OP_AT_REG, OP_A, OP_NONE }, 1 }, // 0xF3
  { "cpl", { OP_A, OP_NONE, OP_NONE }, -1 }, // 0xF4
  { "mov", { OP_IRAM_ADDR, OP_A, OP_NONE }, -1 }, // 0xF5
  { "mov", { OP_AT_REG, OP_A, OP_NONE }, 0 }, // 0xF6
  { "mov", { OP_AT_REG, OP_A, OP_NONE }, 1 }, // 0xF7
  { "mov", { OP_REG, OP_A, OP_NONE }, 0 }, // 0xF8
  { "mov", { OP_REG, OP_A, OP_NONE }, 1 }, // 0xF9
  { "mov", { OP_REG, OP_A, OP_NONE }, 2 }, // 0xFA
  { "mov", { OP_REG, OP_A, OP_NONE }, 3 }, // 0xFB
  { "mov", { OP_REG, OP_A, OP_NONE }, 4 }, // 0xFC
  { "mov", { OP_REG, OP_A, OP_NONE }, 5 }, // 0xFD
  { "mov", { OP_REG, OP_A, OP_NONE }, 6 }, // 0xFE
  { "mov", { OP_REG, OP_A, OP_NONE }, 7 }, // 0xFF
};

struct _address_map address_map[] =
{
  { "B", 0xf0, 1 },
  { "ACC", 0xe0, 1 },
  { "PSW", 0xd0, 1 },
  { "T2CON", 0xc8, 0 },
  { "T2MOD", 0xc9 , 0},
  { "RCAP2L", 0xca, 0 },
  { "RCAP2H", 0xcb, 0 },
  { "TL2", 0xcc, 0 },
  { "TH2", 0xcd, 0 },
  { "IP", 0xb8, 1 },
  { "P3", 0xb0, 1 },
  { "IE", 0xa8, 1 },
  { "P2", 0xa0, 1 },
  { "AUXR1", 0xa2, 0 },
  { "WDTRST", 0xa6, 0 },
  { "SCON", 0x98, 1 },
  { "SBUF", 0x99, 0 },
  { "P1", 0x90, 1 },
  { "TCON", 0x88, 1 },
  { "TMOD", 0x89, 0 },
  { "TL0", 0x8a, 0 },
  { "TL1", 0x8b, 0 },
  { "TH0", 0x8c, 0 },
  { "TH1", 0x8d, 0 },
  { "AUXR", 0x8e, 0 },
  { "P0", 0x80, 1 },
  { "SP", 0x81, 0 },
  { "DPL", 0x82, 0 },
  { "DPH", 0x83, 0 },
  { "DP0L", 0x82, 0 },
  { "DP0H", 0x83, 0 },
  { "DP1L", 0x84, 0 },
  { "DP1H", 0x85, 0 },
  { "PCON", 0x87, 0 },
  { NULL },
};

struct _address_map address_map_psw[] =
{
  { "CY", 0xd7 },
  { "AC", 0xd6 },
  { "F0", 0xd5 },
  { "RS1", 0xd4 },
  { "RS0", 0xd3 },
  { "OV", 0xd2 },
  { "UD", 0xd1 },
  { "P", 0xd0 },
  { NULL },
};
