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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/macros.h"
#include "common/memory_pool.h"
#include "common/symbols.h"
#include "common/tokens.h"

static int get_param_index(char *params, char *name)
{
  int count = 0;
  int ptr = 0;

  while (params[ptr] != 0)
  {
    if (strcmp(params + ptr, name) == 0) { return count + 1; }

    count++;

    ptr = ptr + strlen(params + ptr) + 1;
  }

  return 0;
}

static int macros_parse_token(struct _asm_context *asm_context, char *token, int len, int macro_type)
{
  int ptr = 0;
  char ch;

  while (1)
  {
    ch = tokens_get_char(asm_context);

    if (ch == '\t') { ch = ' '; }

    if (ch == ' ')
    {
      if (ptr == 0) { continue; }

      if (macro_type == IS_DEFINE)
      {
        token[ptr] = 0;
        return 0;
      }

      // Check for (
      while (1)
      {
        ch = tokens_get_char(asm_context);

        if (ch == '\t') { ch = ' '; }
        if (ch == ' ') { continue; }

        if (ch == '(')
        {
          token[ptr] = 0;
          return 1;
        }

        tokens_unget_char(asm_context, ch);
        break;
      }

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
      tokens_unget_char(asm_context, ch);
      break;
    }

    if (ptr == len - 1) { break; }
  }

  token[ptr] = 0;

  return 0;
}

