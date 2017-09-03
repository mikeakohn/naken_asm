/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/6502.h"
#include "asm/65816.h"
#include "asm/6800.h"
#include "asm/6809.h"
#include "asm/68hc08.h"
#include "asm/68000.h"
#include "asm/8051.h"
#include "asm/arm.h"
#include "asm/avr8.h"
#include "asm/common.h"
#include "asm/dspic.h"
#include "asm/epiphany.h"
#include "asm/mips.h"
#include "asm/msp430.h"
#include "asm/stm8.h"
#include "asm/thumb.h"
#include "asm/tms1000.h"
#include "asm/tms9900.h"
#include "asm/z80.h"
#include "common/assembler.h"
#include "common/cpu_list.h"
#include "common/directives_data.h"
#include "common/directives_if.h"
#include "common/directives_include.h"
#include "common/eval_expression.h"
#include "common/tokens.h"
#include "common/ifdef_expression.h"
#include "common/macros.h"
#include "common/symbols.h"
#include "common/print_error.h"
#include "disasm/6502.h"
#include "disasm/6800.h"
#include "disasm/6809.h"
#include "disasm/68hc08.h"
#include "disasm/68000.h"
#include "disasm/arm.h"
#include "disasm/avr8.h"
#include "disasm/dspic.h"
#include "disasm/epiphany.h"
#include "disasm/mips.h"
#include "disasm/msp430.h"
#include "disasm/8051.h"
#include "disasm/stm8.h"
#include "disasm/thumb.h"
#include "disasm/tms1000.h"
#include "disasm/tms9900.h"
#include "disasm/z80.h"

static void configure_cpu(struct _asm_context *asm_context, int index)
{
  asm_context->cpu_type = cpu_list[index].type;
  asm_context->memory.endian = cpu_list[index].default_endian;
  asm_context->bytes_per_address = cpu_list[index].bytes_per_address;
  asm_context->is_dollar_hex = cpu_list[index].is_dollar_hex;
  asm_context->can_tick_end_string = cpu_list[index].can_tick_end_string;
  asm_context->pass_1_write_disable = cpu_list[index].pass_1_write_disable;
  asm_context->parse_instruction = cpu_list[index].parse_instruction;
  asm_context->parse_directive = cpu_list[index].parse_directive;
  asm_context->list_output = cpu_list[index].list_output;
  asm_context->flags = cpu_list[index].flags;
  asm_context->cpu_list_index = index;
}

static int parse_org(struct _asm_context *asm_context)
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

