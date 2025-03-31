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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "asm/pdp11.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/pdp11.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_NUMBER,
  OPERAND_IMMEDIATE,               // #num
  OPERAND_ABSOLUTE,                // @#num
  OPERAND_AT_NUMBER,               // @num
  OPERAND_REGISTER,                // Rn
  OPERAND_REGISTER_PAREN,          // (Rn)
  OPERAND_REGISTER_PAREN_PLUS,     // (Rn)+
  OPERAND_REGISTER_AT_PAREN_PLUS,  // @(Rn)+
  OPERAND_REGISTER_MINUS_PAREN,    // -(Rn)
  OPERAND_REGISTER_AT_MINUS_PAREN, // @-(Rn)
  OPERAND_REGISTER_INDEXED,        // X(Rn)
  OPERAND_REGISTER_AT_INDEXED,     // @X(Rn)
};

struct Operand
{
  int value;
  int type;
  uint8_t flags;
};

static int get_register(const char *token)
{
  if (strcasecmp(token, "sp") == 0) { return 6; }
  if (strcasecmp(token, "pc") == 0) { return 7; }

  if (token[0] != 'r' && token[1] != 'R') { return -1; }
  if (token[1] <= '0' || token[1] >= '7') { return -1; }
  if (token[2] != 0) { return -1; }

  return token[1] - '0';
}

int parse_instruction_pdp11(AsmContext *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct Operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  uint16_t opcode;
  //bool matched = false;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while (true)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      if (operand_count != 0)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }
      break;
    }

    bool has_at    = false;
    bool has_minus = false;
    bool has_hash  = false;
    bool has_paren = false;

    do
    {
      if (IS_TOKEN(token, '@'))
      {
        if (has_at || has_minus || has_hash || has_paren)
        {
          print_error_unexp(asm_context, token);
          return -1;
        }

        has_at = true;
        token_type = tokens_get(asm_context, token, TOKENLEN);

        continue;
      }

      if (IS_TOKEN(token, '-'))
      {
        if (has_minus || has_hash)
        {
          print_error_unexp(asm_context, token);
          return -1;
        }

        has_minus = true;
        token_type = tokens_get(asm_context, token, TOKENLEN);

        continue;
      }

      if (IS_TOKEN(token, '#'))
      {
        if (has_minus || has_hash || has_paren)
        {
          print_error_unexp(asm_context, token);
          return -1;
        }

        has_hash = true;
        token_type = tokens_get(asm_context, token, TOKENLEN);

        continue;
      }

      if (IS_TOKEN(token, '('))
      {
        if (has_hash)
        {
          print_error_unexp(asm_context, token);
          return -1;
        }

        has_paren = true;

        break;
      }

      num = get_register(token);

      if (num != -1)
      {
        operands[operand_count].value = num;
        operands[operand_count].type = OPERAND_REGISTER;
        break;
      }

      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_NUMBER;
    } while (false);

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 2)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  for (n = 0; table_pdp11[n].instr != NULL; n++)
  {
    if (strcmp(table_pdp11[n].instr, instr_case) == 0)
    {
      //matched = true;

      switch (table_pdp11[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          add_bin16(asm_context, table_pdp11[n].opcode, IS_OPCODE);

          return 2;
        }
        case OP_DOUBLE:
        {
        }
        case OP_D_EXTRA:
        {
        }
        case OP_SINGLE:
        {
        }
        case OP_BRANCH:
        {
        }
        case OP_SUB_BR:
        {
        }
        case OP_JSR:
        {
        }
        case OP_REG:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_pdp11[n].opcode | operands[0].value;

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_NN:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "Number", operands[0].value, 0, 63) == -1) { return -1; }

          opcode = table_pdp11[n].opcode | operands[0].value;

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_S_OPER:
        {
        }
        case OP_NZVC:
        {
        }
        default:
        {
          print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }
  }

  print_error_unknown_instr(asm_context, instr);

  return -1;
}

