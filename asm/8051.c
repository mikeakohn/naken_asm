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

#include "asm/8051.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/8051.h"

enum
{
  OPERAND_REG,
  OPERAND_AT_REG,
  OPERAND_A,
  OPERAND_C,
  OPERAND_AB,
  OPERAND_DPTR,
  OPERAND_AT_A_PLUS_DPTR,
  OPERAND_AT_A_PLUS_PC,
  OPERAND_AT_DPTR,
  OPERAND_DATA,
  OPERAND_SLASH_NUM,
  OPERAND_NUM,
  OPERAND_SLASH_BIT_ADDRESS,
  OPERAND_BIT_ADDRESS,
};

struct _operand
{
  int type;
  int value;
  int bit;
};

static int get_register_8051(char *token)
{
  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  if (token[1] >= '0' && token[1] <= '7' && token[2] == 0)
  {
    return token[1] - '0';
  }

  return -1;
}

static int compute_bit_address(
  struct _asm_context *asm_context,
  int address,
  int bit)
{
  if (check_range(asm_context, "Bit", bit, 0, 7) == -1) { return -1; }

  if (address >= 0x80)
  {
    if ((address & 0x07) != 0)
    {
      print_error("Not bit addressable", asm_context);
    }

    return address + bit;
  }

  if (address >= 0x20 && address <= 0x2f)
  {
    return ((address - 0x20) * 8) + bit;
  }

  print_error("Not bit addressable", asm_context);

  return -1;
}

