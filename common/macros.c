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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "get_tokens.h"
#include "address_list.h"
#include "macros.h"
#include "memory_pool.h"

static int get_param_index(char *params, char *name)
{
int count = 1;
int ptr = 0;

  while(params[ptr] != 0)
  {
    if (strcmp(params + ptr,name) == 0) return count;
    count++;
    ptr = ptr + strlen(params + ptr) + 1;
  }

  return 0;
}

static int macros_parse_token(struct _asm_context *asm_context, char *token, int len)
{
int ptr = 0;
char ch;

  while(1)
  {
    ch = get_next_char(asm_context);
    if (ch == ' ')
    {
      if (ptr == 0) continue;
      break;
    }

    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
    {
      token[ptr++] = ch;
    }
      else
    if (ch >= '0' && ch <= '9')
    {
      if (ptr == 0)
      {
        print_error("Bad macro name", asm_context);
        return -1;
      }

      token[ptr++] = ch;
    }
      else
    if (ch == '(')
    {
      token[ptr] = 0;
      return 1;
    }
      else
    {
      unget_next_char(asm_context, ch);
      break;
    }

    if (ptr == len-1) break;
  }

  token[ptr] = 0;

  return 0;
}

static int check_endm(char *macro, int ptr)
{
  ptr--;

  while(ptr > 0 && macro[ptr] != '\n')
  {
    ptr--;
  }

  ptr++;
  if (strncasecmp(macro + ptr, ".endm", 5) == 0 ||
      strncasecmp(macro + ptr, "endm", 4) == 0)
  {
    macro[ptr] = 0;
    return 1;
  }

  return 0;
}

int macros_init(struct _macros *macros)
{
  macros->memory_pool = NULL;
  macros->locked = 0;

  return 0;
}

void macros_free(struct _macros *macros)
{
  memory_pool_free(macros->memory_pool);
  macros->memory_pool = NULL;
  macros->stack_ptr = 0;
}

int macros_append(struct _asm_context *asm_context, char *name, char *value, int param_count)
{
struct _macros *macros = &asm_context->macros;
struct _memory_pool *memory_pool = macros->memory_pool;
int param_count_temp;
int name_len;
int value_len;

  if (macros->locked == 1) { return 0; }

  if (macros_lookup(macros, name, &param_count_temp) != NULL ||
      address_list_lookup(&asm_context->address_list, name) != -1)
  {
    printf("Error: Macro '%s' already defined.\n", name);
    return -1;
  }

  name_len = strlen(name) + 1;
  value_len = strlen(value) + 1;

  // The name of the macro can only be 255 chars
  if (name_len > 255)
  {
    printf("Error: Macro name '%s' is too big.\n", name);
    return -1;
  }

  // Check the size of the new macro against the size of a pool.
  if (name_len + value_len + sizeof(struct _macro_data) > MACROS_HEAP_SIZE)
  {
    printf("Error: Macro '%s' is too big.\n", name);
    return -1;
  }

  // If we have no pool, add one.
  if (memory_pool == NULL)
  {
    memory_pool = memory_pool_add((struct _naken_heap *)macros, MACROS_HEAP_SIZE);
  }

  // Find a pool that has enough area at the end to add this macro.
  // If none can be found, alloc a new one.
  while(1)
  {
     if (memory_pool->ptr + name_len + value_len + sizeof(struct _macro_data) < memory_pool->len)
     {
       break;
     }

     if (memory_pool->next == NULL)
     {
       memory_pool->next = memory_pool_add((struct _naken_heap *)macros, MACROS_HEAP_SIZE);
     }

     memory_pool = memory_pool->next;
  }

  // Set the new macro entry.
  // FIXME - probably should align by 4 for RISC machines.
  struct _macro_data *macro_data =
    (struct _macro_data *)(memory_pool->buffer + memory_pool->ptr);
  macro_data->param_count = (uint8_t)param_count;
  macro_data->name_len = name_len;
  macro_data->value_len = value_len;
  memcpy(macro_data->data, name, name_len);
  memcpy(macro_data->data + name_len, value, value_len);
  memory_pool->ptr += name_len + value_len + sizeof(struct _macro_data);

  return 0;
} 

