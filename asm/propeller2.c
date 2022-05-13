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
#include "asm/propeller.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/propeller2.h"

enum
{
  OPERAND_NUMBER,
  OPERAND_IMMEDIATE,
  OPERAND_ABSOLUTE_ADDRESS,
  OPERAND_CONDITION_CZ,
  OPERAND_P,
  OPERAND_PTR,
};

struct _operand
{
  int type;
  int value;
  uint8_t aug;
};

union _flags
{
  uint8_t has_flag;

  struct
  {
    uint8_t wc    : 1;
    uint8_t wz    : 1;
    uint8_t wcz   : 1;
    uint8_t logic : 2;
  };
};

struct _conditions
{
  const char *name;
  uint8_t value;
};

static struct _conditions conditions[] =
{
  { "_ret_",        0x0 },
  { "if_nc_and_nz", 0x1 },
  { "if_nz_and_nc", 0x1 },
  { "if_a",         0x1 },
  { "if_00",        0x1 },
  { "if_nc_and_z",  0x2 },
  { "if_z_and_nc",  0x2 },
  { "if_01",        0x2 },
  { "if_nc",        0x3 },
  { "if_ae",        0x3 },
  { "if_0x",        0x3 },
  { "if_c_and_nz",  0x4 },
  { "if_nz_and_c",  0x4 },
  { "if_10",        0x4 },
  { "if_nz",        0x5 },
  { "if_ne",        0x5 },
  { "if_x0",        0x5 },
  { "if_c_ne_z",    0x6 },
  { "if_z_ne_c",    0x6 },
  { "if_diff",      0x6 },
  { "if_nc_or_nz",  0x7 },
  { "if_nz_or_nc",  0x7 },
  { "if_not_11",    0x7 },
  { "if_c_and_z",   0x8 },
  { "if_z_and_c",   0x8 },
  { "if_11",        0x8 },
  { "if_c_eq_z",    0x9 },
  { "if_z_eq_c",    0x9 },
  { "if_same",      0x9 },
  { "if_z",         0xa },
  { "if_e",         0xa },
  { "if_x1",        0xa },
  { "if_nc_or_z",   0xb },
  { "if_z_or_nc",   0xb },
  { "if_not_10",    0xb },
  { "if_c",         0xc },
  { "if_b",         0xc },
  { "if_1x",        0xc },
  { "if_c_or_nz",   0xd },
  { "if_nz_or_c",   0xd },
  { "if_not_01",    0xd },
  { "if_c_or_z",    0xe },
  { "if_z_or_c",    0xe },
  { "if_be",        0xe },
  { "if_not_00",    0xe },
};

static struct _conditions conditions_cz[] =
{
  { "_clr",       0x0 },
  { "_nc_and_nz", 0x1 },
  { "_nz_and_nc", 0x1 },
  { "_gt",        0x1 },
  { "_nc_and_z",  0x2 },
  { "_z_and_nc",  0x2 },
  { "_nc",        0x3 },
  { "_ge",        0x3 },
  { "_c_and_nz",  0x4 },
  { "_nz_and_c",  0x4 },
  { "_nz",        0x5 },
  { "_ne",        0x5 },
  { "_c_ne_z",    0x6 },
  { "_z_ne_c",    0x6 },
  { "_nc_or_nz",  0x7 },
  { "_nz_or_nc",  0x7 },
  { "_c_and_z",   0x8 },
  { "_z_and_c",   0x8 },
  { "_c_eq_z",    0x9 },
  { "_z_eq_c",    0x9 },
  { "_z",         0xa },
  { "_e",         0xa },
  { "_nc_or_z",   0xb },
  { "_z_or_nc",   0xb },
  { "_c",         0xc },
  { "_lt",        0xc },
  { "_c_or_nz",   0xd },
  { "_nz_or_c",   0xd },
  { "_c_or_z",    0xe },
  { "_z_or_c",    0xe },
  { "_le",        0xe },
  { "_set",       0xf },
};