int parse_instruction_8051(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[3];
  int operand_count = 0;
  int token_type;
  int matched = 0;
  int num, n, r;
  int count = 1;

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

    num = get_register_8051(token);

    if (num != -1)
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = num;
    }
      else
    if (token_type == TOKEN_STRING &&
       (IS_TOKEN(token,'A') || IS_TOKEN(token,'a')))
    {
      operands[operand_count].type = OPERAND_A;
    }
      else
    if (token_type == TOKEN_STRING &&
       (IS_TOKEN(token,'C') || IS_TOKEN(token,'c')))
    {
      operands[operand_count].type = OPERAND_C;
    }
      else
    if (strcasecmp(token, "ab") == 0)
    {
      operands[operand_count].type = OPERAND_AB;
    }
      else
    if (strcasecmp(token, "dptr") == 0)
    {
      operands[operand_count].type = OPERAND_DPTR;
    }
      else
    if (IS_TOKEN(token,'@'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      num = get_register_8051(token);
      if (num != -1)
      {
        operands[operand_count].type = OPERAND_AT_REG;
        operands[operand_count].value = num;
      }
        else
      if (strcasecmp(token, "dptr") == 0)
      {
        operands[operand_count].type = OPERAND_AT_DPTR;
      }
        else
      if (token_type == TOKEN_STRING && strcasecmp(token, "a") == 0)
      {
        do
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token,'+')) break;
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (strcasecmp(token, "dptr") == 0)
          {
            operands[operand_count].type = OPERAND_AT_A_PLUS_DPTR;
          }
            else
          if (strcasecmp(token, "pc") == 0)
          {
            operands[operand_count].type = OPERAND_AT_A_PLUS_PC;
          }
        } while (0);

        if (operands[operand_count].type == 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
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

      operands[operand_count].type = OPERAND_DATA;
      operands[operand_count].value = num;
    }
      else
    if (IS_TOKEN(token,'/'))
    {
      if (asm_context->pass == 1)
      {
        ignore_operand(asm_context);
        operands[operand_count].value = 0x80;
        operands[operand_count].type = OPERAND_SLASH_BIT_ADDRESS;
      }
        else
      {
        if (eval_expression(asm_context, &num) != 0)
        {
          return -1;
        }

        operands[operand_count].value = num;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token, '.'))
        {
          if (eval_expression(asm_context, &num) != 0)
          {
            return -1;
          }

          operands[operand_count].type = OPERAND_SLASH_BIT_ADDRESS;
          operands[operand_count].bit = num;
        }
          else
        {
          tokens_push(asm_context, token, token_type);
          operands[operand_count].type = OPERAND_SLASH_NUM;
        }
      }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      operands[operand_count].type = OPERAND_NUM;

      if (asm_context->pass == 1)
      {
        // Need to see if there is a . in the operands so it can be
        // marked as a bit field or not.
        while (1)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token, ',') ||
              token_type == TOKEN_EOL ||
              token_type == TOKEN_EOF)
          {
            break;
          }
            else
          if (IS_TOKEN(token, '.'))
          {
            operands[operand_count].value = 0x80;
            operands[operand_count].bit = 0;
            operands[operand_count].type = OPERAND_BIT_ADDRESS;
          }
        }

        tokens_push(asm_context, token, token_type);
      }
        else
      {
        if (eval_expression(asm_context, &num) != 0)
        {
          return -1;
        }

        operands[operand_count].value = num;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token, '.'))
        {
          if (eval_expression(asm_context, &num) != 0)
          {
            return -1;
          }

          operands[operand_count].type = OPERAND_BIT_ADDRESS;
          operands[operand_count].bit = num;
        }
          else
        {
          tokens_push(asm_context, token, token_type);
        }
      }
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 3)
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

  for (n = 0; n < 256; n++)
  {
    if (strcmp(table_8051[n].name, instr_case) == 0)
    {
      matched = 1;

      for (r = 0; r < 3; r++)
      {
        if (table_8051[n].op[r] == OP_NONE) { break; }

        switch (table_8051[n].op[r])
        {
          case OP_REG:
            if (operands[r].type != OPERAND_REG ||
                operands[r].value != table_8051[n].range) { r = 4; }
            break;
          case OP_AT_REG:
            if (operands[r].type != OPERAND_AT_REG ||
                operands[r].value != table_8051[n].range) { r = 4; }
            break;
          case OP_A:
            if (operands[r].type != OPERAND_A) { r = 4; }
            break;
          case OP_C:
            if (operands[r].type != OPERAND_C) { r = 4; }
            break;
          case OP_AB:
            if (operands[r].type != OPERAND_AB) { r = 4; }
            break;
          case OP_DPTR:
            if (operands[r].type != OPERAND_DPTR) { r = 4; }
            break;
          case OP_AT_A_PLUS_DPTR:
            if (operands[r].type != OPERAND_AT_A_PLUS_DPTR) { r = 4; }
            break;
          case OP_AT_A_PLUS_PC:
            if (operands[r].type != OPERAND_AT_A_PLUS_PC) { r = 4; }
            break;
          case OP_AT_DPTR:
            if (operands[r].type != OPERAND_AT_DPTR) { r = 4; }
            break;
          case OP_DATA_16:
            if (operands[r].type != OPERAND_DATA ||
                (operands[r].value < -32768 ||
                 operands[r].value > 0xffff)) { r = 4; }
            break;
          case OP_CODE_ADDR:
            if (operands[r].type != OPERAND_NUM ||
                (operands[r].value < 0 ||
                 operands[r].value > 0xffff)) { r = 4; }
            break;
          case OP_RELADDR:
            if (operands[r].type != OPERAND_NUM) { r = 4; }
            break;
          case OP_DATA:
            if (operands[r].type != OPERAND_DATA ||
                (operands[r].value < -128 ||
                 operands[r].value > 255)) { r = 4; }
            break;
          case OP_SLASH_BIT_ADDR:
            if ((operands[r].type != OPERAND_SLASH_BIT_ADDRESS &&
                 operands[r].type != OPERAND_SLASH_NUM) ||
                (operands[r].value < 0 || operands[r].value > 255))
            {
              r = 4;
            }
            break;
          case OP_BIT_ADDR:
            if (operands[r].type != OPERAND_BIT_ADDRESS)
            {
              // This seems kind of invalid, but some other assembler allows
              // the bit address to be defined as the binary version of the
              // address rather than address.bit for at least clr and setb.
              if (operands[r].type != OPERAND_NUM) { r = 4; }
            }

            if (operands[r].value < 0 || operands[r].value > 255) { r = 4; }
            break;
          case OP_PAGE:
            if (((operands[r].value >> 8) & 7) == table_8051[n].range)
            {
              int high_bits = operands[r].value & 0xf800;
              int address = asm_context->address + 2;

              if (asm_context->pass == 1)
              {
                high_bits = address & 0xf800;
              }

              if (high_bits != (address & 0xf800))
              {
                print_error("Destination address outside 2k block.", asm_context);
                return -1;
              }
            }
              else
            {
              // r = 4 breaks out of the for loop.
              r = 4;
              break;
            }
            break;
          case OP_IRAM_ADDR:
            if (operands[r].type != OPERAND_NUM ||
                (operands[r].value < 0 ||
                 operands[r].value > 255)) { r = 4; }
            break;
          default:
            print_error_internal(asm_context, __FILE__, __LINE__);
            return -1;
        }
      }

      if (r == operand_count)
      {
        add_bin8(asm_context, n, IS_OPCODE);

        // MOV iram_addr, iram_addr
        if (n == 0x85)
        {
          add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);
          add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);
          break;
        }

        for (r = 0; r < 3; r++)
        {
          if (table_8051[n].op[r] == OP_NONE) { break; }

          switch (table_8051[n].op[r])
          {
            case OP_DATA_16:
            case OP_CODE_ADDR:
            {
              uint16_t value = operands[r].value & 0xffff;
              add_bin8(asm_context, value >> 8, IS_OPCODE);
              add_bin8(asm_context, value & 0xff, IS_OPCODE);
              count += 2;
              break;
            }
            case OP_RELADDR:
            {
              num = operands[r].value - (asm_context->address + 1);
              if (asm_context->pass == 1) { num = 0; }

              if (num < -128 || num > 127)
              {
                print_error_range("Offset", -128, 127, asm_context);
                return -1;
              }

              add_bin8(asm_context, num & 0xff, IS_OPCODE);
              count++;
              break;
            }
            case OP_SLASH_BIT_ADDR:
            {
              if (operands[r].type == OPERAND_SLASH_NUM)
              {
                num = operands[r].value;
              }
                else
              {
                num = compute_bit_address(
                  asm_context,
                  operands[r].value,
                  operands[r].bit);
              }

              if (num == -1) { return -1; }

              add_bin8(asm_context, num, IS_OPCODE);
              count++;
              break;
            }
            case OP_BIT_ADDR:
            {
              if (operands[r].type == OPERAND_NUM)
              {
                num = operands[r].value;
              }
                else
              {
                num = compute_bit_address(
                  asm_context,
                  operands[r].value,
                  operands[r].bit);
              }

              if (num == -1) { return -1; }

              add_bin8(asm_context, num, IS_OPCODE);
              count++;
              break;
            }
            case OP_DATA:
            case OP_PAGE:
            case OP_IRAM_ADDR:
            {
              add_bin8(asm_context, operands[r].value & 0xff, IS_OPCODE);
              count++;
              break;
            }
          }
        }

        break;
      }
    }
  }

  if (n == 256)
  {
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

  return count;
}

