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

#include "asm/common.h"
#include "asm/msp430.h"
#include "common/assembler.h"
#include "common/cpu_list.h"
#include "common/directives.h"
#include "common/directives_data.h"
#include "common/directives_if.h"
#include "common/directives_include.h"
#include "common/eval_expression.h"
#include "common/tokens.h"
#include "common/ifdef_expression.h"
#include "common/macros.h"
#include "common/print_error.h"
#include "disasm/msp430.h"

AsmContext::AsmContext() :
  parse_instruction      (NULL),
  parse_directive        (NULL),
  link_function          (NULL),
  list_output            (NULL),
  list                   (NULL),
  address                (0),
  segment                (0),
  pass                   (1),
  instruction_count      (0),
  data_count             (0),
  code_count             (0),
  error_count            (0),
  ifdef_count            (0),
  parsing_ifdef          (0),
  linker                 (NULL),
  def_param_stack_count  (0),
  cpu_list_index         (0),
  cpu_type               (0),
  bytes_per_address      (1),
  is_dollar_hex          (false),
  strings_have_dots      (false),
  strings_have_slashes   (false),
  can_tick_end_string    (false),
  numbers_dont_have_dots (false),
  quiet_output           (false),
  error                  (false),
  msp430_cpu4            (false),
  ignore_symbols         (false),
  pass_1_write_disable   (false),
  write_list_file        (false),
  dump_symbols           (false),
  dump_macros            (false),
  optimize               (false),
  ignore_number_postfix  (false),
  in_repeat              (false),
  flags                  (0),
  extra_context          (0)
{
  memset(&tokens,  0, sizeof(tokens));
  memset(&macros,  0, sizeof(macros));

  memset(def_param_stack_data, 0, sizeof(def_param_stack_data));
  memset(def_param_stack_ptr, 0, sizeof(def_param_stack_ptr));
  memset(include_path, 0, sizeof(include_path));
}

AsmContext::~AsmContext()
{
  delete linker;

  macros_free(&macros);
}

void AsmContext::init()
{
  tokens_reset(this);
#ifndef NO_MSP430
  parse_instruction = parse_instruction_msp430;
  list_output = list_output_msp430;
  cpu_list_index = -1;
#else
  set_cpu(this, 0);
#endif
  address = 0;
  instruction_count = 0;
  code_count = 0;
  data_count = 0;
  ifdef_count = 0;
  parsing_ifdef = 0;
  bytes_per_address = 1;
  in_repeat = 0;

  macros_free(&macros);
  def_param_stack_count = 0;
}

void AsmContext::print_info(FILE *out)
{
  if (quiet_output) { return; }

  fprintf(out, "\nProgram Info:\n");

  if (dump_symbols == 1 || out != stdout)
  {
    symbols.print(out);
  }

  if (dump_macros == 1)
  {
    macros_print(&macros, out);
  }

  fprintf(out, "Include Paths: .\n");

  int ptr = 0;

  if (include_path[ptr] != 0)
  {
    fprintf(out, "               ");

    while (true)
    {
      if (include_path[ptr + 0] == 0 && include_path[ptr + 1] == 0)
      {
        fprintf(out, "\n");
        break;
      }

      if (include_path[ptr] == 0)
      {
        fprintf(out, "\n               ");
        ptr++;
        continue;
      }

      putc(include_path[ptr++], out);
    }
  }

  const uint32_t low_address  = memory.low_address  / bytes_per_address;
  const uint32_t high_address = memory.high_address / bytes_per_address;

  fprintf(out,
    " Instructions: %d\n"
    "   Code Bytes: %d\n"
    "   Data Bytes: %d\n"
    "  Low Address: 0x%04x (%u)\n"
    " High Address: 0x%04x (%u)\n\n",
    instruction_count,
    code_count,
    data_count,
    low_address, low_address,
    high_address, high_address);
}

