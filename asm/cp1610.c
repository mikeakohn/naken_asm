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
#include <stdint.h>

#include "asm/cp1610.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/cp1610.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_REG,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_cp1610(char *token)
{
  if (strcasecmp(token, "sp") == 0) { return 6; }
  if (strcasecmp(token, "pc") == 0) { return 7; }

  if (token[0] == 'r' || token[0] == 'R')
  {
    if (token[2] == 0 && (token[1] >= '0' && token[1] <= '7'))
    {
      return token[1] - '0';
    }
  }

  return -1;
}

int parse_instruction_cp1610(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int opcode;
  int n;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (IS_TOKEN(token, '@') && operand_count == 0)
    {
      if (strlen(token) > 6)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      strcat(instr, "@");
      strcat(instr_case, "@");
      continue;
    }

    if (operand_count != 0)
    {
      if (IS_NOT_TOKEN(token, ','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
    }

    if (operand_count >= 2)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    n = get_register_cp1610(token);

    if (n != -1)
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = n;
    }
    else
    {
      if (IS_TOKEN(token, '#'))
      {
        operands[operand_count].type = OPERAND_NUMBER;
      }
      else
      {
        operands[operand_count].type = OPERAND_ADDRESS;
        tokens_push(asm_context, token, token_type);
      }

      if (asm_context->pass == 1)
      {
        ignore_operand(asm_context);
        operands[operand_count].value = 0;
      }
        else
      {
        if (eval_expression(asm_context, &n) != 0)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        operands[operand_count].value = n;
      }
    }

    operand_count++;
  }

  for (n = 0; table_cp1610[n].instr != NULL; n++)
  {
    if (strcmp(table_cp1610[n].instr, instr_case) == 0)
    {
      switch (table_cp1610[n].type)
      {
        case CP1610_OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          add_bin16(asm_context, table_cp1610[n].opcode, IS_OPCODE);

          return 2;
        }
        case CP1610_OP_REG:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (strcmp("gswd", instr_case) == 0)
          {
            if (operands[0].value > 3)
            {
              print_error_range("Register", 0, 3, asm_context);
              return -1;
            }
          }

          opcode = table_cp1610[n].opcode | operands[0].value;

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case CP1610_OP_SREG_DREG:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_cp1610[n].opcode |
                  (operands[0].value << 3) |
                   operands[1].value;

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case CP1610_OP_AREG_SREG:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_cp1610[n].opcode |
                  (operands[1].value << 3) |
                   operands[0].value;

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case CP1610_OP_IMMEDIATE_REG:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < -32768 || operands[0].value > 65535)
          {
            print_error_range("Immediate", -32768, 65535, asm_context);
            return -1;
          }

          opcode = table_cp1610[n].opcode | operands[1].value;

          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);

          return 4;
        }
        case CP1610_OP_REG_IMMEDIATE:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < -32768 || operands[1].value > 65535)
          {
            print_error_range("Immediate", -32768, 65535, asm_context);
            return -1;
          }

          opcode = table_cp1610[n].opcode | operands[0].value;

          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);

          return 4;
        }
        case CP1610_OP_ADDRESS_REG:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_ADDRESS ||
              operands[1].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 65535)
          {
            print_error_range("Address", 0, 65535, asm_context);
            return -1;
          }

          opcode = table_cp1610[n].opcode | operands[1].value;

          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);

          return 4;
        }
        case CP1610_OP_REG_ADDRESS:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_ADDRESS)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < 0 || operands[1].value > 65535)
          {
            print_error_range("Address", 0, 65535, asm_context);
            return -1;
          }

          opcode = table_cp1610[n].opcode | operands[0].value;

          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);

          return 4;
        }
        case CP1610_OP_1OP:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_REG)
          {
            operands[1].value = 1;
          }
            else
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_ADDRESS)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value > 3)
          {
            print_error_range("Register", 0, 3, asm_context);
            return -1;
          }

          if (asm_context->pass == 2)
          {
            if (operands[1].value < 1 || operands[1].value > 2)
            {
              print_error_range("Constant", 1, 2, asm_context);
              return -1;
            }
          }

          opcode = table_cp1610[n].opcode |
                   operands[0].value |
                 ((operands[1].value - 1) << 2);

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case CP1610_OP_BRANCH:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_ADDRESS)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 65535)
          {
            print_error_range("Address", 0, 65535, asm_context);
            return -1;
          }

          int z;
          int offset;
          int address = asm_context->address / 2;

          if (operands[0].value >= address + 2)
          {
            z = 0;
            offset = operands[0].value - (address + 2);
          }
          else
          {
            z = 1;
            offset = (address + 1) - operands[0].value;
          }

          opcode = table_cp1610[n].opcode | (z << 5);

          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, offset & 0xffff, IS_OPCODE);

          return 4;
        }
        case CP1610_OP_JUMP:
        case CP1610_OP_JSR:
        {
          int address = -1;
          int bb;
          int ii;
          int j;

          for (j = 0; j < 6; j++)
          {
            ii = table_cp1610_jump[j].ii;
            bb = table_cp1610_jump[j].bb;

            if (strcmp(table_cp1610_jump[j].instr, instr_case) == 0)
            {
              if (table_cp1610_jump[j].use_reg == 1)
              {
                if (operand_count != 2 ||
                    operands[0].type != OPERAND_REG ||
                    operands[1].type != OPERAND_ADDRESS)
                {
                  print_error_illegal_operands(instr, asm_context);
                  return -1;
                }

                if (operands[0].value < 4 || operands[0].value > 6)
                {
                  print_error_range("Register", 4, 6, asm_context);
                  return -1;
                }

                bb = operands[0].value - 4;
                address = operands[1].value;

                break;
              }
                else
              {
                if (operand_count != 1 || operands[0].type != OPERAND_ADDRESS)
                {
                  print_error_illegal_operands(instr, asm_context);
                  return -1;
                }

                address = operands[0].value;
              }

              break;
            }
          }

          if (j == 6) { break; }

          if (address < 0 || address > 65535)
          {
            print_error_range("Address", 0, 65535, asm_context);
            return -1;
          }

          opcode = (bb << 8) | (((address >> 10) & 0x3f) << 2) | ii;

          add_bin16(asm_context, table_cp1610[n].opcode, IS_OPCODE);
          add_bin16(asm_context, opcode, IS_OPCODE);
          add_bin16(asm_context, address & 0x3ff, IS_OPCODE);

          return 6;
        }
        case CP1610_OP_JR:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value > 7)
          {
            print_error_range("Register", 0, 7, asm_context);
            return -1;
          }

          opcode = table_cp1610[n].opcode | (operands[0].value << 3);

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case CP1610_OP_REG_X2:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_cp1610[n].opcode |
                  (operands[0].value << 3) |
                   operands[0].value;

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        default:
        {
          print_error_internal(asm_context, __FILE__, __LINE__);
          exit(1);
        }
      }
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

