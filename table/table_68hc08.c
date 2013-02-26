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

#include <stdlib.h>

#include "table_68hc08.h"

struct _m68hc08_table m68hc08_table[] = {
  { "BRSET", CPU08_OP_0_COMMA_OPR_REL, 5 },  // 0x00 0x00
  { "BRCLR", CPU08_OP_0_COMMA_OPR_REL, 5 },  // 0x01 0x01
  { "BRSET", CPU08_OP_1_COMMA_OPR_REL, 5 },  // 0x02 0x02
  { "BRCLR", CPU08_OP_1_COMMA_OPR_REL, 5 },  // 0x03 0x03
  { "BRSET", CPU08_OP_2_COMMA_OPR_REL, 5 },  // 0x04 0x04
  { "BRCLR", CPU08_OP_2_COMMA_OPR_REL, 5 },  // 0x05 0x05
  { "BRSET", CPU08_OP_3_COMMA_OPR_REL, 5 },  // 0x06 0x06
  { "BRCLR", CPU08_OP_3_COMMA_OPR_REL, 5 },  // 0x07 0x07
  { "BRSET", CPU08_OP_4_COMMA_OPR_REL, 5 },  // 0x08 0x08
  { "BRCLR", CPU08_OP_4_COMMA_OPR_REL, 5 },  // 0x09 0x09
  { "BRSET", CPU08_OP_5_COMMA_OPR_REL, 5 },  // 0x0a 0x0a
  { "BRCLR", CPU08_OP_5_COMMA_OPR_REL, 5 },  // 0x0b 0x0b
  { "BRSET", CPU08_OP_6_COMMA_OPR_REL, 5 },  // 0x0c 0x0c
  { "BRCLR", CPU08_OP_6_COMMA_OPR_REL, 5 },  // 0x0d 0x0d
  { "BRSET", CPU08_OP_7_COMMA_OPR_REL, 5 },  // 0x0e 0x0e
  { "BRCLR", CPU08_OP_7_COMMA_OPR_REL, 5 },  // 0x0f 0x0f
  { "BSET", CPU08_OP_0_COMMA_OPR, 4 },  // 0x10 0x10
  { "BCLR", CPU08_OP_0_COMMA_OPR, 4 },  // 0x11 0x11
  { "BSET", CPU08_OP_1_COMMA_OPR, 4 },  // 0x12 0x12
  { "BCLR", CPU08_OP_1_COMMA_OPR, 4 },  // 0x13 0x13
  { "BSET", CPU08_OP_2_COMMA_OPR, 4 },  // 0x14 0x14
  { "BCLR", CPU08_OP_2_COMMA_OPR, 4 },  // 0x15 0x15
  { "BSET", CPU08_OP_3_COMMA_OPR, 4 },  // 0x16 0x16
  { "BCLR", CPU08_OP_3_COMMA_OPR, 4 },  // 0x17 0x17
  { "BSET", CPU08_OP_4_COMMA_OPR, 4 },  // 0x18 0x18
  { "BCLR", CPU08_OP_4_COMMA_OPR, 4 },  // 0x19 0x19
  { "BSET", CPU08_OP_5_COMMA_OPR, 4 },  // 0x1a 0x1a
  { "BCLR", CPU08_OP_5_COMMA_OPR, 4 },  // 0x1b 0x1b
  { "BSET", CPU08_OP_6_COMMA_OPR, 4 },  // 0x1c 0x1c
  { "BCLR", CPU08_OP_6_COMMA_OPR, 4 },  // 0x1d 0x1d
  { "BSET", CPU08_OP_7_COMMA_OPR, 4 },  // 0x1e 0x1e
  { "BCLR", CPU08_OP_7_COMMA_OPR, 4 },  // 0x1f 0x1f
  { "BRA", CPU08_OP_REL, 3 },  // 0x20 0x20
  { "BRN", CPU08_OP_REL, 3 },  // 0x21 0x21
  { "BHI", CPU08_OP_REL, 3 },  // 0x22 0x22
  { "BLS", CPU08_OP_REL, 3 },  // 0x23 0x23
  { "BCC", CPU08_OP_REL, 3 },  // 0x24 0x24
  { "BHS", CPU08_OP_REL, 3 },  // 0x24 0x24
  { "BCS", CPU08_OP_REL, 3 },  // 0x25 0x25
  { "BLO", CPU08_OP_REL, 3 },  // 0x25 0x25
  { "BNE", CPU08_OP_REL, 3 },  // 0x26 0x26
  { "BEQ", CPU08_OP_REL, 3 },  // 0x27 0x27
  { "BHCC", CPU08_OP_REL, 3 },  // 0x28 0x28
  { "BHCS", CPU08_OP_REL, 3 },  // 0x29 0x29
  { "BPL", CPU08_OP_REL, 3 },  // 0x2a 0x2a
  { "BMI", CPU08_OP_REL, 3 },  // 0x2b 0x2b
  { "BMC", CPU08_OP_REL, 3 },  // 0x2c 0x2c
  { "BMS", CPU08_OP_REL, 3 },  // 0x2d 0x2d
  { "BIL", CPU08_OP_REL, 3 },  // 0x2e 0x2e
  { "BIH", CPU08_OP_REL, 3 },  // 0x2f 0x2f
  { "NEG", CPU08_OP_OPR8, 4 },  // 0x30 0x30
  { "CBEQ", CPU08_OP_OPR8_REL, 5 },  // 0x31 0x31
  { NULL, CPU08_OP_NONE, 0 },  // 0x32 NONE
  { "COM", CPU08_OP_OPR8, 4 },  // 0x33 0x33
  { "LSR", CPU08_OP_OPR8, 4 },  // 0x34 0x34
  { "STHX", CPU08_OP_OPR8, 4 },  // 0x35 0x35
  { "ROR", CPU08_OP_OPR8, 4 },  // 0x36 0x36
  { "ASR", CPU08_OP_OPR8, 4 },  // 0x37 0x37
  { "ASL", CPU08_OP_OPR8, 4 },  // 0x38 0x38
  { "LSL", CPU08_OP_OPR8, 4 },  // 0x38 0x38
  { "ROL", CPU08_OP_OPR8, 4 },  // 0x39 0x39
  { "DEC", CPU08_OP_OPR8, 4 },  // 0x3a 0x3a
  { "DBNZ", CPU08_OP_OPR8_REL, 5 },  // 0x3b 0x3b
  { "INC", CPU08_OP_OPR8, 4 },  // 0x3c 0x3c
  { "TST", CPU08_OP_OPR8, 3 },  // 0x3d 0x3d
  { NULL, CPU08_OP_NONE, 0 },  // 0x3e NONE
  { "CLR", CPU08_OP_OPR8, 3 },  // 0x3f 0x3f
  { "NEGA", CPU08_OP_NONE, 1 },  // 0x40 0x40
  { "CBEQA", CPU08_OP_NUM8_REL, 4 },  // 0x41 0x41
  { "MUL", CPU08_OP_NONE, 5 },  // 0x42 0x42
  { "COMA", CPU08_OP_NONE, 1 },  // 0x43 0x43
  { "LSRA", CPU08_OP_NONE, 1 },  // 0x44 0x44
  { "LDHX", CPU08_OP_NUM16, 3 },  // 0x45 0x45
  { "RORA", CPU08_OP_NONE, 1 },  // 0x46 0x46
  { "ASRA", CPU08_OP_NONE, 1 },  // 0x47 0x47
  { "ASLA", CPU08_OP_NONE, 1 },  // 0x48 0x48
  { "LSLA", CPU08_OP_NONE, 1 },  // 0x48 0x48
  { "ROLA", CPU08_OP_NONE, 1 },  // 0x49 0x49
  { "DECA", CPU08_OP_NONE, 1 },  // 0x4a 0x4a
  { "DBNZA", CPU08_OP_REL, 3 },  // 0x4b 0x4b
  { "INCA", CPU08_OP_NONE, 1 },  // 0x4c 0x4c
  { "TSTA", CPU08_OP_NONE, 1 },  // 0x4d 0x4d
  { "MOV", CPU08_OP_OPR8_OPR8, 5 },  // 0x4e 0x4e
  { "CLRA", CPU08_OP_NONE, 1 },  // 0x4f 0x4f
  { "NEGX", CPU08_OP_NONE, 1 },  // 0x50 0x50
  { "CBEQX", CPU08_OP_NUM8_REL, 4 },  // 0x51 0x51
  { "DIV", CPU08_OP_NONE, 7 },  // 0x52 0x52
  { "COMX", CPU08_OP_NONE, 1 },  // 0x53 0x53
  { "LSRX", CPU08_OP_NONE, 1 },  // 0x54 0x54
  { "LDHX", CPU08_OP_OPR8, 4 },  // 0x55 0x55
  { "RORX", CPU08_OP_NONE, 1 },  // 0x56 0x56
  { "ASRX", CPU08_OP_NONE, 1 },  // 0x57 0x57
  { "ASLX", CPU08_OP_NONE, 1 },  // 0x58 0x58
  { "LSLX", CPU08_OP_NONE, 1 },  // 0x58 0x58
  { "ROLX", CPU08_OP_NONE, 1 },  // 0x59 0x59
  { "DECX", CPU08_OP_NONE, 1 },  // 0x5a 0x5a
  { "DBNZX", CPU08_OP_REL, 3 },  // 0x5b 0x5b
  { "INCX", CPU08_OP_NONE, 1 },  // 0x5c 0x5c
  { "TSTX", CPU08_OP_NONE, 1 },  // 0x5d 0x5d
  { "MOV", CPU08_OP_OPR8_X_PLUS, 4 },  // 0x5e 0x5e
  { "CLRX", CPU08_OP_NONE, 1 },  // 0x5f 0x5f
  { "NEG", CPU08_OP_OPR8_X, 4 },  // 0x60 0x60
  { "CBEQ", CPU08_OP_OPR8_X_PLUS_REL, 5 },  // 0x61 0x61
  { "NSA", CPU08_OP_NONE, 3 },  // 0x62 0x62
  { "COM", CPU08_OP_OPR8_X, 4 },  // 0x63 0x63
  { "LSR", CPU08_OP_OPR8_X, 4 },  // 0x64 0x64
  { "CPHX", CPU08_OP_NUM16, 3 },  // 0x65 0x65
  { "ROR", CPU08_OP_OPR8_X, 4 },  // 0x66 0x66
  { "ASR", CPU08_OP_OPR8_X, 4 },  // 0x67 0x67
  { "ASL", CPU08_OP_OPR8_X, 4 },  // 0x68 0x68
  { "LSL", CPU08_OP_OPR8_X, 4 },  // 0x68 0x68
  { "ROL", CPU08_OP_OPR8_X, 4 },  // 0x69 0x69
  { "DEC", CPU08_OP_OPR8_X, 4 },  // 0x6a 0x6a
  { "DBNZ", CPU08_OP_OPR8_X_REL, 5 },  // 0x6b 0x6b
  { "INC", CPU08_OP_OPR8_X, 4 },  // 0x6c 0x6c
  { "TST", CPU08_OP_OPR8_X, 3 },  // 0x6d 0x6d
  { "MOV", CPU08_OP_NUM8_OPR8, 4 },  // 0x6e 0x6e
  { "CLR", CPU08_OP_OPR8_X, 3 },  // 0x6f 0x6f
  { "NEG", CPU08_OP_COMMA_X, 3 },  // 0x70 0x70
  { "CBEQ", CPU08_OP_X_PLUS_REL, 4 },  // 0x71 0x71
  { "DAA", CPU08_OP_NONE, 2 },  // 0x72 0x72
  { "COM", CPU08_OP_COMMA_X, 3 },  // 0x73 0x73
  { "LSR", CPU08_OP_COMMA_X, 3 },  // 0x74 0x74
  { "CPHX", CPU08_OP_OPR8, 4 },  // 0x75 0x75
  { "ROR", CPU08_OP_COMMA_X, 3 },  // 0x76 0x76
  { "ASR", CPU08_OP_COMMA_X, 3 },  // 0x77 0x77
  { "ASL", CPU08_OP_COMMA_X, 3 },  // 0x78 0x78
  { "LSL", CPU08_OP_COMMA_X, 3 },  // 0x78 0x78
  { "ROL", CPU08_OP_COMMA_X, 3 },  // 0x79 0x79
  { "DEC", CPU08_OP_COMMA_X, 3 },  // 0x7a 0x7a
  { "DBNZ", CPU08_OP_X_REL, 4 },  // 0x7b 0x7b
  { "INC", CPU08_OP_COMMA_X, 3 },  // 0x7c 0x7c
  { "TST", CPU08_OP_COMMA_X, 2 },  // 0x7d 0x7d
  { "MOV", CPU08_OP_X_PLUS_OPR8, 4 },  // 0x7e 0x7e
  { "CLR", CPU08_OP_COMMA_X, 2 },  // 0x7f 0x7f
  { "RTI", CPU08_OP_NONE, 7 },  // 0x80 0x80
  { "RTS", CPU08_OP_NONE, 4 },  // 0x81 0x81
  { NULL, CPU08_OP_NONE, 0 },  // 0x82 NONE
  { "SWI", CPU08_OP_NONE, 9 },  // 0x83 0x83
  { "TAP", CPU08_OP_NONE, 2 },  // 0x84 0x84
  { "TPA", CPU08_OP_NONE, 1 },  // 0x85 0x85
  { "PULA", CPU08_OP_NONE, 2 },  // 0x86 0x86
  { "PSHA", CPU08_OP_NONE, 2 },  // 0x87 0x87
  { "PULX", CPU08_OP_NONE, 2 },  // 0x88 0x88
  { "PSHX", CPU08_OP_NONE, 2 },  // 0x89 0x89
  { "PULH", CPU08_OP_NONE, 2 },  // 0x8a 0x8a
  { "PSHH", CPU08_OP_NONE, 2 },  // 0x8b 0x8b
  { "CLRH", CPU08_OP_NONE, 1 },  // 0x8c 0x8c
  { NULL, CPU08_OP_NONE, 0 },  // 0x8d NONE
  { "STOP", CPU08_OP_NONE, 1 },  // 0x8e 0x8e
  { "WAIT", CPU08_OP_NONE, 1 },  // 0x8f 0x8f
  { "BGE", CPU08_OP_REL, 3 },  // 0x90 0x90
  { "BLT", CPU08_OP_REL, 3 },  // 0x91 0x91
  { "BGT", CPU08_OP_REL, 3 },  // 0x92 0x92
  { "BLE", CPU08_OP_REL, 3 },  // 0x93 0x93
  { "TXS", CPU08_OP_NONE, 2 },  // 0x94 0x94
  { "TSX", CPU08_OP_NONE, 2 },  // 0x95 0x95
  { NULL, CPU08_OP_NONE, 0 },  // 0x96 NONE
  { "TAX", CPU08_OP_NONE, 1 },  // 0x97 0x97
  { "CLC", CPU08_OP_NONE, 1 },  // 0x98 0x98
  { "SEC", CPU08_OP_NONE, 1 },  // 0x99 0x99
  { "CLI", CPU08_OP_NONE, 2 },  // 0x9a 0x9a
  { "SEI", CPU08_OP_NONE, 2 },  // 0x9b 0x9b
  { "RSP", CPU08_OP_NONE, 1 },  // 0x9c 0x9c
  { "NOP", CPU08_OP_NONE, 1 },  // 0x9d 0x9d
  { NULL, CPU08_OP_NONE, 0 },  // 0x9e NONE
  { "TXA", CPU08_OP_NONE, 1 },  // 0x9f 0x9f
  { "SUB", CPU08_OP_NUM8, 2 },  // 0xa0 0xa0
  { "CMP", CPU08_OP_NUM8, 2 },  // 0xa1 0xa1
  { "SBC", CPU08_OP_NUM8, 2 },  // 0xa2 0xa2
  { "CPX", CPU08_OP_NUM8, 2 },  // 0xa3 0xa3
  { "AND", CPU08_OP_NUM8, 2 },  // 0xa4 0xa4
  { "BIT", CPU08_OP_NUM8, 2 },  // 0xa5 0xa5
  { "LDA", CPU08_OP_NUM8, 2 },  // 0xa6 0xa6
  { "AIS", CPU08_OP_NUM8, 2 },  // 0xa7 0xa7
  { "EOR", CPU08_OP_NUM8, 2 },  // 0xa8 0xa8
  { "ADC", CPU08_OP_NUM8, 2 },  // 0xa9 0xa9
  { "ORA", CPU08_OP_NUM8, 2 },  // 0xaa 0xaa
  { "ADD", CPU08_OP_NUM8, 2 },  // 0xab 0xab
  { NULL, CPU08_OP_NONE, 0 },  // 0xac NONE
  { "BSR", CPU08_OP_REL, 4 },  // 0xad 0xad
  { "LDX", CPU08_OP_NUM8, 2 },  // 0xae 0xae
  { "AIX", CPU08_OP_NUM8, 2 },  // 0xaf 0xaf
  { "SUB", CPU08_OP_OPR8, 3 },  // 0xb0 0xb0
  { "CMP", CPU08_OP_OPR8, 3 },  // 0xb1 0xb1
  { "SBC", CPU08_OP_OPR8, 3 },  // 0xb2 0xb2
  { "CPX", CPU08_OP_OPR8, 3 },  // 0xb3 0xb3
  { "AND", CPU08_OP_OPR8, 3 },  // 0xb4 0xb4
  { "BIT", CPU08_OP_OPR8, 3 },  // 0xb5 0xb5
  { "LDA", CPU08_OP_OPR8, 3 },  // 0xb6 0xb6
  { "STA", CPU08_OP_OPR8, 3 },  // 0xb7 0xb7
  { "EOR", CPU08_OP_OPR8, 3 },  // 0xb8 0xb8
  { "ADC", CPU08_OP_OPR8, 3 },  // 0xb9 0xb9
  { "ORA", CPU08_OP_OPR8, 3 },  // 0xba 0xba
  { "ADD", CPU08_OP_OPR8, 3 },  // 0xbb 0xbb
  { "JMP", CPU08_OP_OPR8, 2 },  // 0xbc 0xbc
  { "JSR", CPU08_OP_OPR8, 4 },  // 0xbd 0xbd
  { "LDX", CPU08_OP_OPR8, 3 },  // 0xbe 0xbe
  { "STX", CPU08_OP_OPR8, 3 },  // 0xbf 0xbf
  { "SUB", CPU08_OP_OPR16, 4 },  // 0xc0 0xc0
  { "CMP", CPU08_OP_OPR16, 4 },  // 0xc1 0xc1
  { "SBC", CPU08_OP_OPR16, 4 },  // 0xc2 0xc2
  { "CPX", CPU08_OP_OPR16, 4 },  // 0xc3 0xc3
  { "AND", CPU08_OP_OPR16, 4 },  // 0xc4 0xc4
  { "BIT", CPU08_OP_OPR16, 4 },  // 0xc5 0xc5
  { "LDA", CPU08_OP_OPR16, 4 },  // 0xc6 0xc6
  { "STA", CPU08_OP_OPR16, 4 },  // 0xc7 0xc7
  { "EOR", CPU08_OP_OPR16, 4 },  // 0xc8 0xc8
  { "ADC", CPU08_OP_OPR16, 4 },  // 0xc9 0xc9
  { "ORA", CPU08_OP_OPR16, 4 },  // 0xca 0xca
  { "ADD", CPU08_OP_OPR16, 4 },  // 0xcb 0xcb
  { "JMP", CPU08_OP_OPR16, 3 },  // 0xcc 0xcc
  { "JSR", CPU08_OP_OPR16, 5 },  // 0xcd 0xcd
  { "LDX", CPU08_OP_OPR16, 4 },  // 0xce 0xce
  { "STX", CPU08_OP_OPR16, 4 },  // 0xcf 0xcf
  { "SUB", CPU08_OP_OPR16_X, 4 },  // 0xd0 0xd0
  { "CMP", CPU08_OP_OPR16_X, 4 },  // 0xd1 0xd1
  { "SBC", CPU08_OP_OPR16_X, 4 },  // 0xd2 0xd2
  { "CPX", CPU08_OP_OPR16_X, 4 },  // 0xd3 0xd3
  { "AND", CPU08_OP_OPR16_X, 4 },  // 0xd4 0xd4
  { "BIT", CPU08_OP_OPR16_X, 4 },  // 0xd5 0xd5
  { "LDA", CPU08_OP_OPR16_X, 4 },  // 0xd6 0xd6
  { "STA", CPU08_OP_OPR16_X, 4 },  // 0xd7 0xd7
  { "EOR", CPU08_OP_OPR16_X, 4 },  // 0xd8 0xd8
  { "ADC", CPU08_OP_OPR16_X, 4 },  // 0xd9 0xd9
  { "ORA", CPU08_OP_OPR16_X, 4 },  // 0xda 0xda
  { "ADD", CPU08_OP_OPR16_X, 4 },  // 0xdb 0xdb
  { "JMP", CPU08_OP_OPR16_X, 4 },  // 0xdc 0xdc
  { "JSR", CPU08_OP_OPR16_X, 6 },  // 0xdd 0xdd
  { "LDX", CPU08_OP_OPR16_X, 4 },  // 0xde 0xde
  { "STX", CPU08_OP_OPR16_X, 4 },  // 0xdf 0xdf
  { "SUB", CPU08_OP_OPR8_X, 3 },  // 0xe0 0xe0
  { "CMP", CPU08_OP_OPR8_X, 3 },  // 0xe1 0xe1
  { "SBC", CPU08_OP_OPR8_X, 3 },  // 0xe2 0xe2
  { "CPX", CPU08_OP_OPR8_X, 3 },  // 0xe3 0xe3
  { "AND", CPU08_OP_OPR8_X, 3 },  // 0xe4 0xe4
  { "BIT", CPU08_OP_OPR8_X, 3 },  // 0xe5 0xe5
  { "LDA", CPU08_OP_OPR8_X, 3 },  // 0xe6 0xe6
  { "STA", CPU08_OP_OPR8_X, 3 },  // 0xe7 0xe7
  { "EOR", CPU08_OP_OPR8_X, 3 },  // 0xe8 0xe8
  { "ADC", CPU08_OP_OPR8_X, 3 },  // 0xe9 0xe9
  { "ORA", CPU08_OP_OPR8_X, 3 },  // 0xea 0xea
  { "ADD", CPU08_OP_OPR8_X, 3 },  // 0xeb 0xeb
  { "JMP", CPU08_OP_OPR8_X, 3 },  // 0xec 0xec
  { "JSR", CPU08_OP_OPR8_X, 5 },  // 0xed 0xed
  { "LDX", CPU08_OP_OPR8_X, 3 },  // 0xee 0xee
  { "STX", CPU08_OP_OPR8_X, 3 },  // 0xef 0xef
  { "SUB", CPU08_OP_X, 2 },  // 0xf0 0xf0
  { "CMP", CPU08_OP_COMMA_X, 2 },  // 0xf1 0xf1
  { "SBC", CPU08_OP_COMMA_X, 2 },  // 0xf2 0xf2
  { "CPX", CPU08_OP_COMMA_X, 2 },  // 0xf3 0xf3
  { "AND", CPU08_OP_COMMA_X, 2 },  // 0xf4 0xf4
  { "BIT", CPU08_OP_COMMA_X, 2 },  // 0xf5 0xf5
  { "LDA", CPU08_OP_COMMA_X, 2 },  // 0xf6 0xf6
  { "STA", CPU08_OP_COMMA_X, 2 },  // 0xf7 0xf7
  { "EOR", CPU08_OP_COMMA_X, 2 },  // 0xf8 0xf8
  { "ADC", CPU08_OP_COMMA_X, 2 },  // 0xf9 0xf9
  { "ORA", CPU08_OP_COMMA_X, 2 },  // 0xfa 0xfa
  { "ADD", CPU08_OP_COMMA_X, 2 },  // 0xfb 0xfb
  { "JMP", CPU08_OP_COMMA_X, 3 },  // 0xfc 0xfc
  { "JSR", CPU08_OP_COMMA_X, 4 },  // 0xfd 0xfd
  { "LDX", CPU08_OP_COMMA_X, 2 },  // 0xfe 0xfe
  { "STX", CPU08_OP_COMMA_X, 2 },  // 0xff 0xff
};

