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

#include "asm_65xx.h"
#include "asm_680x.h"
#include "asm_68hc08.h"
#include "asm_680x0.h"
#include "asm_805x.h"
#include "asm_arm.h"
#include "asm_avr8.h"
#include "asm_common.h"
#include "asm_dspic.h"
#include "asm_mips.h"
#include "asm_msp430.h"
#include "asm_stm8.h"
#include "asm_thumb.h"
#include "asm_tms1000.h"
#include "asm_tms9900.h"
#include "asm_z80.h"
#include "assembler.h"
#include "cpu_list.h"
#include "disasm_65xx.h"
#include "disasm_680x.h"
#include "disasm_68hc08.h"
#include "disasm_680x0.h"
#include "disasm_arm.h"
#include "disasm_avr8.h"
#include "disasm_dspic.h"
#include "disasm_mips.h"
#include "disasm_msp430.h"
#include "disasm_805x.h"
#include "disasm_stm8.h"
#include "disasm_thumb.h"
#include "disasm_tms1000.h"
#include "disasm_tms9900.h"
#include "disasm_z80.h"
#include "directives_data.h"
#include "directives_if.h"
#include "directives_include.h"
#include "eval_expression.h"
#include "get_tokens.h"
#include "ifdef_expression.h"
#include "address_list.h"
#include "macros.h"
#include "print_error.h"

static int parse_org(struct _asm_context *asm_context)
{
int num;

  if (eval_expression(asm_context, &num) == -1)
  {
    print_error("org expects an address", asm_context);
    return -1;
  }

  asm_context->address = num*asm_context->bytes_per_address;

  return 0;
}

static int parse_align(struct _asm_context *asm_context)
{
int num;

  if (eval_expression(asm_context, &num) == -1 || (num != 16 && num != 32))
  {
    print_error("align expects 16 or 32", asm_context);
    return -1;
  }

  if (num == 16)
  {
    if ((asm_context->address&1) != 0)
    {
      memory_write_inc(asm_context, 0, DL_DATA);
    }
  }
    else
  if (num == 32)
  {
    if ((asm_context->address&3) != 0)
    {
      int n;
      for (n = (asm_context->address&3); n < 4; n++)
      {
        memory_write_inc(asm_context, 0, DL_DATA);
      }
    }
  }

  return 0;
}

static int parse_name(struct _asm_context *asm_context)
{
char token[TOKENLEN];
//int token_type;

  get_token(asm_context, token, TOKENLEN);

  printf("Program name: %s (ignored)\n", token);

  return 0;
}

static int parse_public(struct _asm_context *asm_context)
{
char token[TOKENLEN];
//int token_type;

  get_token(asm_context, token, TOKENLEN);

  printf("Public symbol: %s (ignored)\n", token);

  return 0;
}

static int parse_pragma(struct _asm_context *asm_context)
{
char token[TOKENLEN];
int token_type;

  while(1)
  {
    token_type = get_token(asm_context, token, TOKENLEN);
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

  token_type = get_token(asm_context, token, TOKENLEN);
  if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  token_type = get_token(asm_context, token, TOKENLEN);
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

  token_type = get_token(asm_context, name, TOKENLEN);
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
  token_type = get_token(asm_context, token, TOKENLEN);
  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF &&
      token_type == TOKEN_NUMBER)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }
#endif

  address_list_set(asm_context, name, num);

  asm_context->line++;

  return 0;
}

static int parse_equ(struct _asm_context *asm_context)
{
char token[TOKENLEN];
char name[TOKENLEN];
char value[TOKENLEN];
int token_type;

  // Atmel's include files want:  .equ NAME = VALUE

  token_type = get_token(asm_context, name, TOKENLEN);
  if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  if (expect_token(asm_context, '=') != 0) { return -1; }

  token_type = get_token(asm_context, value, TOKENLEN);

  token_type = get_token(asm_context, token, TOKENLEN);
  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  defines_heap_append(asm_context, name, value, 0);

  asm_context->line++;

  return 0;
}

void assemble_init(struct _asm_context *asm_context)
{
  fseek(asm_context->in, 0, SEEK_SET);
  asm_context->parse_instruction = parse_instruction_msp430;
  asm_context->list_output = list_output_msp430;
  asm_context->address = 0;
  asm_context->line = 1;
  asm_context->instruction_count = 0;
  asm_context->code_count = 0;
  asm_context->data_count = 0;
  asm_context->ifdef_count = 0;
  asm_context->parsing_ifdef = 0;
  asm_context->pushback[0] = 0;
  asm_context->unget[0] = 0;
  asm_context->unget_ptr = 0;
  asm_context->unget_stack_ptr = 0;
  asm_context->unget_stack[0] = 0;
  asm_context->bytes_per_address = 1;
  asm_context->cpu_list_index = -1;

  defines_heap_free(&asm_context->defines_heap);
  asm_context->def_param_stack_count = 0;
  if (asm_context->pass == 1)
  {
    // FIXME - probably need to allow 32 bit data
    memory_init(&asm_context->memory, 1<<25, 1);
  }
}

void assemble_print_info(struct _asm_context *asm_context, FILE *out)
{
  fprintf(out, "\nProgram Info:\n");
#ifdef DEBUG
  address_list_print(&asm_context->address_list);
  defines_heap_print(&asm_context->defines_heap);
#endif

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
    asm_context->memory.low_address/asm_context->bytes_per_address,
    asm_context->memory.low_address/asm_context->bytes_per_address);
  fprintf(out, " High Address: %04x (%d)\n",
    asm_context->memory.high_address/asm_context->bytes_per_address,
    asm_context->memory.high_address/asm_context->bytes_per_address);
  fprintf(out, "\n");
}

