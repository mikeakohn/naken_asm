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
#include "asm/xtensa.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/xtensa.h"

#define MAX_OPERANDS 4

enum
{
  OPERAND_NONE = 0,
  OPERAND_REGISTER_AR = 1,
  OPERAND_REGISTER_FR = 2,
  OPERAND_REGISTER_BR = 3,
  OPERAND_REGISTER_MW = 4,
  OPERAND_NUMBER,
  OPERAND_REGISTER_SAR,
};

struct _operand
{
  int value;
  int type;
  //int16_t offset;
};

static int get_register_xtensa(const char *token, char lo, char up)
{
  if (token[0] == lo || token[0] == up)
  {
    const char *s = token + 1;
    int n = 0, count = 0;

    while (*s != 0)
    {
      if (*s < '0' || *s > '9') { return -1; }
      n = (n * 10) + (*s - '0');
      count++;

      // Disallow leading 0's on registers.
      if (n == 0 && count > 1) { return -1; }

      s++;
    }

    // This token was just a or A.
    if (count == 0) { return -1; }

    // Valid registers are a0 to a15.
    if (n > 15) { return -1; }

    return n;
  }

  return -1;
}

static void compute_offset(struct _asm_context *asm_context, int *offset, int n, int increment)
{
  if (asm_context->pass == 1)
  {
    *offset = 0;
  }
    else
  {
    *offset = n - (asm_context->address + increment);
  }
}

static void get_b4const(struct _asm_context *asm_context, int *c, int value)
{
  int n;

  if (asm_context->pass == 1)
  {
    *c = 0;
  }
    else
  {
    for (n = 0; n < 16; n++)
    {
      if (value == xtensa_b4const[n]) { break; }
    }

    *c = n;
  }
}

static int check_immediate(struct _asm_context *asm_context, int *immediate, int shift)
{
  if (asm_context->pass == 1)
  {
    *immediate = 0;
    return 0;
  }
    else
  {
    int lo = 0;
    int hi;

    switch (shift)
    {
      case 0: hi = 255; break;
      case 1: hi = 510; break;
      case 2: hi = 1020; break;
      case -4: hi = -4; lo = -64; break;
      default: hi = 0; break;
    }

    if (*immediate < lo || *immediate > hi)
    {
      print_error_range("Immediate", lo, hi, asm_context);
      return -1;
    }
  }

  if (shift == -4)
  {
    if ((*immediate & 0x3) != 0)
    {
      printf("Error: Immediate must be a multiple of %d at %s:%d\n",
        1 << shift,
        asm_context->tokens.filename,
        asm_context->tokens.line);
      return -1;
    }

    *immediate = (*immediate >> 2) & 0xf;

    return 0;
  }

  int mask = (1 << shift) - 1;

  if ((*immediate & mask) != 0)
  {
    printf("Error: Immediate must be a multiple of %d at %s:%d\n",
      1 << shift,
      asm_context->tokens.filename,
      asm_context->tokens.line);
    return -1;
  }

  *immediate = (*immediate >> shift) & 0xff;

  return 0;
}

static int check_immediate_n(struct _asm_context *asm_context, int *immediate, int shift)
{
  if (asm_context->pass == 1)
  {
    *immediate = 0;
    return 0;
  }
    else
  {
    int hi;

    switch (shift)
    {
      case 0: hi = 15; break;
      case 1: hi = 30; break;
      case 2: hi = 60; break;
      default: hi = 0; break;
    }

    if (*immediate < 0 || *immediate > hi)
    {
      print_error_range("Immediate", 0, hi, asm_context);
      return -1;
    }
  }

  int mask = (1 << shift) - 1;

  if ((*immediate & mask) != 0)
  {
    printf("Error: Immediate must be a multiple of %d at %s:%d\n",
      1 << shift,
      asm_context->tokens.filename,
      asm_context->tokens.line);
    return -1;
  }

  *immediate = (*immediate >> shift) & 0xf;

  return 0;
}

// Not sure if this would be okay in other assemblers, so leaving here
// for now.
static void add_bin24(struct _asm_context *asm_context, uint32_t b)
{
  int line = asm_context->tokens.line;

  if (asm_context->pass == 1 && asm_context->pass_1_write_disable == 1)
  {
    asm_context->address += 3;
    return;
  }

  if (asm_context->memory.endian == ENDIAN_LITTLE)
  {
    memory_write_inc(asm_context, b & 0xff, line);
    memory_write_inc(asm_context, (b >> 8) & 0xff, line);
    memory_write_inc(asm_context, (b >> 16) & 0xff, line);
  }
    else
  {
    memory_write_inc(asm_context, (b >> 16) & 0xff, line);
    memory_write_inc(asm_context, (b >> 8) & 0xff, line);
    memory_write_inc(asm_context, b & 0xff, line);
  }
}