void macros_lock(struct _macros *macros)
{
  macros->locked = 1;
}

char *macros_lookup(struct _macros *macros, char *name, int *param_count)
{
struct _memory_pool *memory_pool = macros->memory_pool;
char *value;
int ptr;

  while(memory_pool != NULL)
  {
    ptr = 0;
    while(ptr < memory_pool->ptr)
    {
      struct _macro_data *macro_data =
        (struct _macro_data *)(memory_pool->buffer + ptr);
      //name_len = strlen((char *)memory_pool->buffer+ptr) + 1;
      //value_len = strlen((char *)memory_pool->buffer + ptr + name_len) + 1;

      if (strcmp(macro_data->data, name) == 0)
      {
        value = macro_data->data + macro_data->name_len;
        *param_count = macro_data->param_count;
        return value;
      }
      ptr += macro_data->name_len + macro_data->value_len + sizeof(struct _macro_data);
    }

    memory_pool = memory_pool->next;
  }

  return NULL;
}

int macros_iterate(struct _macros *macros, struct _macros_iter *iter)
{
struct _memory_pool *memory_pool = macros->memory_pool;

  if (iter->end_flag == 1) { return -1; }
  if (iter->memory_pool == NULL)
  {
    iter->memory_pool = macros->memory_pool;
    iter->ptr = 0;
  }

  while(memory_pool != NULL)
  {
    if(iter->ptr < memory_pool->ptr)
    {
      struct _macro_data *macro_data =
        (struct _macro_data *)(memory_pool->buffer + iter->ptr);

      iter->param_count = macro_data->param_count;
      iter->name = macro_data->data;
      iter->value = macro_data->data + macro_data->name_len;

      iter->ptr += macro_data->name_len + macro_data->value_len + sizeof(struct _macro_data);

      iter->count++;
      return 0;
    }

    memory_pool = memory_pool->next;
  }

  iter->end_flag = 1;

  return -1;
}

int macros_print(struct _macros *macros)
{
struct _macros_iter iter;

  memset(&iter, 0, sizeof(iter));

  printf("%18s %s\n", "NAME", "VALUE");

  while(macros_iterate(macros, &iter) != -1)
  {
    if (iter.param_count == 0)
    {
      printf("%30s=", iter.name);
    }
      else
    {
      int n;

      printf("%30s(", iter.name);
      for (n = 0; n < iter.param_count; n++)
      {
        if (n != 0) { printf(","); }
        printf("{%d}", n + 1);
      }
      printf(")=");
    }

    char *value = iter.value;
    while (*value != 0)
    {
      if (*value < 10) { printf("{%d}", *value); }
      else { printf("%c", *value); }

      value++;
    }

    printf("\n");
  }

  printf("Total %d.\n\n", iter.count);

  return 0;
}

int macros_push_define(struct _macros *macros, char *define)
{
#ifdef DEBUG
printf("debug> macros_push_define, define=%s macros->stack_ptr=%d\n", define, macros->stack_ptr);
#endif

  if (macros->stack_ptr >= MAX_NESTED_MACROS)
  {
    printf("Internal Error: defines heap stack exhausted.\n");
    return -1;
  }

  macros->stack[macros->stack_ptr++] = define;

  return 0;
}

int macros_get_char(struct _asm_context *asm_context)
{
int stack_ptr;
int ch;

  struct _macros *macros = &asm_context->macros;

  while(1)
  {
    // Is there even a character waiting on the #define stack?
    stack_ptr = macros->stack_ptr-1;
    if (stack_ptr < 0) { return CHAR_EOF; }

    // Pull the next char off the stack
    ch = *macros->stack[stack_ptr];
    macros->stack[stack_ptr]++;

    // If we have a char then break this loop and return (all is good)
    if (ch != 0) { break; }

    // drop the #define stack by 1 level
    if (macros->stack[stack_ptr] >= asm_context->def_param_stack_data &&
        macros->stack[stack_ptr] < asm_context->def_param_stack_data + PARAM_STACK_LEN)
    {
      asm_context->def_param_stack_count--;
      if (asm_context->def_param_stack_count < 0)
      {
        print_error_internal(NULL, __FILE__, __LINE__);
        exit(1);
      }
#ifdef DEBUG
printf("debug> asm_context->def_param_stack_count=%d\n",asm_context->def_param_stack_count);
#endif
    }
    macros->stack_ptr--;
    asm_context->unget_stack_ptr--;

    // Check if something need to be ungetted
    if (asm_context->unget_ptr > asm_context->unget_stack[asm_context->unget_stack_ptr])
    {
#ifdef DEBUG
printf("debug> get_next_char(?) ungetc %d %d '%c'\n", asm_context->unget_stack_ptr, asm_context->unget_stack[asm_context->unget_stack_ptr], asm_context->unget[asm_context->unget_ptr-1]);
#endif
      return asm_context->unget[--asm_context->unget_ptr];
    }
  }

  return ch;
}

