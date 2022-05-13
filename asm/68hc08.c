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
#include "asm/68hc08.h"
#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/tokens.h"
#include "table/68hc08.h"

enum
{
  OPERAND_NONE,
  OPERAND_NUM,
  OPERAND_ADDRESS,
  OPERAND_X,
  OPERAND_X_PLUS,
  OPERAND_SP,
};

struct _operands
{
  int value;
  int8_t type;
  int8_t use_16_bit;
};

int parse_instruction_68hc08(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case_c[TOKENLEN];
  char *instr_case = instr_case_c;
  struct _operands operands[3];
  int operand_count = 0;
  int offset;
  int matched = 0;
  int num;
  int n;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

  if (strcmp("bhs", instr_case) == 0) { instr_case = "bcc"; }
  else if (strcmp("blo", instr_case) == 0) { instr_case = "bcs"; }
  else if (strcmp("lsl", instr_case) == 0) { instr_case = "asl"; }
  else if (strcmp("lslx", instr_case) == 0) { instr_case = "aslx"; }

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

    if (operand_count >= 3)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if (token_type == TOKEN_POUND)
    {
      operands[operand_count].type = OPERAND_NUM;

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          operands[operand_count].use_16_bit = 1;
          memory_write(asm_context, asm_context->address, 1, asm_context->tokens.line);
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        num = 0xffff;
      }

      if (asm_context->pass == 2)
      {
        operands[operand_count].use_16_bit =
          memory_read(asm_context, asm_context->address);
      }

      if (num < -32768 || num > 0xffff)
      {
        print_error_range("Constant", -32768, 65535, asm_context);
        return -1;
      }

      operands[operand_count].value = num;
    }
      else
    if (strcasecmp(token, "x") == 0)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '+'))
      {
        operands[operand_count].type = OPERAND_X_PLUS;
      }
        else
      {
        operands[operand_count].type = OPERAND_X;
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (strcasecmp(token, "sp") == 0)
    {
      operands[operand_count].type = OPERAND_SP;
    }
      else
    if (strcasecmp(token, ",") == 0 && operand_count == 0)
    {
      // operand type should be none
      tokens_push(asm_context, token, token_type);
    }
      else
    {
      tokens_push(asm_context, token, token_type);
      operands[operand_count].type = OPERAND_ADDRESS;

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          operands[operand_count].use_16_bit = 1;
          memory_write(asm_context, asm_context->address, 1, asm_context->tokens.line);
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        num = 0xffff;
      }

      if (asm_context->pass == 2)
      {
        operands[operand_count].use_16_bit =
          memory_read(asm_context, asm_context->address);
      }

      operands[operand_count].value = num;
    }

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  // Done parsing, now assemble.