static int parse_entry_point(struct _asm_context *asm_context)
{
  int num = 0;

  if (asm_context->pass == 1)
  {
    eat_operand(asm_context);
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

static int parse_name(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  //int token_type;

  tokens_get(asm_context, token, TOKENLEN);

  printf("Program name: %s (ignored)\n", token);

  return 0;
}

static int parse_public(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  //int token_type;

  tokens_get(asm_context, token, TOKENLEN);

  printf("Public symbol: %s (ignored)\n", token);

  return 0;
}

static int parse_pragma(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
  }

  asm_context->line++;

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

  asm_context->line++;

  return 0;
}

static int parse_set(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  char name[TOKENLEN];
  //char value[TOKENLEN];
  int num;
  int token_type;

  asm_context->no_symbols = 1;
  token_type = tokens_get(asm_context, name, TOKENLEN);
  asm_context->no_symbols = 0;

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

  asm_context->line++;

  return 0;
}

static int parse_export(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  asm_context->no_symbols = 1;
  token_type = tokens_get(asm_context, token, TOKENLEN);
  asm_context->no_symbols = 0;

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

  //asm_context->line++;

  return 0;
}

static int parse_equ(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  char name[TOKENLEN];
  char value[TOKENLEN];
  int token_type;

  // Atmel's include files want:  .equ NAME = VALUE

  asm_context->no_symbols = 1;
  token_type = tokens_get(asm_context, name, TOKENLEN);
  asm_context->no_symbols = 0;

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

  asm_context->line++;

  return 0;
}

void assembler_init(struct _asm_context *asm_context)
{
  tokens_reset(asm_context);
#ifndef NO_MSP430
  asm_context->parse_instruction = parse_instruction_msp430;
  asm_context->list_output = list_output_msp430;
  asm_context->cpu_list_index = -1;
#else
  configure_cpu(asm_context, 0);
#endif
  asm_context->address = 0;
  asm_context->instruction_count = 0;
  asm_context->code_count = 0;
  asm_context->data_count = 0;
  asm_context->ifdef_count = 0;
  asm_context->parsing_ifdef = 0;
  asm_context->bytes_per_address = 1;

  macros_free(&asm_context->macros);
  asm_context->def_param_stack_count = 0;

  if (asm_context->pass == 1)
  {
    // FIXME - probably need to allow 32 bit data
    //memory_init(&asm_context->memory, 1<<25, 1);
    memory_init(&asm_context->memory, ~((uint32_t)0), 1);
  }
}

void assembler_free(struct _asm_context *asm_context)
{
  symbols_free(&asm_context->symbols);
  macros_free(&asm_context->macros);
  memory_free(&asm_context->memory);
}

void assembler_print_info(struct _asm_context *asm_context, FILE *out)
{
  if (asm_context->quiet_output) { return; }

  fprintf(out, "\nProgram Info:\n");

  if (asm_context->dump_symbols == 1 || out != stdout)
  {
    symbols_print(&asm_context->symbols, out);
  }

  if (asm_context->dump_macros == 1)
  {
    macros_print(&asm_context->macros, out);
  }

  fprintf(out, "Include Paths: .\n");

  int ptr = 0;

  if (asm_context->include_path[ptr] != 0)
  {
    fprintf(out, "               ");

    while(1)
    {
      if (asm_context->include_path[ptr] == 0 &&
          asm_context->include_path[ptr+1] == 0)
      { fprintf(out, "\n"); break; }

      if (asm_context->include_path[ptr] == 0)
      {
        fprintf(out, "\n               ");
        ptr++;
        continue;
      }
      putc(asm_context->include_path[ptr++], out); 
    }
  }

  fprintf(out, " Instructions: %d\n", asm_context->instruction_count);
  fprintf(out, "   Code Bytes: %d\n", asm_context->code_count);
  fprintf(out, "   Data Bytes: %d\n", asm_context->data_count);
  fprintf(out, "  Low Address: %04x (%d)\n",
    asm_context->memory.low_address / asm_context->bytes_per_address,
    asm_context->memory.low_address / asm_context->bytes_per_address);
  fprintf(out, " High Address: %04x (%d)\n",
    asm_context->memory.high_address / asm_context->bytes_per_address,
    asm_context->memory.high_address / asm_context->bytes_per_address);
  fprintf(out, "\n");
}

int check_for_directive(struct _asm_context *asm_context, char *token)
{
  if (strcasecmp(token, "org") == 0)
  {
    if (parse_org(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "entry_point") == 0)
  {
    if (parse_entry_point(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "align") == 0 || strcasecmp(token, "align_bits") == 0)
  {
    if (parse_align_bits(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "align_bytes") == 0)
  {
    if (parse_align_bytes(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "name") == 0)
  {
    if (parse_name(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "public") == 0)
  {
    if (parse_public(asm_context) != 0) { return -1; }
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
  if (strcasecmp(token, "dc") == 0)
  {
    if (parse_dc(asm_context) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "dw") == 0 || strcasecmp(token, "dc16") == 0)
  {
    if (parse_dc16(asm_context) != 0) return -1;
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
  if (strcasecmp(token, "ds") == 0 || strcasecmp(token, "ds8") == 0)
  {
    if (parse_ds(asm_context,1) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "ds16") == 0)
  {
    if (parse_ds(asm_context,2) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "ds32") == 0)
  {
    if (parse_ds(asm_context,4) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "resb") == 0)
  {
    if (parse_resb(asm_context,1) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "resw") == 0)
  {
    if (parse_resb(asm_context,2) != 0) { return -1; }
    return 1;
  }
    else
  if (strcasecmp(token, "end") == 0)
  {
    return 2;
  }
    else
  if (strcasecmp(token, "big_endian") == 0)
  {
    asm_context->memory.endian = ENDIAN_BIG;
    return 1;
  }
    else
  if (strcasecmp(token, "little_endian") == 0)
  {
    asm_context->memory.endian = ENDIAN_LITTLE;
    return 1;
  }
    else
  if (strcasecmp(token, "list") == 0)
  {
    if (asm_context->pass == 2 && asm_context->list != NULL)
    {
      asm_context->write_list_file = 1;
      putc('\n', asm_context->list);
    }
    return 1;
  }
    else
  if (strcasecmp(token, "scope") == 0)
  {
    if (symbols_scope_start(&asm_context->symbols) != 0)
    {
      printf("Error: Nested scopes are not allowed. %s:%d\n",
        asm_context->filename,
        asm_context->line);
      return -1;
    }

    return 1;
  }
    else
  if (strcasecmp(token, "ends") == 0)
  {
    symbols_scope_end(&asm_context->symbols);
    return 1;
  }
    else
  if (strcasecmp(token, "func") == 0)
  {
    char token[TOKENLEN];
    //int token_type;

    tokens_get(asm_context, token, TOKENLEN);
    symbols_append(&asm_context->symbols, token, asm_context->address);

#if 0
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type != TOKEN_EOL)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
#endif

    if (symbols_scope_start(&asm_context->symbols) != 0)
    {
      printf("Error: Nested scopes are not allowed. %s:%d\n",
        asm_context->filename,
        asm_context->line);
      return -1;
    }

    return 1;
  }
    else
  if (strcasecmp(token, "endf") == 0)
  {
    symbols_scope_end(&asm_context->symbols);
    return 1;
  }

  if (asm_context->parse_directive != NULL)
  {
    int ret = asm_context->parse_directive(asm_context, token);
    if (ret == 0) { return 1; }   // Found and used
    if (ret == -1) { return -1; } // Found and there was a problem
  }

  int n = 0;
  while (cpu_list[n].name != NULL)
  {
    if (strcasecmp(token, cpu_list[n].name) == 0)
    {
      configure_cpu(asm_context, n);

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

      return 1;
    }
    n++;
  }

  return 0;
}

int assemble(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
#ifdef DEBUG
    printf("%d: <%d> %s\n", asm_context->line, token_type, token);
#endif
    if (token_type == TOKEN_EOF) break;

    if (token_type == TOKEN_EOL)
    {
      if (asm_context->macros.stack_ptr == 0) { asm_context->line++; }
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

      if (symbols_append(&asm_context->symbols, token, asm_context->address / asm_context->bytes_per_address) == -1)
      {
        return -1;
      }
    }
      else
    if (token_type == TOKEN_POUND || IS_TOKEN(token,'.'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
#ifdef DEBUG
    printf("%d: <%d> %s\n", asm_context->line, token_type, token);
#endif
      if (token_type == TOKEN_EOF) break;

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
          printf("Error: unmatched .endif at %s:%d\n", asm_context->filename, asm_context->ifdef_count);
          return -1;
        }
        return 0;
      }
        else
      if (strcasecmp(token, "else") == 0)
      {
        if (asm_context->ifdef_count < 1)
        {
          printf("Error: unmatched .else at %s:%d\n", asm_context->filename, asm_context->ifdef_count);
          return -1;
        }
        return 2;
      }
        else
      if (strcasecmp(token, "include") == 0)
      {
        if (parse_include(asm_context) != 0) return -1;
      }
        else
      if (strcasecmp(token, "binfile") == 0)
      {
        if (parse_binfile(asm_context) != 0) return -1;
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
        if (parse_export(asm_context) != 0) return -1;
      }
        else
      if (strcasecmp(token, "equ") == 0 || strcasecmp(token, "def")==0)
      {
        if (parse_equ(asm_context) != 0) return -1;
      }
        else
      {
        int ret = check_for_directive(asm_context, token);
        if (ret == 2) break;
        if (ret == -1) return -1;
        if (ret != 1)
        {
          printf("Error: Unknown directive '%s' at %s:%d.\n", token, asm_context->filename, asm_context->line);
          return -1;
        }
      }
    }
      else
    if (token_type == TOKEN_STRING)
    {
      int ret = check_for_directive(asm_context, token);
      if (ret == 2) break;
      if (ret == -1) return -1;
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

          while(1)
          {
            ch = tokens_get_char(asm_context);
            if (ch == EOF || ch == '\n') break;
            if (ch == '*' && ptr > 0 && token2[ptr-1] == '/')
            {
              macros_strip_comment(asm_context);
              ptr--;
              continue;
            }

            token2[ptr++] = ch;
            if (ptr == TOKENLEN-1)
            {
              printf("Internal Error: token overflow at %s:%d.\n", __FILE__, __LINE__);
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

          if (ret < 0) return -1;

          if (asm_context->macros.stack_ptr == 0) { asm_context->line++; }
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
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  if (asm_context->error == 1) { return -1; }

  return 0;
}