void macros_strip(char *macro)
{
  char *s = macro;
  // Remove ; and // comments
  while (*s != 0)
  {
    if (*s == ';') { *s = 0; break; }
    if (*s == '/' && *(s+1)=='/') { *s = 0; break; }
    s++;
  }

  if (s != macro) { s--; }

  // Trim spaces from end of macro
  while (s != macro)
  {
    if (*s != ' ') break;
    *s = 0;
    s--;
  }
}

int macros_parse(struct _asm_context *asm_context, int macro_type)
{
char name[128];
char token[TOKENLEN];
char params[1024];
char *name_test;
char macro[MAX_MACRO_LEN];
int ptr = 0;
int token_type;
int ch;
int cont = 0;
int parens = 0;
int param_count = 0;

  // First pull the name out
  parens = macros_parse_token(asm_context, name, 128);
  if (parens == -1) return -1;

#ifdef DEBUG
printf("#define %s   parens_flag=%d\n", name, parens);
#endif

  // Now pull any params out
  ptr = 0;
  if (parens != 0)
  {
    while(1)
    {
      token_type = get_token(asm_context, token, TOKENLEN);
#ifdef DEBUG
printf("debug> #ifdef param %s\n", token);
#endif
      if (token_type != TOKEN_STRING)
      {
        printf("Error: Expected a param name but got '%s' at %s:%d.\n", token, asm_context->filename,asm_context->line);
        return -1;
      }

      param_count++;

      int len = strlen(token);
      if (ptr + len + 2 > 1024)
      {
        print_error("Macro with too long parameter list\n", asm_context);
        return -1;
      }

      strcpy(params + ptr,token);
      ptr = ptr + len + 1;

      token_type = get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,','))
      {
      }
        else
      if (IS_TOKEN(token,')'))
      {
        break;
      }
        else
      {
        print_error("Expected ',' or ')'", asm_context);
        return -1;
      }
    }
  }

  params[ptr]=0;

#ifdef DEBUG
printf("debug> #define param count=%d\n", param_count);
#endif

  // Now macro time
  ptr = 0;
  name_test = NULL;

  while(1)
  {
    ch = get_next_char(asm_context);
    if (name_test == NULL)
    {
      if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
      {
        name_test = macro + ptr;
      }
    }
      else
    if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
          (ch >= '0' && ch <= '9')))
    {
      if (name_test != NULL)
      {
        macro[ptr] = 0;
        int index = get_param_index(params,name_test);

#ifdef DEBUG
printf("debug> #ifdef param found %s %d\n", name_test, index);
#endif

        if (index != 0)
        {
          ptr = name_test - macro;
          macro[ptr++] = index;
        }
        name_test = NULL;
      }
    }

    if (ch == ';' || (ptr > 0 && macro[ptr-1] == '/'))
    {
      if (macro[ptr-1] == '/') { ptr--; }

      while(1)
      {
        ch = get_next_char(asm_context);
        if (ch == '\n' || ch == EOF) break;
      }

      while(ptr > 0)
      {
        if (macro[ptr-1] != ' ') { break; }
        ptr--;
      }
    }

    if (ch == '\r') continue;
    if (ch == ' ' && (ptr == 0 || cont != 0)) continue;
    if (ch == '\\' && cont == 0)
    {
      if (macro_type == IS_DEFINE)
      {
        cont = 1;
        continue;
      }
    }

    if (ch == '\n' || ch == EOF)
    {
      asm_context->line++;

      if (macro_type == IS_DEFINE)
      {
        if (cont == 1)
        {
          macro[ptr++] = ch;
          cont = 2;
          continue;
        }

        break;
      }
        else
      {
        macro[ptr] = 0;
        if (check_endm(macro,ptr) == 1) break;
      }
    }

    if (cont == 1)
    {
      printf("Parse error: Expecting end-of-line on line %d\n", asm_context->line);
      return -1;
    }

    cont = 0;

    if (ch == '*' && ptr > 0 && macro[ptr-1] == '/')
    {
      macros_strip_comment(asm_context);
      ptr--;
      continue;
    }

    macro[ptr++] = ch;
    if (ptr >= MAX_MACRO_LEN - 1)
    {
      printf("Internal error: macro longer than %d bytes on line %d\n", MAX_MACRO_LEN, asm_context->line);
      return -1;
    }
  }

  macro[ptr++] = 0;

