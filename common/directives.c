/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/common.h"
#include "common/add_bin.h"
#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/directives_data.h"
#include "common/directives_if.h"
#include "common/directives_include.h"

int parse_org(struct _asm_context *asm_context)
{
  int num;

  if (eval_expression(asm_context, &num) == -1)
  {
    print_error("org expects an address", asm_context);
    return -1;
  }

  asm_context->address = num * asm_context->bytes_per_address;

  return 0;
}

static int parse_low_address(struct _asm_context *asm_context)
{
  int num;

  if (eval_expression(asm_context, &num) == -1)
  {
    print_error("low_address expects an address", asm_context);
    return -1;
  }

  asm_context->memory.low_address = num * asm_context->bytes_per_address;

  return 0;
}

static int parse_high_address(struct _asm_context *asm_context)
{
  int num;

  if (eval_expression(asm_context, &num) == -1)
  {
    print_error("high_address expects an address", asm_context);
    return -1;
  }

  asm_context->memory.high_address = num * asm_context->bytes_per_address;

  if (asm_context->bytes_per_address != 1)
  {
    asm_context->memory.high_address += asm_context->bytes_per_address - 1;
  }

  return 0;
}

static int parse_entry_point(struct _asm_context *asm_context)
{
  int num = 0;

  if (asm_context->pass == 1)
  {
    ignore_operand(asm_context);
  }
    else
  if (eval_expression(asm_context, &num) == -1)
  {
    print_error("entry_point expects an address", asm_context);
    return -1;
  }

  asm_context->memory.entry_point = num * asm_context->bytes_per_address;

  return 0;
}

static int parse_pragma(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
  }

  asm_context->tokens.line++;

  return 0;
}

static int parse_device(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  // FIXME - Do nothing right now

  token_type = tokens_get(asm_context, token, TOKENLEN);
  if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);
  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  asm_context->tokens.line++;

  return 0;
}

static int parse_set(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  char name[TOKENLEN];
  //char value[TOKENLEN];
  int num;
  int token_type;

  asm_context->ignore_symbols = 1;
  token_type = tokens_get(asm_context, name, TOKENLEN);
  asm_context->ignore_symbols = 0;

  if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  if (expect_token(asm_context, '=') != 0) { return -1; }

  if (eval_expression(asm_context, &num) == -1)
  {
    print_error("set expects an address", asm_context);
    return -1;
  }

#if 0
  token_type = tokens_get(asm_context, token, TOKENLEN);
  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF &&
      token_type == TOKEN_NUMBER)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }
#endif

  // REVIEW - should num be divided by bytes_per_address for dsPIC and avr8?
  symbols_set(&asm_context->symbols, name, num);

  //asm_context->tokens.line++;

  return 0;
}

static int parse_export(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  asm_context->ignore_symbols = 1;
  token_type = tokens_get(asm_context, token, TOKENLEN);
  asm_context->ignore_symbols = 0;

  if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  if (asm_context->pass == 2)
  {
    if (symbols_export(&asm_context->symbols, token) != 0)
    {
      print_not_defined(asm_context, token);
      return -1;
    }
  }

  return 0;
}

static int parse_equ(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  char name[TOKENLEN];
  char value[TOKENLEN];
  int token_type;

  // Atmel's include files want:  .equ NAME = VALUE

  asm_context->ignore_symbols = 1;
  token_type = tokens_get(asm_context, name, TOKENLEN);
  asm_context->ignore_symbols = 0;

  if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  if (expect_token(asm_context, '=') != 0) { return -1; }

  token_type = tokens_get(asm_context, value, TOKENLEN);

  token_type = tokens_get(asm_context, token, TOKENLEN);
  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  macros_append(asm_context, name, value, 0);

  asm_context->tokens.line++;

  return 0;
}

int parse_repeat(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type = tokens_get(asm_context, token, TOKENLEN);
  int count = atoi(token);

  if (token_type != TOKEN_NUMBER || count <= 0)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  asm_context->in_repeat = 1;

  uint32_t address_start = asm_context->address;

  if (assemble(asm_context) != 3)
  {
    print_error("Missing .endr in .repeat block.", asm_context);
    return -1;
  }

  asm_context->in_repeat = 0;

  uint32_t address_end = asm_context->address;
  int n, r;

  for (n = 0; n < count - 1; n++)
  {
    for (r = address_start; r < address_end; r++)
    {
      uint8_t data = memory_read_m(&asm_context->memory, r);
      add_bin8(asm_context, data, 0);
    }
  }

  if (asm_context->list != NULL && asm_context->write_list_file == 1)
  {
    asm_context->list_output(asm_context, address_end, asm_context->address);
    fprintf(asm_context->list, "\n");
  }

  return 0;
}