// Note to self: This is not the best way to take care of this.
static int check_endm(char *macro, int ptr)
{
  ptr--;

  // Ignore white space at start of the line
  while (ptr > 0 && (macro[ptr] == ' ' || macro[ptr] == '\t'))
  {
    ptr--;
  }

  // Ignore white space before a possible .endm
  while (ptr > 0 && !(macro[ptr] == '\n' || macro[ptr] == ' ' || macro[ptr] == '\t'))
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
  uint32_t address;
  int param_count_temp;
  int name_len;
  int value_len;

  if (macros->locked == 1) { return 0; }

  if (macros_lookup(macros, name, &param_count_temp) != NULL ||
      symbols_lookup(&asm_context->symbols, name, &address) == 0)
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

  // If there is no pool, add one.
  if (memory_pool == NULL)
  {
    memory_pool = memory_pool_add((struct _naken_heap *)macros, MACROS_HEAP_SIZE);
  }

  // Find a pool that has enough area at the end to add this macro.
  // If none can be found, alloc a new one.
  while (1)
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
  // FIXME - probably should align by sizeof(void *) for RISC machines.
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

  while (memory_pool != NULL)
  {
    ptr = 0;
    while (ptr < memory_pool->ptr)
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

  while (memory_pool != NULL)
  {
    if (iter->ptr < memory_pool->ptr)
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

int macros_print(struct _macros *macros, FILE *out)
{
  struct _macros_iter iter;

  memset(&iter, 0, sizeof(iter));

  fprintf(out, "%18s %s\n", "NAME", "VALUE");

  while (macros_iterate(macros, &iter) != -1)
  {
    if (iter.param_count == 0)
    {
      fprintf(out, "%30s=", iter.name);
    }
      else
    {
      int n;

      fprintf(out, "%30s(", iter.name);

      for (n = 0; n < iter.param_count; n++)
      {
        if (n != 0) { fprintf(out, ","); }
        fprintf(out, "{%d}", n + 1);
      }
      fprintf(out, ")=");
    }

    char *value = iter.value;

    while (*value != 0)
    {
      if (*value == 1)
      {
        value++;
        fprintf(out, "{%d}", *value);
      }
      else
      {
        fprintf(out, "%c", *value);
      }

      value++;
    }

    fprintf(out, "\n");
  }

  fprintf(out, "Total %d.\n\n", iter.count);

  return 0;
}

int macros_push_define(struct _macros *macros, char *define)
{
#ifdef DEBUG
printf("debug> macros_push_define(), define=%s macros->stack_ptr=%d\n", define, macros->stack_ptr);
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

  while (1)
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
printf("debug> macros_get_char() asm_context->def_param_stack_count=%d\n",asm_context->def_param_stack_count);
#endif
    }
    macros->stack_ptr--;
    asm_context->tokens.unget_stack_ptr--;

    // Check if something need to be ungetted
    if (asm_context->tokens.unget_ptr > asm_context->tokens.unget_stack[asm_context->tokens.unget_stack_ptr])
    {
#ifdef DEBUG
printf("debug> macros_get_char() tokens_get_char(?) ungetc %d %d '%c'\n", asm_context->tokens.unget_stack_ptr, asm_context->tokens.unget_stack[asm_context->tokens.unget_stack_ptr], asm_context->tokens.unget[asm_context->tokens.unget_ptr-1]);
#endif
      return asm_context->tokens.unget[--asm_context->tokens.unget_ptr];
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
    if (*s == '/' && *(s+1) == '/') { *s = 0; break; }
    s++;
  }

  if (s != macro) { s--; }
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
  int parens = 0;
  int param_count = 0;

  // First pull the name out
  parens = macros_parse_token(asm_context, name, 128, macro_type);
  if (parens == -1) { return -1; }

#ifdef DEBUG
printf("debug> macros_parse() name=%s parens_flag=%d\n", name, parens);
#endif

  // Now pull any params out
  ptr = 0;

  if (parens != 0)
  {
    while (1)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
#ifdef DEBUG
printf("debug> macros_parse() param %s\n", token);
#endif
      if (token_type != TOKEN_STRING)
      {
        printf("Error: Expected a param name but got '%s' at %s:%d.\n", token,
          asm_context->tokens.filename, asm_context->tokens.line);
        return -1;
      }

      param_count++;

      if (param_count > 255)
      {
        print_error("Error: Too many macro parameters", asm_context);
        return -1;
      }

      int len = strlen(token);

      if (ptr + len + 2 > 1024)
      {
        print_error("Error: Macro with too long parameter list", asm_context);
        return -1;
      }

      strcpy(params + ptr, token);
      ptr = ptr + len + 1;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      // End of parameter list
      if (IS_TOKEN(token,')'))
      {
        break;
      }

      // Make sure there is a comma between parameters
      if (IS_NOT_TOKEN(token,','))
      {
        print_error("Expected ',' or ')'", asm_context);
        return -1;
      }
    }
  }

  params[ptr] = 0;

  if (macro_type != IS_DEFINE)
  {
    // Fixing where the user could have extra crap at the end of the macro
    // line.
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type != TOKEN_EOL)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    asm_context->tokens.line++;
  }

#ifdef DEBUG
printf("debug> macros_parse() param count=%d\n", param_count);
#endif

  // Now macro time
  ptr = 0;
  name_test = NULL;

  while (1)
  {
    ch = tokens_get_char(asm_context);

    // Tabs :(
    if (ch == '\t') { ch = ' '; }

    if (name_test == NULL)
    {
      if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
      {
        name_test = macro + ptr;
      }
    }
      else
    if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
          (ch >= '0' && ch <= '9') || ch == '_'))
    {
      if (name_test != NULL)
      {
        macro[ptr] = 0;

        int index = get_param_index(params,name_test);

#ifdef DEBUG
printf("debug> macros_parse() name_test='%s' %d\n", name_test, index);
#endif

        if (index != 0)
        {
          ptr = name_test - macro;

          // A paramter in the macro text is chr(1), index
          macro[ptr++] = 1;
          macro[ptr++] = index;
        }

        name_test = NULL;
      }
    }

    // If there is a comment on this line of the macro, ignore the rest of
    // of the line.
    if (ch == ';' || (ptr > 0 && ch == '/' && macro[ptr-1] == '/'))
    {
      if (macro[ptr-1] == '/') { ptr--; }

      while (1)
      {
        ch = tokens_get_char(asm_context);
        if (ch == '\t') { ch = ' '; }
        if (ch == '\n' || ch == EOF) { break; }
      }

      while (ptr > 0)
      {
        if (macro[ptr-1] != ' ') { break; }
        ptr--;
      }
    }

    if (ch == '\r') { continue; }
    if (ch == ' ' && ptr == 0) { continue; }

    if (ch == '\\')
    {
      if (macro_type == IS_DEFINE)
      {
        while (1)
        {
          ch = tokens_get_char(asm_context);
          if (ch != '\r') { break; }
        }

        if (ch != '\n')
        {
          print_error("Error: Expected end-of_line", asm_context);
          return -1;
        }

        asm_context->tokens.line++;

        continue;
      }
    }

    if (ch == '\n' || ch == EOF)
    {
      asm_context->tokens.line++;

      if (macro_type == IS_DEFINE)
      {
        break;
      }
        else
      {
        macro[ptr] = 0;
        if (check_endm(macro, ptr) == 1) { break; }
      }
    }

    if (ch == '*' && ptr > 0 && macro[ptr-1] == '/')
    {
      macros_strip_comment(asm_context);
      ptr--;
      continue;
    }

    macro[ptr++] = ch;
    if (ptr >= MAX_MACRO_LEN - 2)
    {
      printf("Internal error: macro longer than %d bytes on line %d\n", MAX_MACRO_LEN, asm_context->tokens.line);
      return -1;
    }
  }

  // Adding a single whitespace to the end to separate this macro
  // from the next token.
  macro[ptr++] = ' ';
  macro[ptr++] = 0;

  macros_strip(macro);

