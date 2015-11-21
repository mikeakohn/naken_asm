/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
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
typedef void (*list_output_t)(struct _asm_context *, int);
typedef void (*disasm_range_t)(struct _memory *, int, int);
//typedef struct _simulate *(*simulate_init_t)(struct _memory *memory);

enum
{
  CPU_TYPE_MSP430=0,
  CPU_TYPE_MSP430X,
  CPU_TYPE_65XX,
  CPU_TYPE_65816,
  CPU_TYPE_680X,
  CPU_TYPE_68HC08,
  CPU_TYPE_680X0,
  CPU_TYPE_805X,
  CPU_TYPE_ARM,
  CPU_TYPE_AVR8,
  CPU_TYPE_DSPIC,
  CPU_TYPE_EPIPHANY,
  CPU_TYPE_MIPS32,
  CPU_TYPE_PIC24,
  CPU_TYPE_POWERPC,
  CPU_TYPE_STM8,
  CPU_TYPE_THUMB,
  CPU_TYPE_TMS1000,
  CPU_TYPE_TMS1100,
  CPU_TYPE_TMS9900,
  CPU_TYPE_Z80
};

struct _cpu_list
{
  char *name;
  char type;
  char default_endian;
  char bytes_per_address;
  char is_dollar_hex:1;
  char can_tick_end_string:1;
  char pass_1_write_disable:1;
  parse_instruction_t parse_instruction;
  parse_directive_t parse_directive;
  list_output_t list_output;
  disasm_range_t disasm_range;
  simulate_init_t simulate_init;
};

extern struct _cpu_list cpu_list[];

#endif

