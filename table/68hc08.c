/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#include <stdlib.h>

#include "table/68hc08.h"

struct _m68hc08_table m68hc08_table[] =
{
  { "brset", CPU08_OP_0_COMMA_OPR_REL, 5 },  // 0x00 0x00
  { "brclr", CPU08_OP_0_COMMA_OPR_REL, 5 },  // 0x01 0x01
  { "brset", CPU08_OP_1_COMMA_OPR_REL, 5 },  // 0x02 0x02
  { "brclr", CPU08_OP_1_COMMA_OPR_REL, 5 },  // 0x03 0x03
  { "brset", CPU08_OP_2_COMMA_OPR_REL, 5 },  // 0x04 0x04
  { "brclr", CPU08_OP_2_COMMA_OPR_REL, 5 },  // 0x05 0x05
  { "brset", CPU08_OP_3_COMMA_OPR_REL, 5 },  // 0x06 0x06
  { "brclr", CPU08_OP_3_COMMA_OPR_REL, 5 },  // 0x07 0x07
  { "brset", CPU08_OP_4_COMMA_OPR_REL, 5 },  // 0x08 0x08
  { "brclr", CPU08_OP_4_COMMA_OPR_REL, 5 },  // 0x09 0x09
  { "brset", CPU08_OP_5_COMMA_OPR_REL, 5 },  // 0x0a 0x0a
  { "brclr", CPU08_OP_5_COMMA_OPR_REL, 5 },  // 0x0b 0x0b
  { "brset", CPU08_OP_6_COMMA_OPR_REL, 5 },  // 0x0c 0x0c
  { "brclr", CPU08_OP_6_COMMA_OPR_REL, 5 },  // 0x0d 0x0d
  { "brset", CPU08_OP_7_COMMA_OPR_REL, 5 },  // 0x0e 0x0e
  { "brclr", CPU08_OP_7_COMMA_OPR_REL, 5 },  // 0x0f 0x0f
  { "bset", CPU08_OP_0_COMMA_OPR, 4 },  // 0x10 0x10
  { "bclr", CPU08_OP_0_COMMA_OPR, 4 },  // 0x11 0x11
  { "bset", CPU08_OP_1_COMMA_OPR, 4 },  // 0x12 0x12
  { "bclr", CPU08_OP_1_COMMA_OPR, 4 },  // 0x13 0x13
  { "bset", CPU08_OP_2_COMMA_OPR, 4 },  // 0x14 0x14
  { "bclr", CPU08_OP_2_COMMA_OPR, 4 },  // 0x15 0x15
  { "bset", CPU08_OP_3_COMMA_OPR, 4 },  // 0x16 0x16
  { "bclr", CPU08_OP_3_COMMA_OPR, 4 },  // 0x17 0x17
  { "bset", CPU08_OP_4_COMMA_OPR, 4 },  // 0x18 0x18
  { "bclr", CPU08_OP_4_COMMA_OPR, 4 },  // 0x19 0x19
  { "bset", CPU08_OP_5_COMMA_OPR, 4 },  // 0x1a 0x1a
  { "bclr", CPU08_OP_5_COMMA_OPR, 4 },  // 0x1b 0x1b
  { "bset", CPU08_OP_6_COMMA_OPR, 4 },  // 0x1c 0x1c
  { "bclr", CPU08_OP_6_COMMA_OPR, 4 },  // 0x1d 0x1d
  { "bset", CPU08_OP_7_COMMA_OPR, 4 },  // 0x1e 0x1e
  { "bclr", CPU08_OP_7_COMMA_OPR, 4 },  // 0x1f 0x1f
  { "bra", CPU08_OP_REL, 3 },  // 0x20 0x20
  { "brn", CPU08_OP_REL, 3 },  // 0x21 0x21
  { "bhi", CPU08_OP_REL, 3 },  // 0x22 0x22
  { "bls", CPU08_OP_REL, 3 },  // 0x23 0x23
  { "bcc", CPU08_OP_REL, 3 },  // 0x24 0x24
  // "bhs"
  { "bcs", CPU08_OP_REL, 3 },  // 0x25 0x25
  // "blo"
  { "bne", CPU08_OP_REL, 3 },  // 0x26 0x26
  { "beq", CPU08_OP_REL, 3 },  // 0x27 0x27
  { "bhcc", CPU08_OP_REL, 3 },  // 0x28 0x28
  { "bhcs", CPU08_OP_REL, 3 },  // 0x29 0x29
  { "bpl", CPU08_OP_REL, 3 },  // 0x2a 0x2a
  { "bmi", CPU08_OP_REL, 3 },  // 0x2b 0x2b
  { "bmc", CPU08_OP_REL, 3 },  // 0x2c 0x2c
  { "bms", CPU08_OP_REL, 3 },  // 0x2d 0x2d
  { "bil", CPU08_OP_REL, 3 },  // 0x2e 0x2e
  { "bih", CPU08_OP_REL, 3 },  // 0x2f 0x2f
  { "neg", CPU08_OP_OPR8, 4 },  // 0x30 0x30
  { "cbeq", CPU08_OP_OPR8_REL, 5 },  // 0x31 0x31
  { NULL, CPU08_OP_NONE, 0 },  // 0x32 NONE
  { "com", CPU08_OP_OPR8, 4 },  // 0x33 0x33
  { "lsr", CPU08_OP_OPR8, 4 },  // 0x34 0x34
  { "sthx", CPU08_OP_OPR8, 4 },  // 0x35 0x35
  { "ror", CPU08_OP_OPR8, 4 },  // 0x36 0x36
  { "asr", CPU08_OP_OPR8, 4 },  // 0x37 0x37
  { "asl", CPU08_OP_OPR8, 4 },  // 0x38 0x38
  // "lsl"
  { "rol", CPU08_OP_OPR8, 4 },  // 0x39 0x39
  { "dec", CPU08_OP_OPR8, 4 },  // 0x3a 0x3a
  { "dbnz", CPU08_OP_OPR8_REL, 5 },  // 0x3b 0x3b
  { "inc", CPU08_OP_OPR8, 4 },  // 0x3c 0x3c
  { "tst", CPU08_OP_OPR8, 3 },  // 0x3d 0x3d
  { NULL, CPU08_OP_NONE, 0 },  // 0x3e NONE
  { "clr", CPU08_OP_OPR8, 3 },  // 0x3f 0x3f
  { "nega", CPU08_OP_NONE, 1 },  // 0x40 0x40
  { "cbeqa", CPU08_OP_NUM8_REL, 4 },  // 0x41 0x41
  { "mul", CPU08_OP_NONE, 5 },  // 0x42 0x42
  { "coma", CPU08_OP_NONE, 1 },  // 0x43 0x43
  { "lsra", CPU08_OP_NONE, 1 },  // 0x44 0x44
  { "ldhx", CPU08_OP_NUM16, 3 },  // 0x45 0x45
  { "rora", CPU08_OP_NONE, 1 },  // 0x46 0x46
  { "asra", CPU08_OP_NONE, 1 },  // 0x47 0x47
  { "asla", CPU08_OP_NONE, 1 },  // 0x48 0x48
  // "lsla"
  { "rola", CPU08_OP_NONE, 1 },  // 0x49 0x49
  { "deca", CPU08_OP_NONE, 1 },  // 0x4a 0x4a
  { "dbnza", CPU08_OP_REL, 3 },  // 0x4b 0x4b
  { "inca", CPU08_OP_NONE, 1 },  // 0x4c 0x4c
  { "tsta", CPU08_OP_NONE, 1 },  // 0x4d 0x4d
  { "mov", CPU08_OP_OPR8_OPR8, 5 },  // 0x4e 0x4e
  { "clra", CPU08_OP_NONE, 1 },  // 0x4f 0x4f
  { "negx", CPU08_OP_NONE, 1 },  // 0x50 0x50
  { "cbeqx", CPU08_OP_NUM8_REL, 4 },  // 0x51 0x51
  { "div", CPU08_OP_NONE, 7 },  // 0x52 0x52
  { "comx", CPU08_OP_NONE, 1 },  // 0x53 0x53
  { "lsrx", CPU08_OP_NONE, 1 },  // 0x54 0x54
  { "ldhx", CPU08_OP_OPR8, 4 },  // 0x55 0x55
  { "rorx", CPU08_OP_NONE, 1 },  // 0x56 0x56
  { "asrx", CPU08_OP_NONE, 1 },  // 0x57 0x57
  { "aslx", CPU08_OP_NONE, 1 },  // 0x58 0x58
  // "lslx"
  { "rolx", CPU08_OP_NONE, 1 },  // 0x59 0x59
  { "decx", CPU08_OP_NONE, 1 },  // 0x5a 0x5a
  { "dbnzx", CPU08_OP_REL, 3 },  // 0x5b 0x5b
  { "incx", CPU08_OP_NONE, 1 },  // 0x5c 0x5c
  { "tstx", CPU08_OP_NONE, 1 },  // 0x5d 0x5d
  { "mov", CPU08_OP_OPR8_X_PLUS, 4 },  // 0x5e 0x5e
  { "clrx", CPU08_OP_NONE, 1 },  // 0x5f 0x5f
  { "neg", CPU08_OP_OPR8_X, 4 },  // 0x60 0x60
  { "cbeq", CPU08_OP_OPR8_X_PLUS_REL, 5 },  // 0x61 0x61
  { "nsa", CPU08_OP_NONE, 3 },  // 0x62 0x62
  { "com", CPU08_OP_OPR8_X, 4 },  // 0x63 0x63
  { "lsr", CPU08_OP_OPR8_X, 4 },  // 0x64 0x64
  { "cphx", CPU08_OP_NUM16, 3 },  // 0x65 0x65
  { "ror", CPU08_OP_OPR8_X, 4 },  // 0x66 0x66
  { "asr", CPU08_OP_OPR8_X, 4 },  // 0x67 0x67
  { "asl", CPU08_OP_OPR8_X, 4 },  // 0x68 0x68
  // "lsl"
  { "rol", CPU08_OP_OPR8_X, 4 },  // 0x69 0x69
  { "dec", CPU08_OP_OPR8_X, 4 },  // 0x6a 0x6a
  { "dbnz", CPU08_OP_OPR8_X_REL, 5 },  // 0x6b 0x6b
  { "inc", CPU08_OP_OPR8_X, 4 },  // 0x6c 0x6c
  { "tst", CPU08_OP_OPR8_X, 3 },  // 0x6d 0x6d
  { "mov", CPU08_OP_NUM8_OPR8, 4 },  // 0x6e 0x6e
  { "clr", CPU08_OP_OPR8_X, 3 },  // 0x6f 0x6f
  { "neg", CPU08_OP_COMMA_X, 3 },  // 0x70 0x70
  { "cbeq", CPU08_OP_X_PLUS_REL, 4 },  // 0x71 0x71
  { "daa", CPU08_OP_NONE, 2 },  // 0x72 0x72
  { "com", CPU08_OP_COMMA_X, 3 },  // 0x73 0x73
  { "lsr", CPU08_OP_COMMA_X, 3 },  // 0x74 0x74
  { "cphx", CPU08_OP_OPR8, 4 },  // 0x75 0x75
  { "ror", CPU08_OP_COMMA_X, 3 },  // 0x76 0x76
  { "asr", CPU08_OP_COMMA_X, 3 },  // 0x77 0x77
  { "asl", CPU08_OP_COMMA_X, 3 },  // 0x78 0x78
  // "lsl"
  { "rol", CPU08_OP_COMMA_X, 3 },  // 0x79 0x79
  { "dec", CPU08_OP_COMMA_X, 3 },  // 0x7a 0x7a
  { "dbnz", CPU08_OP_X_REL, 4 },  // 0x7b 0x7b
  { "inc", CPU08_OP_COMMA_X, 3 },  // 0x7c 0x7c
  { "tst", CPU08_OP_COMMA_X, 2 },  // 0x7d 0x7d
  { "mov", CPU08_OP_X_PLUS_OPR8, 4 },  // 0x7e 0x7e
  { "clr", CPU08_OP_COMMA_X, 2 },  // 0x7f 0x7f
  { "rti", CPU08_OP_NONE, 7 },  // 0x80 0x80
  { "rts", CPU08_OP_NONE, 4 },  // 0x81 0x81
  { NULL, CPU08_OP_NONE, 0 },  // 0x82 NONE
  { "swi", CPU08_OP_NONE, 9 },  // 0x83 0x83
  { "tap", CPU08_OP_NONE, 2 },  // 0x84 0x84
  { "tpa", CPU08_OP_NONE, 1 },  // 0x85 0x85
  { "pula", CPU08_OP_NONE, 2 },  // 0x86 0x86
  { "psha", CPU08_OP_NONE, 2 },  // 0x87 0x87
  { "pulx", CPU08_OP_NONE, 2 },  // 0x88 0x88
  { "pshx", CPU08_OP_NONE, 2 },  // 0x89 0x89
  { "pulh", CPU08_OP_NONE, 2 },  // 0x8a 0x8a
  { "pshh", CPU08_OP_NONE, 2 },  // 0x8b 0x8b
  { "clrh", CPU08_OP_NONE, 1 },  // 0x8c 0x8c
  { NULL, CPU08_OP_NONE, 0 },  // 0x8d NONE
  { "stop", CPU08_OP_NONE, 1 },  // 0x8e 0x8e
  { "wait", CPU08_OP_NONE, 1 },  // 0x8f 0x8f
  { "bge", CPU08_OP_REL, 3 },  // 0x90 0x90
  { "blt", CPU08_OP_REL, 3 },  // 0x91 0x91
  { "bgt", CPU08_OP_REL, 3 },  // 0x92 0x92
  { "ble", CPU08_OP_REL, 3 },  // 0x93 0x93
  { "txs", CPU08_OP_NONE, 2 },  // 0x94 0x94
  { "tsx", CPU08_OP_NONE, 2 },  // 0x95 0x95
  { NULL, CPU08_OP_NONE, 0 },  // 0x96 NONE
  { "tax", CPU08_OP_NONE, 1 },  // 0x97 0x97
  { "clc", CPU08_OP_NONE, 1 },  // 0x98 0x98
  { "sec", CPU08_OP_NONE, 1 },  // 0x99 0x99
  { "cli", CPU08_OP_NONE, 2 },  // 0x9a 0x9a
  { "sei", CPU08_OP_NONE, 2 },  // 0x9b 0x9b
  { "rsp", CPU08_OP_NONE, 1 },  // 0x9c 0x9c
  { "nop", CPU08_OP_NONE, 1 },  // 0x9d 0x9d
  { NULL, CPU08_OP_NONE, 0 },  // 0x9e NONE
  { "txa", CPU08_OP_NONE, 1 },  // 0x9f 0x9f
  { "sub", CPU08_OP_NUM8, 2 },  // 0xa0 0xa0
  { "cmp", CPU08_OP_NUM8, 2 },  // 0xa1 0xa1
  { "sbc", CPU08_OP_NUM8, 2 },  // 0xa2 0xa2
  { "cpx", CPU08_OP_NUM8, 2 },  // 0xa3 0xa3
  { "and", CPU08_OP_NUM8, 2 },  // 0xa4 0xa4
  { "bit", CPU08_OP_NUM8, 2 },  // 0xa5 0xa5
  { "lda", CPU08_OP_NUM8, 2 },  // 0xa6 0xa6
  { "ais", CPU08_OP_NUM8, 2 },  // 0xa7 0xa7
  { "eor", CPU08_OP_NUM8, 2 },  // 0xa8 0xa8
  { "adc", CPU08_OP_NUM8, 2 },  // 0xa9 0xa9
  { "ora", CPU08_OP_NUM8, 2 },  // 0xaa 0xaa
  { "add", CPU08_OP_NUM8, 2 },  // 0xab 0xab
  { NULL, CPU08_OP_NONE, 0 },  // 0xac NONE
  { "bsr", CPU08_OP_REL, 4 },  // 0xad 0xad
  { "ldx", CPU08_OP_NUM8, 2 },  // 0xae 0xae
  { "aix", CPU08_OP_NUM8, 2 },  // 0xaf 0xaf
  { "sub", CPU08_OP_OPR8, 3 },  // 0xb0 0xb0
  { "cmp", CPU08_OP_OPR8, 3 },  // 0xb1 0xb1
  { "sbc", CPU08_OP_OPR8, 3 },  // 0xb2 0xb2
  { "cpx", CPU08_OP_OPR8, 3 },  // 0xb3 0xb3
  { "and", CPU08_OP_OPR8, 3 },  // 0xb4 0xb4
  { "bit", CPU08_OP_OPR8, 3 },  // 0xb5 0xb5
  { "lda", CPU08_OP_OPR8, 3 },  // 0xb6 0xb6
  { "sta", CPU08_OP_OPR8, 3 },  // 0xb7 0xb7
  { "eor", CPU08_OP_OPR8, 3 },  // 0xb8 0xb8
  { "adc", CPU08_OP_OPR8, 3 },  // 0xb9 0xb9
  { "ora", CPU08_OP_OPR8, 3 },  // 0xba 0xba
  { "add", CPU08_OP_OPR8, 3 },  // 0xbb 0xbb
  { "jmp", CPU08_OP_OPR8, 2 },  // 0xbc 0xbc
  { "jsr", CPU08_OP_OPR8, 4 },  // 0xbd 0xbd
  { "ldx", CPU08_OP_OPR8, 3 },  // 0xbe 0xbe
  { "stx", CPU08_OP_OPR8, 3 },  // 0xbf 0xbf
  { "sub", CPU08_OP_OPR16, 4 },  // 0xc0 0xc0
  { "cmp", CPU08_OP_OPR16, 4 },  // 0xc1 0xc1
  { "sbc", CPU08_OP_OPR16, 4 },  // 0xc2 0xc2
  { "cpx", CPU08_OP_OPR16, 4 },  // 0xc3 0xc3
  { "and", CPU08_OP_OPR16, 4 },  // 0xc4 0xc4
  { "bit", CPU08_OP_OPR16, 4 },  // 0xc5 0xc5
  { "lda", CPU08_OP_OPR16, 4 },  // 0xc6 0xc6
  { "sta", CPU08_OP_OPR16, 4 },  // 0xc7 0xc7
  { "eor", CPU08_OP_OPR16, 4 },  // 0xc8 0xc8
  { "adc", CPU08_OP_OPR16, 4 },  // 0xc9 0xc9
  { "ora", CPU08_OP_OPR16, 4 },  // 0xca 0xca
  { "add", CPU08_OP_OPR16, 4 },  // 0xcb 0xcb
  { "jmp", CPU08_OP_OPR16, 3 },  // 0xcc 0xcc
  { "jsr", CPU08_OP_OPR16, 5 },  // 0xcd 0xcd
  { "ldx", CPU08_OP_OPR16, 4 },  // 0xce 0xce
  { "stx", CPU08_OP_OPR16, 4 },  // 0xcf 0xcf
  { "sub", CPU08_OP_OPR16_X, 4 },  // 0xd0 0xd0
  { "cmp", CPU08_OP_OPR16_X, 4 },  // 0xd1 0xd1
  { "sbc", CPU08_OP_OPR16_X, 4 },  // 0xd2 0xd2
  { "cpx", CPU08_OP_OPR16_X, 4 },  // 0xd3 0xd3
  { "and", CPU08_OP_OPR16_X, 4 },  // 0xd4 0xd4
  { "bit", CPU08_OP_OPR16_X, 4 },  // 0xd5 0xd5
  { "lda", CPU08_OP_OPR16_X, 4 },  // 0xd6 0xd6
  { "sta", CPU08_OP_OPR16_X, 4 },  // 0xd7 0xd7
  { "eor", CPU08_OP_OPR16_X, 4 },  // 0xd8 0xd8
  { "adc", CPU08_OP_OPR16_X, 4 },  // 0xd9 0xd9
  { "ora", CPU08_OP_OPR16_X, 4 },  // 0xda 0xda
  { "add", CPU08_OP_OPR16_X, 4 },  // 0xdb 0xdb
  { "jmp", CPU08_OP_OPR16_X, 4 },  // 0xdc 0xdc
  { "jsr", CPU08_OP_OPR16_X, 6 },  // 0xdd 0xdd
  { "ldx", CPU08_OP_OPR16_X, 4 },  // 0xde 0xde
  { "stx", CPU08_OP_OPR16_X, 4 },  // 0xdf 0xdf
  { "sub", CPU08_OP_OPR8_X, 3 },  // 0xe0 0xe0
  { "cmp", CPU08_OP_OPR8_X, 3 },  // 0xe1 0xe1
  { "sbc", CPU08_OP_OPR8_X, 3 },  // 0xe2 0xe2
  { "cpx", CPU08_OP_OPR8_X, 3 },  // 0xe3 0xe3
  { "and", CPU08_OP_OPR8_X, 3 },  // 0xe4 0xe4
  { "bit", CPU08_OP_OPR8_X, 3 },  // 0xe5 0xe5
  { "lda", CPU08_OP_OPR8_X, 3 },  // 0xe6 0xe6
  { "sta", CPU08_OP_OPR8_X, 3 },  // 0xe7 0xe7
  { "eor", CPU08_OP_OPR8_X, 3 },  // 0xe8 0xe8
  { "adc", CPU08_OP_OPR8_X, 3 },  // 0xe9 0xe9
  { "ora", CPU08_OP_OPR8_X, 3 },  // 0xea 0xea
  { "add", CPU08_OP_OPR8_X, 3 },  // 0xeb 0xeb
  { "jmp", CPU08_OP_OPR8_X, 3 },  // 0xec 0xec
  { "jsr", CPU08_OP_OPR8_X, 5 },  // 0xed 0xed
  { "ldx", CPU08_OP_OPR8_X, 3 },  // 0xee 0xee
  { "stx", CPU08_OP_OPR8_X, 3 },  // 0xef 0xef
  { "sub", CPU08_OP_X, 2 },  // 0xf0 0xf0
  { "cmp", CPU08_OP_COMMA_X, 2 },  // 0xf1 0xf1
  { "sbc", CPU08_OP_COMMA_X, 2 },  // 0xf2 0xf2
  { "cpx", CPU08_OP_COMMA_X, 2 },  // 0xf3 0xf3
  { "and", CPU08_OP_COMMA_X, 2 },  // 0xf4 0xf4
  { "bit", CPU08_OP_COMMA_X, 2 },  // 0xf5 0xf5
  { "lda", CPU08_OP_COMMA_X, 2 },  // 0xf6 0xf6
  { "sta", CPU08_OP_COMMA_X, 2 },  // 0xf7 0xf7
  { "eor", CPU08_OP_COMMA_X, 2 },  // 0xf8 0xf8
  { "adc", CPU08_OP_COMMA_X, 2 },  // 0xf9 0xf9
  { "ora", CPU08_OP_COMMA_X, 2 },  // 0xfa 0xfa
  { "add", CPU08_OP_COMMA_X, 2 },  // 0xfb 0xfb
  { "jmp", CPU08_OP_COMMA_X, 3 },  // 0xfc 0xfc
  { "jsr", CPU08_OP_COMMA_X, 4 },  // 0xfd 0xfd
  { "ldx", CPU08_OP_COMMA_X, 2 },  // 0xfe 0xfe
  { "stx", CPU08_OP_COMMA_X, 2 },  // 0xff 0xff
};