#ifdef DEBUG
printf("debug> Adding macro '%s'\n", macro);
#endif

  macros_append(asm_context, name, macro, param_count);

  return 0;
}

char *macros_expand_params(struct _asm_context *asm_context, char *define, int param_count)
{
  int ch;
  char params[1024];
  int params_ptr[256];
  int count,ptr;
  uint8_t in_string = 0;
  uint8_t open_parens = 0;

  while (1)
  {
    ch = tokens_get_char(asm_context);

    if (ch == '\t') { ch = ' '; }
    if (ch != ' ') { break; }
  }

  if (ch != '(')
  {
    print_error("Macro expects params", asm_context);
    asm_context->error = 1;
    return NULL;
  }

  count = 0;
  ptr = 0;
  params_ptr[count] = ptr;

  while (1)
  {
    ch = tokens_get_char(asm_context);

    if (ch == '\t') { ch = ' '; }
    if (ch == '\r') { continue; }
    // skip whitespace immediately after opening parenthesis or a comma
    if ((ch == ' ' || ch == '\t') && (ptr == 0 || params[ptr-1] == 0)) { continue; }
    if (ch == '"') { in_string = in_string ^ 1; }
    if (ch == ')' && in_string == 0 && open_parens == 0) { break; }

    if (ch == '\n' || ch == EOF)
    {
      print_error("Macro expects ')'", asm_context);
      return NULL;
    }

    if (ch == ',' && !in_string && open_parens == 0)
    {
      params[ptr++] = 0;
      params_ptr[++count] = ptr;
      continue;
    }

    if (ch == '(' && !in_string) { open_parens++; }
    if (ch == ')' && !in_string) { open_parens--; }

    params[ptr++] = ch;
  }

  params[ptr++] = 0;
  count++;

  if (count != param_count)
  {
    printf("Error: Macro expects %d params, but got only %d at %s:%d.\n",
      param_count, count, asm_context->tokens.filename, asm_context->tokens.line);
    return NULL;
  }

#ifdef DEBUG
printf("debug> macros_expand_params() with params: pass=%d\n", asm_context->pass);
int n;
for (n = 0; n < count; n++)
{
  printf("debug>   %s\n", params + params_ptr[n]);
}
#endif

  ptr = asm_context->def_param_stack_ptr[asm_context->def_param_stack_count];

  while (*define != 0)
  {
    if (*define == 1)
    {
      define++;

      strcpy(asm_context->def_param_stack_data + ptr, params + params_ptr[((int)*define)-1]);

      while (*(asm_context->def_param_stack_data + ptr) != 0) { ptr++; }
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
  int ch, last = 0;

#ifdef DEBUG
printf("debug> macros_strip_comment()\n");
#endif

  // Look for /*  */ comment and remove.
  while (1)
  {
    ch = tokens_get_char(asm_context);
    if (ch == '\t') { ch = ' '; }
    if (ch == EOF) break;
    if (ch == '\n') asm_context->tokens.line++;
    if (ch == '/' && last == '*') { break; }
    last = ch;
  }
}

