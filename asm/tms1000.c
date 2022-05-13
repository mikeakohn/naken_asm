/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/common.h"
#include "asm/tms1000.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/tms1000.h"

static char *tmsinstr_1[] = { "sbit", "rbit", "tbit1", "ldx" };
static char *tmsinstr_2[] = { "tcy", "ynec", "tcmiy", "alec", "ldp" };
static char *tms_branch[] = { "br", "call" };

static void add_bin_lsfr(struct _asm_context *asm_context, uint8_t data, int flags)
{
  int line = DL_NO_CG;

  if (asm_context->pass == 2 && flags == IS_OPCODE)
  {
    line = asm_context->tokens.line;
  }

  if (asm_context->pass == 1 && asm_context->pass_1_write_disable == 1)
  {
    asm_context->address++;
    return;
  }

  // CC PPPP PCPCPC
  int page = asm_context->address & 0xfc0;
  int pc = asm_context->address & 0x3f;

  memory_write_m(&asm_context->memory, page | tms1000_address_to_lsfr[pc], data);
  memory_debug_line_set(asm_context, page | tms1000_address_to_lsfr[pc], line);

  asm_context->address++;
}

int parse_instruction_tms1000(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  int n;

  lower_copy(instr_case, instr);

  n = 0;
  while (table_tms1000[n].instr != NULL)
  {
    if (strcmp(instr_case, table_tms1000[n].instr) == 0 &&
        table_tms1000[n].op1000 != 0xffff)
    {
      add_bin_lsfr(asm_context, table_tms1000[n].op1000, IS_OPCODE);

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      return 1;
    }
    n++;
  }

  for (n = 0; n < 4; n++)
  {
    if (strcmp(instr_case, tmsinstr_1[n]) == 0)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (token_type != TOKEN_NUMBER)
      {
        if (asm_context->pass == 1) { return 1; }
        print_error_unexp(token, asm_context);
        return -1;
      }

      int num = atoi(token);

      if (num < 0 || num > 3)
      {
        print_error_range("Constant", 0, 3, asm_context);
        return -1;
      }

      num = tms1000_reverse_bit_address[num];

      add_bin_lsfr(asm_context, ((0xc + n) << 2) | num, IS_OPCODE);

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      return 1;
    }
  }

  for (n = 0; n < 5; n++)
  {
    if (strcmp(instr_case, tmsinstr_2[n]) == 0)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (token_type != TOKEN_NUMBER)
      {
        if (asm_context->pass == 1) { return 1; }
        print_error_unexp(token, asm_context);
        return -1;
      }

      int num = atoi(token);
      if (num < 0 || num > 15)
      {
        print_error_range("Constant", 0, 15, asm_context);
        return -1;
      }

      num = tms1000_reverse_constant[num];

      if (n < 4)
      {
        add_bin_lsfr(asm_context, ((0x4 + n) << 4) | num, IS_OPCODE);
      }
        else
      {
        add_bin_lsfr(asm_context, 0x10 | num, IS_OPCODE);
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      return 1;
    }
  }

  for (n = 0; n < 2; n++)
  {
    // Branch / call instructions
    if (strcmp(instr_case, tms_branch[n]) == 0)
    {
      int address = 0;
      int page;

      if (eval_expression(asm_context, &address) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        ignore_operand(asm_context);
        address = asm_context->address;
      }

      int curr_page = asm_context->address >> 6;

      page = address >> 6;
      address &= 0x3f;

      if (page < 0 || page > 15)
      {
        print_error_range("Page", 0, 15, asm_context);
        return -1;
      }

      if (asm_context->pass == 2 && page != curr_page)
      {
        //add_bin_lsfr(asm_context, (0x10) | (page & 0xf), IS_OPCODE);
        printf("Warning: Branch crosses page boundary at %s:%d\n", asm_context->tokens.filename, asm_context->tokens.line);
      }

      add_bin_lsfr(asm_context, (0x80 | (n << 6)) | tms1000_address_to_lsfr[(address & 0x3f)], IS_OPCODE);

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      return 1;
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

int parse_instruction_tms1100(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  int n;

  lower_copy(instr_case, instr);

  n = 0;
  while (table_tms1000[n].instr != NULL)
  {
    if (strcmp(instr_case, table_tms1000[n].instr) == 0 &&
        table_tms1000[n].op1100 != 0xffff)
    {
      add_bin_lsfr(asm_context, table_tms1000[n].op1100, IS_OPCODE);

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      return 1;
    }
    n++;
  }

  for (n = 0; n < 4; n++)
  {
    if (strcmp(instr_case, tmsinstr_1[n]) == 0)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (token_type != TOKEN_NUMBER)
      {
        if (asm_context->pass == 1) { return 1; }
        print_error_unexp(token, asm_context);
        return -1;
      }

      int num = atoi(token);

      if (n == 3)
      {
        if (num < 0 || num > 7)
        {
          print_error_range("Constant", 0, 7, asm_context);
          return -1;
        }

        num = tms1000_reverse_constant[num] >> 1;

        add_bin_lsfr(asm_context, (0x5 << 3) | num, IS_OPCODE);
      }
        else
      {
        if (num < 0 || num > 3)
        {
          print_error_range("Constant", 0, 3, asm_context);
          return -1;
        }

        num = tms1000_reverse_bit_address[num];

        add_bin_lsfr(asm_context, ((0xc + n) << 2) | num, IS_OPCODE);
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      return 1;
    }
  }

  for (n = 0; n < 5; n++)
  {
    if (n == 3) { continue; }
    if (strcmp(instr_case, tmsinstr_2[n]) == 0)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (token_type != TOKEN_NUMBER)
      {
        if (asm_context->pass == 1) { return 1; }
        print_error_unexp(token, asm_context);
        return -1;
      }

      int num = atoi(token);
      if (num < 0 || num > 15)
      {
        print_error_range("Constant", 0, 15, asm_context);
        return -1;
      }

      num = tms1000_reverse_constant[num];

      if (n < 4)
      {
        add_bin_lsfr(asm_context, ((0x4 + n) << 4) | num, IS_OPCODE);
      }
        else
      {
        add_bin_lsfr(asm_context, 0x10 | num, IS_OPCODE);
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      return 1;
    }
  }

  for (n = 0; n < 2; n++)
  {
    // Branch / call instructions
    if (strcmp(instr_case, tms_branch[n]) == 0)
    {
      int address = 0;
      int page;

      if (eval_expression(asm_context, &address) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        ignore_operand(asm_context);
        address = asm_context->address;
      }

      int curr_page = asm_context->address >> 6;

      page = address >> 6;
      address &= 0x3f;

      if (page < 0 || page > 15)
      {
        print_error_range("Page", 0, 15, asm_context);
        return -1;
      }

      if (asm_context->pass == 2 && page != curr_page)
      {
        //add_bin_lsfr(asm_context, (0x10) | (page & 0xf), IS_OPCODE);
        printf("Warning: Branch crosses page boundary at %s:%d\n",
          asm_context->tokens.filename, asm_context->tokens.line);
      }

      add_bin_lsfr(asm_context, (0x80 | (n << 6)) | tms1000_address_to_lsfr[(address & 0x3f)], IS_OPCODE);

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      return 1;
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