static int lookup_flag(const char *token, union _flags *flags)
{
  if (strcasecmp(token, "wc") == 0)
  {
    flags->wc = 1;
  }
    else
  if (strcasecmp(token, "wz") == 0)
  {
    flags->wz = 1;
  }
    else
  if (strcasecmp(token, "wcz") == 0)
  {
    flags->wcz = 1;
  }
    else
  if (strcasecmp(token, "andc") == 0)
  {
    flags->wc = 1;
    flags->logic = LOGIC_AND;
  }
    else
  if (strcasecmp(token, "andz") == 0)
  {
    flags->wz = 1;
    flags->logic = LOGIC_AND;
  }
    else
  if (strcasecmp(token, "orc") == 0)
  {
    flags->wc = 1;
    flags->logic = LOGIC_OR;
  }
    else
  if (strcasecmp(token, "orz") == 0)
  {
    flags->wz = 1;
    flags->logic = LOGIC_OR;
  }
    else
  if (strcasecmp(token, "xorc") == 0)
  {
    flags->wc = 1;
    flags->logic = LOGIC_XOR;
  }
    else
  if (strcasecmp(token, "xorz") == 0)
  {
    flags->wz = 1;
    flags->logic = LOGIC_XOR;
  }

  return flags->has_flag;
}

static int get_condition_cz(const char *token)
{
  int n;
  const int len = sizeof(conditions_cz) / sizeof(struct _conditions);

  for (n = 0; n < len; n++)
  {
    if (strcasecmp(token, conditions_cz[n].name) == 0)
    {
      return conditions_cz[n].value;
    }
  }

  return -1;
}

static int get_aug(struct _asm_context *asm_context, struct _operand *operands)
{
  int token_type;
  char token[TOKENLEN];

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_NOT_TOKEN(token, '#'))
  {
    tokens_push(asm_context, token, token_type);
  }
    else
  {
    if (expect_token(asm_context, '#') != 0) { return -2; }

    operands->aug = 1;
  }

  return 0;
}

static int get_p(
  struct _asm_context *asm_context,
  struct _operand *operands,
  const char *s,
  const char *instr)
{
  int token_type;
  char token[TOKENLEN];
  int value = 0;
  int n;

  if (strcasecmp(s, "pa") == 0)
  {
    operands->type = OPERAND_P;
    operands->value = 0;
    return 0;
  }

  if (strcasecmp(s, "pb") == 0)
  {
    operands->type = OPERAND_P;
    operands->value = 1;
    return 0;
  }

  if (strcasecmp(s, "ptra") == 0)
  {
    value = 2;
  }
    else
  if (strcasecmp(s, "ptrb") == 0)
  {
    value = 3;
  }
    else
  {
    return -1;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_NOT_TOKEN(token, '+') &&
      IS_NOT_TOKEN(token, '-') &&
      IS_NOT_TOKEN(token, '['))
  {
    tokens_push(asm_context, token, token_type);
    operands->type = OPERAND_P;
    operands->value = value;
    return 0;
  }

  value = value << 7;

  if (IS_TOKEN(token, '+'))
  {
    if (expect_token(asm_context, '+') != 0) { return -2; }

    value |= 0x60;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_NOT_TOKEN(token, '['))
    {
      tokens_push(asm_context, token, token_type);
      operands->type = OPERAND_PTR;
      operands->value = value | 0x01;
      return 0;
    }

    if (get_aug(asm_context, operands) != 0) { return -2; }

    if (eval_expression(asm_context, &n) != 0)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression(instr, asm_context);
        return -2;
      }

      ignore_operand(asm_context);
      n = 1;
    }

    if (operands->aug == 0)
    {
      if (check_range(asm_context, "Index", n, 1, 16) == -1) { return -2; }
      if (n == 16) { n = 0; }
      value |= n & 0xf;
    }
      else
    {
      value = value << 15;
      value |= n & 0xfffff;
    }

    operands->type = OPERAND_PTR;
    operands->value = value;

    if (expect_token(asm_context, ']') != 0) { return -2; }
    return 0;
  }
    else
  if (IS_TOKEN(token, '-'))
  {
    if (expect_token(asm_context, '-') != 0) { return -2; }

    value |= 0x60;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_NOT_TOKEN(token, '['))
    {
      tokens_push(asm_context, token, token_type);
      operands->type = OPERAND_PTR;
      operands->value = value | 0x1f;
      return 0;
    }

    if (get_aug(asm_context, operands) != 0) { return -2; }

    if (eval_expression(asm_context, &n) != 0)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression(instr, asm_context);
        return -2;
      }

      ignore_operand(asm_context);
      n = -1;
    }

    if (operands->aug == 0)
    {
      if (check_range(asm_context, "Index", n, 1, 16) == -1) { return -2; }
      n = -n;
      value |= n & 0x1f;
    }
      else
    {
      n = -n;
      value = value << 15;
      value |= n & 0x0fffff;
    }

    operands->type = OPERAND_PTR;
    operands->value = value;

    if (expect_token(asm_context, ']') != 0) { return -2; }
    return 0;
  }
    else
  if (IS_TOKEN(token, '['))
  {
    if (get_aug(asm_context, operands) != 0) { return -2; }

    if (eval_expression(asm_context, &n) != 0)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression(instr, asm_context);
        return -2;
      }

      ignore_operand(asm_context);
      n = 0;
    }

    if (operands->aug == 0)
    {
      if (check_range(asm_context, "Index", n, -32, 31) == -1) { return -2; }
      value |= n & 0x3f;
    }
      else
    {
      value = value << 15;
      value |= n & 0xffffff;
    }

    operands->type = OPERAND_PTR;
    operands->value = value;

    if (expect_token(asm_context, ']') != 0) { return -2; }
    return 0;
  }

  return -2;
}

