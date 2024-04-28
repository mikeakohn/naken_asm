/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "asm/f8.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/f8.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_NUMBER,
  OPERAND_A,
  OPERAND_DPCHR,
  OPERAND_R,
  //OPERAND_S,
  //OPERAND_U,
  //OPERAND_D,
  OPERAND_IS,
  OPERAND_K,
  OPERAND_H,
  OPERAND_Q,
  OPERAND_PC0,
  OPERAND_PC1,
  OPERAND_DC0,
  OPERAND_W,
  OPERAND_J,
};

struct _operand
{
  int value;
  int type;
};

int parse_instruction_f8(AsmContext *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  uint32_t opcode;
  bool matched = false;
  int num, n, offset;

  lower_copy(instr_case, instr);

  memset(&operands, 0, sizeof(operands));

  while (true)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (operand_count == MAX_OPERANDS)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }

    if (IS_TOKEN(token, 'a') || IS_TOKEN(token, 'A'))
    {
      operands[operand_count].type = OPERAND_A;
    }
      else
    if (IS_TOKEN(token, 's') || IS_TOKEN(token, 'S'))
    {
      operands[operand_count].type = OPERAND_R;
      operands[operand_count].value = 12; 
    }
      else
    if (IS_TOKEN(token, 'i') || IS_TOKEN(token, 'I'))
    {
      operands[operand_count].type = OPERAND_R;
      operands[operand_count].value = 13; 
    }
      else
    if (IS_TOKEN(token, 'd') || IS_TOKEN(token, 'D'))
    {
      operands[operand_count].type = OPERAND_R;
      operands[operand_count].value = 14; 
    }
      else
    if (strcasecmp(token, "ku") == 0)
    {
      operands[operand_count].type = OPERAND_DPCHR;
      operands[operand_count].value = 0;
    }
      else
    if (strcasecmp(token, "kl") == 0)
    {
      operands[operand_count].type = OPERAND_DPCHR;
      operands[operand_count].value = 1;
    }
      else
    if (strcasecmp(token, "qu") == 0)
    {
      operands[operand_count].type = OPERAND_DPCHR;
      operands[operand_count].value = 2;
    }
      else
    if (strcasecmp(token, "ql") == 0)
    {
      operands[operand_count].type = OPERAND_DPCHR;
      operands[operand_count].value = 3;
    }
      else
    if (strcasecmp(token, "is") == 0 || strcasecmp(token, "isar") == 0)
    {
      operands[operand_count].type = OPERAND_IS;
    }
      else
    if (IS_TOKEN(token, 'k') || IS_TOKEN(token, 'K'))
    {
      operands[operand_count].type = OPERAND_K;
    }
      else
    if (IS_TOKEN(token, 'h') || IS_TOKEN(token, 'H'))
    {
      operands[operand_count].type = OPERAND_H;
    }
      else
    if (IS_TOKEN(token, 'q') || IS_TOKEN(token, 'Q'))
    {
      operands[operand_count].type = OPERAND_Q;
    }
      else
    if (strcasecmp(token, "pc0") == 0)
    {
      operands[operand_count].type = OPERAND_PC0;
    }
      else
    if (strcasecmp(token, "pc1") == 0)
    {
      operands[operand_count].type = OPERAND_PC1;
    }
      else
    if (strcasecmp(token, "dc0") == 0)
    {
      operands[operand_count].type = OPERAND_DC0;
    }
      else
    if (IS_TOKEN(token, 'w') || IS_TOKEN(token, 'W'))
    {
      operands[operand_count].type = OPERAND_W;
    }
      else
    if (IS_TOKEN(token, 'j') || IS_TOKEN(token, 'J'))
    {
      operands[operand_count].type = OPERAND_J;
    }
      else
    if (IS_TOKEN(token, '['))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      operands[operand_count].type = OPERAND_R;

      if (strcasecmp(token, "isar") == 0)
      {
        operands[operand_count].value = 12; 
      }
        else
      {
        tokens_push(asm_context, token, token_type);

        if (eval_expression(asm_context, &num) != 0)
        {
          print_error_illegal_expression(asm_context, instr);
          return -1;
        }

        operands[operand_count].value = num; 
      }

      if (expect_token(asm_context, ']') == -1) { return -1; }

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token, '+'))
      {
        operands[operand_count].value = 13; 
      }
        else
      if (IS_TOKEN(token, '-'))
      {
        operands[operand_count].value = 14; 
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    {
      if (asm_context->pass == 1)
      {
        ignore_operand(asm_context);
        num = 0;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
        if (eval_expression(asm_context, &num) != 0)
        {
          print_error_illegal_expression(asm_context, instr);
          return -1;
        }
      }

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

  for (n = 0; table_f8[n].instr != NULL; n++)
  {
    if (strcmp(table_f8[n].instr, instr_case) == 0)
    {
      matched = true;

      switch (table_f8[n].type)
      {
        case F8_OP_NONE:
        {
          if (operand_count == 0)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_DATA3:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[0].value, 0, 7) == -1) { return -1; }

            opcode = table_f8[n].opcode | operands[0].value;
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_DATA4:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[0].value, 0, 15) == -1) { return -1; }

            opcode = table_f8[n].opcode | operands[0].value;
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_DATA8:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[0].value, 0, 255) == -1) { return -1; }

            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            return 2;
          }

          break;
        }
        case F8_OP_ADDR:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Address", operands[0].value, 0, 0xffff) == -1) { return -1; }

            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            add_bin16(asm_context, operands[0].value, IS_OPCODE);
            return 3;
          }

          break;
        }
        case F8_OP_DISP:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            offset = operands[0].value - (asm_context->address + 1);

            if (asm_context->pass == 2)
            {
              if (check_range(asm_context, "Offset", offset, -128, 127) == -1)
              {
                return -1;
              }
            }

            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            add_bin8(asm_context, offset & 0xff, IS_OPCODE);
            return 2;
          }

          break;
        }
        case F8_OP_DATA3_DISP:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[0].value, 0, 7) == -1) { return -1; }

            offset = operands[1].value - (asm_context->address + 1);

            if (asm_context->pass == 2)
            {
              if (check_range(asm_context, "Offset", offset, -128, 127) == -1)
              {
                return -1;
              }
            }

            opcode = table_f8[n].opcode | operands[0].value;

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, offset & 0xff, IS_OPCODE);
            return 2;
          }

          break;
        }
        case F8_OP_R:
        {
          if (operand_count == 1)
          {
            if (operands[0].type != OPERAND_NUMBER &&
                operands[0].type != OPERAND_R)
            {
              break;
            }

            if (check_range(asm_context, "Register", operands[0].value, 0, 15) == -1) { return -1; }

            opcode = table_f8[n].opcode | operands[0].value;
            add_bin8(asm_context, opcode, IS_OPCODE);

            return 1;
          }

          break;
        }
        case F8_OP_SHIFT_1:
        {
          if (operand_count == 1)
          {
            if (asm_context->pass == 1)
            {
              add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
              return 1;
            }
              else
            if (operands[0].type == OPERAND_NUMBER && operands[0].value == 1)
            {
              add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
              return 1;
            }
          }

          break;
        }
        case F8_OP_SHIFT_4:
        {
          if (operand_count == 1)
          {
            if (asm_context->pass == 1)
            {
              add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
              return 1;
            }
              else
            if (operands[0].type == OPERAND_NUMBER && operands[0].value == 4)
            {
              add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
              return 1;
            }
          }

          break;
        }
        case F8_OP_A_DPCHR:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_A &&
              operands[1].type == OPERAND_DPCHR)
          {
            opcode = table_f8[n].opcode | operands[1].value;

            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_DPCHR_A:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_DPCHR &&
              operands[1].type == OPERAND_A)
          {
            opcode = table_f8[n].opcode | operands[0].value;

            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_A_IS:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_A &&
              operands[1].type == OPERAND_IS)
          {
            //if (operands[1].value != 12) { break; }

            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_IS_A:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_IS &&
              operands[1].type == OPERAND_A)
          {
            //if (operands[0].value != 12) { break; }

            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_A_R:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_A)
          {
            if (operands[1].type != OPERAND_NUMBER &&
                operands[1].type != OPERAND_R)
            {
              break;
            }

            if (check_range(asm_context, "Register", operands[1].value, 0, 15) == -1) { return -1; }

            opcode = table_f8[n].opcode | operands[1].value;
            add_bin8(asm_context, opcode, IS_OPCODE);

            return 1;
          }

          break;
        }
        case F8_OP_R_A:
        {
          if (operand_count == 2 && operands[1].type == OPERAND_A)
          {
            if (operands[0].type != OPERAND_NUMBER &&
                operands[0].type != OPERAND_R)
            {
              break;
            }

            if (check_range(asm_context, "Register", operands[0].value, 0, 15) == -1) { return -1; }

            opcode = table_f8[n].opcode | operands[0].value;
            add_bin8(asm_context, opcode, IS_OPCODE);

            return 1;
          }

          break;
        }
        case F8_OP_H_DC0:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_H &&
              operands[1].type == OPERAND_DC0)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_Q_DC0:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_Q &&
              operands[1].type == OPERAND_DC0)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_DC0_H:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_DC0 &&
              operands[1].type == OPERAND_H)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_DC0_Q:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_DC0 &&
              operands[1].type == OPERAND_Q)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_K_PC1:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_K &&
              operands[1].type == OPERAND_PC1)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_PC1_K:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_PC1 &&
              operands[1].type == OPERAND_K)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_PC0_Q:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_PC0 &&
              operands[1].type == OPERAND_Q)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_W_J:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_W &&
              operands[1].type == OPERAND_J)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case F8_OP_J_W:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_J &&
              operands[1].type == OPERAND_W)
          {
            add_bin8(asm_context, table_f8[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        default:
        {
          break;
        }
      }
    }
  }

  if (matched)
  {
    print_error_unknown_operand_combo(asm_context, instr);
  }
    else
  {
    print_error_unknown_instr(asm_context, instr);
  }

  return -1;
}

