/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/xtensa.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/xtensa.h"

#define MAX_OPERANDS 5

enum
{
  OPERAND_NONE,
  OPERAND_REGISTER_AR,
  OPERAND_REGISTER_FR,
  OPERAND_REGISTER_BR,
  OPERAND_NUMBER,
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

    while(*s != 0)
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

#if 0
static int get_register_fr_xtensa(const char *token)
{
  if (token[0] == 'f' || token[0] == 'F')
  {
    const char *s = token + 1;
    int n = 0, count = 0;

    while(*s != 0)
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
#endif

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

  while(1)
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

      // Assume this is just a number
      operands[operand_count].type = OPERAND_NUMBER;

      if (asm_context->pass == 1)
      {
        eat_operand(asm_context);
        operands[operand_count].value = 0;
      }
      else
      {
        tokens_push(asm_context, token, token_type);

        if (eval_expression(asm_context, &n) != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].value = n;
      }

      break;
    } while(0);

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

  return operand_count;
}

int parse_instruction_xtensa(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count, n;
  uint32_t opcode;

  lower_copy(instr_case, instr);

  operand_count = get_operands(asm_context, operands, instr, instr_case);

  if (operand_count < 0) { return -1; }

  //token_type=tokens_get(asm_context, token, TOKENLEN);
  //tokens_push(asm_context, token, token_type);

  n = 0;

  while(table_xtensa[n].instr != NULL)
  {
    if (strcmp(table_xtensa[n].instr, instr_case) == 0)
    {
      switch(table_xtensa[n].type)
      {
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
        case XTENSA_OP_FR_FS:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_FR ||
              operands[1].type != OPERAND_REGISTER_FR)
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
          if (operand_count != 3 ||
              operands[0].type != OPERAND_REGISTER_FR ||
              operands[1].type != OPERAND_REGISTER_FR ||
              operands[2].type != OPERAND_REGISTER_FR)
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
        case XTENSA_OP_AR_AS_AT:
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
                    (operands[0].value << 8) |
                    (operands[1].value << 12) |
                    (operands[2].value << 16);
          }

          add_bin24(asm_context, opcode);

          return 3;
        case XTENSA_OP_AT_AS_I8:
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
        case XTENSA_OP_AT_AS_IM8:
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
        case XTENSA_OP_N_AR_AS_I4:
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
        case XTENSA_OP_BOOL4:
        case XTENSA_OP_BOOL8:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REGISTER_BR ||
              operands[1].type != OPERAND_REGISTER_BR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (table_xtensa[n].type == XTENSA_OP_BOOL4)
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

