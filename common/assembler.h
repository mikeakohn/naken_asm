/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include <stdio.h>

#include "cpu_list.h"
#include "lookup_tables.h"
#include "macros.h"
#include "memory.h"
#include "memory_pool.h"
#include "print_error.h"

#define TOKENLEN 512
#define PARAM_STACK_LEN 4096
#define INCLUDE_PATH_LEN 1024

//#define DL_EMPTY -1
//#define DL_DATA -2
//#define DL_NO_CG -3

#define SEGMENT_CODE 0
#define SEGMENT_BSS 1

struct _asm_context
{
  FILE *in;
  FILE *list;
  struct _memory memory;
  struct _address_list address_list;
  struct _defines_heap defines_heap;
  parse_instruction_t parse_instruction;
  list_output_t list_output;
  int address;
  int segment;
  int line;
  int pass;
  int instruction_count;
  int data_count;
  int code_count;
  int ifdef_count;
  int parsing_ifdef;
  char pushback[TOKENLEN];
  int pushback_type;
  char unget[512];
  int unget_ptr;
  int unget_stack[MAX_NESTED_MACROS+1];
  int unget_stack_ptr;
  int debug_file;
  char def_param_stack_data[PARAM_STACK_LEN];
  int def_param_stack_ptr[MAX_NESTED_MACROS+1];
  int def_param_stack_count;
  char include_path[INCLUDE_PATH_LEN];
  int include_count;
  int cpu_list_index;
  const char *filename;
  uint8_t cpu_type;
  uint8_t bytes_per_address;
  uint8_t is_dollar_hex:1;
  uint8_t can_tick_end_string:1;
  uint8_t error:1;
  uint8_t msp430_cpu4:1;
};

int add_to_include_path(struct _asm_context *asm_context, char *paths);
void assemble_init(struct _asm_context *asm_context);
void assemble_print_info(struct _asm_context *asm_context, FILE *out);
int assemble(struct _asm_context *asm_context);

#endif

