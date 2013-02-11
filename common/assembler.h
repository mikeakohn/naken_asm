/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include <stdio.h>

#include "memory.h"

#define MAX_NESTED_MACROS 128
#define TOKENLEN 512
#define MAX_MACRO_LEN 1024
#define PARAM_STACK_LEN 4096
#define INCLUDE_PATH_LEN 1024

#define DL_EMPTY -1
#define DL_DATA -2
#define DL_NO_CG -3

#define SEGMENT_CODE 0
#define SEGMENT_BSS 1

enum
{
  CPU_TYPE_MSP430=0,
  CPU_TYPE_65XX,
  CPU_TYPE_805X,
  CPU_TYPE_ARM,
  CPU_TYPE_DSPIC,
  CPU_TYPE_MIPS,
  CPU_TYPE_STM8,
  CPU_TYPE_TMS1000,
  CPU_TYPE_TMS1100
};

typedef int (*parse_instruction_t)(struct _asm_context *, char *);
typedef void (*list_output_t)(struct _asm_context *, int);

struct _naken_heap
{
  struct _memory_pool *memory_pool;
};

/*
  address_heap buffer looks like this:
  struct
  {
    char name[];
    int address;
  };
*/

struct _memory_pool
{
  struct _memory_pool *next;
  int len;
  int ptr;
  unsigned char buffer[];
};

struct _address_heap
{
  struct _memory_pool *memory_pool;
  int locked;
};

/*
  defines_heap buffer looks like this:
  struct
  {
    char name[];
    unsigned char value[];  // params are binary 0x01 to 0x09
    int param_count;
  };
*/

struct _defines_heap
{
  struct _memory_pool *memory_pool;
  int locked;
  char *stack[MAX_NESTED_MACROS];
  int stack_ptr;
};

struct _asm_context
{
  FILE *in;
  FILE *list;
  struct _memory memory;
  struct _address_heap address_heap;
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
  const char *filename;
  char cpu_type;
  char is_dollar_hex:1;
};

struct _memory_pool *add_pool(struct _naken_heap *heap, int heap_len);
void free_pools(struct _memory_pool *memory_pool);

void print_error(const char *s, struct _asm_context *asm_context);
void print_error_unexp(const char *s, struct _asm_context *asm_context);
void print_error_unknown_instr(const char *instr, struct _asm_context *asm_context);
void print_error_opcount(const char *instr, struct _asm_context *asm_context);
void print_error_illegal_operands(const char *instr, struct _asm_context *asm_context);
void print_error_range(const char *s, int r1, int r2, struct _asm_context *asm_context);
int add_to_include_path(struct _asm_context *asm_context, char *paths);
void assemble_init(struct _asm_context *asm_context);
void assemble_print_info(struct _asm_context *asm_context, FILE *out);
int assemble(struct _asm_context *asm_context);

#endif

