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

#include "asm/pic18.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/pic18.h"

#define MAX_OPERANDS 3

enum
{
  OPERAND_NUMBER,
  OPERAND_W,
  OPERAND_F,
  OPERAND_A,
  OPERAND_S,
};

struct _operand
{
  int value;
  int type;
};

static int set_d(int type, int value, int *d)
{
  if (type == OPERAND_W) { *d = 0; return 0; }
  if (type == OPERAND_F) { *d = 1; return 0; }

  if (type == OPERAND_NUMBER)
  {
    if (value == 1) { *d = 1; return 0; }
    if (value == 0) { *d = 0; return 0; }
  }

  return -1;
}

static int set_a(int type, int value, int *a)
{
  if (type == OPERAND_A) { *a = 1; return 0; }

  if (type == OPERAND_NUMBER)
  {
    if (value == 1) { *a = 1; return 0; }
    if (value == 0) { *a = 0; return 0; }
  }

  return -1;
}

static int set_s(int type, int value, int *s)
{
  if (type == OPERAND_S) { *s = 1; return 0; }

  if (type == OPERAND_NUMBER)
  {
    if (value == 1) { *s = 1; return 0; }
    if (value == 0) { *s = 0; return 0; }
  }

  return -1;
}

int parse_instruction_pic18(AsmContext *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int offset;
  int num, n;
  int d, a, s;
  uint16_t opcode;

  lower_copy(instr_case, instr);

  if (asm_context->pass == 1)
  {
    ignore_line(asm_context);

    for (n = 0; table_pic18[n].instr != NULL; n++)
    {
      if (strcmp(table_pic18[n].instr, instr_case) == 0)
      {
        switch (table_pic18[n].type)
        {
          case OP_FS_FD:
          case OP_CALL:
          case OP_GOTO:
          case OP_F_K12:
            add_bin16(asm_context, 0, IS_OPCODE);
            add_bin16(asm_context, 0, IS_OPCODE);
            return 4;
          default:
            add_bin16(asm_context, 0, IS_OPCODE);
            return 2;
        }
      }
    }

    print_error_unknown_instr(asm_context, instr);
    return -1;
  }

  memset(&operands, 0, sizeof(operands));

  while (1)
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

    if (token_type == TOKEN_STRING &&
       (IS_TOKEN(token, 'W') || IS_TOKEN(token, 'w')))
    {
      operands[operand_count].type = OPERAND_W;
    }
      else
    if (token_type == TOKEN_STRING &&
       (IS_TOKEN(token, 'F') || IS_TOKEN(token, 'f')))
    {
      operands[operand_count].type = OPERAND_F;
    }
      else
    if (token_type == TOKEN_STRING &&
       (IS_TOKEN(token, 'A') || IS_TOKEN(token, 'a')))
    {
      operands[operand_count].type = OPERAND_A;
    }
      else
    if (token_type == TOKEN_STRING &&
       (IS_TOKEN(token, 'S') || IS_TOKEN(token, 's')))
    {
      operands[operand_count].type = OPERAND_S;
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_NUMBER;
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == MAX_OPERANDS)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  for (n = 0; table_pic18[n].instr != NULL; n++)
  {
    if (strcmp(table_pic18[n].instr, instr_case) == 0)
    {
      switch (table_pic18[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          add_bin16(asm_context, table_pic18[n].opcode, IS_OPCODE);

          return 2;
        }
        case OP_F_D_A:
        {
          d = 1;
          a = 0;

          if (operand_count == 0 || operand_count > 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 255)
          {
            print_error_range(asm_context, "Register", 0, 255);
            return -1;
          }

          if (operand_count > 1)
          {
            if (set_d(operands[1].type, operands[1].value, &d) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }
          }

          if (operand_count == 3)
          {
            if (set_a(operands[2].type, operands[2].value, &a) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }
          }

          opcode = table_pic18[n].opcode |
            (operands[0].value & 0xff) |
            (a << 8) | (d << 9);

          add_bin16(asm_context, opcode, IS_OPCODE);
          return 2;
        }
        case OP_F_A:
        {
          a = 0;

          if (operand_count == 0 || operand_count > 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 255)
          {
            print_error_range(asm_context, "Register", 0, 255);
            return -1;
          }

          if (operand_count == 2)
          {
            if (set_a(operands[1].type, operands[1].value, &a) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }
          }

          opcode = table_pic18[n].opcode |
            (operands[0].value & 0xff) |
            (a << 8);

          add_bin16(asm_context, opcode, IS_OPCODE);
          return 2;
        }
        case OP_FS_FD:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "operand", operands[0].value, 0, 0xfff) != 0)
          {
            return -1;
          }

          if (check_range(asm_context, "operand", operands[1].value, 0, 0xfff) != 0)
          {
            return -1;
          }

          opcode = table_pic18[n].opcode | operands[0].value;
          add_bin16(asm_context, opcode, IS_OPCODE);

          opcode = (0xffff << 12) | operands[1].value;
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_F_B_A:
        {
          a = 0;

          if (operand_count < 2 || operand_count > 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 255)
          {
            print_error_range(asm_context, "Register", 0, 255);
            return -1;
          }

          if (operands[1].value < 0 || operands[1].value > 7)
          {
            print_error_range(asm_context, "Bit", 0, 7);
            return -1;
          }

          if (operand_count == 3)
          {
            if (set_a(operands[2].type, operands[2].value, &a) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }
          }

          opcode = table_pic18[n].opcode |
                  (operands[0].value & 0xff) |
                  (operands[1].value << 9) |
                  (a << 8);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_BRANCH_8:
        {
          if (operand_count != 1)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          offset = operands[0].value - ((asm_context->address + 2) / 2);

          if (asm_context->pass == 2)
          {
            if (check_range(asm_context, "offset", operands[0].value, -128, 127) != 0)
            {
              return -1;
            }
          }

          opcode = table_pic18[n].opcode | (offset & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);
          return 2;
        }
        case OP_BRANCH_11:
        {
          if (operand_count != 1)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          offset = operands[0].value - ((asm_context->address + 2) / 2);

          if (asm_context->pass == 2)
          {
            if (check_range(asm_context, "offset", operands[0].value, -1024, 1023) != 0)
            {
              return -1;
            }
          }

          opcode = table_pic18[n].opcode | (offset & 0x7ff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_CALL:
        {
          s = 0;

          if (operand_count != 1 && operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "address", operands[0].value, 0, 0xfffff) != 0)
          {
            return -1;
          }

          if (operand_count == 2)
          {
            if (set_s(operands[1].type, operands[1].value, &s) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }
          }

          opcode = table_pic18[n].opcode |
            (operands[0].value & 0xff) |
            (s << 8);
          add_bin16(asm_context, opcode, IS_OPCODE);

          opcode = table_pic18[n].opcode |
            ((operands[0].value >> 8) & 0xfff) |
            (0xf << 12);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_GOTO:
        {
          if (operand_count != 1)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "address", operands[0].value, 0, 0xfffff) != 0)
          {
            return -1;
          }

          opcode = table_pic18[n].opcode | (operands[0].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          opcode = table_pic18[n].opcode |
            ((operands[0].value >> 8) & 0xfff) |
            (0xf << 12);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_S:
        {
          s = 0;

          if (operand_count > 1)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operand_count == 1)
          {
            if (set_s(operands[1].type, operands[1].value, &s) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }
          }

          opcode = table_pic18[n].opcode | s;
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_K8:
        {
          if (operand_count != 1)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value < -128 || operands[0].value > 255)
          {
            print_error_range(asm_context, "Literal", -128, 255);
            return -1;
          }

          opcode = table_pic18[n].opcode | (operands[0].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_F_K12:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 3)
          {
            print_error_range(asm_context, "Literal", 0, 3);
            return -1;
          }

          if (operands[0].value < -2048 || operands[0].value > 4095)
          {
            print_error_range(asm_context, "Literal", -2048, 4095);
            return -1;
          }

          int value = operands[1].value & 0xfff;

          opcode = table_pic18[n].opcode |
            (operands[0].value << 4) |
            (value & 0xf);
          add_bin16(asm_context, opcode, IS_OPCODE);

          opcode = (0xffff << 12) |
            ((value >> 4) & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_K4:
        {
          if (operand_count != 1)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 15)
          {
            print_error_range(asm_context, "Literal", 0, 15);
            return -1;
          }

          opcode = table_pic18[n].opcode | (operands[0].value & 0xf);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
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