static int get_ptr(struct _asm_context *asm_context, const char *token)
{
  if (strcasecmp(token, "ptra") == 0)
  {
    return 0x100;
  }
   else
  if (strcasecmp(token, "ptrb") == 0)
  {
    return 0x180;
  }

  print_error_unexp(token, asm_context);
  return -2;
}

static int get_inc_dec_p(
  struct _asm_context *asm_context,
  struct _operand *operands,
  const char *s,
  const char *instr)
{
  int token_type;
  char token[TOKENLEN];
  int value = 0;
  int n;

  if (IS_TOKEN(s, '+'))
  {
    if (expect_token(asm_context, '+') != 0) { return -2; }

    token_type = tokens_get(asm_context, token, TOKENLEN);
    value = get_ptr(asm_context, token);

    if (value == -2) { return -2; }
    value |= 0x40;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_NOT_TOKEN(token, '['))
    {
      tokens_push(asm_context, token, token_type);
      operands->type = OPERAND_PTR;
      operands->value = value | 0x01;
      return 0;
    }

    if (get_aug(asm_context, operands) != 0) { return -2; }

    if (eval_expression(asm_context, &n) != 0)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression(instr, asm_context);
        return -2;
      }

      ignore_operand(asm_context);
      n = 1;
    }

    if (operands->aug == 0)
    {
      if (check_range(asm_context, "Index", n, 1, 16) == -1) { return -2; }
      if (n == 16) { n = 0; }
      value |= n & 0xf;
    }
      else
    {
      value = value << 15;
      value |= n & 0xfffff;
    }

    operands->type = OPERAND_PTR;
    operands->value = value;

    if (expect_token(asm_context, ']') != 0) { return -2; }
    return 0;
  }
    else
  if (IS_TOKEN(s, '-'))
  {
    if (expect_token(asm_context, '-') != 0) { return -2; }

    token_type = tokens_get(asm_context, token, TOKENLEN);
    value = get_ptr(asm_context, token);

    if (value == -2) { return -2; }
    value |= 0x40;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_NOT_TOKEN(token, '['))
    {
      tokens_push(asm_context, token, token_type);
      operands->type = OPERAND_PTR;
      operands->value = value | 0x1f;
      return 0;
    }

    if (get_aug(asm_context, operands) != 0) { return -2; }

    if (eval_expression(asm_context, &n) != 0)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression(instr, asm_context);
        return -2;
      }

      ignore_operand(asm_context);
      n = -1;
    }

    if (operands->aug == 0)
    {
      if (check_range(asm_context, "Index", n, 1, 16) == -1) { return -2; }
      n = -n;
      value |= n & 0x1f;
    }
      else
    {
      n = -n;
      value = value << 15;
      value |= n & 0x0fffff;
    }

    operands->type = OPERAND_PTR;
    operands->value = value;

    if (expect_token(asm_context, ']') != 0) { return -2; }
    return 0;
  }

  return -2;
}

