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

#include "asm/arc.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/arc.h"

#define MAX_OPERANDS 3

#define COMPUTE_B(a) (((a & 0x7) << 24) | (((a >> 3) & 0x7) << 12))

#define REG_GP 26
#define REG_FP 27
#define REG_SP 28
#define REG_BLINK 31
#define REG_LP_COUNT 60
#define REG_LONG_IMMEDIATE 62

enum
{
  OPERAND_REG,
  OPERAND_NUMBER,
};

struct _operand
{
  int value;
  int type;
};

struct _condition_codes
{
  uint8_t code;
  char *name;
};

static struct _condition_codes condition_codes[] =
{
  { 0x00, "al" },
  { 0x00, "ra" },
  { 0x01, "eq" },
  { 0x01, "z" },
  { 0x02, "ne" },
  { 0x02, "nz" },
  { 0x03, "pl" },
  { 0x03, "p" },
  { 0x04, "mi" },
  { 0x04, "n" },
  { 0x05, "cs" },
  { 0x05, "c" },
  { 0x05, "lo" },
  { 0x06, "cc" },
  { 0x06, "nc" },
  { 0x06, "hs" },
  { 0x07, "vs" },
  { 0x07, "v" },
  { 0x08, "vc" },
  { 0x08, "nv" },
  { 0x09, "gt" },
  { 0x0a, "ge" },
  { 0x0b, "lt" },
  { 0x0c, "le" },
  { 0x0d, "hi" },
  { 0x0e, "ls" },
  { 0x0f, "pnz" },
};

#if 0
static int compute_s12(int s12)
{
  int data;

  s12 = s12 & 0xfff;

  data = ((s12 & 0x3f) << 6) | ((s12 >> 6) & 0x3f);

  return data;
}
#endif

static void add_bin(struct _asm_context *asm_context, uint32_t opcode, int flags)
{
  add_bin16(asm_context, opcode >> 16, flags);
  add_bin16(asm_context, opcode & 0xffff, flags);
}

static int get_register_arc(char *token)
{
  if (token[0] == 'r' || token[0] == 'R')
  {
    const char *s = token + 1;
    int n = 0, count = 0;

    while (*s != 0)
    {
      if (*s < '0' || *s > '9') { return -1; }
      n = (n * 10) + (*s - '0');
      count++;

      // Disallow leading 0's on registers.
      if (n == 0 && count >1) { return -1; }

      s++;
    }

    // This token was just r or R.
    if (count == 0) { return -1; }

    return n;
  }

  if (strcasecmp(token, "gp") == 0) { return 26; }
  if (strcasecmp(token, "fp") == 0) { return 27; }
  if (strcasecmp(token, "sp") == 0) { return 28; }
  if (strcasecmp(token, "ilink1") == 0) { return 29; }
  if (strcasecmp(token, "ilink2") == 0) { return 30; }
  if (strcasecmp(token, "blink") == 0) { return 31; }
  if (strcasecmp(token, "lp_count") == 0) { return 60; }
  if (strcasecmp(token, "pcl") == 0) { return 63; }

  return -1;
}

#if 0
static int map_16bit_reg(int r32)
{
  // Page 40 of the ARCompact Instruction Set Architecture... looks
  // like for 16 bit instructions, only 8 of the ARC's registers can
  // be used and they aren't in numerical order.
  if (r32 >= 0 && r32 <= 3) { return r32; }
  if (r32 >= 12 && r32 <= 15) { return r32 - 8; }

  return -1;
}
#endif

int parse_instruction_arc(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  int found = 0;
  uint32_t opcode;
  uint8_t f_flag = 0;
  uint8_t cc_flag = 0;
  uint8_t cc_value = 0;
  //int d_flag = 0;
  //int a, b, c, h;
  int a, b, c;
  //int offset;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (operand_count == 0 && IS_TOKEN(token, '.'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, 'f') || IS_TOKEN(token, 'F'))
      {
        f_flag = 1;
        continue;
      }

#if 0
      if (IS_TOKEN(token, 'd') || IS_TOKEN(token, 'D'))
      {
        d_flag = 1;
        continue;
      }
#endif

      int len = sizeof(condition_codes) / sizeof(struct _condition_codes);

      for (n = 0; n < len; n++)
      {
        if (strcasecmp(token, condition_codes[n].name) == 0)
        {
          cc_flag = 1;
          cc_value = condition_codes[n].code;
          break;
        }
      }

      if (n < len) { continue; }

      print_error_unexp(token, asm_context);
      return -1;
    }

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      if (operand_count != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      break;
    }

    num = get_register_arc(token);

    if (num != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_REG;
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
          print_error_illegal_expression(instr, asm_context);
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
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#if 0
for (n = 0; n < operand_count; n++)
{
  printf("%d) type=%d value=%d\n", n, operands[n].type, operands[n].value);
}
#endif

  // ALU instructions.
  if (operand_count == 3)
  {
    for (n = 0; table_arc_alu[n].instr != NULL; n++)
    {
      if (strcasecmp(instr_case, table_arc_alu[n].instr) != 0) { continue; }

      opcode = 0x20000000 | (table_arc_alu[n].opcode << 16) | (f_flag << 15);
      found = 1;
      a = operands[0].value;
      b = operands[1].value;
      c = operands[2].value;

      if (operands[0].type == OPERAND_REG &&
          operands[1].type == OPERAND_REG &&
          operands[2].type == OPERAND_REG &&
          cc_flag == 0)
      {
        opcode |= ((b & 7) << 24) | ((b >> 3) << 12) | (c << 6) | a;

        add_bin(asm_context, opcode, IS_OPCODE);

        return 4;
      }

      if (operands[0].type == OPERAND_REG &&
          operands[1].type == OPERAND_REG &&
          operands[2].type == OPERAND_REG &&
          operands[0].value == operands[1].value &&
          cc_flag == 1)
      {
        opcode |= ((b & 7) << 24) | ((b >> 3) << 12) | (c << 6) | cc_value;

        add_bin(asm_context, opcode, IS_OPCODE);

        return 4;
      }
    }
  }

  if (found == 1)
  {
    print_error_illegal_operands(instr, asm_context);
    return -1;
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}