static int get_operands(struct _asm_context *asm_context, struct _operand *operands, char *instr, char *instr_case)
{
  char token[TOKENLEN];
  int token_type;
  int operand_count = 0;
  int n;

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
      n = get_register_xtensa(token, 'a', 'A');

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_REGISTER_AR;
        operands[operand_count].value = n;
        break;
      }

      n = get_register_xtensa(token, 'f', 'F');

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_REGISTER_FR;
        operands[operand_count].value = n;
        break;
      }

      n = get_register_xtensa(token, 'b', 'B');

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_REGISTER_BR;
        operands[operand_count].value = n;
        break;
      }

      n = get_register_xtensa(token, 'm', 'M');

      if (n >= 0 && n <= 3)
      {
        operands[operand_count].type = OPERAND_REGISTER_MW;
        operands[operand_count].value = n;
        break;
      }

      if (strcasecmp(token, "sar") == 0)
      {
        operands[operand_count].type = OPERAND_REGISTER_SAR;
        operands[operand_count].value = 3;
        break;
      }

      // Assume this is just a number
      operands[operand_count].type = OPERAND_NUMBER;

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

      break;
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

  if (strncmp(instr_case, "rsr.", 4) == 0 ||
      strncmp(instr_case, "rur.", 4) == 0 ||
      strncmp(instr_case, "wsr.", 4) == 0 ||
      strncmp(instr_case, "wur.", 4) == 0 ||
      strncmp(instr_case, "xsr.", 4) == 0)
  {
    if (strcasecmp(instr_case + 4, "sar") == 0)
    {
      instr_case[3] = 0;
      operands[operand_count].type = OPERAND_REGISTER_SAR;
      operands[operand_count].value = 3;
      operand_count++;
    }
  }

  return operand_count;
}