#ifdef DEBUG
printf("Debug: adding macro '%s'\n", macro);
#endif

  macros_strip(macro);
  macros_append(asm_context, name, macro, param_count);

  return 0;
}

char *macros_expand_params(struct _asm_context *asm_context, char *define, int param_count)
{
int ch;
char params[1024];
int params_ptr[256];
int count,ptr;

  ch = get_next_char(asm_context);

  if (ch != '(')
  {
    print_error("Macro expects params", asm_context);
    return 0;
  }

  count = 0;
  ptr = 0;
  params_ptr[count] = ptr;

  while(1)
  {
    ch = get_next_char(asm_context);
    if (ch == '\r') continue;
    if (ch == ')') break;
    if (ch == '\n' || ch == EOF)
    {
      print_error("Macro expects ')'", asm_context);
      return NULL;
    }
    if (ch == ',')
    {
      params[ptr++] = 0;
      params_ptr[++count] = ptr;
      continue;
    }

    params[ptr++] = ch;
  }

  params[ptr++] = 0;
  count++;
  if (count != param_count)
  {
    printf("Error: Macro expects %d params, but got only %d at %s:%d.\n", param_count, count, asm_context->filename, asm_context->line);
    return NULL;
  }

#ifdef DEBUG
printf("debug> Expanding macro with params: pass=%d\n", asm_context->pass);
int n;
for (n = 0; n < count; n++)
{
  printf("debug>   %s\n", params + params_ptr[n]);
}
#endif

  ptr = asm_context->def_param_stack_ptr[asm_context->def_param_stack_count];

  while(*define != 0)
  {
    if (*define < 10)
    {
      strcpy(asm_context->def_param_stack_data + ptr, params + params_ptr[((int)*define)-1]);
      while(*(asm_context->def_param_stack_data + ptr) != 0) { ptr++; }
    }
    else
    {
      asm_context->def_param_stack_data[ptr++] = *define;
    }

    if (ptr >= PARAM_STACK_LEN)
    {
      print_error_internal(NULL, __FILE__, __LINE__);
      exit(1);
    }

    define++;
  }

#ifdef DEBUG
printf("debug>   ptr=%d\n", ptr);
#endif
  asm_context->def_param_stack_data[ptr++] = 0;

#ifdef DEBUG
printf("debug> Expanded macro becomes: %s\n", asm_context->def_param_stack_data+asm_context->def_param_stack_ptr[asm_context->def_param_stack_count]);
#endif

  asm_context->def_param_stack_ptr[++asm_context->def_param_stack_count] = ptr;

  return asm_context->def_param_stack_data +
         asm_context->def_param_stack_ptr[asm_context->def_param_stack_count-1];
}

void macros_strip_comment(struct _asm_context *asm_context)
{
int it, cl = 0;

#ifdef DEBUG
printf("debug> macros_strip_comment()\n");
#endif

  // Look for /*  */ comment and remove.
  while(1)
  {
    it = get_next_char(asm_context);
    if (it == EOF) break;
    if (it == '\n') asm_context->line++;
    if (it == '/' && cl == '*') break;
    cl = it;
  }
}

