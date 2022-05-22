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

#include "asm/unsp.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/unsp.h"

#define MAX_OPERANDS 3

enum
{
  OPERAND_NONE = 0,
  OPERAND_REGISTER,
  OPERAND_NUMBER,
  OPERAND_INDIRECT_BP_IMM6,
  OPERAND_IMMEDIATE,
  OPERAND_INDIRECT_RS,
  OPERAND_INDIRECT_RS_DEC,
  OPERAND_INDIRECT_RS_INC,
  OPERAND_INDIRECT_INC_RS,
  OPERAND_D_INDIRECT_RS,
  OPERAND_D_INDIRECT_RS_DEC,
  OPERAND_D_INDIRECT_RS_INC,
  OPERAND_D_INDIRECT_INC_RS,
  OPERAND_RS_ASR_SHIFT,
  OPERAND_RS_LSL_SHIFT,
  OPERAND_RS_LSR_SHIFT,
  OPERAND_RS_ROL_SHIFT,
  OPERAND_RS_ROR_SHIFT,
  OPERAND_INDIRECT_ADDRESS,
  OPERAND_REGISTER_RANGE,
};

struct _operand
{
  int value;
  int type;
  int shift;
  int end_reg;
};

static int get_register_unsp(const char *token)
{
  if ((token[0] == 'r' || token[0] == 'R') &&
      (token[1] >= '1' && token[1] <= '5') &&
      (token[2] == 0))
  {
    return token[1] - '0';
  }

  if (strcasecmp(token, "sp") == 0) { return 0; }
  if (strcasecmp(token, "bp") == 0) { return 5; }
  if (strcasecmp(token, "sr") == 0) { return 6; }
  if (strcasecmp(token, "pc") == 0) { return 7; }

  return -1;
}

static int get_number(
  struct _asm_context *asm_context,
  struct _operand *operand,
  const char *instr)
{
  int n;

  if (eval_expression(asm_context, &n) != 0)
  {
    if (asm_context->pass == 2)
    {
      print_error_illegal_expression(instr, asm_context);
      return -1;
    }

    memory_write_m(&asm_context->memory, asm_context->address, 1);
    ignore_operand(asm_context);
    n = 0;
  }

  operand->value = n;

  return 0;
}

