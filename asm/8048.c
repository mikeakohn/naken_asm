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

#include "asm/8048.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/8048.h"

enum
{
  OPERAND_CONST,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_P,
  OPERAND_R,
  OPERAND_AT_R,
};

struct _operand
{
  int operand;
  int value;
  int type;
};

static int get_register_8048(char *token)
{
  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  if (token[1] >= '0' && token[1] <= '7' && token[2] == 0)
  {
    return token[1] - '0';
  }

  return -1;
}

static int check_match(int type, int operand, int table_type)
{
  if (type != table_type)
  {
    if (operand == OPERAND_P)
    {
      switch (table_type)
      {
        case OP_PP:
        case OP_P0:
        case OP_P03:
        case OP_P12:
          return 0;
        default:
          return -1;
      }
    }
      else
    if (operand == OPERAND_ADDRESS)
    {
      if (table_type == OP_PADDR || table_type == OP_ADDR)
      {
        return 0;
      }

      return -1;
    }
      else
    {
      return -1;
    }
  }

  return 0;
}

static int process_op(
  struct _asm_context *asm_context,
  struct _operand *operand,
  uint8_t type,
  uint8_t *data)
{
  if (operand->operand == OPERAND_CONST)
  {
    switch (type)
    {
      case OP_A:
      case OP_C:
      case OP_I:
      case OP_T:
      case OP_F0:
      case OP_F1:
      case OP_BUS:
      case OP_CLK:
      case OP_CNT:
      case OP_MB0:
      case OP_MB1:
      case OP_RB0:
      case OP_RB1:
      case OP_PSW:
      case OP_TCNT:
      case OP_TCNTI:
      case OP_AT_A:
      case OP_DMA:
      case OP_FLAGS:
      case OP_STS:
      case OP_DBB:
        return 1;
      default:
        return -1;
    }
  }
    else
  if (operand->operand == OPERAND_P)
  {
    if (type == OP_PP && operand->value >= 4 && operand->value <= 7)
    {
      data[0] |= ((operand->value - 4) & 0x3);
      return 1;
    }

    if (type == OP_P12 && operand->value >= 1 && operand->value <= 2)
    {
      data[0] |= (operand->value & 0x3);
      return 1;
    }

    if (type == OP_P03 && operand->value >= 0 && operand->value <= 3)
    {
      data[0] |= (operand->value & 0x3);
      return 1;
    }

    if (type == OP_P0 && operand->value == 0)
    {
      data[0] |= (operand->value & 0x3);
      return 1;
    }

    return -1;
  }
    else
  if (operand->operand == OPERAND_R)
  {
    if (type != OP_RR) { return -1; }

    data[0] |= (operand->value & 0x7);

    return 1;
  }
    else
  if (operand->operand == OPERAND_AT_R)
  {
    if (type != OP_AT_R) { return -1; }
    if (operand->value > 1) { return -1; }

    data[0] |= (operand->value & 0x1);

    return 1;
  }
    else
  if (operand->operand == OPERAND_NUMBER && type == OP_NUM)
  {
    data[1] = operand->value;
    return 2;
  }
    else
  if (operand->operand == OPERAND_ADDRESS)
  {
    int address = operand->value;

    if (address > 8191)
    {
      print_error_range("Address", 0, 8191, asm_context);
      return -1;
    }

    if (type == OP_ADDR)
    {
      address &= 2047;

      data[0] |= (address >> 8) << 5;
      data[1] = address & 0xff;

      return 2;
    }
      else
    if (type == OP_PADDR)
    {
      if (asm_context->pass == 2 && ((address & 0xff00) != (asm_context->address & 0xff00)))
      {
        print_error("Address isn't on same 256-byte page", asm_context);
        return -1;
      }

      data[1] = address & 0xff;
      return 2;
    }
  }

  return -1;
}

