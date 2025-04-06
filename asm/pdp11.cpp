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
  int offset;
  uint8_t flags;
};

struct Extra
{
  Extra() : count (0) { }

  void push(uint16_t value)
  {
    if (count == 2) { return; }
    values[count++] = value;
  }

  void add_values(AsmContext *asm_context)
  {
    for (int n = 0; n < count; n++)
    {
      add_bin16(asm_context, values[n], IS_OPCODE);
    }
  }

  int size() { return count * 2; }

  int count;
  uint16_t values[2];
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

static int get_extra(Operand &operand, int &reg, Extra &extra, bool is_dest)
{
  switch (operand.type)
  {
    case OPERAND_NUMBER:
    {
      break;
    }
    case OPERAND_IMMEDIATE:
    {
      // #num  aka (pc)+  (010_111)
      if (is_dest) { return -1; }

      extra.push(operand.offset);
      reg = 0x17;
      return 0;
    }
    case OPERAND_ABSOLUTE:
    {
      // @#num  aka @(pc)+ (011_111)
      extra.push(operand.offset);
      reg = 0x1f;
      return 0;
    }
    case OPERAND_AT_NUMBER:
    {
      // @num  aka @x(pc)  (111_111)
      extra.push(operand.offset);
      reg = 0x3f;
      return 0;
    }
    case OPERAND_REGISTER:
    {
      // Rn  000_rrr
      reg = operand.value;
      return 0;
    }
    case OPERAND_REGISTER_PAREN:
    {
      // (Rn)  001_rrr
      reg = (1 << 3) | operand.value;
      return 0;
    }
    case OPERAND_REGISTER_PAREN_PLUS:
    {
      // (Rn)+  010_rrr
      reg = (2 << 3) | operand.value;
      return 0;
    }
    case OPERAND_REGISTER_AT_PAREN_PLUS:
    {
      // @(Rn)+  011_rrr
      reg = (3 << 3) | operand.value;
      return 0;
    }
    case OPERAND_REGISTER_MINUS_PAREN:
    {
      // -(Rn)  100_rrr
      reg = (4 << 3) | operand.value;
      return 0;
    }
    case OPERAND_REGISTER_AT_MINUS_PAREN:
    {
      // @-(Rn) 101_rrr
      reg = (5 << 3) | operand.value;
      return 0;
    }
    case OPERAND_REGISTER_INDEXED:
    {
      // X(Rn)  110_rrr
      extra.push(operand.offset);
      reg = (6 << 3) | operand.value;
      return 0;
    }
    case OPERAND_REGISTER_AT_INDEXED:
    {
      // @X(Rn)  111_rrr
      extra.push(operand.offset);
      reg = (7 << 3) | operand.value;
      return 0;
    }
    default:
    {
      break;
    }
  }

  return -1;
}

static int parse_immediate(AsmContext *asm_context, Operand &operand)
{
  char token[TOKENLEN];
  int token_type;
  int num;

  if (asm_context->pass == 1)
  {
    ignore_operand(asm_context);
  }
    else
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    tokens_push(asm_context, token, token_type);

    if (eval_expression(asm_context, &num) != 0)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  operand.value  = 7;
  operand.offset = num;

  return 0;
}

int parse_paren(AsmContext *asm_context, Operand &operand)
{
  char token[TOKENLEN];
  int token_type;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  int num = get_register(token);

  if (num == -1)
  {
    print_error_unexp(asm_context, token);
    return -1;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_NOT_TOKEN(token, ')'))
  {
    print_error_unexp(asm_context, token);
    return -1;
  }

  operand.offset = num;
  operand.type   = OPERAND_REGISTER_PAREN;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token, '+'))
  {
    operand.type  = OPERAND_REGISTER_PAREN_PLUS;
  }
    else
  {
    tokens_push(asm_context, token, token_type);
  }

  return 0;
}

int parse_minus(AsmContext *asm_context, Operand &operand)
{
  if (parse_paren(asm_context, operand) == -1) { return -1; }

  if (operand.type != OPERAND_REGISTER_PAREN)
  {
    // NOTE: parse_paren() only sets a type of PAREN or PAREN_PLUS.
    print_error_unexp(asm_context, "+");
    return -1;
  }

  operand.type  = OPERAND_REGISTER_MINUS_PAREN;

  return 0;
}

