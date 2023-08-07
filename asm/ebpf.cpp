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

#include "asm/ebpf.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/ebpf.h"

#define MAX_OPERANDS 3

enum
{
  OPERAND_REG,
  OPERAND_NUMBER,
  OPERAND_PLUS_NUMBER,
  OPERAND_REG_OFFSET,
};

struct _operand
{
  int value;
  int offset;
  int type;
};

static int get_register_ebpf(const char *token)
{
  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  token++;

  if (token[0] != 0 && token[1] == 0)
  {
    if (*token < '0' || *token > '7') { return -1; }

    return (*token) - '0';
  }

  if (memcmp(token, "10", 3) == 0) { return 10; }

  return -1;
}

static int write_intruction(
  AsmContext *asm_context,
  uint8_t opcode,
  uint8_t src_reg,
  uint8_t dst_reg,
  int16_t offset,
  int32_t imm)
{
  int reg;

  if (asm_context->memory.endian == ENDIAN_LITTLE)
  {
    reg = (src_reg << 4) | dst_reg;
  }
    else
  {
    reg = (dst_reg << 4) | src_reg;
  }

  add_bin8(asm_context, opcode, 0);
  add_bin8(asm_context, reg, 0);
  add_bin16(asm_context, offset, 0);
  add_bin32(asm_context, imm, 0);

  return 0;
}

int parse_instruction_ebpf(AsmContext *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  //int offset, value;
  int found = 0;

  lower_copy(instr_case, instr);
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

    num = get_register_ebpf(token);

    if (num != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_REG;
    }
      else
    if (IS_TOKEN(token,'+'))
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
          print_error_illegal_expression(asm_context, instr);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_PLUS_NUMBER;
      operands[operand_count].value = num;
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          num = 0;
        }
          else
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

    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token, ',') || operand_count == MAX_OPERANDS)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  for (n = 0; table_ebpf[n].instr != NULL; n++)
  {
    if (strcmp(table_ebpf[n].instr, instr_case) == 0)
    {
      found = 1;

      switch (table_ebpf[n].type)
      {
        case OP_NONE:
        {
          if (operand_count == 0)
          {
            write_intruction(
              asm_context,
              table_ebpf[n].opcode,
              0,
              0,
              0,
              0);

            return 8;
          }

          break;
        }
        case OP_IMM:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            write_intruction(
              asm_context,
              table_ebpf[n].opcode,
              0,
              0,
              0,
              operands[0].value);

            return 8;
          }

          break;
        }
        case OP_REG:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_REG)
          {
            write_intruction(
              asm_context,
              table_ebpf[n].opcode,
              0,
              operands[0].value,
              0,
              0);

            return 8;
          }

          break;
        }
        case OP_REG_REG:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG)
          {
            write_intruction(
              asm_context,
              table_ebpf[n].opcode,
              operands[1].value,
              operands[0].value,
              0,
              0);

            return 8;
          }

          break;
        }
        case OP_REG_IMM:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_NUMBER)
          {
            write_intruction(
              asm_context,
              table_ebpf[n].opcode,
              0,
              operands[0].value,
              0,
              operands[1].value);

            return 8;
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

  if (found == 1)
  {
    print_error_illegal_operands(asm_context, instr);
    return -1;
  }
    else
  {
    print_error_unknown_instr(asm_context, instr);
  }

  return -1;
}