int parse_instruction_8048(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  char token[TOKENLEN];
  struct _operand operands[2];
  int operand_count = 0;
  int token_type;
  int matched = 0;
  uint8_t data[2];
  int length;
  int num, n, r;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      if (operand_count != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      break;
    }

    num = get_register_8048(token);

    if (num != -1)
    {
      operands[operand_count].operand = OPERAND_R;
      operands[operand_count].type = OP_RR;
      operands[operand_count].value = num;
    }
      else
    if (IS_TOKEN(token,'A') || IS_TOKEN(token,'a'))
    {
      operands[operand_count].type = OP_A;
    }
      else
    if (IS_TOKEN(token,'C') || IS_TOKEN(token,'c'))
    {
      operands[operand_count].type = OP_C;
    }
      else
    if (IS_TOKEN(token,'I') || IS_TOKEN(token,'i'))
    {
      operands[operand_count].type = OP_I;
    }
      else
    if (IS_TOKEN(token,'T') || IS_TOKEN(token,'t'))
    {
      operands[operand_count].type = OP_T;
    }
      else
    if ((token[0] == 'P' || token[0] == 'p') && token[2] == 0 &&
         token[1] >= '0' && token[2] <= 7)
    {
      operands[operand_count].operand = OPERAND_P;
      operands[operand_count].type = OP_PP;
      operands[operand_count].value = token[1] - '0';
    }
      else
    if (IS_TOKEN(token,'@'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      num = get_register_8048(token);

      if (num != -1)
      {
        if (num > 1)
        {
          print_error_range("@reg", 0, 1, asm_context);
          return -1;
        }

        operands[operand_count].operand = OPERAND_AT_R;
        operands[operand_count].type = OP_AT_R;
        operands[operand_count].value = num;
      }
        else
      if (IS_TOKEN(token,'A') || IS_TOKEN(token,'a'))
      {
        operands[operand_count].type = OP_AT_A;
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }
      else
    if (token_type == TOKEN_POUND)
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
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].operand = OPERAND_NUMBER;
      operands[operand_count].type = OP_NUM;
      operands[operand_count].value = num;
    }
      else
    if (strcasecmp(token, "bus") == 0)
    {
      operands[operand_count].type = OP_BUS;
    }
      else
    if (strcasecmp(token, "clk") == 0)
    {
      operands[operand_count].type = OP_CLK;
    }
      else
    if (strcasecmp(token, "cnt") == 0)
    {
      operands[operand_count].type = OP_CNT;
    }
      else
    if (strcasecmp(token, "tcnt") == 0)
    {
      operands[operand_count].type = OP_TCNT;
    }
      else
    if (strcasecmp(token, "tcnti") == 0)
    {
      operands[operand_count].type = OP_TCNTI;
    }
      else
    if (strcasecmp(token, "PSW") == 0)
    {
      operands[operand_count].type = OP_PSW;
    }
      else
    if (strcasecmp(token, "f0") == 0)
    {
      operands[operand_count].type = OP_F0;
    }
      else
    if (strcasecmp(token, "f1") == 0)
    {
      operands[operand_count].type = OP_F1;
    }
      else
    if (strcasecmp(token, "mb0") == 0)
    {
      operands[operand_count].type = OP_MB0;
    }
      else
    if (strcasecmp(token, "mb1") == 0)
    {
      operands[operand_count].type = OP_MB1;
    }
      else
    if (strcasecmp(token, "rb0") == 0)
    {
      operands[operand_count].type = OP_RB0;
    }
      else
    if (strcasecmp(token, "rb1") == 0)
    {
      operands[operand_count].type = OP_RB1;
    }
      else
    if (strcasecmp(token, "dma") == 0)
    {
      operands[operand_count].type = OP_DMA;
    }
      else
    if (strcasecmp(token, "flags") == 0)
    {
      operands[operand_count].type = OP_FLAGS;
    }
      else
    if (strcasecmp(token, "sts") == 0)
    {
      operands[operand_count].type = OP_STS;
    }
      else
    if (strcasecmp(token, "dbb") == 0)
    {
      operands[operand_count].type = OP_DBB;
    }
      else
    {
      operands[operand_count].type = OP_ADDR;
      operands[operand_count].operand = OPERAND_ADDRESS;

      if (asm_context->pass == 1)
      {
        ignore_operand(asm_context);
        operands[operand_count].value = 0;
      }
        else
      {
        tokens_push(asm_context, token, token_type);

        if (eval_expression(asm_context, &n) != 0)
        {
          return -1;
        }

        operands[operand_count].value = n;
      }
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 2)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
printf("-----\n");
for (n = 0; n < operand_count; n++)
{
printf("[%d %d]", operands[n].type, operands[n].value);
}
printf("\n");
#endif

  for (n = 0; table_8048[n].name != NULL; n++)
  {
    if (table_8048[n].flags == FLAG_8048)
    {
      if (asm_context->cpu_type != CPU_TYPE_8048) { continue; }
    }

    if (table_8048[n].flags == FLAG_8041)
    {
      if (asm_context->cpu_type != CPU_TYPE_8041) { continue; }
    }

    if (strcmp(table_8048[n].name, instr_case) == 0)
    {
      matched = 1;

      do
      {
        if (operand_count == table_8048[n].operand_count)
        {
          if (operand_count == 0)
          {
            add_bin8(asm_context, table_8048[n].opcode, IS_OPCODE);

            return 1;
          }
            else
          if (operand_count == 1)
          {
            if (check_match(operands[0].type,
                            operands[0].operand,
                            table_8048[n].operand_1) == -1)
            {
              break;
            }

            data[0] = table_8048[n].opcode;

            r = process_op(asm_context, &operands[0], table_8048[n].operand_1,  data);
            if (r == -1) { break; }
            length = r;

            for (r = 0; r < length; r++)
            {
              add_bin8(asm_context, data[r], IS_OPCODE);
            }

            return length;
          }
            else
          if (operand_count == 2)
          {
            if (check_match(operands[0].type,
                            operands[0].operand,
                            table_8048[n].operand_1) == -1)
            {
              break;
            }

            if (check_match(operands[1].type,
                            operands[1].operand,
                            table_8048[n].operand_2) == -1)
            {
              break;
            }

            data[0] = table_8048[n].opcode;

            r = process_op(asm_context, &operands[0], table_8048[n].operand_1,  data);
            if (r == -1) { break; }
            length = r;

            r = process_op(asm_context, &operands[1], table_8048[n].operand_2, data);
            if (r == -1) { break; }
            length = r > length ? r : length;

            for (r = 0; r < length; r++)
            {
              add_bin8(asm_context, data[r], IS_OPCODE);
            }

            return length;
          }
        }
      } while (0);
    }
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