static int get_register(const char *token)
{
  int n;

  for (n = 0; n < registers_propeller2_len; n++)
  {
    if (strcasecmp(token, registers_propeller2[n].name) == 0)
    {
      return registers_propeller2[n].value;
    }
  }

  return -1;
}

int parse_instruction_propeller2(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  struct _operand operands[3];
  int operand_count;
  uint32_t opcode, opcode_aug;
  int n, i, r;
  int8_t cond = 0xf;
  union _flags flags;
  int found = 0;

  lower_copy(instr_case, instr);

  memset(&flags, 0, sizeof(flags));
  memset(operands, 0, sizeof(operands));
  operand_count = 0;

  const int len = sizeof(conditions) / sizeof(struct _conditions);

  for (n = 0; n < len; n++)
  {
    if (strcmp(instr_case, conditions[n].name) == 0)
    {
      token_type = tokens_get(asm_context, instr, TOKENLEN);
      lower_copy(instr_case, instr);

      cond = conditions[n].value;
      break;
    }
  }

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (flags.has_flag != 0)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (lookup_flag(token, &flags) != 0) { continue; }

    if (IS_TOKEN(token, '+') || IS_TOKEN(token, '-'))
    {
      r = get_inc_dec_p(asm_context, &operands[operand_count], token, instr);

      if (r != 0) { return -1; }
    }
      else
    if ((r = get_condition_cz(token)) != -1)
    {
      operands[operand_count].type = OPERAND_CONDITION_CZ;
      operands[operand_count].value = r;
    }
      else
    if ((r = get_p(asm_context, &operands[operand_count], token, instr)) != -1)
    {
      if (r == -2) { return -1; }
    }
      else
    if ((r = get_register(token)) != -1)
    {
      operands[operand_count].type = OPERAND_NUMBER;
      operands[operand_count].value = r;
    }
      else
    {
      if (token_type == TOKEN_POUND)
      {
        operands[operand_count].type = OPERAND_IMMEDIATE;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (token_type == TOKEN_POUND)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          operands[operand_count].aug = 1;
        }

        if (IS_TOKEN(token, '\\'))
        {
          operands[operand_count].type = OPERAND_ABSOLUTE_ADDRESS;
        }
          else
        {
          tokens_push(asm_context, token, token_type);
        }
      }
        else
      {
        operands[operand_count].type = OPERAND_NUMBER;
        tokens_push(asm_context, token, token_type);
      }

      if (eval_expression(asm_context, &operands[operand_count].value) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        ignore_operand(asm_context);
        operands[operand_count].value = 0;
      }
    }

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (IS_NOT_TOKEN(token, ','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#if 0
printf("%s operand_count=%d\n", instr, operand_count);
for (n = 0; n < operand_count; n++)
{
  printf("%d) type=%d value=%d\n", n, operands[n].type, operands[n].value);
}
#endif

  for (n = 0; table_propeller2[n].instr != NULL; n++)
  {
    if (strcmp(instr_case, table_propeller2[n].instr) == 0)
    {
      found = 1;

      if (operand_count != table_propeller2[n].operand_count) { continue; }
      if (flags.logic != table_propeller2[n].logic) { continue; }

      if ((flags.wc == 1 && table_propeller2[n].wc == 0) ||
          (flags.wz == 1 && table_propeller2[n].wz == 0) ||
          (flags.wcz == 1 && table_propeller2[n].wcz == 0))
      {
        continue;
      }

      opcode = table_propeller2[n].opcode;
      opcode_aug = 0;

      int no_match = 0;

      for (i = 0; i < operand_count; i++)
      {
        switch (table_propeller2[n].operands[i])
        {
          case OP_D:
            if (operands[i].type == OPERAND_P)
            {
              r = 0x1f6 + operands[i].value;
            }
              else
            if (operands[i].type == OPERAND_NUMBER)
            {
              r =  operands[i].value;
            }
              else
            {
              no_match = 1;
              break;
            }

            if (check_range(asm_context, "Register", r, 0, 511) == -1) { return -1; }
            opcode |= r << 9;

            if (i == 0 && table_propeller2[n].operands[1] == OP_D)
            {
              opcode |= operands[i].value;
            }

            break;
          case OP_NUM_D:
            if (operands[i].type == OPERAND_NUMBER)
            {
              if (check_range(asm_context, "Register", operands[i].value, 0, 511) == -1) { return -1; }
            }
              else
            if (operands[i].type == OPERAND_IMMEDIATE)
            {
              if (operands[i].aug == 1)
              {
                opcode_aug =
                  0xff800000 | ((operands[i].value >> 9) & 0x007fffff);
              }
                else
              {
                if (check_range(asm_context, "Immediate", operands[i].value, -256, 511) == -1) { return -1; }
              }

              if (i == 0 &&
                 (table_propeller2[n].operands[1] == OP_BRANCH ||
                  table_propeller2[n].operands[1] == OP_NUM_S ||
                  table_propeller2[n].operands[1] == OP_NUM_SP))
              {
                opcode |= (1 << 19);
              }
                else
              {
                opcode |= (1 << 18);
              }
            }
              else
            {
              print_error_unknown_operand_combo(instr, asm_context);
              return -1;
            }

            opcode |= (operands[i].value & 0x1ff) << 9;
            break;
          case OP_NUM_S:
            if (operands[i].type == OPERAND_NUMBER)
            {
              if (check_range(asm_context, "Register", operands[i].value, 0, 511) == -1) { return -1; }
            }
              else
            if (operands[i].type == OPERAND_P)
            {
              opcode |= operands[i].value + 0x1f6;
              break;
            }
              else
            if (operands[i].type == OPERAND_IMMEDIATE)
            {
              if (operands[i].aug == 1)
              {
                opcode_aug =
                  0xff000000 | ((operands[i].value >> 9) & 0x007fffff);
              }
                else
              {
                if (check_range(asm_context, "Immediate", operands[i].value, -256, 511) == -1) { return -1; }
              }

              opcode |= 1 << 18;
            }
              else
            {
              print_error_unknown_operand_combo(instr, asm_context);
              return -1;
            }

            opcode |= operands[i].value & 0x1ff;
            break;
          case OP_NUM_SP:
            if (operands[i].type == OPERAND_NUMBER)
            {
              if (check_range(asm_context, "Register", operands[i].value, 0, 511) == -1) { return -1; }
            }
              else
            if (operands[i].type == OPERAND_IMMEDIATE)
            {
              if (operands[i].aug == 1)
              {
                opcode_aug =
                  0xff000000 | ((operands[i].value >> 9) & 0x007fffff);
              }
                else
              {
                if (check_range(asm_context, "Immediate", operands[i].value, 0, 0xff) == -1) { return -1; }
              }

              opcode |= 1 << 18;
            }
              else
            if (operands[i].type == OPERAND_P)
            {
              opcode |= 1 << 18;

              if (operands[i].value == 2)
              {
                opcode |= 0x100;
                break;
              }
                else
              if (operands[i].value == 3)
              {
                opcode |= 0x180;
                break;
              }
                else
              {
                no_match = 1;
                break;
              }
            }
              else
            if (operands[i].type == OPERAND_PTR)
            {
              if (operands[i].aug == 1)
              {
                opcode_aug =
                  0xff000000 | ((operands[i].value >> 9) & 0x007fffff);
              }

              opcode |= 1 << 18;
            }
              else
            {
              print_error_unknown_operand_combo(instr, asm_context);
              return -1;
            }

            opcode |= operands[i].value & 0x1ff;
            break;
          case OP_N_1:
          case OP_N_2:
          case OP_N_3:
            if (operands[i].type != OPERAND_IMMEDIATE)
            {
              print_error_unknown_operand_combo(instr, asm_context);
              return -1;
            }

            r = 1;

            if (table_propeller2[n].operands[i] == OP_N_2) { r = 3; }
              else
            if (table_propeller2[n].operands[i] == OP_N_3) { r = 7; }

            if (check_range(asm_context, "Immediate", operands[i].value, 0, r) == -1) { return -1; }

            opcode |= operands[i].value << 19;

            break;
          case OP_N_23:
            if (operands[i].type != OPERAND_IMMEDIATE)
            {
              print_error_unknown_operand_combo(instr, asm_context);
              return -1;
            }

            {
              uint32_t value = operands[i].value;
              value = value >> 9;
              opcode |= value;
            }

            break;
          case OP_A:
            if (operands[i].type == OPERAND_IMMEDIATE)
            {
              int a = operands[i].value >= 0x400 ?
                 operands[i].value : operands[i].value * 4;
              int offset = a - (asm_context->address + 4);
              opcode |= (1 << 20) | (offset & 0xfffff);
            }
              else
            if (operands[i].type == OPERAND_ABSOLUTE_ADDRESS)
            {
              opcode |= operands[i].value;
            }
              else
            {
              print_error_unknown_operand_combo(instr, asm_context);
              return -1;
            }

            break;
          case OP_BRANCH:
            if (operands[i].type == OPERAND_NUMBER)
            {
              opcode |= operands[i].value;
            }
              else
            if (operands[i].type == OPERAND_IMMEDIATE)
            {
              int offset = operands[i].value - ((asm_context->address / 4) + 1);

              if (offset < -256 || offset > 255)
              {
                if (i == 1 && operands[0].type == OPERAND_P)
                {
                  no_match = 1;
                  break;
                }

                print_error_range("Offset", -256, 255, asm_context);
                return -1;
              }

              opcode |= (1 << 18) | (offset & 0x1ff);
            }
              else
            if (operands[i].type == OPERAND_ABSOLUTE_ADDRESS)
            {
              no_match = 1;
              break;
            }
              else
            {
              print_error_unknown_operand_combo(instr, asm_context);
              return -1;
            }

            break;
          case OP_P:
            opcode |= operands[i].value << 21;
            break;
          case OP_C:
            if (operands[i].type == OPERAND_NUMBER)
            {
              if (check_range(asm_context, "c", operands[i].value, 0, 15) == -1) { return -1; }
            }
              else
            if (operands[i].type != OPERAND_CONDITION_CZ)
            {
              print_error_unknown_operand_combo(instr, asm_context);
              return -1;
            }

            opcode |= operands[i].value << 13;

            break;
          case OP_Z:
            if (operands[i].type == OPERAND_NUMBER)
            {
              if (check_range(asm_context, "z", operands[i].value, 0, 15) == -1) { return -1; }
            }
              else
            if (operands[i].type != OPERAND_CONDITION_CZ)
            {
              print_error_unknown_operand_combo(instr, asm_context);
              return -1;
            }

            opcode |= operands[i].value << 9;

            break;
          default:
            no_match = 1;
            break;
        }
      }

      if (i != operand_count) { continue; }
      if (no_match == 1) { continue; }

      if (flags.wz == 1)  { opcode |= 0x00080000; }
      if (flags.wc == 1)  { opcode |= 0x00100000; }
      if (flags.wcz == 1) { opcode |= 0x00180000; }

      if ((table_propeller2[n].mask >> 28) == 0)
      {
        opcode |= cond << 28;
      }

      if (opcode_aug != 0)
      {
        add_bin32(asm_context, opcode_aug, IS_OPCODE);
      }
        else
      {
        for (i = 0; i < operand_count; i++)
        {
          if (operands[i].aug != 0)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }
        }
      }

      add_bin32(asm_context, opcode, IS_OPCODE);

      return 4;
    }
  }

  if (found == 1)
  {
    print_error_illegal_operands(instr, asm_context);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