void AsmContext::set_cpu(int index)
{
  cpu_type               = cpu_list[index].type;
  memory.endian          = cpu_list[index].default_endian;
  bytes_per_address      = cpu_list[index].bytes_per_address;
  is_dollar_hex          = cpu_list[index].is_dollar_hex;
  strings_have_dots      = cpu_list[index].strings_have_dots;
  strings_have_slashes   = cpu_list[index].strings_have_slashes;
  can_tick_end_string    = cpu_list[index].can_tick_end_string;
  pass_1_write_disable   = cpu_list[index].pass_1_write_disable;
  ignore_number_postfix  = cpu_list[index].ignore_number_postfix;
  numbers_dont_have_dots = cpu_list[index].numbers_dont_have_dots;
  parse_instruction      = cpu_list[index].parse_instruction;
  parse_directive        = cpu_list[index].parse_directive;
  link_function          = cpu_list[index].link_function;
  list_output            = cpu_list[index].list_output;
  flags                  = cpu_list[index].flags;
  cpu_list_index         = index;
}

int assembler_link_file(AsmContext *asm_context, const char *filename)
{
  int n;

  // FIXME: Checking the extension is redundant.
  n = strlen(filename);

  while (n >= 0)
  {
    n--;
    if (filename[n] == '.') { break; }
  }

  if (strcmp(filename + n, ".a") != 0 && strcmp(filename + n, ".o") != 0)
  {
    return -1;
  }

  if (asm_context->linker == NULL)
  {
    asm_context->linker = new Linker();
  }

  return asm_context->linker->add_file(filename);
}

int assembler_link(AsmContext *asm_context)
{
  if (asm_context->linker == NULL) { return 0; }

  Imports *imports;
  int index = 0;

  while (true)
  {
    const char *symbol = asm_context->linker->get_symbol_at_index(index);

    if (symbol == NULL) { break; }

    asm_context->symbols.append(symbol, asm_context->address);

    uint8_t *code;
    uint32_t function_offset;
    uint32_t function_size;
    uint8_t *obj_file;
    uint32_t obj_size;

    code = asm_context->linker->get_code_from_symbol(
      &imports,
      symbol,
      &function_offset,
      &function_size,
      &obj_file,
      &obj_size);

    int ret = asm_context->link_function(
      asm_context,
      imports,
      code,
      function_offset,
      function_size,
      obj_file,
      obj_size);

    if (ret != 0)
    {
      return -1;
    }

    if (asm_context->pass == 2)
    {
      if (asm_context->list != NULL && asm_context->write_list_file == 1)
      {
        uint32_t address;

        fprintf(asm_context->list, "[import]\n%s:", symbol);

        if (asm_context->symbols.lookup((char *)symbol, &address) == 0)
        {
          asm_context->list_output(asm_context, address, address + function_size);
          fprintf(asm_context->list, "\n");
        }
      }
    }

    index++;
  }

  return 0;
}