int check_for_directive(struct _asm_context *asm_context, char *token)
{
  if (strcasecmp(token, "org") == 0)
  {
    if (parse_org(asm_context) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "align") == 0)
  {
    if (parse_align(asm_context) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "name") == 0)
  {
    if (parse_name(asm_context) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "public") == 0)
  {
    if (parse_public(asm_context) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "db") == 0 ||
      strcasecmp(token, "dc8") == 0 ||
      strcasecmp(token, "ascii") == 0)
  {
    if (parse_db(asm_context, 0) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "asciiz") == 0)
  {
    if (parse_db(asm_context, 1) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "dc") == 0)
  {
    if (parse_dc(asm_context) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "dw") == 0 || strcasecmp(token, "dc16") == 0)
  {
    if (parse_dw(asm_context) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "dl") == 0 || strcasecmp(token, "dc32") == 0)
  {
    if (parse_dl(asm_context) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "ds") == 0 || strcasecmp(token, "ds8") == 0)
  {
    if (parse_ds(asm_context,1) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "ds16") == 0)
  {
    if (parse_ds(asm_context,2) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "resb") == 0)
  {
    if (parse_resb(asm_context,1) != 0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "resw") == 0)
  {
    if (parse_resb(asm_context,2) != 0) return -1;
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
    asm_context->memory.endian=ENDIAN_BIG;
  }
    else
  if (strcasecmp(token, "little_endian") == 0)
  {
    asm_context->memory.endian=ENDIAN_LITTLE;
  }

  int n = 0;
  while (cpu_list[n].name != NULL)
  {
    if (strcasecmp(token, cpu_list[n].name) == 0)
    {
      asm_context->cpu_type = cpu_list[n].type;
      asm_context->memory.endian = cpu_list[n].default_endian;
      asm_context->bytes_per_address = cpu_list[n].bytes_per_address;
      asm_context->is_dollar_hex = cpu_list[n].is_dollar_hex;
      asm_context->can_tick_end_string = cpu_list[n].can_tick_end_string;
      asm_context->parse_instruction = cpu_list[n].parse_instruction;
      asm_context->list_output = cpu_list[n].list_output;
      asm_context->cpu_list_index = n;
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
    token_type = get_token(asm_context, token, TOKENLEN);
#ifdef DEBUG
    printf("%d: <%d> %s\n", asm_context->line, token_type, token);
#endif
    if (token_type == TOKEN_EOF) break;

    if (token_type == TOKEN_EOL)
    {
      if (asm_context->defines_heap.stack_ptr == 0) { asm_context->line++; }
    }
      else
    if (token_type == TOKEN_LABEL)
    {
      if (address_list_append(asm_context, token, asm_context->address) == -1)
      { return -1; }
    }
      else
    if (token_type == TOKEN_POUND || IS_TOKEN(token,'.'))
    {
      token_type = get_token(asm_context, token, TOKENLEN);
#ifdef DEBUG
    printf("%d: <%d> %s\n", asm_context->line, token_type, token);
#endif
      if (token_type == TOKEN_EOF) break;

      if (strcasecmp(token, "define") == 0)
      {
        if (parse_macro(asm_context, IS_DEFINE) != 0) return -1;
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
          printf("Error: unmatched #endif at %s:%d\n", asm_context->filename, asm_context->ifdef_count);
          return -1;
        }
        return 0;
      }
        else
      if (strcasecmp(token, "else") == 0)
      {
        if (asm_context->ifdef_count < 1)
        {
          printf("Error: unmatched #else at %s:%d\n", asm_context->filename, asm_context->ifdef_count);
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
        if (parse_macro(asm_context, IS_MACRO) != 0) return -1;
      }
        else
      if (strcasecmp(token, "pragma") == 0)
      {
        if (parse_pragma(asm_context) != 0) return -1;
      }
        else
      if (strcasecmp(token, "device") == 0)
      {
        if (parse_device(asm_context) != 0) return -1;
      }
        else
      if (strcasecmp(token, "set") == 0)
      {
        if (parse_set(asm_context) != 0) return -1;
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

        token_type2 = get_token(asm_context, token2, TOKENLEN);

        if (strcasecmp(token2, "equ") == 0)
        {
          //token_type2=get_token(asm_context, token2, TOKENLEN);
          int ptr = 0;
          int ch = '\n';

          while(1)
          {
            ch = get_next_char(asm_context);
            if (ch == EOF || ch == '\n') break;
            if (ch == '*' && ptr > 0 && token2[ptr-1] == '/')
            {
              eatout_star_comment(asm_context);
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
          unget_next_char(asm_context, ch);
          strip_macro(token2);
          defines_heap_append(asm_context, token, token2, 0);
        }
          else
        {
          pushback(asm_context, token2, token_type2);
          //int address=asm_context->address;

          //ret=parse_instruction_msp430(asm_context, token);
          ret = asm_context->parse_instruction(asm_context, token);
          if (asm_context->pass == 2 && asm_context->list != NULL &&
              asm_context->include_count==0)
          {
            asm_context->list_output(asm_context, start_address);
            fprintf(asm_context->list, "\n");
          }

          if (ret < 0) return -1;

#if 0
if (asm_context->address-start_address==0)
{
  printf("ZOMG %x  ret=%d %d\n", start_address, ret, asm_context->address-start_address);
}
#endif
          if (asm_context->defines_heap.stack_ptr == 0) { asm_context->line++; }
          asm_context->instruction_count++;

          if (asm_context->address>start_address)
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

