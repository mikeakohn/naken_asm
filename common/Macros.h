/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_MACROS_H
#define NAKEN_ASM_MACROS_H

#include <stdint.h>

#include "MemoryPool.h"

//typedef struct _asm_context AsmContext;
//struct AsmContext;

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

struct MacroData
{
  int8_t param_count; // number of macro parameters
  int8_t name_len;    // length of the macro name
  int16_t value_len;  // length of the macro
  char data[];        // name[], value[]
};

class Macros
{
public:
  Macros();
  ~Macros();

  void reset();
  int get_stack_ptr() { return stack_ptr; }
  void lock()         { locked = true; }
  bool is_locked()    { return locked; }

  int dump(FILE *out);

//private:
  static bool is_letter(char ch)
  {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
  }

  static bool is_digit(char ch)
  {
    return ch >= '0' && ch <= '9';
  }

  MemoryPool *memory_pool;
  int locked;
  int stack_ptr;
  char *stack[MAX_NESTED_MACROS];
};

class MacrosIter
{
public:
  MacrosIter(Macros &macros) :
    memory_pool (NULL),
    param_count (0),
    name        (NULL),
    value       (NULL),
    ptr         (0),
    count       (0),
    is_done     (false)
  {
    memory_pool = macros.memory_pool;
  }

  int next();

  MemoryPool *memory_pool;
  uint8_t param_count;
  char *name;
  char *value;
  int ptr;
  int count;
  int is_done;
};

int macros_append(AsmContext *asm_context, char *name, char *value, int param_count);
char *macros_lookup(Macros *macros, char *name, int *param_count);
//int macros_iterate(Macros *macros, MacrosIter *iter);
int macros_push_define(Macros *macros, char *define);
int macros_get_char(AsmContext *asm_context);
void macros_strip(char *macro);
int macros_parse(AsmContext *asm_context, int is_define);
char *macros_expand_params(AsmContext *asm_context, char *define, int param_count);
void macros_strip_comment(AsmContext *asm_context);

#endif