#if 0
printf("---------- %s operand_count=%d\n", instr, operand_count);
for (n = 0; n < operand_count; n++)
{
printf("%04x %d\n", operands[n].value, operands[n].type);
}
#endif

  for (n = 0; n < 256; n++)
  {
    if (m68hc08_table[n].instr == NULL) { continue; }

    if (strcmp(m68hc08_table[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch (m68hc08_table[n].type)
      {
        case CPU08_OP_NONE:
        {
          add_bin8(asm_context, n, IS_OPCODE);
          return 1;
        }
        case CPU08_OP_0_COMMA_OPR:
        case CPU08_OP_1_COMMA_OPR:
        case CPU08_OP_2_COMMA_OPR:
        case CPU08_OP_3_COMMA_OPR:
        case CPU08_OP_4_COMMA_OPR:
        case CPU08_OP_5_COMMA_OPR:
        case CPU08_OP_6_COMMA_OPR:
        case CPU08_OP_7_COMMA_OPR:
        {
          int num = m68hc08_table[n].type - CPU08_OP_0_COMMA_OPR;

          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[0].value == num &&
              operands[1].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Address", operands[1].value, 0, 0xff) == -1) { return -1; }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);
            return 2;
          }

          break;
        }
        case CPU08_OP_0_COMMA_OPR_REL:
        case CPU08_OP_1_COMMA_OPR_REL:
        case CPU08_OP_2_COMMA_OPR_REL:
        case CPU08_OP_3_COMMA_OPR_REL:
        case CPU08_OP_4_COMMA_OPR_REL:
        case CPU08_OP_5_COMMA_OPR_REL:
        case CPU08_OP_6_COMMA_OPR_REL:
        case CPU08_OP_7_COMMA_OPR_REL:
        {
          int num = m68hc08_table[n].type - CPU08_OP_0_COMMA_OPR_REL;

          if (operand_count == 3 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[0].value == num &&
              operands[1].type == OPERAND_ADDRESS &&
              operands[2].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Address", operands[1].value, 0, 0xff) == -1) { return -1; }

            offset = operands[2].value - (asm_context->address + 3);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);
            return 2;
          }

          break;
        }
        case CPU08_OP_NUM16:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUM)
          {
            if (operands[0].use_16_bit == 1 ||
                operands[0].value < -128 ||
                operands[0].value >= 0xff)
            {
              add_bin8(asm_context, n, IS_OPCODE);
              add_bin8(asm_context, operands[0].value >> 8, IS_OPCODE);
              add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);

              return 3;
            }
          }

          break;
        }
        case CPU08_OP_NUM8:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUM)
          {
            if (operands[0].use_16_bit == 1 ||
                operands[0].value < -128 ||
                operands[0].value >= 0xff)
            {
              break;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);

            return 2;
          }

          break;
        }
        case CPU08_OP_NUM8_OPR8:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUM &&
              operands[1].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Constant", operands[0].value, 0, 0xff) == -1) { return -1; }
            if (check_range(asm_context, "Address", operands[1].value, 0, 0xff) == -1) { return -1; }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);

            return 3;
          }

          break;
        }
        case CPU08_OP_NUM8_REL:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUM &&
              operands[1].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Constant", operands[0].value, 0, 0xff) == -1) { return -1; }

            offset = operands[1].value - (asm_context->address + 3);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);

            return 3;
          }

          break;
        }
        case CPU08_OP_OPR16:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            if (operands[0].use_16_bit == 1 ||
                operands[0].value < -128 ||
                operands[0].value >= 0xff)
            {
              add_bin8(asm_context, n, IS_OPCODE);
              add_bin8(asm_context, operands[0].value >> 8, IS_OPCODE);
              add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);

              return 3;
            }
          }

          break;
        }
        case CPU08_OP_OPR16_X:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_X)
          {
            if (operands[0].use_16_bit == 1 ||
                operands[0].value < -128 ||
                operands[0].value >= 0xff)
            {
              add_bin8(asm_context, n, IS_OPCODE);
              add_bin8(asm_context, operands[0].value >> 8, IS_OPCODE);
              add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);

              return 3;
            }
          }

          break;
        }
        case CPU08_OP_OPR8:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            if (operands[0].use_16_bit == 1 ||
                operands[0].value < -128 ||
                operands[0].value >= 0xff)
            {
              break;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);

            return 2;
          }

          break;
        }
        case CPU08_OP_OPR8_OPR8:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_ADDRESS)
          {
            if (operands[0].use_16_bit == 1 ||
                operands[0].value < -128 ||
                operands[0].value >= 0xff ||
                operands[1].use_16_bit == 1 ||
                operands[1].value < -128 ||
                operands[1].value >= 0xff)
            {
              print_error_range("Address", 0, 0xff, asm_context);
              return -1;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);

            return 3;
          }

          break;
        }
        case CPU08_OP_OPR8_REL:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Address", operands[0].value, 0, 0xff) == -1) { return -1; }

            offset = operands[1].value - (asm_context->address + 3);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);

            return 3;
          }

          break;
        }
        case CPU08_OP_OPR8_X:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_X)
          {
            if (operands[0].use_16_bit == 1 ||
                operands[0].value < -128 ||
                operands[0].value >= 0xff)
            {
              break;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);

            return 2;
          }

          break;
        }
        case CPU08_OP_OPR8_X_PLUS:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_X_PLUS)
          {
            if (operands[0].use_16_bit == 1 ||
                operands[0].value < -128 ||
                operands[0].value >= 0xff)
            {
              break;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);

            return 2;
          }

          break;
        }
        case CPU08_OP_OPR8_X_PLUS_REL:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_X_PLUS &&
              operands[2].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Address", operands[0].value, 0, 0xff) == -1) { return -1; }

            offset = operands[2].value - (asm_context->address + 3);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);

            return 3;
          }

          break;
        }
        case CPU08_OP_OPR8_X_REL:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_X &&
              operands[2].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Address", operands[0].value, 0, 0xff) == -1) { return -1; }

            offset = operands[2].value - (asm_context->address + 3);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);

            return 3;
          }

          break;
        }
        case CPU08_OP_REL:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            offset = operands[0].value - (asm_context->address + 2);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);

            return 2;
          }

          break;
        }
        case CPU08_OP_COMMA_X:
        case CPU08_OP_X:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_X)
          {
            add_bin8(asm_context, n, IS_OPCODE);
            return 1;
          }

          break;
        }
        case CPU08_OP_X_PLUS_OPR8:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_X_PLUS &&
              operands[1].type == OPERAND_ADDRESS)
          {
             if (check_range(asm_context, "Address", operands[1].value, 0, 0xff) == -1) { return -1; }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);

            return 2;
          }

          break;
        }
        case CPU08_OP_X_PLUS_REL:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_X_PLUS &&
              operands[1].type == OPERAND_ADDRESS)
          {
            offset = operands[1].value - (asm_context->address + 2);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);

            return 2;
          }

          break;
        }
        case CPU08_OP_X_REL:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_X &&
              operands[1].type == OPERAND_ADDRESS)
          {
            offset = operands[2].value - (asm_context->address + 2);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            add_bin8(asm_context, n, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);

            return 2;
          }

          break;
        }
        default:
          break;
      }
    }
  }

  for (n = 0; m68hc08_16_table[n].instr != NULL; n++)
  {
    if (strcmp(m68hc08_16_table[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch (m68hc08_16_table[n].type)
      {
        case CPU08_OP_OPR8_SP:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_SP)
          {
            if (operands[0].use_16_bit == 1 ||
                operands[0].value < 0 ||
                operands[0].value > 0xff)
            {
              if (m68hc08_16_table[n].has_16_bit_version == 1)
              {
                break;
              }

              print_error_range("Address", 0, 0xff, asm_context);
              return -1;
            }

            add_bin16(asm_context, m68hc08_16_table[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);

            return 3;
          }

          break;
        }
        case CPU08_OP_OPR16_SP:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_SP)
          {
            add_bin16(asm_context, m68hc08_16_table[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value >> 8, IS_OPCODE);
            add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);

            return 4;
          }

          break;
        }
        case CPU08_OP_OPR8_SP_REL:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_SP &&
              operands[2].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Address", operands[0].value, 0, 0xff) == -1) { return -1; }

            offset = operands[1].value - (asm_context->address + 4);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            add_bin16(asm_context, m68hc08_16_table[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            add_bin8(asm_context, offset, IS_OPCODE);

            return 4;
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

  if (matched == 1)
  {
    print_error_unknown_operand_combo(instr, asm_context);
    return -1;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

