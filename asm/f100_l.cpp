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
#include <stdint.h>

#include "asm/f100_l.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/f100_l.h"

#define MAX_OPERANDS 3

enum
{
  OPERAND_NUMBER,
  OPERAND_COMMA_D,
  OPERAND_SLASH_P,
  OPERAND_SLASH_P_PLUS,
  OPERAND_SLASH_P_MINUS,
  OPERAND_DOT_W,
  OPERAND_A,
  OPERAND_CR,
};

struct _operand
{
  int value;
  int type;
};

int get_num(AsmContext *asm_context, int &num, const char *instr)
{
  if (eval_expression(asm_context, &num) != 0)
  {
    if (asm_context->pass == 1)
    {
      ignore_operand(asm_context);
      num = 0;
    }
      else
    {
      print_error_illegal_expression(asm_context, instr);
      return -1;
    }
  }

  return 0;
}

int parse_instruction_f100_l(AsmContext *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  //int count;
  uint16_t opcode;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while (true)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (operand_count == 3)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }

    if (strcasecmp(token, "a") == 0)
    {
      operands[operand_count].type = OPERAND_A;
    }
      else
    if (strcasecmp(token, "cr") == 0)
    {
      operands[operand_count].type = OPERAND_CR;
    }
      else
    if (IS_TOKEN(token, '['))
    {
      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          while (true)
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);
            if (IS_TOKEN(token, ']')) { break; }
            if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
          }

          tokens_push(asm_context, token, token_type);
          num = 1;
        }
          else
        {
          print_error_illegal_expression(asm_context, instr);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_SLASH_P;
      operands[operand_count].value = num;

      if (expect_token(asm_context, ']') == -1) { return -1; }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '+'))
      {
        operands[operand_count].type = OPERAND_SLASH_P_PLUS;
      }
        else
      if (IS_TOKEN(token, '-'))
      {
        operands[operand_count].type = OPERAND_SLASH_P_MINUS;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (IS_TOKEN(token, ',') || IS_TOKEN(token, '#'))
    {
      if (get_num(asm_context, num, instr) == -1) { return -1; }
      operands[operand_count].type = OPERAND_COMMA_D;
      operands[operand_count].value = num;
    }
      else
    if (IS_TOKEN(token, '.') || strcasecmp(token, "long") == 0)
    {
      if (get_num(asm_context, num, instr) == -1) { return -1; }
      operands[operand_count].type = OPERAND_DOT_W;
      operands[operand_count].value = num;
    }
      else
    if (IS_TOKEN(token, '/'))
    {
      //if (get_num(asm_context, num, instr) == -1) { return -1; }
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_NUMBER)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      num = strtol(token, NULL, 0);

      operands[operand_count].type = OPERAND_SLASH_P;
      operands[operand_count].value = num;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '+'))
      {
        operands[operand_count].type = OPERAND_SLASH_P_PLUS;
      }
        else
      if (IS_TOKEN(token, '-'))
      {
        operands[operand_count].type = OPERAND_SLASH_P_MINUS;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (get_num(asm_context, num, instr) == -1) { return -1; }
      operands[operand_count].type = OPERAND_NUMBER;
      operands[operand_count].value = num;
    }

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token, ',')) { continue; }

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      if (token_type == TOKEN_EOF) { break; }
      if (token_type != TOKEN_EOL)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      break;
    }
  }

  for (n = 0; table_f100_l[n].instr != NULL; n++)
  {
    if (strcmp(table_f100_l[n].instr, instr_case) == 0)
    {

      switch (table_f100_l[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          add_bin16(asm_context, table_f100_l[n].opcode, IS_OPCODE);

          return 2;
        }
        case OP_ALU:
        {
          if (operand_count != 1)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          switch (operands[0].type)
          {
            case OPERAND_NUMBER:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0x7ff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | (operands[0].value & 0x7ff);

              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_COMMA_D:
              if (strcasecmp(instr_case, "jmp") == 0)
              {
                print_error(asm_context, "jmp cannot be used with an immediate\n.");
                return -1;
              }

              if (check_range(asm_context, "Immediate", operands[0].value, -32768, 0xffff) == -1) { return -1; }
              opcode = table_f100_l[n].opcode;

              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);
              return 4;
            case OPERAND_SLASH_P:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0xff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | 0x0800 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_SLASH_P_PLUS:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0xff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | 0x0900 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_SLASH_P_MINUS:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0xff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | 0x0b00 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_DOT_W:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0x7fff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | 0x0800;

              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[0].value, IS_OPCODE);
              return 4;
            default:
              print_error_illegal_operands(asm_context, instr);
              return -1;
          }

          return 2;
        }
        case OP_BIT:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
          }

          if (check_range(asm_context, "bit", operands[0].value, 0, 15) == -1) { return -1; }

          switch (operands[1].type)
          {
            case OPERAND_A:
              opcode = table_f100_l[n].opcode;
              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_CR:
              opcode = table_f100_l[n].opcode | 0x0100;
              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_NUMBER:
              if (check_range(asm_context, "Address", operands[1].value, 1, 0x7fff) == -1) { return -1; }
              opcode = table_f100_l[n].opcode | 0x0300;

              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              return 4;
            default:
              print_error_illegal_operands(asm_context, instr);
              return -1;
          }
        }
        case OP_HALT:
        {
          opcode = table_f100_l[n].opcode;

          if (operand_count == 1)
          {
            if (operands[0].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(asm_context, instr);
            }

            if (check_range(asm_context, "halt_number", operands[0].value, 0, 1024) == -1) { return -1; }
            opcode |= operands[0].value;
          }
            else
          {
            if (operand_count != 0)
            {
              print_error_opcount(asm_context, instr);
              return -1;
            }
          }

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_INC:
        {
          if (operand_count != 2 || operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (asm_context->pass == 2)
          {
            if (check_range(asm_context, "Address", operands[1].value, 0, 0x7fff) == -1) { return -1; }
          }

          switch (operands[0].type)
          {
            case OPERAND_NUMBER:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0x7ff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | (operands[0].value & 0x7ff);

              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              return 4;
            case OPERAND_COMMA_D:
              if (check_range(asm_context, "Immediate", operands[0].value, -32768, 0xffff) == -1) { return -1; }
              opcode = table_f100_l[n].opcode;

              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              return 6;
            case OPERAND_SLASH_P:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0xff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | 0x0800 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              return 4;
            case OPERAND_SLASH_P_PLUS:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0xff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | 0x0900 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              return 4;
            case OPERAND_SLASH_P_MINUS:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0xff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | 0x0b00 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              return 4;
            case OPERAND_DOT_W:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[0].value, 1, 0x7fff) == -1) { return -1; }
              }

              opcode = table_f100_l[n].opcode | 0x0800;

              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[0].value, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              return 6;
            default:
              print_error_illegal_operands(asm_context, instr);
              return -1;
          }

          return 2;
        }
        case OP_COND_JMP:
        {
          if (operand_count != 3 ||
              operands[0].type != OPERAND_NUMBER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "bit", operands[0].value, 0, 15) == -1) { return -1; }

          if (asm_context->pass == 2)
          {
            if (check_range(asm_context, "Address", operands[2].value, 1, 0x7fff) == -1) { return -1; }
          }

          switch (operands[1].type)
          {
            case OPERAND_A:
              opcode = table_f100_l[n].opcode | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[2].value, IS_OPCODE);
              return 4;
            case OPERAND_CR:
              opcode = table_f100_l[n].opcode | 0x0100 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[2].value, IS_OPCODE);
              return 4;
            case OPERAND_NUMBER:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[1].value, 1, 0x7fff) == -1) { return -1; }
              }
              opcode = table_f100_l[n].opcode | 0x0300 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              add_bin16(asm_context, operands[2].value, IS_OPCODE);
              return 6;
            default:
              print_error_illegal_operands(asm_context, instr);
              return -1;
          }
        }
        case OP_SHIFT:
        {
          if (operand_count != 2 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "bit", operands[0].value, 0, 15) == -1) { return -1; }

          switch (operands[1].type)
          {
            case OPERAND_A:
              opcode = table_f100_l[n].opcode | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_CR:
              opcode = table_f100_l[n].opcode | 0x0100 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_NUMBER:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[1].value, 1, 0x7fff) == -1) { return -1; }
              }
              opcode = table_f100_l[n].opcode | 0x0300 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              return 4;
            default:
              print_error_illegal_operands(asm_context, instr);
              return -1;
          }
        }
        case OP_SHIFT_D:
        {
          if (operand_count != 2 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "bit", operands[0].value, 0, 31) == -1) { return -1; }

          switch (operands[1].type)
          {
            case OPERAND_A:
              opcode = table_f100_l[n].opcode | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_CR:
              opcode = table_f100_l[n].opcode | 0x0100 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              return 2;
            case OPERAND_NUMBER:
              if (asm_context->pass == 2)
              {
                if (check_range(asm_context, "Address", operands[1].value, 1, 0x7fff) == -1) { return -1; }
              }
              opcode = table_f100_l[n].opcode | 0x0300 | operands[0].value;
              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);
              return 4;
            default:
              print_error_illegal_operands(asm_context, instr);
              return -1;
          }
        }
        default:
          print_error_internal(asm_context, __FILE__, __LINE__);
          break;
      }
    }
  }

  print_error_unknown_instr(asm_context, instr);

  return -1;
}

