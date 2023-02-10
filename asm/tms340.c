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
#include "asm/tms340.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/tms340.h"

enum
{
  OPERAND_NONE,
  OPERAND_REGISTER,
  OPERAND_REGISTER_INDIRECT,
  OPERAND_REGISTER_INDIRECT_DISP,
  OPERAND_REGISTER_INDIRECT_INC,
  OPERAND_REGISTER_INDIRECT_DEC,
  OPERAND_REGISTER_INDIRECT_XY,
  OPERAND_NUMBER,
  OPERAND_AT_ADDRESS,
  OPERAND_LIST_OF_REGISTERS,
  OPERAND_B,
  OPERAND_L,
  OPERAND_W,
  OPERAND_XY,
};

struct _operand
{
  uint8_t type;
  uint8_t reg;
  uint8_t use_long;
  int value;
  int r;
};

static int get_register_tms340(char *token, int *r)
{
  if (strcasecmp(token, "sp") == 0)
  {
    *r = 0;
    return 15;
  }

  if (token[0] == 'a' || token[0] == 'A')
  {
    *r = 0;
  }
    else
  if (token[0] == 'b' || token[0] == 'B')
  {
    *r = 1;
  }
    else
  {
    return -1;
  }

  if (token[2] == 0 && (token[1] >= '0' && token[1] <= '9'))
  {
    return token[1] - '0';
  }
    else
  if (token[3] == 0 && token[1] == '1' &&
      token[2] >= '0' && token[2] <= '5')
  {
    return 10 + (token[2] - '0');
  }

  return -1;
}

static int is_register(int operand_type)
{
  if (operand_type >= OPERAND_REGISTER &&
      operand_type <= OPERAND_REGISTER_INDIRECT_XY)
  {
    return 1;
  }

  return 0;
}

static int reverse_bits(int value)
{
  int n, i = 0;

  for (n = 0; n < 16; n++)
  {
    i = i << 1;
    if ((value & 1) == 1) { i |= 1; }
    value = value >> 1;
  }

  return i;
}

static int get_operands_register_list(
  struct _asm_context *asm_context,
  struct _operand *operands,
  int *operand_count,
  char *instr)
{
  int token_type;
  char token[TOKENLEN];
  int r, n;
  int i = 0;

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      i++;

      tokens_push(asm_context, token, token_type);

      if (i != 2 || operands[1].value == 0)
      {
        print_error_unknown_operand_combo(instr, asm_context);
        return -1;
      }

      *operand_count = i;

      return 0;
    }

    if ((n = get_register_tms340(token, &r)) == -1)
    {
      if (i == 1 && operands[1].value == 0)
      {
        tokens_push(asm_context, token, token_type);

        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
        }
          else
        {
          if (eval_expression(asm_context, &n) != 0)
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }

          operands[1].value = n;
        }

        *operand_count = 2;

        return 0;
      }

      print_error_unexp(token, asm_context);
      return -1;
    }

    if (i == 0)
    {
      operands[i].type = OPERAND_REGISTER;
      operands[i].reg = n;
      operands[i].r = r;

      i++;

      operands[i].type = OPERAND_LIST_OF_REGISTERS;
      operands[i].value = 0;
    }
      else
    {
      if (operands[0].r != r)
      {
        print_error("Mismatched a/b registers.", asm_context);
        return -1;
      }

      operands[i].value |= 1 << n;
    }

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      tokens_push(asm_context, token, token_type);
      continue;
    }

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }
}

