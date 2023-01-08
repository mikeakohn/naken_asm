/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_MACROS_H
#define NAKEN_ASM_MACROS_H

#include <stdint.h>

#define MAX_NESTED_MACROS 128
#define MAX_MACRO_LEN 1024
#define MACROS_HEAP_SIZE 32768
#define MAX_MACRO_LEN 1024
#define CHAR_EOF -1
#define IS_DEFINE 1
#define IS_MACRO 0

/*
  defines_heap buffer looks like this:
  struct
  {
    char name[];
    unsigned char value[];  // params are binary 0x01 to 0x09
    int param_count;
  };
*/

struct _macro_data
{
  int8_t param_count; // number of macro parameters
  int8_t name_len;    // length of the macro name
  int16_t value_len;  // length of the macro
  char data[];        // name[], value[]
};

struct _macros
{
  struct _memory_pool *memory_pool;
  int locked;
  char *stack[MAX_NESTED_MACROS];
  int stack_ptr;
};

struct _macros_iter
{
  struct _memory_pool *memory_pool;
  uint8_t param_count;
  char *name;
  char *value;
  int ptr;
  int count;
  int end_flag;
};

int macros_init(struct _macros *macros);
void macros_free(struct _macros *macros);
int macros_append(struct _asm_context *asm_context, char *name, char *value, int param_count);
void macros_lock(struct _macros *macros);
char *macros_lookup(struct _macros *macros, char *name, int *param_count);
int macros_iterate(struct _macros *macros, struct _macros_iter *iter);
int macros_print(struct _macros *macros, FILE *out);
int macros_push_define(struct _macros *macros, char *define);
int macros_get_char(struct _asm_context *asm_context);
void macros_strip(char *macro);
int macros_parse(struct _asm_context *asm_context, int is_define);
char *macros_expand_params(struct _asm_context *asm_context, char *define, int param_count);
void macros_strip_comment(struct _asm_context *asm_context);

#endif