int parse_instruction_xtensa(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count, offset, immediate, shift;
  int n, i, r, s, t;
  uint32_t opcode;

  lower_copy(instr_case, instr);

  operand_count = get_operands(asm_context, operands, instr, instr_case);

  if (operand_count < 0) { return -1; }

  //token_type=tokens_get(asm_context, token, TOKENLEN);
  //tokens_push(asm_context, token, token_type);

  n = 0;

  while (table_xtensa[n].instr != NULL)
  {
    if (strcmp(table_xtensa[n].instr, instr_case) == 0)
    {
      switch (table_xtensa[n].type)
      {
        case XTENSA_OP_NONE:
          if (operand_count != 0)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le;
          }
            else
          {
            opcode = table_xtensa[n].opcode_be;
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_N_NONE:
          if (operand_count != 0)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le;
          }
            else
          {
            opcode = table_xtensa[n].opcode_be;
          }

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case XTENSA_OP_AR_AT:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AT_AS:
          if (operand_count != 2 ||
              operands[0].type != mask_xtensa[table_xtensa[n].type].reg_0 ||
              operands[1].type != mask_xtensa[table_xtensa[n].type].reg_1)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) |
                    (operands[1].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) |
                    (operands[1].value << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_FR_FS:
        case XTENSA_OP_AR_FS:
        case XTENSA_OP_AR_AS:
        case XTENSA_OP_FR_AS:
          if (operand_count != 2 ||
              operands[0].type != mask_xtensa[table_xtensa[n].type].reg_0 ||
              operands[1].type != mask_xtensa[table_xtensa[n].type].reg_1)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_FR_FS_FT:
        case XTENSA_OP_AR_AS_AT:
        case XTENSA_OP_BR_BS_BT:
        case XTENSA_OP_FR_AS_AT:
        case XTENSA_OP_FR_FS_AT:
        case XTENSA_OP_AR_AS_BT:
        case XTENSA_OP_FR_FS_BT:
        case XTENSA_OP_BR_FS_FT:
          if (operand_count != 3 ||
              operands[0].type != mask_xtensa[table_xtensa[n].type].reg_0 ||
              operands[1].type != mask_xtensa[table_xtensa[n].type].reg_1 ||
              operands[2].type != mask_xtensa[table_xtensa[n].type].reg_2)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8) |
                    (operands[2].value << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 12) |
                    (operands[2].value << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AT_AS_N128_127:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < -128 || operands[2].value > 127)
          {
            print_error_range("Constant", -128, 127, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) |
                    (operands[1].value << 8) |
                   ((operands[2].value & 0xff) << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) |
                    (operands[1].value << 12) |
                    (operands[2].value & 0xff);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AT_AS_N32768_32512:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < -32768 || operands[2].value > 32512)
          {
            print_error_range("Constant", -32768, 32512, asm_context);
            return -1;
          }

          if ((operands[2].value & 0xff) != 0)
          {
            printf("Error: Constant cannot be shifted by 8 at %s:%d\n",
              asm_context->tokens.filename, asm_context->tokens.line);
            return -1;
          }

          operands[2].value >>= 8;

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) |
                    (operands[1].value << 8) |
                   ((operands[2].value & 0xff) << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) |
                    (operands[1].value << 12) |
                    (operands[2].value & 0xff);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_N_AR_AS_AT:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_REGISTER_AR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8) |
                    (operands[2].value << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                     operands[0].value |
                    (operands[1].value << 4) |
                    (operands[2].value << 8);
          }

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case XTENSA_OP_N_AR_AS_N1_15:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1) { operands[2].value = 1; }

          if (operands[2].value < -1 ||
              operands[2].value > 15 ||
              operands[2].value == 0)
          {
            print_error_range("Constant", -1, 15, asm_context);
            return -1;
          }

          if (operands[2].value == -1) { operands[2].value = 0; }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8) |
                   ((operands[2].value & 0xff) << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                     operands[0].value |
                    (operands[1].value << 4) |
                   ((operands[2].value & 0xff) << 8);
          }

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case XTENSA_OP_BT_BS4:
        case XTENSA_OP_BT_BS8:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_BR ||
              operands[1].type != OPERAND_REGISTER_BR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (table_xtensa[n].type == XTENSA_OP_BT_BS4)
          {
            if ((operands[1].value & 0x3) != 0)
            {
              printf("Error: Source register must be {b0,b4,b8,b12} at %s:%d\n",
                asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }
          }
            else
          {
            if ((operands[1].value & 0x7) != 0)
            {
              printf("Error: Source register must be {b0,b8} at %s:%d\n",
                asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) |
                    (operands[1].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) |
                    (operands[1].value << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_BRANCH_AS_AT_I8:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          compute_offset(asm_context, &offset, operands[2].value, 4);

          if (offset < -128 || offset > 127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) |
                    (operands[1].value << 4) |
                   ((offset & 0xff) << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) |
                    (operands[1].value << 16) |
                    (offset & 0xff);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_BRANCH_AS_B5_I8:
        case XTENSA_OP_BRANCH_AS_B5_I8_L:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          compute_offset(asm_context, &offset, operands[2].value, 4);

          if (offset < -128 || offset > 127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }

          if (operands[1].value < 0 || operands[1].value > 31)
          {
            print_error_range("Constant", 0, 31, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE ||
              table_xtensa[n].type == XTENSA_OP_BRANCH_AS_B5_I8_L)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) |
                   ((operands[1].value & 0xf) << 4) |
                  (((operands[1].value >> 4) & 0x1) << 12) |
                   ((offset & 0xff) << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) |
                   ((operands[1].value & 0xf) << 16) |
                  (((operands[1].value >> 4) & 0x1) << 8) |
                    (offset & 0xff);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_BRANCH_AS_C4_I8:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          compute_offset(asm_context, &offset, operands[2].value, 4);
          get_b4const(asm_context, &i, operands[1].value);

          if (offset < -128 || offset > 127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }

          if (i == 16)
          {
            printf("Error: Constant must be { -1, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 32, 64, 128, 256} at %s:%d\n",
              asm_context->tokens.filename, asm_context->tokens.line);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) |
                    (i << 12) |
                   ((offset & 0xff) << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) |
                    (i << 8) |
                    (offset & 0xff);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_BRANCH_AS_I12:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          compute_offset(asm_context, &offset, operands[1].value, 4);

          if (offset < -2048 || offset > 2047)
          {
            print_error_range("Offset", -2048, 2047, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) |
                   ((offset & 0xfff) << 12);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) |
                    (offset & 0xfff);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_BRANCH_N_AS_I6:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          compute_offset(asm_context, &offset, operands[1].value, 4);

          if (offset < 0 || offset > 63)
          {
            print_error_range("Offset", 0, 63, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) |
                   ((offset & 0xf) << 12) |
                   (((offset >> 4) & 0x3) << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) |
                    (offset & 0xf) |
                  (((offset >> 4) & 0x3) << 8);
          }

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case XTENSA_OP_BRANCH_BS_I8:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_BR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          compute_offset(asm_context, &offset, operands[1].value, 4);

          if (offset < -128 || offset > 127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) | ((offset & 0xff) << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) | (offset & 0xff);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_NUM_NUM:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 15 ||
              operands[1].value < 0 || operands[1].value > 15)
          {
            print_error_range("Constant", 0, 15, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) | (operands[1].value << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) | (operands[1].value << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_N_NUM:
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 15)
          {
            print_error_range("Constant", 0, 15, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le | (operands[0].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be | (operands[0].value << 4);
          }

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case XTENSA_OP_CALL_I18:
        case XTENSA_OP_JUMP_I18:
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          i = table_xtensa[n].type == XTENSA_OP_CALL_I18 ? 1 : 4;

          compute_offset(asm_context, &offset, operands[0].value, i);

          if (offset < -131072 || offset > 131071)
          {
            print_error_range("Offset", -131072, 131071, asm_context);
            return -1;
          }

          offset = offset & 0x3ffff;

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le | (offset << 6);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be | offset;
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AS:
          if (operand_count != 1 || operands[0].type != OPERAND_REGISTER_AR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le | (operands[0].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be | (operands[0].value << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AR_FS_0_15:
        case XTENSA_OP_FR_AS_0_15:
          if (operand_count != 3 ||
              operands[0].type != mask_xtensa[table_xtensa[n].type].reg_0 ||
              operands[1].type != mask_xtensa[table_xtensa[n].type].reg_1 ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < 0 || operands[2].value > 15)
          {
            print_error_range("Constant", 0, 15, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8) |
                    (operands[2].value << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 12) |
                    (operands[2].value << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AR_AS_7_22:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 2)
          {
            if (operands[2].value < 7 || operands[2].value > 22)
            {
              print_error_range("Constant", 7, 22, asm_context);
              return -1;
            }
          }

          operands[2].value -= 7;

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8) |
                    (operands[2].value << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 12) |
                    (operands[2].value << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AS_0_1020:
        case XTENSA_OP_AS_0_240:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            immediate = 0;
          }
            else
          {
            immediate = operands[1].value;

            int hi = table_xtensa[n].type == XTENSA_OP_AS_0_1020 ? 1020 : 240;

            if (immediate < 0 || immediate > hi)
            {
              print_error_range("Immediate", 0, hi, asm_context);
              return -1;
            }
          }

          if (table_xtensa[n].type == XTENSA_OP_AS_0_1020)
          {
            if ((immediate & 0x3) != 0)
            {
              printf("Error: Immediate must be a multiple of 4 at %s:%d\n",
                asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }

            immediate = immediate >> 2;
          }
            else
          {
            if ((immediate & 0x3) != 0)
            {
              printf("Error: Immediate must be a multiple of 16 at %s:%d\n",
                asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }

            if (asm_context->memory.endian == ENDIAN_BIG)
            {
              immediate = immediate >> 4;
            }
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) | (immediate << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) | immediate;
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AS_0_32760:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            immediate = 0;
          }
            else
          {
            immediate = operands[1].value;

            if ((immediate & 0x7) != 0)
            {
              printf("Error: Immediate must be a multiple of 4 at %s:%d\n",
                asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }

            if (immediate < 0 || immediate > 32760)
            {
              print_error_range("Immediate", 0, 32760, asm_context);
              return -1;
            }

            immediate = (immediate >> 3) & 0xfff;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) | (immediate << 12);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) | immediate;
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AR_AT_SHIFT_MASK:
          if (operand_count != 4 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER ||
              operands[3].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < 0 || operands[2].value > 31)
          {
            print_error_range("Shift", 0, 31, asm_context);
            return -1;
          }

          if (asm_context->pass == 1) { operands[3].value = 1; }

          if (operands[3].value < 1 || operands[3].value > 16)
          {
            print_error_range("Mask", 1, 16, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 4) |
                   ((operands[2].value & 0xf) << 8) |
                  (((operands[2].value >> 4) & 0x1) << 16) |
                   ((operands[3].value - 1) << 20);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 16) |
                   ((operands[2].value & 0xf) << 12) |
                  (((operands[2].value >> 4) & 0x1) << 4) |
                    (operands[3].value - 1);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AT_AS_0_255:
        case XTENSA_OP_AT_AS_0_510:
        case XTENSA_OP_AT_AS_0_1020:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          switch (table_xtensa[n].type)
          {
            case XTENSA_OP_AT_AS_0_255: shift = 0; break;
            case XTENSA_OP_AT_AS_0_510: shift = 1; break;
            case XTENSA_OP_AT_AS_0_1020: shift = 2; break;
            default: shift = 0; break;
          }

          immediate = operands[2].value;

          if (check_immediate(asm_context, &immediate, shift) != 0)
          {
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) |
                    (operands[1].value << 8) |
                    (immediate << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) |
                    (operands[1].value << 12) |
                     immediate;
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AT_AS_N64_N4:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          immediate = operands[2].value;

          if (check_immediate(asm_context, &immediate, -4) != 0)
          {
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) |
                    (operands[1].value << 8) |
                    (immediate << 12);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) |
                    (operands[1].value << 12) |
                    (immediate << 8);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_N_AT_AS_0_60:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          immediate = operands[2].value;

          if (check_immediate_n(asm_context, &immediate, 2) != 0)
          {
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) |
                    (operands[1].value << 8) |
                    (immediate << 12);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 4) |
                     immediate;
          }

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case XTENSA_OP_AT_I16:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          offset = operands[1].value - (asm_context->address + 3);

          if (asm_context->pass == 1) { offset = -4; }

          if (offset < -262141 || offset > -4)
          {
            print_error_range("Offset", 0, 15, asm_context);
            return -1;
          }

          if ((offset & 0x3) != 0)
          {
            printf("Error: Offset must be a multiple of 4 at %s:%d\n",
              asm_context->tokens.filename,
              asm_context->tokens.line);
            return -1;
          }

          offset = (offset >> 2) & 0xffff;

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) | (offset << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) | offset;
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_MW_AS:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_MW ||
              operands[1].type != OPERAND_REGISTER_AR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_LOOP_AS_LABEL:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          offset = operands[1].value - (asm_context->address + 4);

          if (asm_context->pass == 1) { offset = 0; }

          if (offset < 0 || offset > 255)
          {
            print_error_range("Offset", 0, 255, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) | (offset << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 12) | offset;
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_FT_AS_0_1020:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_FR ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          immediate = operands[2].value;

          if (check_immediate(asm_context, &immediate, 2) != 0)
          {
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) |
                    (operands[1].value << 8) |
                    (immediate << 16);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) |
                    (operands[1].value << 12) |
                     immediate;
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AR_AS_AS:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8) |
                    (operands[1].value << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 12) |
                    (operands[1].value << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_N_AT_AS:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_REGISTER_AR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) | (operands[1].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) | (operands[1].value << 4);
          }

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case XTENSA_OP_AT_N2048_2047:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < -2048 || operands[1].value > 2047)
          {
            print_error_range("Immediate", 0, 255, asm_context);
            return -1;
          }

          operands[1].value &= 0xfff;

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) |
                   ((operands[1].value & 0xff) << 16) |
                   ((operands[1].value >> 8) << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) |
                    (operands[1].value & 0xff) |
                   ((operands[1].value >> 8) << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_N_AS_N2048_2047:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < -32 || operands[1].value > 95)
          {
            print_error_range("Immediate", -32, 95, asm_context);
            return -1;
          }

          operands[1].value &= 0xfff;

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 8) |
                   ((operands[1].value & 0xf) << 12) |
                  (((operands[1].value >> 4) & 0x7) << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 4) |
                    (operands[1].value & 0xf) |
                  (((operands[1].value >> 4) & 0x7) << 8);
          }

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case XTENSA_OP_AS_AT:
        case XTENSA_OP_AS_MY:
        case XTENSA_OP_MX_AT:
        case XTENSA_OP_MX_MY:
          if (operand_count != 2 ||
              operands[0].type != mask_xtensa[table_xtensa[n].type].reg_0 ||
              operands[1].type != mask_xtensa[table_xtensa[n].type].reg_1)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          r = 0;
          s = 0;
          t = 0;

          if (operands[0].type == OPERAND_REGISTER_MW)
          {
            if (operands[0].value > 1)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            r = operands[0].value << 2;
          }

          if (operands[1].type == OPERAND_REGISTER_MW)
          {
            if (operands[1].value < 2)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            t = operands[1].value != 0 ? 4 : 0;
          }

          if (operands[0].type == OPERAND_REGISTER_AR) { s = operands[0].value; }
          if (operands[1].type == OPERAND_REGISTER_AR) { t = operands[1].value; }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (r << 12) | (s << 8) | (t << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (r << 8) | (s << 12) | (t << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_MW_AS_MX_AT:
          if (operand_count != 4 ||
              operands[0].type != OPERAND_REGISTER_MW ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_REGISTER_MW ||
              operands[3].type != OPERAND_REGISTER_AR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value > 1)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8) |
                    (operands[2].value << 14) |
                    (operands[3].value << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 12) |
                    (operands[2].value << 10) |
                    (operands[3].value << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_MW_AS_MX_MY:
          if (operand_count != 4 ||
              operands[0].type != OPERAND_REGISTER_MW ||
              operands[1].type != OPERAND_REGISTER_AR ||
              operands[2].type != OPERAND_REGISTER_MW ||
              operands[3].type != OPERAND_REGISTER_MW)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value > 1)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[3].value < 2)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 8) |
                    (operands[2].value << 14) |
                   ((operands[3].value - 2) << 6);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 12) |
                    (operands[2].value << 10) |
                   ((operands[3].value - 2) << 18);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_0_15:
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 15)
          {
            print_error_range("Constant", 0, 15, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le | (operands[0].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be | (operands[0].value << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_N8_7:
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < -8 || operands[0].value > 7)
          {
            print_error_range("Constant", -8, 7, asm_context);
            return -1;
          }

          immediate = operands[0].value & 0xf;

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le | (immediate << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be | (immediate << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AT_0_15:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < 0 || operands[1].value > 15)
          {
            print_error_range("Constant", 0, 15, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) | (operands[1].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) | (operands[1].value << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AT_SPR:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
             (operands[1].type != OPERAND_NUMBER &&
              operands[1].type != OPERAND_REGISTER_SAR))
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < 0 || operands[1].value > 255)
          {
            print_error_range("SPR", 0, 255, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 4) | (operands[1].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 16) | (operands[1].value << 8);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AR_UR:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_AR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < 0 || operands[1].value > 255)
          {
            print_error_range("User register", 0, 255, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) | (operands[1].value << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) | (operands[1].value << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AR_AS_1_31:
        case XTENSA_OP_AR_AT_1_31:
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_AR ||
             (operands[1].type != OPERAND_REGISTER_AR &&
              operands[2].type != OPERAND_NUMBER))
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1) { operands[2].value = 1; }

          if (operands[2].value < 1 || operands[2].value > 31)
          {
            print_error_range("Immediate", 1, 31, asm_context);
            return -1;
          }

          if (table_xtensa[n].type == XTENSA_OP_AR_AS_1_31)
          {
            operands[2].value = (32 - operands[2].value) & 0x1f;
            s = operands[1].value;
            t = operands[2].value & 0xf;
          }
            else
          {
            s = operands[2].value & 0xf;
            t = operands[1].value;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) | (s << 8) | (t << 4) |
                   ((operands[2].value >> 4) << 20);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) | (s << 12) | (t << 16) |
                    (operands[2].value >> 4);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AR_AT_0_15:
          if (operand_count != 3 ||
              operands[0].type != mask_xtensa[table_xtensa[n].type].reg_0 ||
              operands[1].type != mask_xtensa[table_xtensa[n].type].reg_1 ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < 0 || operands[2].value > 15)
          {
            print_error_range("Constant", 0, 15, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                    (operands[0].value << 12) |
                    (operands[1].value << 4) |
                    (operands[2].value << 8);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                    (operands[0].value << 8) |
                    (operands[1].value << 16) |
                    (operands[2].value << 12);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_0_31:
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 31)
          {
            print_error_range("Immediate", 0, 31, asm_context);
            return -1;
          }

          if (asm_context->memory.endian == ENDIAN_LITTLE)
          {
            opcode = table_xtensa[n].opcode_le |
                   ((operands[0].value & 0xf) << 8) |
                   ((operands[0].value >> 4) << 4);
          }
            else
          {
            opcode = table_xtensa[n].opcode_be |
                   ((operands[0].value & 0xf) << 12) |
                   ((operands[0].value >> 4) << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        default:
          print_error_internal(asm_context, __FILE__, __LINE__);
          return -1;
      }

      break;
    }

    n++;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