int parse_at(AsmContext *asm_context, Operand &operand)
{
  char token[TOKENLEN];
  int token_type;
  int num;

  operand.type  = OPERAND_AT_NUMBER;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token, '#'))
  {
    if (parse_immediate(asm_context, operand) == -1) { return -1; }

    operand.type  = OPERAND_ABSOLUTE;
    return 0;
  }

  if (IS_TOKEN(token, '-'))
  {
    return parse_minus(asm_context, operand);
  }

  if (IS_TOKEN(token, '('))
  {
    return parse_paren(asm_context, operand);
  }

  // This should be a number. On pass 1 who cares what it really is.
  if (asm_context->pass == 1)
  {
    ignore_operand(asm_context);
  }
    else
  {
    if (eval_expression(asm_context, &num) != 0)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  operand.offset = num;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token, '('))
  {
    if (parse_paren(asm_context, operand) == -1) { return -1; }

    if (operand.type != OPERAND_REGISTER_PAREN)
    {
      // NOTE: parse_paren() only sets a type of PAREN or PAREN_PLUS.
      print_error_unexp(asm_context, "+");
      return -1;
    }

    operand.type = OPERAND_REGISTER_AT_INDEXED;
  }
    else
  {
    tokens_push(asm_context, token, token_type);
    operand.type  = OPERAND_REGISTER_AT_INDEXED;
    operand.value = 7;
  }

  return 0;
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
  Extra extra;
  int reg_s = 0, reg_d = 0;
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

    do
    {
      num = get_register(token);

      if (num != -1)
      {
        operands[operand_count].value = num;
        operands[operand_count].type = OPERAND_REGISTER;
        break;
      }

      if (IS_TOKEN(token, '#'))
      {
        if (parse_immediate(asm_context, operands[operand_count]))
        {
          return -1;
        }

        operands[operand_count].type  = OPERAND_IMMEDIATE;
        break;
      }

      if (IS_TOKEN(token, '@'))
      {
        if (parse_at(asm_context, operands[operand_count]))
        {
          return -1;
        }

        break;
      }

      if (IS_TOKEN(token, '-'))
      {
        if (parse_minus(asm_context, operands[operand_count]) == -1)
        {
          return -1;
        }

        break;
      }

      if (IS_TOKEN(token, '('))
      {
        if (parse_paren(asm_context, operands[operand_count]) == -1)
        {
          return -1;
        }

        break;
      }

      tokens_push(asm_context, token, token_type);

      if (parse_immediate(asm_context, operands[operand_count]) == -1)
      {
        return -1;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '('))
      {
        if (parse_paren(asm_context, operands[operand_count]) == -1)
        {
          return -1;
        }

        if (operands[operand_count].type != OPERAND_REGISTER_PAREN)
        {
          // NOTE: parse_paren() only sets a type of PAREN or PAREN_PLUS.
          print_error_unexp(asm_context, "+");
          return -1;
        }

        operands[operand_count].type = OPERAND_REGISTER_INDEXED;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
        operands[operand_count].type  = OPERAND_REGISTER_INDEXED;
        operands[operand_count].value = 7;
      }
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
        case OP_SINGLE:
        {
          if (operand_count != 1)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (get_extra(operands[0], reg_d, extra, true) == -1)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_pdp11[n].opcode | reg_d;

          add_bin16(asm_context, opcode, IS_OPCODE);

          extra.add_values(asm_context);
          return 2 + extra.size();
        }
        case OP_DOUBLE:
        {
          if (operand_count != 2)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (get_extra(operands[0], reg_s, extra, false) == -1)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (get_extra(operands[1], reg_d, extra, true) == -1)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_pdp11[n].opcode | (reg_s << 6) | reg_d;

          add_bin16(asm_context, opcode, IS_OPCODE);

          extra.add_values(asm_context);
          return 2 + extra.size();
        }
        case OP_REG_D:
        {
          if (operand_count != 2 || operands[0].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (get_extra(operands[1], reg_d, extra, true) == -1)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_pdp11[n].opcode | (operands[0].value << 6) | reg_d;

          add_bin16(asm_context, opcode, IS_OPCODE);

          extra.add_values(asm_context);
          return 2 + extra.size();
        }
        case OP_REG_S:
        {
          if (operand_count != 2 || operands[1].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (get_extra(operands[0], reg_s, extra, false) == -1)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_pdp11[n].opcode | (operands[1].value << 6) | reg_s;

          add_bin16(asm_context, opcode, IS_OPCODE);

          extra.add_values(asm_context);
          return 2 + extra.size();
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