struct _m68hc08_16_table m68hc08_16_table[] = {
  { "NEG", 0x9e60, CPU08_OP_OPR8_SP, 5 },
  { "CBEQ", 0x9e61, CPU08_OP_OPR8_SP_REL, 6 },
  { "COM", 0x9e63, CPU08_OP_OPR8_SP, 5 },
  { "LSR", 0x9e64, CPU08_OP_OPR8_SP, 5 },
  { "ROR", 0x9e66, CPU08_OP_OPR8_SP, 5 },
  { "ASR", 0x9e67, CPU08_OP_OPR8_SP, 5 },
  { "ASL", 0x9e68, CPU08_OP_OPR8_SP, 5 },
  { "LSL", 0x9e68, CPU08_OP_OPR8_SP, 5 },
  { "ROL", 0x9e69, CPU08_OP_OPR8_SP, 5 },
  { "DEC", 0x9e6a, CPU08_OP_OPR8_SP, 5 },
  { "DBNZ", 0x9e6b, CPU08_OP_OPR8_SP_REL, 6 },
  { "INC", 0x9e6c, CPU08_OP_OPR8_SP, 5 },
  { "TST", 0x9e6d, CPU08_OP_OPR8_SP, 4 },
  { "CLR", 0x9e6f, CPU08_OP_OPR8_SP, 4 },
  { "SUB", 0x9ed0, CPU08_OP_OPR16_SP, 5 },
  { "CMP", 0x9ed1, CPU08_OP_OPR16_SP, 5 },
  { "SBC", 0x9ed2, CPU08_OP_OPR16_SP, 5 },
  { "CPX", 0x9ed3, CPU08_OP_OPR16_SP, 5 },
  { "AND", 0x9ed4, CPU08_OP_OPR16_SP, 5 },
  { "BIT", 0x9ed5, CPU08_OP_OPR16_SP, 5 },
  { "LDA", 0x9ed6, CPU08_OP_OPR16_SP, 5 },
  { "STA", 0x9ed7, CPU08_OP_OPR16_SP, 5 },
  { "EOR", 0x9ed8, CPU08_OP_OPR16_SP, 5 },
  { "ADC", 0x9ed9, CPU08_OP_OPR16_SP, 5 },
  { "ORA", 0x9eda, CPU08_OP_OPR16_SP, 5 },
  { "ADD", 0x9edb, CPU08_OP_OPR16_SP, 5 },
  { "LDX", 0x9ede, CPU08_OP_OPR16_SP, 5 },
  { "STX", 0x9edf, CPU08_OP_OPR16_SP, 5 },
  { "SUB", 0x9ee0, CPU08_OP_OPR8_SP, 4 },
  { "CMP", 0x9ee1, CPU08_OP_OPR8_SP, 4 },
  { "SBC", 0x9ee2, CPU08_OP_OPR8_SP, 4 },
  { "CPX", 0x9ee3, CPU08_OP_OPR8_SP, 4 },
  { "AND", 0x9ee4, CPU08_OP_OPR8_SP, 4 },
  { "BIT", 0x9ee5, CPU08_OP_OPR8_SP, 4 },
  { "LDA", 0x9ee6, CPU08_OP_OPR8_SP, 4 },
  { "STA", 0x9ee7, CPU08_OP_OPR8_SP, 4 },
  { "EOR", 0x9ee8, CPU08_OP_OPR8_SP, 4 },
  { "ADC", 0x9ee9, CPU08_OP_OPR8_SP, 4 },
  { "ORA", 0x9eea, CPU08_OP_OPR8_SP, 4 },
  { "ADD", 0x9eeb, CPU08_OP_OPR8_SP, 4 },
  { "LDX", 0x9eee, CPU08_OP_OPR8_SP, 4 },
  { "STX", 0x9eef, CPU08_OP_OPR8_SP, 4 },
  { NULL, 0, CPU08_OP_NONE, 0 },
};