int parse_directives(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  token_type = tokens_get(asm_context, token, TOKENLEN);

#ifdef DEBUG
  printf("%d: <%d> %s\n", asm_context->tokens.line, token_type, token);
#endif

  if (token_type == TOKEN_EOF)
  {
    print_error("Missing directive", asm_context);
    return -1;
  }

  if (strcasecmp(token, "define") == 0)
  {
    if (macros_parse(asm_context, IS_DEFINE) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "ifdef") == 0)
  {
    parse_ifdef(asm_context, 0);
  }
    else
  if (strcasecmp(token, "ifndef") == 0)
  {
    parse_ifdef(asm_context, 1);
  }
    else
  if (strcasecmp(token, "if") == 0)
  {
    parse_if(asm_context);
  }
    else
  if (strcasecmp(token, "endif") == 0)
  {
    if (asm_context->ifdef_count < 1)
    {
      printf("Error: unmatched .endif at %s:%d\n",
        asm_context->tokens.filename, asm_context->ifdef_count);
      return -1;
    }

    return 0;
  }
    else
  if (strcasecmp(token, "else") == 0)
  {
    if (asm_context->ifdef_count < 1)
    {
      printf("Error: Unmatched .else at %s:%d\n",
        asm_context->tokens.filename,
        asm_context->ifdef_count);
      return -1;
    }

    return 4;
  }
    else
  if (strcasecmp(token, "repeat") == 0)
  {
    if (asm_context->in_repeat == 1)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (parse_repeat(asm_context) == -1) { return -1; }
  }
    else
  if (strcasecmp(token, "endr") == 0)
  {
    if (asm_context->in_repeat == 0)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    return 3;
  }
    else
  if (strcasecmp(token, "include") == 0)
  {
    if (include_parse(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "binfile") == 0)
  {
    if (binfile_parse(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "code") == 0)
  {
    asm_context->segment = SEGMENT_CODE;
  }
    else
  if (strcasecmp(token, "bss") == 0)
  {
    asm_context->segment = SEGMENT_BSS;
  }
    else
  if (strcasecmp(token, "msp430_cpu4") == 0)
  {
    asm_context->msp430_cpu4 = 1;
  }
    else
  if (strcasecmp(token, "macro") == 0)
  {
    if (macros_parse(asm_context, IS_MACRO) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "pragma") == 0)
  {
    if (parse_pragma(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "device") == 0)
  {
    if (parse_device(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "set") == 0)
  {
    if (parse_set(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "export") == 0)
  {
    if (parse_export(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "entry_point") == 0)
  {
    if (parse_entry_point(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "align") == 0 ||
      strcasecmp(token, "align_bits") == 0)
  {
    if (parse_align_bits(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "align_bytes") == 0)
  {
    if (parse_align_bytes(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "equ") == 0 || strcasecmp(token, "def") == 0)
  {
    if (parse_equ(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "scope") == 0)
  {
    if (symbols_scope_start(&asm_context->symbols) != 0)
    {
      printf("Error: Nested scopes are not allowed. %s:%d\n",
        asm_context->tokens.filename,
        asm_context->tokens.line);
      return -1;
    }
  }
    else
  if (strcasecmp(token, "ends") == 0)
  {
    symbols_scope_end(&asm_context->symbols);
  }
    else
  if (strcasecmp(token, "func") == 0)
  {
    char token[TOKENLEN];
    //int token_type;

    tokens_get(asm_context, token, TOKENLEN);
    symbols_append(
      &asm_context->symbols,
      token,
      asm_context->address / asm_context->bytes_per_address);

    if (symbols_scope_start(&asm_context->symbols) != 0)
    {
      printf("Error: Nested scopes are not allowed. %s:%d\n",
        asm_context->tokens.filename,
        asm_context->tokens.line);
      return -1;
    }
  }
    else
  if (strcasecmp(token, "endf") == 0)
  {
    symbols_scope_end(&asm_context->symbols);
  }
    else
  if (strcasecmp(token, "low_address") == 0)
  {
    if (parse_low_address(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "high_address") == 0)
  {
    if (parse_high_address(asm_context) != 0) { return -1; }
  }
    else
  if (strcasecmp(token, "big_endian") == 0)
  {
    asm_context->memory.endian = ENDIAN_BIG;
  }
    else
  if (strcasecmp(token, "little_endian") == 0)
  {
    asm_context->memory.endian = ENDIAN_LITTLE;
  }
    else
  if (strcasecmp(token, "list") == 0)
  {
    if (asm_context->pass == 2 && asm_context->list != NULL)
    {
      asm_context->write_list_file = 1;
      putc('\n', asm_context->list);
    }
  }
    else
  if (strcasecmp(token, "data_fill") == 0)
  {
    if (parse_data_fill(asm_context) != 0) { return -1; }
  }
    else
  {
    int ret = 0;

    do
    {
      // If the assembler wants a specific directive, it can be added
      // with this function pointer.
      if (asm_context->parse_directive != NULL)
      {
        ret = asm_context->parse_directive(asm_context, token);
        if (ret == 1) { break; }      // Found and used
        if (ret == -1) { return -1; } // Found and there was a problem
      }

      ret = assembler_directive(asm_context, token);

      if (ret == 1 || ret == 2) { break; }
      if (ret == -1) { return -1; }

      int n = 0;
      while (cpu_list[n].name != NULL)
      {
        if (strcasecmp(token, cpu_list[n].name) == 0)
        {
          assembler_set_cpu(asm_context, n);

#if 0
          if (strcmp(token, "65816") == 0)
          {
            asm_context->parse_directive = parse_directive_65816;
          }
            else
#endif
          {
            asm_context->parse_directive = NULL;
          }

          ret = 1;
          break;
        }

        n++;
      }

      if (ret == 1) { break; }

      printf("Error: Unknown directive '%s' at %s:%d.\n",
        token, asm_context->tokens.filename, asm_context->tokens.line);
      return -1;

    } while (0);
  }

  return 0;
}

