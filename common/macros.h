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

#ifndef _MACROS_H
#define _MACROS_H

#define DEFINES_HEAP_SIZE 32768
#define MAX_MACRO_LEN 1024
#define CHAR_EOF -1
#define IS_DEFINE 1
#define IS_MACRO 0

struct _defines_heap_iter
{
  struct _memory_pool *memory_pool;
  unsigned char *name;
  unsigned char *value;
  unsigned char param_count;
  int ptr;
  int count;
  int end_flag;
};

void strip_macro(char *macro);
void eatout_star_comment(struct _asm_context *asm_context);
int parse_macro(struct _asm_context *asm_context, int is_define);
char *expand_params(struct _asm_context *asm_context, char *define, int param_count);

int defines_heap_init(struct _defines_heap *defines_heap);
void defines_heap_free(struct _defines_heap *defines_heap);
int defines_heap_append(struct _asm_context *asm_context, char *name, char *value, int param_count);
void defines_heap_lock(struct _defines_heap *defines_heap);
char *defines_heap_lookup(struct _defines_heap *defines_heap, char *name, int *param_count);
int defines_heap_iterate(struct _defines_heap *defines_heap, struct _defines_heap_iter *iter);
int defines_heap_print(struct _defines_heap *defines_heap);
int defines_heap_push_define(struct _defines_heap *defines_heap, char *define);
int defines_heap_get_char(struct _asm_context *asm_context);


#endif