int parse_instruction_tms340(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  struct _operand operands[3];
  int operand_count;
  int matched = 0, ignore;
  int offset;
  int count = 2;
  //int offset;
  int opcode;
  int n, r, i;
  uint16_t extra[8];
  int extra_count;

  lower_copy(instr_case, instr);

  operand_count = 0;
  memset(operands, 0, sizeof(operands));

  // Get operands for awkward register list instructions.
  if (strcmp("mmfm", instr_case) == 0 ||
      strcmp("mmtm", instr_case) == 0)
  {
    if (get_operands_register_list(asm_context, operands, &operand_count, instr) == -1)
    {
      return -1;
    }
  }

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (operand_count >= 3)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if (IS_TOKEN(token, '-'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '*'))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);

        n = get_register_tms340(token, &r);

        if (n == -1)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].type = OPERAND_REGISTER_INDIRECT_DEC;
        operands[operand_count].reg = n;
        operands[operand_count].r = r;
      }
        else
      {
        tokens_push(asm_context, token, token_type);

        strcpy(token, "-");
        token_type = TOKEN_SYMBOL;
      }
    }

    if (operands[operand_count].type != OPERAND_NONE)
    {
    }
      else
    if ((n = get_register_tms340(token, &r)) != -1)
    {
      operands[operand_count].type = OPERAND_REGISTER;
      operands[operand_count].reg = n;
      operands[operand_count].r = r;
    }
      else
    if (IS_TOKEN(token, '*'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      n = get_register_tms340(token, &r);

      if (n == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].type = OPERAND_REGISTER_INDIRECT;
      operands[operand_count].reg = n;
      operands[operand_count].r = r;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '+'))
      {
        operands[operand_count].type = OPERAND_REGISTER_INDIRECT_INC;
      }
        else
      if (IS_TOKEN(token, '.'))
      {
        if (expect_token_s(asm_context, "XY") != 0) { return -1; }

        operands[operand_count].type = OPERAND_REGISTER_INDIRECT_XY;
      }
        else
      if (IS_TOKEN(token, '('))
      {
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

          if (expect_token(asm_context, ')') == -1) { return -1; }

          operands[operand_count].value = n;

          if (n < -32768 || n > 32767)
          {
            print_error_range("Displacement", -32768, 32767, asm_context);
            return -1;
          }
        }

        operands[operand_count].type = OPERAND_REGISTER_INDIRECT_DISP;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (IS_TOKEN(token, 'B') || IS_TOKEN(token, 'b'))
    {
      operands[operand_count].type = OPERAND_B;
    }
      else
    if (IS_TOKEN(token, 'L') || IS_TOKEN(token, 'l'))
    {
      operands[operand_count].type = OPERAND_L;
    }
      else
    if (IS_TOKEN(token, 'W') || IS_TOKEN(token, 'w'))
    {
      operands[operand_count].type = OPERAND_W;
    }
      else
    if (strcasecmp(token, "xy") == 0)
    {
      operands[operand_count].type = OPERAND_XY;
    }
      else
    if (IS_TOKEN(token, '@'))
    {
      operands[operand_count].type = OPERAND_AT_ADDRESS;

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
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &n) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        ignore_operand(asm_context);

        memory_write_m(&asm_context->memory, asm_context->address, 1);
        n = 0;
      }

      if (memory_read_m(&asm_context->memory, asm_context->address) == 1)
      {
        operands[operand_count].use_long = 1;
      }

      operands[operand_count].type = OPERAND_NUMBER;
      operands[operand_count].value = n;
    }

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;

    if (IS_NOT_TOKEN(token,',') || operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  // Check that the register file of all registers match.
  if (operand_count > 1)
  {
    r = -1;

    for (n = 0; n < operand_count; n++)
    {
      if (is_register(operands[n].type) == 1)
      {
        if (r == -1)
        {
          // SP can be in either register file.
          if (operands[n].reg != 15) { r = operands[n].r; }
        }
          else
        if (r != operands[n].r)
        {
          print_error("Mismatched a/b registers.", asm_context);
          return -1;
        }
      }
    }

    // Issue #120: If no register file was selected, just use 0.
    if (r == -1) { r = 0; }

    // Make sure all registers are in the same register file.
    for (n = 0; n < operand_count; n++)
    {
      if (is_register(operands[n].type) == 1)
      {
        operands[n].r = r;
      }
    }
  }

  for (n = 0; table_tms340[n].instr != NULL; n++)
  {
    if (strcmp(table_tms340[n].instr, instr_case) == 0)
    {
      matched = 1;

      if (operand_count == 0 && table_tms340[n].operand_types[0] == OP_NN)
      {
        add_bin16(asm_context, table_tms340[n].opcode, IS_OPCODE);
        return 2;
      }

      if (table_tms340[n].operand_count != operand_count)
      {
        i = table_tms340[n].operand_count;

        if (i > 0 &&
            table_tms340[n].operand_types[i - 1] == OP_F &&
            operand_count == i - 1)
        {
        }
          else
        if (i == 2 &&
           (table_tms340[n].operand_types[0] == OP_IW ||
            table_tms340[n].operand_types[0] == OP_IL))
        {
        }
          else
        {
          continue;
        }
      }

      if (table_tms340[n].operand_count == 0 && operand_count == 0)
      {
        add_bin16(asm_context, table_tms340[n].opcode, IS_OPCODE);
        return 2;
      }

      opcode = table_tms340[n].opcode;
      ignore = 0;
      extra_count = 0;

      for (i = 0; i < table_tms340[n].operand_count; i++)
      {
        switch (table_tms340[n].operand_types[i])
        {
          case OP_RS:
            if (operands[i].type != OPERAND_REGISTER)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg << 5;
            opcode |= operands[i].r << 4;

            break;
          case OP_RD:
            if (operands[i].type != OPERAND_REGISTER)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg;
            opcode |= operands[i].r << 4;

            break;
          case OP_RDS:
            if (operands[i].type != OPERAND_REGISTER)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg;
            opcode |= operands[i].r << 4;
            opcode |= operands[i].reg << 5;

            break;
          case OP_P_RS:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg << 5;
            opcode |= operands[i].r << 4;

            break;
          case OP_P_RD:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg;
            opcode |= operands[i].r << 4;

            break;
          case OP_P_RS_DISP:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT_DISP)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg << 5;
            opcode |= operands[i].r << 4;

            extra[extra_count++] = operands[i].value;

            break;
          case OP_P_RD_DISP:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT_DISP)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg;
            opcode |= operands[i].r << 4;

            extra[extra_count++] = operands[i].value;

            break;
          case OP_P_RS_P:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT_INC)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg << 5;
            opcode |= operands[i].r << 4;

            break;
          case OP_P_RD_P:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT_INC)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg;
            opcode |= operands[i].r << 4;

            break;
          case OP_P_RS_XY:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT_XY)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg << 5;
            opcode |= operands[i].r << 4;

            break;
          case OP_P_RD_XY:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT_XY)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg;
            opcode |= operands[i].r << 4;

            break;
          case OP_MP_RS:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT_DEC)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg << 5;
            opcode |= operands[i].r << 4;

            break;
          case OP_MP_RD:
            if (operands[i].type != OPERAND_REGISTER_INDIRECT_DEC)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].reg;
            opcode |= operands[i].r << 4;

            break;
          case OP_ADDRESS:
            if (operands[i].type != OPERAND_NUMBER)
            {
              ignore = 1;
              break;
            }

            extra[extra_count++] = operands[i].value & 0xffff;
            extra[extra_count++] = (operands[i].value >> 16) & 0xffff;

            break;
          case OP_AT_ADDR:
            if (operands[i].type != OPERAND_AT_ADDRESS)
            {
              ignore = 1;
              break;
            }

            extra[extra_count++] = operands[i].value & 0xffff;
            extra[extra_count++] = (operands[i].value >> 16) & 0xffff;

            break;
          case OP_LIST:
            if (operands[i].type != OPERAND_LIST_OF_REGISTERS)
            {
              ignore = 1;
              break;
            }

            if ((table_tms340[n].opcode & 0x0020) == 0)
            {
              extra[extra_count++] = reverse_bits(operands[i].value);
            }
              else
            {
              extra[extra_count++] = operands[i].value;
            }

            break;
          case OP_B:
            if (operands[i].type != OPERAND_B)
            {
              ignore = 1;
              break;
            }

            break;
          case OP_F:
            if (operands[i].type == OPERAND_NONE && i == 2) { break; }

            if (operands[i].type != OPERAND_NUMBER ||
                operands[i].value < 0 ||
                operands[i].value > 1)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].value << 9;

            break;
          case OP_K32:
            if (operands[i].type != OPERAND_NUMBER ||
               (asm_context->pass == 2 &&
               (operands[i].value < 1 || operands[i].value > 32)))
            {
              ignore = 1;
              break;
            }

            opcode |= (operands[i].value & 0x1f) << 5;

            break;
          case OP_K:
          case OP_1K:
          case OP_2K:
            if (operands[i].type != OPERAND_NUMBER ||
               (asm_context->pass == 2 &&
               (operands[i].value < 0 || operands[i].value > 31)))
            {
              ignore = 1;
              break;
            }

            if (table_tms340[n].operand_types[i] == OP_1K)
            {
              operands[i].value = ~operands[i].value;
            }
              else
            if (table_tms340[n].operand_types[i] == OP_2K)
            {
              operands[i].value = -operands[i].value;
            }

            opcode |= (operands[i].value & 0x1f) << 5;

            break;
          case OP_L:
            if (operands[i].type != OPERAND_L)
            {
              ignore = 1;
              break;
            }

            break;
          case OP_N:
            if (operands[i].type != OPERAND_NUMBER ||
                operands[i].value < 0 || operands[i].value > 31)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].value & 0x1f;

            break;
          case OP_Z:
            if (operands[i].type != OPERAND_NUMBER ||
                operands[i].value < 0 || operands[i].value > 1)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].value << 7;

            break;
          case OP_FE:
            if (operands[i].type != OPERAND_NUMBER ||
                operands[i].value < 0 || operands[i].value > 1)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].value << 5;

            break;
          case OP_FS:
            if (operands[i].type != OPERAND_NUMBER ||
               (asm_context->pass == 2 &&
               (operands[i].value < 1 || operands[i].value > 32)))
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].value & 0x1f;

            break;
          case OP_IL:
          case OP_NIL:
            if (operands[i].type != OPERAND_NUMBER)
            {
              ignore = 1;
              break;
            }

            if (table_tms340[n].operand_types[i] == OP_IL)
            {
              extra[extra_count++] = operands[i].value & 0xffff;
              extra[extra_count++] = (operands[i].value >> 16) & 0xffff;
            }
              else
            {
              extra[extra_count++] = ~(operands[i].value & 0xffff);
              extra[extra_count++] = ~((operands[i].value >> 16) & 0xffff);
            }

            break;
          case OP_IW:
          case OP_NIW:
            if (operands[i].type != OPERAND_NUMBER ||
                operands[operand_count - 1].type == OPERAND_L)
            {
              ignore = 1;
              break;
            }

            if (operands[operand_count - 1].type != OPERAND_W)
            {
              if (operands[i].use_long == 1 ||
                  operands[i].value < -32768 || operands[i].value > 32767)
              {
                ignore = 1;
                break;
              }
            }

            if (table_tms340[n].operand_types[i] == OP_IW)
            {
              extra[extra_count++] = operands[i].value & 0xffff;
            }
              else
            {
              extra[extra_count++] = ~(operands[i].value & 0xffff);
            }

            break;
          case OP_NN:
            if (operands[i].type != OPERAND_NUMBER ||
                operands[i].value < 0 ||
                operands[i].value > 31)
            {
              ignore = 1;
              break;
            }

            opcode |= operands[i].value;

            break;
          case OP_XY:
            if (operands[i].type != OPERAND_XY)
            {
              ignore = 1;
              break;
            }

            break;
          case OP_DISP:
            if (operands[i].type != OPERAND_NUMBER)
            {
              ignore = 1;
              break;
            }

            if (asm_context->pass == 1)
            {
              extra[extra_count++] = 0;
            }
              else
            {
              offset = operands[i].value - (asm_context->address + 4);

              if ((offset & 1) != 0)
              {
                print_error_align(asm_context, 2);
                return -1;
              }

              if (offset < -65536 || offset > 65534)
              {
                print_error_range("Displacement", -65536, 65534, asm_context);
              }

              extra[extra_count++] = (offset >> 1) & 0xffff;
            }

            break;
          case OP_SKIP:
            if (operands[i].type != OPERAND_NUMBER)
            {
              ignore = 1;
              break;
            }

            if (asm_context->pass == 2)
            {
              offset = operands[i].value - (asm_context->address + 2);

              if ((offset & 1) != 0)
              {
                print_error_align(asm_context, 2);
                return -1;
              }

              offset = offset / 2;

              if (offset < -31 || offset > 31)
              {
                print_error_range("Displacement", -31, 31, asm_context);
              }

              if (offset < 0)
              {
                offset = -offset;
                opcode |= 0x0400;
              }

              opcode |= (offset & 0x1f) << 5;
            }

            break;
          case OP_JUMP_REL:
            if (operands[i].type != OPERAND_NUMBER)
            {
              ignore = 1;
              break;
            }

            if (asm_context->pass == 1)
            {
              if (operands[i].use_long == 0)
              {
                offset = operands[i].value - (asm_context->address + 2);

                if (offset < -256 || offset > 254)
                {
                  extra[extra_count++] = 0;
                  memory_write_m(&asm_context->memory, asm_context->address, 1);
                }
              }
                else
              {
                extra[extra_count++] = 0;
              }
            }
              else
            {
              if (operands[i].use_long == 0)
              {
                offset = operands[i].value - (asm_context->address + 2);
                opcode |= (offset >> 1) & 0xff;
              }
                else
              {
                offset = operands[i].value - (asm_context->address + 4);

                if (offset < -65536 || offset > 0xffff)
                {
                  print_error_range("Displacement", -65536, 0xffff, asm_context);
                  return -1;
                }

                offset = offset >> 1;
                extra[extra_count++] = offset & 0xffff;
              }
            }

            break;
          default:
            ignore = 1;
            break;
        }

        if (ignore == 1) { break; }
      }

      if (i == table_tms340[n].operand_count)
      {
        add_bin16(asm_context, opcode, IS_OPCODE);

        for (n = 0; n < extra_count; n++)
        {
          add_bin16(asm_context, extra[n], IS_OPCODE);
        }

        return count;
      }
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