static int generate_alu_2(
  struct _asm_context *asm_context,
  struct _operand *operands,
  uint16_t opcode)
{
  switch (operands[1].type)
  {
    case OPERAND_INDIRECT_BP_IMM6:
    {
      if (operands[1].value < 0 || operands[1].value > 0x3f)
      {
        print_error_range("Constant", 0, 0x3f, asm_context);
        return -1;
      }

      if (operands[0].value == 7)
      {
        print_error("Cannot use PC as destination with [BP+imm6]", asm_context);
        return -1;
      }

      opcode |=
        (operands[0].value << 9) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_IMMEDIATE:
    {
      if (operands[1].value < -32768 || operands[1].value > 0xffff)
      {
        print_error_range("Constant", -32768, 0xffff, asm_context);
        return -1;
      }

      int force_long =
        memory_read_m(&asm_context->memory, asm_context->address);

      if (operands[0].value == 7) { force_long = 1; }

      if (operands[1].value >= 0 &&
          operands[1].value <= 0x3f &&
          force_long == 0)
      {
        opcode |=
          (operands[0].value << 9) | (1 << 6) |
           operands[1].value;

        add_bin16(asm_context, opcode, IS_OPCODE);

        return 2;
      }
        else
      {
        opcode |=
          (operands[0].value << 9) | (4 << 6) | (1 << 3) |
           operands[0].value;

        add_bin16(asm_context, opcode, IS_OPCODE);
        add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);

        return 4;
      }
    }
    case OPERAND_INDIRECT_RS:
    {
      opcode |=
        (operands[0].value << 9) | (3 << 6) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_INDIRECT_RS_DEC:
    {
      opcode |=
        (operands[0].value << 9) | (3 << 6) | (1 << 3) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_INDIRECT_RS_INC:
    {
      opcode |=
        (operands[0].value << 9) | (3 << 6) | (2 << 3) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_INDIRECT_INC_RS:
    {
      opcode |=
        (operands[0].value << 9) | (3 << 6) | (3 << 3) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_D_INDIRECT_RS:
    {
      opcode |=
        (operands[0].value << 9) | (3 << 6) | (4 << 3) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_D_INDIRECT_RS_DEC:
    {
      opcode |=
        (operands[0].value << 9) | (3 << 6) | (5 << 3) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_D_INDIRECT_RS_INC:
    {
      opcode |=
        (operands[0].value << 9) | (3 << 6) | (6 << 3) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_D_INDIRECT_INC_RS:
    {
      opcode |=
        (operands[0].value << 9) | (3 << 6) | (7 << 3) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_REGISTER:
    {
      opcode |=
        (operands[0].value << 9) | (4 << 6) |
         operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_RS_ASR_SHIFT:
    {
      opcode |= (operands[0].value << 9) | (4 << 6) |
                ((4 + (operands[1].shift - 1)) << 3) |
                 operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_RS_LSL_SHIFT:
    {
      opcode |= (operands[0].value << 9) | (5 << 6) |
               ((operands[1].shift - 1) << 3) |
                 operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_RS_LSR_SHIFT:
    {
      opcode |= (operands[0].value << 9) | (5 << 6) |
                ((4 + (operands[1].shift - 1)) << 3) |
                 operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_RS_ROL_SHIFT:
    {
      opcode |= (operands[0].value << 9) | (6 << 6) |
               ((operands[1].shift - 1) << 3) |
                 operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_RS_ROR_SHIFT:
    {
      opcode |= (operands[0].value << 9) | (6 << 6) |
                ((4 + (operands[1].shift - 1)) << 3) |
                 operands[1].value;

      add_bin16(asm_context, opcode, IS_OPCODE);

      return 2;
    }
    case OPERAND_INDIRECT_ADDRESS:
    {
      int force_long =
        memory_read_m(&asm_context->memory, asm_context->address);

      if (operands[1].value >= 0x00 &&
          operands[1].value <= 0x3f &&
          force_long == 0)
      {
        opcode |=
          (operands[0].value << 9) | (7 << 6) |
           operands[1].value;

        add_bin16(asm_context, opcode, IS_OPCODE);

        return 2;
      }
        else
      {
        opcode |=
           (operands[0].value << 9) | (4 << 6) | (2 << 3) |
            operands[0].value;

        add_bin16(asm_context, opcode, IS_OPCODE);
        add_bin16(asm_context, operands[1].value, IS_OPCODE);

        return 4;
      }
    }
  }

  return -2;
}

int parse_instruction_unsp(struct _asm_context *asm_context, char *instr)
{
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  int token_type;
  int opcode;
  int matched = 0;
  int n;

  lower_copy(instr_case, instr);

  if (strcmp(instr_case, "int") == 0 ||
      strcmp(instr_case, "fir_mov") == 0 ||
      strcmp(instr_case, "irq") == 0 ||
      strcmp(instr_case, "fiq") == 0)
  {
    int count;

    strcat(instr_case, " ");

    for (count = 0; count < 3; count++)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
      {
        tokens_push(asm_context, token, token_type);
        break;
      }

      if (strlen(token) > 10)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      lower_copy(instr_case + strlen(instr_case), token);
    }
  }

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    do
    {
      // Check for registers
      n = get_register_unsp(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_REGISTER;
        operands[operand_count].value = n;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token, '-'))
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);

          n = get_register_unsp(token);

          if (n == -1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].type = OPERAND_REGISTER_RANGE;

          if (n < operands[operand_count].value)
          {
            operands[operand_count].end_reg = operands[operand_count].value;
            operands[operand_count].value = n;
          }
            else
          {
            operands[operand_count].end_reg = n;
          }
          break;
        }

        if (strcasecmp(token, "asr") == 0)
        {
          operands[operand_count].type = OPERAND_RS_ASR_SHIFT;
        }
          else
        if (strcasecmp(token, "lsl") == 0)
        {
          operands[operand_count].type = OPERAND_RS_LSL_SHIFT;
        }
          else
        if (strcasecmp(token, "lsr") == 0)
        {
          operands[operand_count].type = OPERAND_RS_LSR_SHIFT;
        }
          else
        if (strcasecmp(token, "rol") == 0)
        {
          operands[operand_count].type = OPERAND_RS_ROL_SHIFT;
        }
          else
        if (strcasecmp(token, "ror") == 0)
        {
          operands[operand_count].type = OPERAND_RS_ROR_SHIFT;
        }

        if (operands[operand_count].type == OPERAND_REGISTER)
        {
          tokens_push(asm_context, token, token_type);
        }
          else
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (token_type != TOKEN_NUMBER)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          int num = atoi(token);

          if (num < 1 || num > 4)
          {
            print_error_range("Shift", 1, 4, asm_context);
            return -1;
          }

          operands[operand_count].shift = num;
        }

        break;
      }

      int d = 0;

      if (token_type == TOKEN_LABEL && strcasecmp(token, "d") == 0)
      {
        d = 4;

        token_type = tokens_get(asm_context, token, TOKENLEN);
      }

      if (IS_TOKEN(token, '['))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token, '+'))
        {
          if (expect_token(asm_context, '+') == -1) { return -1; }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          n = get_register_unsp(token);

          if (n == -1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].type = OPERAND_INDIRECT_INC_RS + d;
          operands[operand_count].value = n;
          if (expect_token(asm_context, ']') == -1) { return -1; }
          break;
        }

        // Check for registers
        n = get_register_unsp(token);

        if (n != -1)
        {
          operands[operand_count].type = OPERAND_INDIRECT_RS + d;
          operands[operand_count].value = n;

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token, ']')) { break; }

          if (IS_TOKEN(token, '+'))
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);
            tokens_push(asm_context, token, token_type);

            // Check for [bp+num] or [rs++]
            if (IS_NOT_TOKEN(token, '+'))
            {
              if (get_number(asm_context, &operands[operand_count], instr) != 0)
              {
                return -1;
              }

              operands[operand_count].type = OPERAND_INDIRECT_BP_IMM6;

              if (expect_token(asm_context, ']') == -1) { return -1; }
              break;
            }

            if (expect_token(asm_context, '+') == -1) { return -1; }
            if (expect_token(asm_context, ']') == -1) { return -1; }
            operands[operand_count].type = OPERAND_INDIRECT_RS_INC + d;
            break;
          }

          if (IS_TOKEN(token, '-'))
          {
            if (expect_token(asm_context, '-') == -1) { return -1; }
            if (expect_token(asm_context, ']') == -1) { return -1; }
            operands[operand_count].type = OPERAND_INDIRECT_RS_DEC + d;
            break;
          }

          print_error_unexp(token, asm_context);
          return -1;
        }

        if (d != 0)
        {
          print_error_unexp("d:", asm_context);
          return -1;
        }

        tokens_push(asm_context, token, token_type);

        operands[operand_count].type = OPERAND_INDIRECT_ADDRESS;

        int num;

        if (eval_expression(asm_context, &num) != 0)
        {
          if (asm_context->pass == 2)
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }

          memory_write_m(&asm_context->memory, asm_context->address, 1);
          ignore_operand(asm_context);
          num = 0;
          tokens_push(asm_context, "]", TOKEN_SYMBOL);
        }

        operands[operand_count].value = num;

        if (num < 0 || num > 0xffff)
        {
          print_error_range("Address", 0, 0xffff, asm_context);
          return -1;
        }

        if (expect_token(asm_context, ']') == -1) { return -1; }
        break;
      }

      if (d != 0)
      {
        print_error_unexp("d:", asm_context);
        return -1;
      }

      operands[operand_count].type = OPERAND_NUMBER;

      if (IS_TOKEN(token, '#'))
      {
        operands[operand_count].type = OPERAND_IMMEDIATE;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }

      if (get_number(asm_context, &operands[operand_count], instr) != 0)
      {
        return -1;
      }
    } while (0);

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token, ',') || operand_count == 5)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (operand_count == MAX_OPERANDS)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  for (n = 0; table_unsp[n].instr != NULL; n++)
  {
    if (strcmp(table_unsp[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch (table_unsp[n].type)
      {
        case UNSP_OP_NONE:
        {
          add_bin16(asm_context, table_unsp[n].opcode, IS_OPCODE);
          return 2;
        }
        case UNSP_OP_GOTO:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            if (operands[0].value < 0 || operands[0].value > 0x3fffff)
            {
              print_error_range("Constant", 0, 0x3fffff, asm_context);
              return -1;
            }

            opcode =
              table_unsp[n].opcode |
            ((operands[0].value >> 16) & 0x3f);

            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);

            return 4;
          }

          break;
        }
        case UNSP_OP_MUL:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER)
          {
            opcode =
              table_unsp[n].opcode |
             (operands[0].value << 9) |
              operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);

            return 2;
          }

          break;
        }
        case UNSP_OP_MAC:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_INDIRECT_RS &&
              operands[1].type == OPERAND_INDIRECT_RS &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (operands[2].value < 1 || operands[2].value > 16)
            {
              print_error_range("mac", 1, 16, asm_context);
              return -1;
            }

            if (operands[0].value != 1 &&
                operands[0].value != 2 &&
                operands[0].value != 5)
            {
              print_error("Registers for mac must be r1, r2, r5.", asm_context);
              return -1;
            }

            if (operands[1].value != 1 &&
                operands[1].value != 2 &&
                operands[1].value != 5)
            {
              print_error("Registers for mac must be r1, r2, r5.", asm_context);
              return -1;
            }

            int opn = operands[2].value;

            if (opn == 16) { opn = 0; }

            opcode =
              table_unsp[n].opcode |
             (operands[0].value << 9) | (opn << 3) |
              operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);

            return 2;
          }

          break;
        }
        case UNSP_OP_JMP:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            int offset = operands[0].value - ((asm_context->address / 2) + 1);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -0x3f || offset > 0x3f)
            {
              print_error_range("Offset", -0x3f, 0x3f, asm_context);
              return -1;
            }

            if (offset < 0)
            {
              offset = -offset;
              offset &= 0x3f;
              offset |= 0x40;
            }

            add_bin16(asm_context, table_unsp[n].opcode | offset, IS_OPCODE);

            return 2;
          }

          break;
        }
        case UNSP_OP_ALU:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER &&
              operands[2].type == OPERAND_INDIRECT_ADDRESS)
          {
            opcode = table_unsp[n].opcode |
                     (operands[0].value << 9) | (4 << 6) | (2 << 3) |
                      operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, operands[2].value, IS_OPCODE);

            return 4;
          }

          if (operand_count == 3 &&
              operands[0].type == OPERAND_INDIRECT_ADDRESS &&
              operands[1].type == OPERAND_REGISTER &&
              operands[2].type == OPERAND_REGISTER)
          {
            opcode = table_unsp[n].opcode |
                     (operands[2].value << 9) | (4 << 6) | (3 << 3) |
                      operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, operands[0].value, IS_OPCODE);

            return 4;
          }

          if (operand_count == 3 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER &&
              operands[2].type == OPERAND_IMMEDIATE)
          {
            opcode = table_unsp[n].opcode |
                     (operands[0].value << 9) | (4 << 6) | (1 << 3) |
                      operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, operands[2].value & 0xffff, IS_OPCODE);

            return 4;
          }

          if (operand_count == 2 && operands[0].type == OPERAND_REGISTER)
          {
            int r = generate_alu_2(asm_context, operands, table_unsp[n].opcode);
            if (r == -1) { return -1; }
            if (r > 0) { return r; }
          }

          break;
        }
        case UNSP_OP_ALU_2:
        {
          int force_long =
            memory_read_m(&asm_context->memory, asm_context->address);

          if (table_unsp[n].opcode == 0xd000)
          {
            switch (operands[1].type)
            {
              case OPERAND_IMMEDIATE:
              case OPERAND_REGISTER:
              case OPERAND_RS_ASR_SHIFT:
              case OPERAND_RS_LSL_SHIFT:
              case OPERAND_RS_LSR_SHIFT:
              case OPERAND_RS_ROL_SHIFT:
              case OPERAND_RS_ROR_SHIFT:
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
            }
          }

          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_INDIRECT_ADDRESS)
          {
            if (operands[1].value >= 0x00 &&
                operands[1].value <= 0x3f &&
                force_long == 0)
            {
              opcode = table_unsp[n].opcode |
                      (operands[0].value << 9) | (7 << 6) |
                       operands[1].value;

              add_bin16(asm_context, opcode, IS_OPCODE);

              return 2;
            }
              else
            {
              int opn = table_unsp[n].opcode == 0xd000 ? 3 : 2;

              opcode = table_unsp[n].opcode |
                       (operands[0].value << 9) | (4 << 6) | (opn << 3) |
                        operands[0].value;

              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value, IS_OPCODE);

              return 4;
            }
          }

          if (operand_count == 2 &&
              operands[0].type == OPERAND_INDIRECT_ADDRESS &&
              operands[1].type == OPERAND_REGISTER)
          {
            if (table_unsp[n].opcode != 0x6000)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode = table_unsp[n].opcode |
                     (operands[1].value << 9) | (4 << 6) | (3 << 3) |
                      operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, operands[0].value, IS_OPCODE);

            return 4;
          }

          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_IMMEDIATE)
          {
            int force_long =
              memory_read_m(&asm_context->memory, asm_context->address);

            if (operands[1].value < 0 ||
                operands[1].value > 0x3f ||
                force_long == 1)
            {
              opcode = table_unsp[n].opcode |
                       (operands[0].value << 9) | (4 << 6) | (1 << 3) |
                        operands[0].value;

              add_bin16(asm_context, opcode, IS_OPCODE);
              add_bin16(asm_context, operands[1].value & 0xffff, IS_OPCODE);
            }
              else
            {
              opcode = table_unsp[n].opcode |
                       (operands[0].value << 9) | (1 << 6) |
                        operands[1].value;

              add_bin16(asm_context, opcode, IS_OPCODE);
            }

            return 4;
          }

          if (operand_count == 2 && operands[0].type == OPERAND_REGISTER)
          {
            int r = generate_alu_2(asm_context, operands, table_unsp[n].opcode);
            if (r == -1) { return -1; }
            if (r > 0) { return r; }
          }

          break;
        }
        case UNSP_OP_POP:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_INDIRECT_RS)
          {
            if (operands[0].value == 0)
            {
              print_error("Cannot pop SP", asm_context);
              return -1;
            }

            opcode = table_unsp[n].opcode |
                    ((operands[0].value - 1) << 9) | (2 << 6) | (1 << 3) |
                      operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
            else
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER_RANGE &&
              operands[1].type == OPERAND_INDIRECT_RS)
          {
            if (operands[0].value == 0)
            {
              print_error("Cannot pop SP", asm_context);
              return -1;
            }

            int reg_count = (operands[0].end_reg - operands[0].value) + 1;
            opcode = table_unsp[n].opcode |
                    ((operands[0].value - 1) << 9) |
                     (2 << 6) | (reg_count << 3) |
                      operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
            else
          if (operand_count == 1 && operands[0].type == OPERAND_REGISTER)
          {
            if (operands[0].value == 0)
            {
              print_error("Cannot pop SP", asm_context);
              return -1;
            }

            opcode = table_unsp[n].opcode |
                    ((operands[0].value - 1) << 9) | (2 << 6) | (1 << 3);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
            else
          if (operand_count == 1 && operands[0].type == OPERAND_REGISTER_RANGE)
          {
            if (operands[0].value == 0)
            {
              print_error("Cannot pop SP", asm_context);
              return -1;
            }

            int reg_count = (operands[0].end_reg - operands[0].value) + 1;
            opcode = table_unsp[n].opcode |
                    ((operands[0].value - 1) << 9) |
                     (2 << 6) | (reg_count << 3);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case UNSP_OP_PUSH:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_INDIRECT_RS)
          {
            opcode = table_unsp[n].opcode |
                     (operands[0].value << 9) | (2 << 6) | (1 << 3) |
                      operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
            else
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER_RANGE &&
              operands[1].type == OPERAND_INDIRECT_RS)
          {
            int reg_count = (operands[0].end_reg - operands[0].value) + 1;

            if (reg_count >= 8)
            {
              print_error("Cannot push more than seven registers", asm_context);
              return -1;
            }

            opcode = table_unsp[n].opcode |
                     (operands[0].end_reg << 9) |
                     (2 << 6) | (reg_count << 3) |
                      operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
            else
          if (operand_count == 1 && operands[0].type == OPERAND_REGISTER)
          {
            opcode = table_unsp[n].opcode |
                     (operands[0].value << 9) | (2 << 6) | (1 << 3);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
            else
          if (operand_count == 1 && operands[0].type == OPERAND_REGISTER_RANGE)
          {
            int reg_count = (operands[0].end_reg - operands[0].value) + 1;

            if (reg_count >= 8)
            {
              print_error("Cannot push more than seven registers", asm_context);
              return -1;
            }

            opcode = table_unsp[n].opcode |
                     (operands[0].end_reg << 9) |
                     (2 << 6) | (reg_count << 3);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        default:
        {
          break;
        }
      }

      break;
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