struct _m68hc08_16_table m68hc08_16_table[] =
{
  { "adc", 0x9ee9, CPU08_OP_OPR8_SP,  4, 1 },
  { "adc", 0x9ed9, CPU08_OP_OPR16_SP, 5, 1 },
  { "add", 0x9eeb, CPU08_OP_OPR8_SP,  4, 1 },
  { "add", 0x9edb, CPU08_OP_OPR16_SP, 5, 1 },
  { "and", 0x9ee4, CPU08_OP_OPR8_SP,  4, 1 },
  { "and", 0x9ed4, CPU08_OP_OPR16_SP, 5, 1 },
  { "asr", 0x9e67, CPU08_OP_OPR8_SP,  5, 0 },
  { "asl", 0x9e68, CPU08_OP_OPR8_SP,  5, 0 },
  { "bit", 0x9ee5, CPU08_OP_OPR8_SP,  4, 1 },
  { "bit", 0x9ed5, CPU08_OP_OPR16_SP, 5, 1 },
  { "clr", 0x9e6f, CPU08_OP_OPR8_SP,  4, 0 },
  { "cmp", 0x9ee1, CPU08_OP_OPR8_SP,  4, 1 },
  { "cmp", 0x9ed1, CPU08_OP_OPR16_SP, 5, 1 },
  { "com", 0x9e63, CPU08_OP_OPR8_SP,  5, 0 },
  { "cpx", 0x9ee3, CPU08_OP_OPR8_SP,  4, 1 },
  { "cpx", 0x9ed3, CPU08_OP_OPR16_SP, 5, 1 },
  { "dec", 0x9e6a, CPU08_OP_OPR8_SP,  5, 0 },
  { "eor", 0x9ee8, CPU08_OP_OPR8_SP,  4, 1 },
  { "eor", 0x9ed8, CPU08_OP_OPR16_SP, 5, 1 },
  { "inc", 0x9e6c, CPU08_OP_OPR8_SP,  5, 0 },
  { "lda", 0x9ee6, CPU08_OP_OPR8_SP,  4, 1 },
  { "lda", 0x9ed6, CPU08_OP_OPR16_SP, 5, 1 },
  { "ldx", 0x9eee, CPU08_OP_OPR8_SP,  4, 1 },
  { "ldx", 0x9ede, CPU08_OP_OPR16_SP, 5, 1 },
  { "lsl", 0x9e68, CPU08_OP_OPR8_SP,  5, 0 },
  { "lsr", 0x9e64, CPU08_OP_OPR8_SP,  5, 0 },
  { "neg", 0x9e60, CPU08_OP_OPR8_SP,  5, 0 },
  { "ora", 0x9eea, CPU08_OP_OPR8_SP,  4, 1 },
  { "ora", 0x9eda, CPU08_OP_OPR16_SP, 5, 1 },
  { "rol", 0x9e69, CPU08_OP_OPR8_SP,  5, 0 },
  { "ror", 0x9e66, CPU08_OP_OPR8_SP,  5, 0 },
  { "sbc", 0x9ee2, CPU08_OP_OPR8_SP,  4, 1 },
  { "sbc", 0x9ed2, CPU08_OP_OPR16_SP, 5, 1 },
  { "sta", 0x9ee7, CPU08_OP_OPR8_SP,  4, 1 },
  { "sta", 0x9ed7, CPU08_OP_OPR16_SP, 5, 1 },
  { "stx", 0x9eef, CPU08_OP_OPR8_SP,  4, 1 },
  { "stx", 0x9edf, CPU08_OP_OPR16_SP, 5, 1 },
  { "sub", 0x9ee0, CPU08_OP_OPR8_SP,  4, 1 },
  { "sub", 0x9ed0, CPU08_OP_OPR16_SP, 5, 1 },
  { "tst", 0x9e6d, CPU08_OP_OPR8_SP,  4, 0 },
  { "dbnz", 0x9e6b, CPU08_OP_OPR8_SP_REL, 6, 0 },
  { "cbeq", 0x9e61, CPU08_OP_OPR8_SP_REL, 6, 0 },
  { NULL, 0, CPU08_OP_NONE, 0, 0 },
};