int assembler_directive(AsmContext *asm_context, char *token)
{
  if (strcasecmp(token, "org") == 0)
  {
    if (parse_org(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "db") == 0 ||
      strcasecmp(token, "dc8") == 0 ||
      strcasecmp(token, "ascii") == 0)
  {
    if (parse_db(asm_context, 0) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "asciiz") == 0)
  {
    if (parse_db(asm_context, 1) != 0) { return -1; }
    return 1;
  }
    else
#if 0
  if (strcasecmp(token, "dc") == 0)
  {
    if (parse_dc(asm_context) != 0) { return -1; }
    return 1;
  }
    else
#endif
  if (strcasecmp(token, "dw") == 0 || strcasecmp(token, "dc16") == 0)
  {
    if (parse_dc16(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "dl") == 0 ||
      strcasecmp(token, "dc32") == 0 ||
      strcasecmp(token, "dd") == 0)
  {
    if (parse_dc32(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "dc64") == 0 ||
      strcasecmp(token, "dq") == 0)
  {
    if (parse_dc64(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "varuint") == 0)
  {
    if (parse_varuint(asm_context, 0) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "varuint32") == 0)
  {
    if (parse_varuint(asm_context, 5) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "resb") == 0)
  {
    if (parse_resb(asm_context, 1) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "resw") == 0)
  {
    if (parse_resb(asm_context, 2) != 0) { return -1; }
    return 1;
  }
    else
  if (asm_context->cpu_type != CPU_TYPE_WEBASM && strcasecmp(token, "end") == 0)
  {
    // This is breaking webasm which has an "end" instruction.
    return 2;
  }

  return 0;
}

int assemble(AsmContext *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  while (1)
  {
    if (asm_context->error_count > 0) { return -1; }

    token_type = tokens_get(asm_context, token, TOKENLEN);

#ifdef DEBUG
    printf("%d: <%d> %s\n", asm_context->tokens.line, token_type, token);
#endif

    if (token_type == TOKEN_EOF) { break; }

    if (token_type == TOKEN_EOL)
    {
      if (asm_context->macros.stack_ptr == 0) { asm_context->tokens.line++; }
    }
      else
    if (token_type == TOKEN_LABEL)
    {
      int param_count_temp;
      if (macros_lookup(&asm_context->macros, token, &param_count_temp) != NULL)
      {
        print_already_defined(asm_context, token);
        return -1;
      }

      if (asm_context->symbols.append(token, asm_context->address / asm_context->bytes_per_address) == -1)
      {
        return -1;
      }
    }
      else
    if (token_type == TOKEN_POUND || IS_TOKEN(token,'.'))
    {
      int n = parse_directives(asm_context);

      // If n is 3, then this is ending a .repeat directive.
      if (n == 3) { return 3; }

      // If n is 4, then this is ending a .else directive.
      if (n == 4) { return 2; }

      // Otherwise there is a problem.
      if (n != 0) { return -1; }
    }
      else
    if (token_type == TOKEN_STRING)
    {
      int ret = assembler_directive(asm_context, token);

      if (ret == 2) { break; }
      if (ret == -1) { return -1; }

      if (ret != 1)
      {
        int start_address = asm_context->address;
        char token2[TOKENLEN];
        int token_type2;

        token_type2 = tokens_get(asm_context, token2, TOKENLEN);

        if (strcasecmp(token2, "equ") == 0)
        {
          //token_type2 = tokens_get(asm_context, token2, TOKENLEN);
          int ptr = 0;
          int ch = '\n';

          while (1)
          {
            ch = tokens_get_char(asm_context);
            if (ch == EOF || ch == '\n') break;
            if (ch == '\t') { ch = ' '; }
            if (ch == '*' && ptr > 0 && token2[ptr - 1] == '/')
            {
              macros_strip_comment(asm_context);
              ptr--;
              continue;
            }

            token2[ptr++] = ch;

            if (ptr == TOKENLEN - 1)
            {
              printf("Internal Error: token overflow at %s:%d.\n",
                __FILE__, __LINE__);
              return -1;
            }
          }

          token2[ptr] = 0;
          tokens_unget_char(asm_context, ch);
          macros_strip(token2);
          macros_append(asm_context, token, token2, 0);
        }
          else
        {
          tokens_push(asm_context, token2, token_type2);

          ret = asm_context->parse_instruction(asm_context, token);

          if (asm_context->list != NULL && asm_context->write_list_file == 1)
          {
            asm_context->list_output(asm_context, start_address, asm_context->address);
            fprintf(asm_context->list, "\n");
          }

          if (ret < 0) { return -1; }

          if (asm_context->macros.stack_ptr == 0) { asm_context->tokens.line++; }
          asm_context->instruction_count++;

          if (asm_context->address > start_address)
          {
            asm_context->code_count += (asm_context->address - start_address);
          }
        }
      }
    }
      else
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  if (asm_context->error == 1) { return -1; }

  return 0;
}

