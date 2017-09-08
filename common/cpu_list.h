/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _CPU_LIST_H
#define _CPU_LIST_H

#include "simulate/common.h"

struct _asm_context;
struct _memory;
struct _simulate;

typedef int (*parse_instruction_t)(struct _asm_context *, char *);
typedef int (*parse_directive_t)(struct _asm_context *, const char *);
typedef void (*list_output_t)(struct _asm_context *, uint32_t, uint32_t);
typedef void (*disasm_range_t)(struct _memory *, uint32_t, uint32_t, uint32_t);
//typedef struct _simulate *(*simulate_init_t)(struct _memory *memory);

#define ALIGN_1 1
#define ALIGN_2 2
#define ALIGN_4 4
#define ALIGN_8 8
#define ALIGN_16 16 

#define SREC_16 0
#define SREC_24 1
#define SREC_32 2

enum
{
  CPU_TYPE_MSP430=0,
  CPU_TYPE_MSP430X,
  CPU_TYPE_4004,
  CPU_TYPE_6502,
  CPU_TYPE_65816,
  CPU_TYPE_6800,
  CPU_TYPE_6809,
  CPU_TYPE_68HC08,
  CPU_TYPE_68000,
  CPU_TYPE_8051,
  CPU_TYPE_ARC,
  CPU_TYPE_ARM,
  CPU_TYPE_AVR8,
  CPU_TYPE_CELL,
  CPU_TYPE_DSPIC,
  CPU_TYPE_EMOTION_ENGINE,
  CPU_TYPE_EPIPHANY,
  CPU_TYPE_LC3,
  CPU_TYPE_MIPS32,
  CPU_TYPE_PDP8,
  CPU_TYPE_PIC14,
  CPU_TYPE_PIC24,
  CPU_TYPE_POWERPC,
  CPU_TYPE_PROPELLER,
  CPU_TYPE_PS2_EE_VU,
  CPU_TYPE_RISCV,
  CPU_TYPE_STM8,
  CPU_TYPE_SUPER_FX,
  CPU_TYPE_THUMB,
  CPU_TYPE_TMS1000,
  CPU_TYPE_TMS1100,
  CPU_TYPE_TMS9900,
  CPU_TYPE_Z80
};

struct _cpu_list
{
  char *name;
  int8_t type;
  int8_t default_endian;
  int8_t bytes_per_address;
  int8_t alignment;
  int8_t is_dollar_hex : 1;
  int8_t can_tick_end_string : 1;
  int8_t pass_1_write_disable : 1;
  uint8_t srec_size : 2;
  parse_instruction_t parse_instruction;
  parse_directive_t parse_directive;
  list_output_t list_output;
  disasm_range_t disasm_range;
  simulate_init_t simulate_init;
  uint32_t flags;
};

extern struct _cpu_list cpu_list[];

#endif

