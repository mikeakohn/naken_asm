/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/cell.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/cell.h"

#define MAX_OPERANDS 5

enum
{
  OPERAND_INVALID,
  OPERAND_REGISTER,
  OPERAND_SPR,
  OPERAND_TBR,
  OPERAND_CR,
  OPERAND_NUMBER,
  OPERAND_REGISTER_OFFSET,
};

struct _operand
{
  int value;
  int type;
  int16_t offset;
};

struct _modifiers
{
  int has_dot;
};

static int get_register_number(char *token)
{ 
  int num;
  
  if (token[0] < '0' || token[0] > '9') { return -1; }
  if (token[1] == 0) { return token[0] - '0'; }
  if (token[0] == '0' || token[2] != 0) { return -1; }
  if (token[1] < '0' || token[1] > '9') { return -1; }
  
  num = ((token[0] - '0') * 10) + (token[1] - '0');
  
  return (num < 32) ? num : -1;
}

static int get_register_cell(char *token)
{ 
  if (token[0] != 'r') { return -1; }
  
  return get_register_number(token + 1);
}

static int get_operands(struct _asm_context *asm_context, struct _operand *operands, char *instr, char *instr_case)
{
  char token[TOKENLEN];
  int token_type;
  int operand_count = 0;
  int n;

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    do
    {
      // Check for registers
      n = get_register_cell(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_REGISTER;
        operands[operand_count].value = n;
        break;
      }

      // Assume this is just a number
      operands[operand_count].type = OPERAND_NUMBER;

      if (asm_context->pass == 1)
      {
        eat_operand(asm_context);
        operands[operand_count].value = 0;
      }
      else
      {
        tokens_push(asm_context, token, token_type);
        if (eval_expression(asm_context, &n) != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].value = n;

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token, '('))
        {
          if (operands[operand_count].value < -32768 ||
              operands[operand_count].value > 32767)
          {
            print_error_range("Offset", -32768, 32767, asm_context);
            return -1;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          n = get_register_cell(token);
          if (n == -1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].offset = (uint16_t)operands[operand_count].value;
          operands[operand_count].type = OPERAND_REGISTER_OFFSET;
          operands[operand_count].value = n;

          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token, ')'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
          else
        {
          tokens_push(asm_context, token, token_type);
        }
      }

      break;
    } while(0);

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token, ',') || operand_count == 5)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  return operand_count;
}

int parse_instruction_cell(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count;
  int matched = 0;
  //uint32_t opcode;
  //int32_t offset;
  //int temp;
  int n;

  memset(&operands, 0, sizeof(operands));

  lower_copy(instr_case, instr);

  operand_count = get_operands(asm_context, operands, instr, instr_case);

  if (operand_count < 0) { return -1; }

  n = 0;
  while(table_cell[n].instr != NULL)
  {
    if (strcmp(table_cell[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch(table_cell[n].type)
      {
        default:
          break;
      }
    }

    n++;
  }

  if (matched == 1)
  {
    print_error_unknown_operand_combo(instr, asm_context); 
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}



