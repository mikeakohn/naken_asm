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
#include <ctype.h>

#include "asm/common.h"
#include "asm/mips.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "common/eval_expression_ex.h"
#include "common/imports_obj.h"
#include "table/mips.h"

enum
{
  OPERAND_TREG,
  OPERAND_FREG,
  OPERAND_WREG,
  OPERAND_VIREG,
  OPERAND_VFREG,
  OPERAND_IMMEDIATE,
  OPERAND_IMMEDIATE_RS,
  OPERAND_I,
  OPERAND_Q,
  OPERAND_P,
  OPERAND_R,
  OPERAND_ACC,
  OPERAND_OFFSET_VBASE,
  OPERAND_OFFSET_VBASE_DEC,
  OPERAND_OFFSET_VBASE_INC,
  OPERAND_RSP_VREG,
};

enum
{
  RSP_ELEMENT_VECTOR = 0x0,
  RSP_ELEMENT_QUARTER = 0x2,
  RSP_ELEMENT_HALF = 0x4,
  RSP_ELEMENT_WHOLE = 0x8,
};

struct _rsp_element
{
  uint8_t index;
  uint8_t type;
};

struct _operand
{
  int value;
  int type;
  int reg2;
  struct _rsp_element rsp_element;
  int field_mask;
};

static int get_number(char *s)
{
  int n = 0;

  while (*s != 0)
  {
    if (*s < '0' || *s > '9') { return -1; }
    n = (n * 10) + (*s - '0');
    s++;
  }

  return n;
}

static int get_field_number(int field_mask)
{
  uint8_t value[16] =
  {
    -1,  3,  2, -1,  // 0
     1, -1, -1, -1,  // 4
     0, -1, -1, -1,  // 8
    -1, -1, -1, -1,  // 12
  };

  return value[field_mask];
}

static int get_tregister(char *token)
{
  int num;

  if (token[1] >= '0' && token[1] <= '9')
  {
    num = get_number(token + 1);

    if (num >= 0 && num <= 31)
    {
      return num;
    }

    return -1;
  }

  num = get_number(token + 2);

  if (token[2] == 0) { num = -1; }

  if (token[1] == 'v' && num >= 0 && num <= 1) { return 2 + num; }
  if (token[1] == 'a' && num >= 0 && num <= 3) { return 4 + num; }
  if (token[1] == 't' && num >= 0 && num <= 7) { return 8 + num; }
  if (token[1] == 's' && num >= 0 && num <= 7) { return 16 + num; }
  if (token[1] == 't' && num >= 8 && num <= 9) { return 24 + (num - 8); }
  if (token[1] == 'k' && num >= 0 && num <= 1) { return 26 + num; }
  if (token[1] == 's' && num == 8) { return 30; }

  if (strcasecmp(token, "$zero") == 0) { return 0; }
  if (strcasecmp(token, "$at") == 0) { return 1; }
  if (strcasecmp(token, "$gp") == 0) { return 28; }
  if (strcasecmp(token, "$sp") == 0) { return 29; }
  if (strcasecmp(token, "$s8") == 0) { return 30; }
  if (strcasecmp(token, "$fp") == 0) { return 30; }
  if (strcasecmp(token, "$ra") == 0) { return 31; }

  return -1;
}

static int get_register_mips_rsp(
  struct _asm_context *asm_context,
  char *token,
  struct _rsp_element *rsp_element)
{
  int token_type;
  char token_1[TOKENLEN];
  int num = 0;
  int e = 0;

  if (token[0] != '$') { return -1; }
  if (token[1] != 'v') { return -1; }

  num = get_number(token + 2);
  if (num < 0 || num > 31) { return -1; }

  token_type = tokens_get(asm_context, token_1, TOKENLEN);

  if (IS_NOT_TOKEN(token_1, '['))
  {
    tokens_push(asm_context, token_1, token_type);

    if (num >= 2) { return num; }

    return -1;
  }

  token_type = tokens_get(asm_context, token_1, TOKENLEN);

  uint8_t type = RSP_ELEMENT_WHOLE;
  int l = strlen(token_1);

  if (token_1[l - 1] == 'q')
  {
    type = RSP_ELEMENT_QUARTER;
    token_1[l - 1] = 0;
  }
    else
  if (token_1[l - 1] == 'h')
  {
    type = RSP_ELEMENT_HALF;
    token_1[l - 1] = 0;
  }

  e = get_number(token_1);

  if (e < 0 || e > 15)
  {
    print_error_range("Vector element", 0, 15, asm_context);
    return -2;
  }

  if (expect_token(asm_context, ']') == -1) { return -2; }

  rsp_element->index = e;
  rsp_element->type = type;

  return num;
}

static int check_element(
  struct _asm_context *asm_context,
  int element,
  int element_max,
  int element_step)
{
  if (element < 0 || element > element_max)
  {
    printf("Warning: Vector element %d out of range (%d, %d) at %s:%d.\n",
      element,
      0,
      element_max,
      asm_context->tokens.filename,
      asm_context->tokens.line);
  }

  if (element_step != 0)
  {
    if ((element % element_step) != 0)
    {
      print_warning("Invalid vector element", asm_context);
    }
  }

  return 0;
}

static int get_register_mips(char *token, struct _operand *operand)
{
  int num;

  if (token[0] != '$')
  {
    if (strcasecmp(token, "zero") == 0)
    {
      operand->type = OPERAND_VIREG;
      operand->value = 0;
      return 0;
    }

    return -1;
  }

  if (token[1] == 0) { return -1; }

  num = get_tregister(token);

  if (num != -1)
  {
    operand->type = OPERAND_TREG;
    operand->value = num;
    return num;
  }

  if (token[1] == 'v')
  {
    if (token[2] == 'i')
    {
      operand->type = OPERAND_VIREG;
    }
      else
    if (token[2] == 'f')
    {
      operand->type = OPERAND_VFREG;
    }
      else
    {
      return -1;
    }

    if (token[3] == 0) { return -1; }

    num = 0;
    token = token + 3;

    while (*token != 0)
    {
      if (*token < '0' || *token > '9') { break; }
      num = (num * 10) + (*token - '0');
      token++;
    }

    while (*token != 0)
    {
      if (*token == 'x') { operand->field_mask |= 0x8; }
      else if (*token == 'y') { operand->field_mask |= 0x4; }
      else if (*token == 'z') { operand->field_mask |= 0x2; }
      else if (*token == 'w') { operand->field_mask |= 0x1; }
      else { return -1; }
      token++;
    }

    if (num >= 0 && num <= 31)
    {
      operand->value = num;
      return num;
    }

    return -1;
  }

  if (token[1] == 'f' || token[1] == 'w')
  {
    if (token[1] == 'f') { operand->type = OPERAND_FREG; }
    else { operand->type = OPERAND_WREG; }

    num = get_number(token + 2);

    if (num >= 0 && num <= 31)
    {
      operand->value = num;
      return num;
    }

    return -1;
  }

  return -1;
}

static int add_offset(
  struct _asm_context *asm_context,
  int address,
  uint32_t *opcode)
{
  int32_t offset = address - (asm_context->address + 4);

  if (offset < -(1 << 17) || offset > (1 << 17) - 1)
  {
    print_error_range("Offset", -(1 << 17), (1 << 17) - 1, asm_context);
    return -1;
  }

  if ((offset & 0x3) != 0)
  {
    print_error_align(asm_context, 4);
    return -1;
  }

  *opcode |= (offset >> 2) & 0xffff;

  return 0;
}

static uint32_t find_opcode(const char *instr_case)
{
  int n;

  for (n = 0; mips_i_table[n].instr != NULL; n++)
  {
    if (strcmp(instr_case, mips_i_table[n].instr) == 0)
    {
      return mips_i_table[n].function << 26;
    }
  }

  return 0xffffffff;
}

void get_dest(char *instr_case, int *dest)
{
  char *period = NULL;

  while (*instr_case != 0)
  {
    if (*instr_case == '.')
    {
      period = instr_case;
      break;
    }

    instr_case++;
  }

  if (period == NULL) { return; }

  instr_case++;
  while (*instr_case != 0)
  {
    if (*instr_case == 'x') { *dest |= 8; }
    else if (*instr_case == 'y') { *dest |= 4; }
    else if (*instr_case == 'z') { *dest |= 2; }
    else if (*instr_case == 'w') { *dest |= 1; }
    else
    {
      *dest = 0;
      return;
    }

    instr_case++;
  }

  *period = 0;
}

static int check_type(
  struct _asm_context *asm_context,
  char *instr,
  int user_type,
  int table_type,
  int value)
{
  if (table_type == MIPS_OP_RD ||
      table_type == MIPS_OP_RS ||
      table_type == MIPS_OP_RT)
  {
    if (user_type != OPERAND_TREG)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }
  }
    else
  if (table_type == MIPS_OP_LABEL)
  {
    if (user_type != OPERAND_IMMEDIATE)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }
  }
    else
  if (table_type == MIPS_OP_IMMEDIATE)
  {
    if (user_type != OPERAND_IMMEDIATE)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (value < 0 || value > 0xffff)
    {
      print_error_range("Constant", 0, 0xffff, asm_context);
      return -1;
    }
  }
    else
  if (table_type == MIPS_OP_IMMEDIATE_SIGNED)
  {
    if (user_type != OPERAND_IMMEDIATE)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (value < -32768 || value > 32767)
    {
      print_error_range("Constant", -32768, 32767, asm_context);
      return -1;
    }
  }
    else
  if (table_type == MIPS_OP_SA)
  {
    if (user_type != OPERAND_IMMEDIATE)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (value < 0 || value > 31)
    {
      print_error_range("Constant", 0, 31, asm_context);
      return -1;
    }
  }
    else
  if (table_type == MIPS_OP_HINT)
  {
    if (user_type != OPERAND_IMMEDIATE)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (value < 0 || value > 31)
    {
      print_error_range("Constant", 0, 31, asm_context);
      return -1;
    }
  }
    else
  if (table_type == MIPS_OP_IMMEDIATE_RS)
  {
    if (user_type != OPERAND_IMMEDIATE_RS)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (value < -32768 || value > 32767)
    {
      print_error_range("Constant", -32768, 32767, asm_context);
      return -1;
    }
  }

  return 0;
}

static int check_for_pseudo_instruction(
  struct _asm_context *asm_context,
  struct _operand *operands,
  int *operand_count,
  char *instr_case,
  const char *instr)
{
  // Check pseudo-instructions
  if (strcmp(instr_case, "move") == 0 &&
      *operand_count == 2 &&
      operands[0].type == OPERAND_TREG &&
      operands[1].type == OPERAND_TREG)
  {
    strcpy(instr_case, "addu");
    operands[2].value = 0;
    operands[2].type = OPERAND_TREG;
    *operand_count = 3;
  }
    else
  if (strcmp(instr_case, "nop") == 0 && *operand_count == 0)
  {
    strcpy(instr_case, "sll");
    *operand_count = 3;
    operands[2].type = OPERAND_IMMEDIATE;
  }
    else
  if (strcmp(instr_case, "not") == 0 && *operand_count == 2)
  {
    strcpy(instr_case, "nor");
    *operand_count = 3;
    operands[2].type = OPERAND_TREG;
  }
    else
  if (strcmp(instr_case, "negu") == 0 &&
      *operand_count == 2 &&
      operands[0].type == OPERAND_TREG &&
      operands[1].type == OPERAND_TREG)
  {
    // negu rd, rs = subu rd, 0, rs
    strcpy(instr_case, "subu");
    memcpy(&operands[2], &operands[1], sizeof(struct _operand));
    operands[1].value = 0;
    operands[1].type = OPERAND_TREG;
    *operand_count = 3;
  }
    else
  if (strcmp(instr_case, "b") == 0 &&
      *operand_count == 1 &&
      operands[0].type == OPERAND_IMMEDIATE)
  {
    strcpy(instr_case, "beq");
    memcpy(&operands[2], &operands[0], sizeof(struct _operand));
    operands[0].value = 0;
    operands[0].type = OPERAND_TREG;
    operands[1].value = 0;
    operands[1].type = OPERAND_TREG;
    *operand_count = 3;
  }
    else
  if (strcmp(instr_case, "bal") == 0 &&
      *operand_count == 1 &&
      operands[0].type == OPERAND_IMMEDIATE)
  {
    strcpy(instr_case, "bgezal");
    memcpy(&operands[1], &operands[0], sizeof(struct _operand));
    operands[0].value = 0;
    operands[0].type = OPERAND_TREG;
    *operand_count = 2;
  }
    else
  if (strcmp(instr_case, "beqz") == 0 &&
      *operand_count == 2 &&
      operands[0].type == OPERAND_TREG &&
      operands[1].type == OPERAND_IMMEDIATE)
  {
    strcpy(instr_case, "beq");
    memcpy(&operands[2], &operands[1], sizeof(struct _operand));
    operands[1].value = 0;
    operands[1].type = OPERAND_TREG;
    *operand_count = 3;
  }
    else
  if (strcmp(instr_case, "bnez") == 0 &&
      *operand_count == 2 &&
      operands[0].type == OPERAND_TREG &&
      operands[1].type == OPERAND_IMMEDIATE)
  {
    strcpy(instr_case, "bne");
    memcpy(&operands[2], &operands[1], sizeof(struct _operand));
    operands[1].value = 0;
    operands[1].type = OPERAND_TREG;
    *operand_count = 3;
  }
#if 0
    else
  if (strcmp(instr_case, "bal") == 0 &&
      *operand_count == 1 &&
      operands[0].type == OPERAND_IMMEDIATE)
  {
    strcpy(instr_case, "bgezal");
    memcpy(&operands[1], &operands[0], sizeof(struct _operand));
    operands[0].value = 0;
    operands[0].type = OPERAND_TREG;
    *operand_count = 2;
  }
#endif

  return 4;
}

static int get_operands_li(
  struct _asm_context *asm_context,
  struct _operand *operands,
  char *instr,
  char *instr_case)
{
  int operand_count = 0;
  int force_long = 0;
  int num;
  int token_type;
  char token[TOKENLEN];
  struct _var var;
  uint64_t temp;

  // Get operands
  token_type = tokens_get(asm_context, token, TOKENLEN);
  if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  num = get_register_mips(token, &operands[operand_count]);
  if (num == -1)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  operands[0].type = OPERAND_TREG;
  operands[0].value = num;
  operand_count++;

  if (expect_token(asm_context, ',') == -1) { return -1; }

  if (eval_expression_ex(asm_context, &var) != 0)
  {
    if (asm_context->pass == 2)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    ignore_operand(asm_context);

    num = 0;
    force_long = 1;
    memory_write_m(&asm_context->memory, asm_context->address, force_long);
  }
    else
  {
    temp = var_get_int64(&var);
    uint64_t mask = temp & 0xffffffff00000000ULL;

    if (mask != 0xffffffff00000000ULL && mask != 0)
    {
      print_error_range("Constant", -0x80000000LL, 0xffffffff, asm_context);
      return -1;
    }

    num = temp;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  // If data size was unknown on pass 1, force_long.
  if (asm_context->pass == 2)
  {
    force_long = memory_read(asm_context, asm_context->address);
  }

  // Apply operands to memory.
  uint32_t opcode_lui = find_opcode("lui") | (operands[0].value << 16);
  uint32_t opcode_ori = find_opcode("ori") | (operands[0].value << 16);
  uint32_t opcode_addi = find_opcode("addiu") | (operands[0].value << 16);

  if (force_long == 1)
  {
    opcode_ori |= (operands[0].value << 21);
    add_bin32(asm_context, opcode_lui | ((num >> 16) & 0xffff), IS_OPCODE);
    add_bin32(asm_context, opcode_ori | (num & 0xffff), IS_OPCODE);
    return 8;
  }
    else
  if (num >= 0 && num <= 0xffff)
  {
    add_bin32(asm_context, opcode_ori | (num & 0xffff), IS_OPCODE);
    return 4;
  }
    else
  if ((num & 0xffff) == 0)
  {
    add_bin32(asm_context, opcode_lui | ((num >> 16) & 0xffff), IS_OPCODE);
    return 4;
  }
    else
  if (num >= -32768 && num <= -1)
  {
    add_bin32(asm_context, opcode_addi | (num & 0xffff), IS_OPCODE);
    return 4;
  }
    else
  {
    opcode_ori |= (operands[0].value << 21);
    add_bin32(asm_context, opcode_lui | ((num >> 16) & 0xffff), IS_OPCODE);
    add_bin32(asm_context, opcode_ori | (num & 0xffff), IS_OPCODE);
    return 8;
  }
}

static int get_operands(
  struct _asm_context *asm_context,
  struct _operand *operands,
  char *instr,
  char *instr_case)
{
  int operand_count = 0;
  int is_cache = 0;
  int paren_flag = 0;
  int n, num;
  int token_type;
  char token[TOKENLEN];
  int inc_dec_flag;

  if (strcmp("cache", instr_case) == 0) { is_cache = 1; }

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }

    if (operand_count == 0 && IS_TOKEN(token,'.'))
    {
      strcat(instr_case, ".");
      strcat(instr, ".");
      token_type = tokens_get(asm_context, token, TOKENLEN);
      strcat(instr, token);
      n = 0;
      while (token[n] != 0) { token[n] = tolower(token[n]); n++; }
      strcat(instr_case, token);
      continue;
    }

    do
    {
      if (is_cache == 1 && operand_count == 0)
      {
        int i = 0;

        while (mips_cache[i].name != NULL)
        {
          if (strcasecmp(token, mips_cache[i].name) == 0)
          {
            operands[operand_count].value = mips_cache[i].op;
            break;
          }

          i++;
        }

        if (mips_cache[i].name != NULL) { break; }
      }

      // MIPS SGI Reality Signal Processor vector registers.
      if ((asm_context->flags & MIPS_RSP) != 0)
      {
        num = get_register_mips_rsp(
          asm_context,
          token,
          &operands[operand_count].rsp_element);

        if (num == -2)
        {
          return -1;
        }
          else
        if (num != -1)
        {
           operands[operand_count].type = OPERAND_RSP_VREG;
           operands[operand_count].value = num;
           break;
        }
      }

      num = get_register_mips(token, &operands[operand_count]);

      if (num != -1)
      {
        break;
      }

      if (IS_TOKEN(token, 'I') || IS_TOKEN(token, 'i'))
      {
        operands[operand_count].type = OPERAND_I;
        break;
      }
        else
      if (IS_TOKEN(token, 'Q') || IS_TOKEN(token, 'q'))
      {
        operands[operand_count].type = OPERAND_Q;
        break;
      }
        else
      if (IS_TOKEN(token, 'P') || IS_TOKEN(token, 'p'))
      {
        operands[operand_count].type = OPERAND_P;
        break;
      }
        else
      if (IS_TOKEN(token, 'R') || IS_TOKEN(token, 'r'))
      {
        operands[operand_count].type = OPERAND_R;
        break;
      }
        else
      if (strcasecmp(token, "acc") == 0)
      {
        operands[operand_count].type = OPERAND_ACC;
        break;
      }

      paren_flag = 0;
      inc_dec_flag = 0;

      if (IS_TOKEN(token,'('))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);
        paren_flag = 1;

        if (IS_TOKEN(token, '-'))
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token, '-'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);
          inc_dec_flag = -1;
        }
      }

      num = get_register_mips(token, &operands[operand_count]);

      // dafuq?
      if (num != -1)
      {
        if (paren_flag == 0) { break; }
      }
        else
      if (paren_flag == 0)
      {
        num = get_register_mips(token, &operands[operand_count]);

        if (num != -1)
        {
          break;
        }
      }
        else
      {
        paren_flag = 0;

        tokens_push(asm_context, token, token_type);

        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
        }
          else
        {
          if (eval_expression(asm_context, &num) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].type = OPERAND_IMMEDIATE;
          operands[operand_count].value = num;

          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token,')'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }

        break;
      }

      if (paren_flag == 1)
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token, '+') &&
            operands[operand_count].type == OPERAND_VIREG)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token, '+'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);
          inc_dec_flag = 1;
        }

        if (IS_NOT_TOKEN(token,')'))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].reg2 = operands[operand_count].value;
        operands[operand_count].value = 0;

        if (operands[operand_count].type == OPERAND_TREG)
        {
          operands[operand_count].type = OPERAND_IMMEDIATE_RS;
        }
          else
        if (operands[operand_count].type == OPERAND_VIREG)
        {
          if (inc_dec_flag == 1)
          {
            operands[operand_count].type = OPERAND_OFFSET_VBASE_INC;
          }
            else
          if (inc_dec_flag == -1)
          {
            operands[operand_count].type = OPERAND_OFFSET_VBASE_DEC;
          }
            else
          {
            operands[operand_count].type = OPERAND_OFFSET_VBASE;
          }
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        break;
      }

      operands[operand_count].type = OPERAND_IMMEDIATE;

      if (asm_context->pass == 1)
      {
        ignore_operand(asm_context);
        break;
      }

      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value = num;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,'('))
      {
        int value = operands[operand_count].value;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        num = get_register_mips(token, &operands[operand_count]);

        if (num == -1)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].reg2 = num;
        operands[operand_count].value = value;

        if (operands[operand_count].type == OPERAND_TREG)
        {
          operands[operand_count].type = OPERAND_IMMEDIATE_RS;
        }
          else
        if (operands[operand_count].type == OPERAND_VIREG)
        {
          operands[operand_count].type = OPERAND_OFFSET_VBASE;
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_NOT_TOKEN(token,')'))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }

    } while (0);

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (operand_count == 4)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  return operand_count;
}

static int check_ee_instruction(
  struct _asm_context *asm_context,
  const char *instr,
  const char *instr_case,
  struct _operand *operands,
  int operand_count,
  int *found)
{
  int n, r;

  for (n = 0; mips_ee[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_ee[n].version & asm_context->flags) == 0)
    {
      continue;
    }

    if (strcmp(instr_case, mips_ee[n].instr) == 0)
    {
      *found = 1;

      if (operand_count != mips_ee[n].operand_count &&
          mips_ee[n].operand[0] != MIPS_OP_OPTIONAL)
      {
        continue;
      }

      uint32_t opcode = mips_ee[n].opcode;

      for (r = 0; r < mips_ee[n].operand_count; r++)
      {
        switch (mips_ee[n].operand[r])
        {
          case MIPS_OP_RS:
            if (operands[r].type != OPERAND_TREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 21;

            break;
          case MIPS_OP_RT:
            if (operands[r].type != OPERAND_TREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 16;

            break;
          case MIPS_OP_RD:
            if (operands[r].type != OPERAND_TREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 11;

            break;
          case MIPS_OP_FT:
            if (operands[r].type != OPERAND_FREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 16;

            break;
          case MIPS_OP_FS:
            if (operands[r].type != OPERAND_FREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 11;

            break;
          case MIPS_OP_FD:
            if (operands[r].type != OPERAND_FREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 6;

            break;
          case MIPS_OP_VIS:
            if (operands[r].type != OPERAND_VIREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 11;

            break;
          case MIPS_OP_VFT:
            if (operands[r].type != OPERAND_VFREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 16;

            break;
          case MIPS_OP_VFS:
            if (operands[r].type != OPERAND_VFREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 11;

            break;
          case MIPS_OP_SA:
            if (operands[r].type != OPERAND_IMMEDIATE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < 0 || operands[r].value > 31)
            {
              print_error_range("Constant", 0, 31, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 6;

            break;
          case MIPS_OP_IMMEDIATE_SIGNED:
            if (operands[r].type != OPERAND_IMMEDIATE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < -32768 || operands[r].value > 32767)
            {
              print_error_range("Constant", -32768, 32767, asm_context);
              return -1;
            }

            opcode |= operands[r].value & 0xffff;

            break;
          case MIPS_OP_IMMEDIATE_RS:
            if (operands[r].type != OPERAND_IMMEDIATE_RS)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < -32768 || operands[r].value > 32767)
            {
              print_error_range("Constant", -32768, 32767, asm_context);
              return -1;
            }

            opcode |= operands[r].value & 0xffff;
            opcode |= operands[r].reg2 << 21;

            break;
          case MIPS_OP_LABEL:
            if (operands[r].type != OPERAND_IMMEDIATE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (add_offset(asm_context, operands[r].value, &opcode) == -1)
            {
              return -1;
            }

            break;
          case MIPS_OP_PREG:
            if (operands[r].type != OPERAND_IMMEDIATE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < 0 || operands[r].value > 1)
            {
              print_error_range("Constant", 0, 1, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 1;
            break;
          case MIPS_OP_OPTIONAL:
            if (operand_count == 1)
            {
              if (operands[r].type != OPERAND_IMMEDIATE)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              if (operands[r].value < 0 || operands[r].value >= (1 << 20))
              {
                print_error_range("Constant", 0, (1 << 20) - 1, asm_context);
                return -1;
              }

              opcode |= operands[r].value << 6;
            }
            break;
          case MIPS_OP_ID_REG:
            if (operands[r].type == OPERAND_IMMEDIATE)
            {
              if (operands[r].value < 0 || operands[r].value > 0x1f)
              {
                print_error_range("Constant", 0, 0x1f, asm_context);
                return -1;
              }
            }
              else
            if (operands[r].type == OPERAND_R)
            {
              operands[r].value = 20;
            }
              else
            if (operands[r].type == OPERAND_I)
            {
              operands[r].value = 21;
            }
              else
            if (operands[r].type == OPERAND_Q)
            {
              operands[r].value = 22;
            }
              else
            if (operands[r].type == OPERAND_VIREG)
            {
            }

            opcode |= (operands[r].value & 0x1f) << 11;

            break;
          default:
            print_error_illegal_operands(instr, asm_context);
            return -1;
        }
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;
    }
  }

  return 0;
}

static int check_other_instruction(
  struct _asm_context *asm_context,
  const char *instr,
  const char *instr_case,
  struct _operand *operands,
  int operand_count,
  int *found)
{
  int n, r;

  for (n = 0; mips_other[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_other[n].version & asm_context->flags) == 0) { continue; }

    if (strcmp(instr_case, mips_other[n].instr) == 0)
    {
      *found = 1;

      if (operand_count != mips_other[n].operand_count &&
          mips_other[n].operand[0] != MIPS_OP_OPTIONAL)
      {
        continue;
      }

      uint32_t opcode = mips_other[n].opcode;

      for (r = 0; r < mips_other[n].operand_count; r++)
      {
        switch (mips_other[n].operand[r])
        {
          case MIPS_OP_RS:
            if (operands[r].type != OPERAND_TREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 21;

            break;
          case MIPS_OP_RT:
            if (operands[r].type != OPERAND_TREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 16;

            break;
          case MIPS_OP_RD:
            if (operands[r].type != OPERAND_TREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 11;

            break;
          case MIPS_OP_FT:
            if (operands[r].type != OPERAND_FREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 16;

            break;
          case MIPS_OP_FS:
            if (operands[r].type != OPERAND_FREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 11;

            break;
          case MIPS_OP_FD:
            if (operands[r].type != OPERAND_FREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 6;

            break;
          case MIPS_OP_VIS:
            if (operands[r].type != OPERAND_VIREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 11;

            break;
          case MIPS_OP_VFT:
            if (operands[r].type != OPERAND_VFREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 16;

            break;
          case MIPS_OP_VFS:
            if (operands[r].type != OPERAND_VFREG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 11;

            break;
          case MIPS_OP_SA:
            if (operands[r].type != OPERAND_IMMEDIATE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < 0 || operands[r].value > 31)
            {
              print_error_range("Constant", 0, 31, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 6;

            break;
          case MIPS_OP_IMMEDIATE_SIGNED:
            if (operands[r].type != OPERAND_IMMEDIATE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < -32768 || operands[r].value > 32767)
            {
              print_error_range("Constant", -32768, 32767, asm_context);
              return -1;
            }

            opcode |= operands[r].value & 0xffff;

            break;
          case MIPS_OP_IMMEDIATE_RS:
            if (operands[r].type != OPERAND_IMMEDIATE_RS)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < -32768 || operands[r].value > 32767)
            {
              print_error_range("Constant", -32768, 32767, asm_context);
              return -1;
            }

            opcode |= operands[r].value & 0xffff;
            opcode |= operands[r].reg2 << 21;

            break;
          case MIPS_OP_LABEL:
            if (operands[r].type != OPERAND_IMMEDIATE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (add_offset(asm_context, operands[r].value, &opcode) == -1)
            {
              return -1;
            }

            break;
          case MIPS_OP_PREG:
            if (operands[r].type != OPERAND_IMMEDIATE)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (operands[r].value < 0 || operands[r].value > 1)
            {
              print_error_range("Constant", 0, 1, asm_context);
              return -1;
            }

            opcode |= operands[r].value << 1;
            break;
          case MIPS_OP_OPTIONAL:
            if (operand_count == 1)
            {
              if (operands[r].type != OPERAND_IMMEDIATE)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              if (operands[r].value < 0 || operands[r].value >= (1 << 20))
              {
                print_error_range("Constant", 0, (1 << 20) - 1, asm_context);
                return -1;
              }

              opcode |= operands[r].value << 6;
            }
            break;
          case MIPS_OP_ID_REG:
            if (operands[r].type == OPERAND_IMMEDIATE)
            {
              if (operands[r].value < 0 || operands[r].value > 0x1f)
              {
                print_error_range("Constant", 0, 0x1f, asm_context);
                return -1;
              }
            }
              else
            if (operands[r].type == OPERAND_R)
            {
              operands[r].value = 20;
            }
              else
            if (operands[r].type == OPERAND_I)
            {
              operands[r].value = 21;
            }
              else
            if (operands[r].type == OPERAND_Q)
            {
              operands[r].value = 22;
            }
              else
            if (operands[r].type == OPERAND_VIREG)
            {
            }

            opcode |= (operands[r].value & 0x1f) << 11;

            break;
          default:
            print_error_illegal_operands(instr, asm_context);
            return -1;
        }
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return 4;
    }
  }

  return 0;
}

int parse_instruction_mips(struct _asm_context *asm_context, char *instr)
{
  struct _operand operands[4];
  int operand_count = 0;
  char instr_case[TOKENLEN];
  int n, r;
  uint32_t opcode;
  int opcode_size = 4;
  int found = 0;
  int32_t offset;

  lower_copy(instr_case, instr);
  memset(operands, 0, sizeof(operands));

  if (strcmp(instr_case, "li") == 0 || strcmp(instr_case, "la") == 0)
  {
    return get_operands_li(asm_context, operands, instr, instr_case);
  }
    else
  {
    operand_count = get_operands(asm_context, operands, instr, instr_case);
  }

  if (operand_count < 0) { return -1; }

  n = check_for_pseudo_instruction(
    asm_context,
    operands,
    &operand_count,
    instr_case,
    instr);

  if (n != 4)
  {
    return opcode_size;
  }

  if (asm_context->pass == 1)
  {
    add_bin32(asm_context, 0, IS_OPCODE);
    return opcode_size;
  }

  // R-Type Instruction [ op 6, rs 5, rt 5, rd 5, sa 5, function 6 ]
  for (n = 0; mips_r_table[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_r_table[n].version & asm_context->flags) == 0)
    {
      continue;
    }

    if (strcmp(instr_case, mips_r_table[n].instr) == 0)
    {
      char shift_table[] = { 0, 11, 21, 16, 6 };

      if (mips_r_table[n].operand_count != operand_count)
      {
        found = 1;
        continue;
      }

      opcode = mips_r_table[n].function;

      for (r = 0; r < operand_count; r++)
      {
        if (check_type(asm_context, instr, operands[r].type, mips_r_table[n].operand[r], operands[r].value))
        {
          return -1;
        }

        if (mips_r_table[n].operand[r] == MIPS_OP_SA)
        {
          if (operands[r].type != OPERAND_IMMEDIATE)
          {
            printf("Error: '%s' expects registers at %s:%d\n", instr, asm_context->tokens.filename, asm_context->tokens.line);
            return -1;
          }
        }
          else
        {
          if (operands[r].type != OPERAND_TREG)
          {
            printf("Error: '%s' expects registers at %s:%d\n", instr, asm_context->tokens.filename, asm_context->tokens.line);
            return -1;
          }
        }

        opcode |= operands[r].value << shift_table[(int)mips_r_table[n].operand[r]];
      }

      add_bin32(asm_context, opcode, IS_OPCODE);
      return opcode_size;
    }
  }

  // J-Type Instruction [ op 6, target 26 ] (jump instructions)
  if (strcmp(instr_case, "j") == 0 || strcmp(instr_case, "jal") == 0)
  {
    if (operand_count != 1)
    {
      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (operands[0].type != OPERAND_IMMEDIATE)
    {
      printf("Error: Expecting address for '%s' at %s:%d\n",
        instr, asm_context->tokens.filename, asm_context->tokens.line);
      return -1;
    }

    const uint32_t jump_address = operands[0].value & 0x0fffffff;
    const uint32_t address = operands[0].value & 0xf0000000;

    if ((address & 0xf0000000) != (operands[0].value & 0xf0000000))
    {
      printf("Error: Jump address on wrong page at %s:%d\n",
        asm_context->tokens.filename, asm_context->tokens.line);
      return -1;
    }

    if (instr_case[1] == 0)
    {
      // j instruction.
      opcode = 2 << 26;
    }
      else
    {
      // jal instruction.
      opcode = 3 << 26;
    }

    add_bin32(asm_context, opcode | jump_address >> 2, IS_OPCODE);

    return opcode_size;
  }

  // I-Type?  [ op 6, rs 5, rt 5, imm 16 ]
  for (n = 0; mips_i_table[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_i_table[n].version & asm_context->flags) == 0)
    {
      continue;
    }

    if (strcmp(instr_case, mips_i_table[n].instr) == 0)
    {
      char shift_table[] = { 0, 0, 21, 16, 0, 0, 0, 0, 0, 0, 16, 0, 6, 11, 16 };
      if (mips_i_table[n].operand_count != operand_count)
      {
        print_error_opcount(instr, asm_context);
        return -1;
      }

      opcode = mips_i_table[n].function << 26;

      for (r = 0; r < mips_i_table[n].operand_count; r++)
      {
        if (check_type(asm_context, instr, operands[r].type, mips_i_table[n].operand[r], operands[r].value))
        {
          return -1;
        }

        if ((mips_i_table[n].operand[r] == MIPS_OP_RT ||
             mips_i_table[n].operand[r] == MIPS_OP_RS) &&
             operands[r].type == OPERAND_TREG)
        {
          opcode |= operands[r].value << shift_table[(int)mips_i_table[n].operand[r]];
        }
          else
        if (mips_i_table[n].operand[r] == MIPS_OP_LABEL)
        {
          offset = operands[r].value - (asm_context->address + 4);

          if (offset < -(1 << 17) ||
              offset > (1 << 17) - 1)
          {
            print_error_range("Offset", -(1 << 17), (1 << 17) - 1, asm_context);
            return -1;
          }

          if ((offset & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          opcode |= (offset >> 2) & 0xffff;
        }
          else
        if (mips_i_table[n].operand[r] == MIPS_OP_IMMEDIATE ||
            mips_i_table[n].operand[r] == MIPS_OP_IMMEDIATE_SIGNED)
        {
          opcode |= operands[r].value & 0xffff;
        }
          else
        if (mips_i_table[n].operand[r] == MIPS_OP_IMMEDIATE_RS)
        {
          opcode |= operands[r].value & 0xffff;
          opcode |= operands[r].reg2 << 21;
        }
          else
        if (mips_i_table[n].operand[r] == MIPS_OP_HINT ||
            mips_i_table[n].operand[r] == MIPS_OP_CACHE)
        {
          opcode |= operands[r].value << 16;
        }
          else
        if (mips_i_table[n].operand[r] == MIPS_OP_FT &&
            operands[r].type == OPERAND_FREG)
        {
          opcode |= operands[r].value << shift_table[(int)mips_i_table[n].operand[r]];
        }
          else
        {
          print_error_illegal_operands(instr, asm_context);
          return -1;
        }
      }

      add_bin32(asm_context, opcode, IS_OPCODE);

      return opcode_size;
    }
  }

  for (n = 0; mips_branch_table[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_branch_table[n].version & asm_context->flags) == 0)
    {
      continue;
    }

    if (strcmp(instr_case, mips_branch_table[n].instr) == 0)
    {
      if (mips_branch_table[n].op_rt == -1)
      {
        if (operand_count != 3)
        {
          print_error_opcount(instr, asm_context);
          return -1;
        }

        if (operands[0].type != OPERAND_TREG ||
            operands[1].type != OPERAND_TREG ||
            operands[2].type != OPERAND_IMMEDIATE)
        {
          print_error_illegal_operands(instr, asm_context);
          return -1;
        }

        opcode = (mips_branch_table[n].opcode << 26) |
                 (operands[0].value << 21) |
                 (operands[1].value << 16);

        if (add_offset(asm_context, operands[2].value, &opcode) == -1)
        {
          return -1;
        }

        add_bin32(asm_context, opcode, IS_OPCODE);

        return opcode_size;
      }
        else
      {
        if (operand_count != 2)
        {
          print_error_opcount(instr, asm_context);
          return -1;
        }

        if (operands[0].type != OPERAND_TREG ||
            operands[1].type != OPERAND_IMMEDIATE)
        {
          print_error_illegal_operands(instr, asm_context);
          return -1;
        }

        opcode = (mips_branch_table[n].opcode << 26) |
                 (operands[0].value << 21) |
                 (mips_branch_table[n].op_rt << 16);

        if (add_offset(asm_context, operands[1].value, &opcode) == -1)
        {
          return -1;
        }

        add_bin32(asm_context, opcode, IS_OPCODE);

        return opcode_size;
      }
    }
  }

  // Special2 / Special3 type
  for (n = 0; mips_special_table[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_special_table[n].version & asm_context->flags) == 0) { continue; }

    if (strcmp(instr_case, mips_special_table[n].instr) == 0)
    {
      if (mips_special_table[n].operand_count != operand_count)
      {
        print_error_illegal_operands(instr, asm_context);
        return -1;
      }

      int shift;

      opcode = (mips_special_table[n].format << 26) |
                mips_special_table[n].function;

      if (mips_special_table[n].type == SPECIAL_TYPE_REGS)
      {
        opcode |= mips_special_table[n].operation << 6;
        shift = 21;
      }
        else
      if (mips_special_table[n].type == SPECIAL_TYPE_SA)
      {
        opcode |= mips_special_table[n].operation << 21;
        shift = 16;
      }
        else
      if (mips_special_table[n].type == SPECIAL_TYPE_BITS)
      {
        shift = 21;
      }
        else
      if (mips_special_table[n].type == SPECIAL_TYPE_BITS2)
      {
        shift = 21;
      }
        else
      {
        print_error_internal(asm_context, __FILE__, __LINE__);
        return -1;
      }

      for (r = 0; r < 4; r++)
      {
        int operand_index = mips_special_table[n].operand[r];

        if (operand_index != -1)
        {
          if (r < 2 || mips_special_table[n].type == SPECIAL_TYPE_REGS)
          {
            if (operands[operand_index].type != OPERAND_TREG)
            {
              printf("Error: '%s' expects registers at %s:%d\n",
                instr, asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }
          }
            else
          {
            // SPECIAL_TYPE_SA and SPECIAL_TYPE_BITS
            if (operands[operand_index].type != OPERAND_IMMEDIATE)
            {
              printf("Error: '%s' expects immediate %s:%d\n",
                instr, asm_context->tokens.filename, asm_context->tokens.line);
              return -1;
            }

            if (operand_index == 3 &&
                mips_special_table[n].type == SPECIAL_TYPE_BITS)
            {
              if (operands[operand_index].value < 1 ||
                  operands[operand_index].value > 32)
              {
                print_error_range("Constant", 1, 32, asm_context);
                return -1;
              }

              operands[operand_index].value--;
            }
              else
            if (operand_index == 3 &&
                mips_special_table[n].type == SPECIAL_TYPE_BITS2)
            {
              if (operands[operand_index].value < 1 ||
                  operands[operand_index].value > 32)
              {
                print_error_range("size", 1, 32, asm_context);
                return -1;
              }

              operands[operand_index].value += operands[2].value - 1;

              if (operands[operand_index].value < 1 ||
                  operands[operand_index].value > 32)
              {
                print_error_range("pos+size", 1, 32, asm_context);
                return -1;
              }
            }
              else
            {
              if (operands[r].value < 0 || operands[r].value > 31)
              {
                print_error_range("Constant", 0, 31, asm_context);
                return -1;
              }
            }
          }

          opcode |= operands[operand_index].value << shift;
        }

        shift -= 5;
      }

      // FIXME - Is this always true?
      //opcode |= operands[0].value << shift_table[3];

      add_bin32(asm_context, opcode, IS_OPCODE);
      return opcode_size;
    }
  }

  // Some MIPS instructions seem to have 4 registers.
  for (n = 0; mips_four_reg[n].instr != NULL; n++)
  {
    // Check of this specific MIPS chip uses this instruction.
    if ((mips_four_reg[n].version & asm_context->flags) == 0) { continue; }

    if (strcmp(instr_case, mips_four_reg[n].instr) != 0) { continue; }

    found = 1;

    if (operand_count != 4) { continue; }

    if (operands[0].type != OPERAND_FREG ||
        operands[1].type != OPERAND_FREG ||
        operands[2].type != OPERAND_FREG ||
        operands[3].type != OPERAND_FREG)
    {
      continue;
    }

    opcode = mips_four_reg[n].opcode |
             (operands[0].value << 6) |
             (operands[1].value << 21) |
             (operands[2].value << 11) |
             (operands[3].value << 16);

    add_bin32(asm_context, opcode, IS_OPCODE);

    return 4;
  }

  if (asm_context->cpu_type == CPU_TYPE_EMOTION_ENGINE)
  {
    int size = check_ee_instruction(
      asm_context,
      instr,
      instr_case,
      operands,
      operand_count,
      &found);

    if (size == -1) { return -1; }
    if (size != 0) { return size; }
  }

  int size = check_other_instruction(
    asm_context,
    instr,
    instr_case,
    operands,
    operand_count,
    &found);

  if (size == -1) { return -1; }
  if (size != 0) { return size; }

  if ((asm_context->flags & MIPS_MSA) != 0)
  {
    for (n = 0; mips_msa[n].instr != NULL; n++)
    {
      if (strcmp(instr_case, mips_msa[n].instr) == 0)
      {
        found = 1;

        if (operand_count != mips_msa[n].operand_count)
        {
          continue;
        }

        opcode = mips_msa[n].opcode;

        for (r = 0; r < mips_msa[n].operand_count; r++)
        {
          switch (mips_msa[n].operand[r])
          {
            case MIPS_OP_WT:
              if (operands[r].type != OPERAND_WREG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              opcode |= operands[r].value << 26;

              break;
            case MIPS_OP_WS:
              if (operands[r].type != OPERAND_WREG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              opcode |= operands[r].value << 11;

              break;
            case MIPS_OP_WD:
              if (operands[r].type != OPERAND_WREG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              opcode |= operands[r].value << 6;

              break;
            default:
              print_error_illegal_operands(instr, asm_context);
              return -1;
          }
        }

        add_bin32(asm_context, opcode, IS_OPCODE);
        return opcode_size;
      }
    }
  }

  if ((asm_context->flags & MIPS_EE_VU) != 0)
  {
    int dest = 0;

    get_dest(instr_case, &dest);

    for (n = 0; mips_ee_vector[n].instr != NULL; n++)
    {
      if (strcmp(instr_case, mips_ee_vector[n].instr) == 0)
      {
        found = 1;

        if (operand_count != mips_ee_vector[n].operand_count)
        {
          continue;
        }

        if ((mips_ee_vector[n].flags & FLAG_XYZ) != 0 && dest != 0xe)
        {
          print_error_illegal_operands(instr, asm_context);
          return -1;
        }

        if ((mips_ee_vector[n].flags & FLAG_DEST) == 0 && dest != 0x00)
        {
          print_error_illegal_operands(instr, asm_context);
          return -1;
        }

        opcode = mips_ee_vector[n].opcode;

        if (asm_context->pass == 1) { return 4; }

        for (r = 0; r < mips_ee_vector[n].operand_count; r++)
        {
          switch (mips_ee_vector[n].operand[r])
          {
            case MIPS_OP_VFT:
              if (operands[r].type != OPERAND_VFREG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              opcode |= (operands[r].value << 16);

              if ((mips_ee_vector[n].flags & FLAG_TE) != 0)
              {
                int field = get_field_number(operands[r].field_mask);
                if (field == -1) { return -1; }
                opcode |= field << 23;
              }
              break;
            case MIPS_OP_VFS:
              if (operands[r].type != OPERAND_VFREG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              opcode |= (operands[r].value << 11);

              if ((mips_ee_vector[n].flags & FLAG_SE) != 0)
              {
                int field = get_field_number(operands[r].field_mask);
                if (field == -1) { return -1; }
                opcode |= field << 21;
              }
              break;
            case MIPS_OP_VFD:
              if (operands[r].type != OPERAND_VFREG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              opcode |= (operands[r].value << 6);
              break;
            case MIPS_OP_VIT:
              if (operands[r].type != OPERAND_VIREG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              opcode |= (operands[r].value << 16);
              break;
            case MIPS_OP_VIS:
              if (operands[r].type != OPERAND_VIREG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              opcode |= (operands[r].value << 11);
              break;
            case MIPS_OP_VID:
              if (operands[r].type != OPERAND_VIREG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              opcode |= (operands[r].value << 6);
              break;
            case MIPS_OP_VI01:
              if (operands[r].type != OPERAND_VIREG || operands[r].value != 1)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              break;
            case MIPS_OP_VI27:
              if (operands[r].type != OPERAND_VIREG || operands[r].value != 27)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              break;
            case MIPS_OP_I:
              if (operands[r].type != OPERAND_I)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              break;
            case MIPS_OP_Q:
              if (operands[r].type != OPERAND_Q)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              break;
            case MIPS_OP_P:
              if (operands[r].type != OPERAND_P)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              break;
            case MIPS_OP_R:
              if (operands[r].type != OPERAND_R)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              break;
            case MIPS_OP_ACC:
              if (operands[r].type != OPERAND_ACC)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              break;
            case MIPS_OP_OFFSET_VBASE:
              if (operands[r].type != OPERAND_OFFSET_VBASE)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              if (get_field_number(dest) == -1)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              offset = operands[r].value;

              if (offset < -0x400 || offset > 0x3ff)
              {
                print_error_range("Address", -0x400, 0x3ff, asm_context);
                return -1;
              }

              if (mips_ee_vector[n].operand[0] == MIPS_OP_FS)
              {
                opcode |= operands[r].reg2 << 16;
              }
                else
              {
                opcode |= operands[r].reg2 << 11;
              }
              opcode |= offset & 0x7ff;

              break;
            case MIPS_OP_VBASE:
              if (operands[r].type != OPERAND_OFFSET_VBASE ||
                  operands[r].value != 0)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              if (mips_ee_vector[n].operand[0] == MIPS_OP_FS)
              {
                opcode |= operands[r].reg2 << 16;
              }
                else
              {
                opcode |= operands[r].reg2 << 11;
              }

              break;
            case MIPS_OP_VBASE_DEC:
              if (operands[r].type != OPERAND_OFFSET_VBASE_DEC ||
                  operands[r].value != 0)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              if (mips_ee_vector[n].operand[0] == MIPS_OP_VFS)
              {
                opcode |= operands[r].reg2 << 16;
              }
                else
              {
                opcode |= operands[r].reg2 << 11;
              }

              break;
            case MIPS_OP_VBASE_INC:
              if (operands[r].type != OPERAND_OFFSET_VBASE_INC ||
                  operands[r].value != 0)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              if (mips_ee_vector[n].operand[0] == MIPS_OP_VFS)
              {
                opcode |= operands[r].reg2 << 16;
              }
                else
              {
                opcode |= operands[r].reg2 << 11;
              }

              break;
            case MIPS_OP_IMMEDIATE15_2:
              if (operands[r].type != OPERAND_IMMEDIATE)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              if ((operands[r].value & 0x7) != 0)
              {
                print_error_align(asm_context, 8);
                return -1;
              }

              int immediate = operands[r].value >> 3;

              if (operands[r].value < 0 || operands[r].value > 0x7fff)
              {
                print_error_range("Immediate", 0, 0x7fff << 8, asm_context);
                return -1;
              }

              opcode |= (immediate & 0x7ff) << 6;

              break;
            case MIPS_OP_IMMEDIATE5:
              if (operands[r].type != OPERAND_IMMEDIATE)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }

              if (operands[r].value < -16 || operands[r].value > 15)
              {
                print_error_range("Immediate", -16, 15, asm_context);
                return -1;
              }

              opcode |= (operands[r].value & 0x1f) << 6;

              break;
            default:
              print_error_illegal_operands(instr, asm_context);
              return -1;
          }
        }

        opcode |= dest << 21;

        add_bin32(asm_context, opcode, IS_OPCODE);
        return opcode_size;
      }
    }
  }

  if ((asm_context->flags & MIPS_RSP) != 0)
  {
    for (n = 0; mips_rsp_vector[n].instr != NULL; n++)
    {
      if (strcmp(instr_case, mips_rsp_vector[n].instr) != 0) { continue; }

      found = 1;

      if (operand_count != mips_rsp_vector[n].operand_count)
      {
        continue;
      }

      switch (mips_rsp_vector[n].type)
      {
        case OP_MIPS_RSP_NONE:
        {
          add_bin32(asm_context, mips_rsp_vector[n].opcode, IS_OPCODE);
          return 4;
        }
        case OP_MIPS_RSP_LOAD_STORE:
        {
          if (operands[0].type == OPERAND_RSP_VREG &&
              operands[0].rsp_element.type == RSP_ELEMENT_WHOLE &&
              operands[1].type == OPERAND_IMMEDIATE_RS)
          {
            const int element = operands[0].rsp_element.index;
            const int element_max = mips_rsp_vector[n].element_max;
            const int element_step = mips_rsp_vector[n].element_step;
            const int offset_mask = (1 << mips_rsp_vector[n].shift) - 1;
            const int offset_max = (0x40 << mips_rsp_vector[n].shift) - 1;
            const int offset_min = -(offset_max + 1);

            if (check_element(asm_context, element, element_max, element_step) != 0)
            {
              return -1;
            }

            if (operands[1].value < offset_min ||
                operands[1].value > offset_max)
            {
              print_error_range("Offset", offset_min, offset_max, asm_context);
              return -1;
            }

            offset = operands[1].value;

            if ((offset & offset_mask) != 0)
            {
              print_error_align(asm_context, mips_rsp_vector[n].shift);
              return -1;
            }

            offset = offset >> mips_rsp_vector[n].shift;

            opcode =
              mips_rsp_vector[n].opcode |
              (operands[1].reg2 << 21) |
              (operands[0].value << 16) |
              (operands[0].rsp_element.index << 7) |
              (offset & 0x7f);

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        case OP_MIPS_RSP_REG_MOVE:
        {
          if (operands[0].type == OPERAND_TREG &&
              operands[1].type == OPERAND_RSP_VREG &&
              operands[1].rsp_element.type == RSP_ELEMENT_WHOLE)
          {
            const int element = operands[1].rsp_element.index;
            const int element_max = mips_rsp_vector[n].element_max;
            const int element_step = mips_rsp_vector[n].element_step;

            if (check_element(asm_context, element, element_max, element_step) != 0)
            {
              return -1;
            }

            opcode =
              mips_rsp_vector[n].opcode |
              (operands[0].value << 16) |
              (operands[1].value << 11) |
              (operands[1].rsp_element.index << 7);

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        case OP_MIPS_RSP_REG_2:
        {
          if (operands[0].type == OPERAND_RSP_VREG &&
              operands[0].rsp_element.type == RSP_ELEMENT_WHOLE &&
              operands[1].type == OPERAND_RSP_VREG &&
              operands[1].rsp_element.type == RSP_ELEMENT_WHOLE)
          {
            if (check_range(asm_context, "Element", operands[0].rsp_element.index, 0, 15) == -1) { return -1; }
            if (check_range(asm_context, "Element", operands[1].rsp_element.index, 0, 15) == -1) { return -1; }

            opcode =
              mips_rsp_vector[n].opcode |
              (operands[1].rsp_element.index << 21) |
              (operands[1].value << 16) |
              (operands[0].rsp_element.index << 11) |
              (operands[0].value << 6);

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        case OP_MIPS_RSP_ALU:
        {
          int i;

          // Sorry :(. Unfortunately MIPS defines $v0 and $v1 as
          // regular registers ($2 and $3) while RSP syntax says it
          // could be a vector register. If the assembler detects
          // an RSP_ALU instruction and it's a regular register, need
          // to rename it as RSP vector register.
          for (i = 0; i < 3; i++)
          {
            if (operands[i].type == OPERAND_TREG &&
                operands[i].value >= 2 && operands[i].value <= 3)
            {
              operands[i].type = OPERAND_RSP_VREG;
              operands[i].value = operands[i].value - 2;
            }
          }

          if (operands[0].type == OPERAND_RSP_VREG &&
              operands[1].type == OPERAND_RSP_VREG &&
              operands[2].type == OPERAND_RSP_VREG)
          {
            if (operands[0].rsp_element.type != RSP_ELEMENT_VECTOR ||
                operands[1].rsp_element.type != RSP_ELEMENT_VECTOR)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            int element =
              operands[2].rsp_element.index |
              operands[2].rsp_element.type;

            if (operands[2].rsp_element.type == RSP_ELEMENT_QUARTER)
            {
              if (check_range(asm_context,
                              "Element",
                              operands[2].rsp_element.index, 0, 1) == -1)
              {
                return -1;
              }
            }
              else
            if (operands[2].rsp_element.type == RSP_ELEMENT_HALF)
            {
              if (check_range(asm_context,
                              "Element",
                              operands[2].rsp_element.index, 0, 3) == -1)
              {
                return -1;
              }
            }
              else
            if (operands[2].rsp_element.type == RSP_ELEMENT_WHOLE)
            {
              if (check_range(asm_context,
                              "Element",
                              operands[2].rsp_element.index, 0, 7) == -1)
              {
                return -1;
              }
            }

            opcode =
              mips_rsp_vector[n].opcode |
              (element << 21) |
              (operands[2].value << 16) |
              (operands[1].value << 11) |
              (operands[0].value << 6);

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        default:
        {
          print_error_internal(asm_context, __FILE__, __LINE__);
          return -1;
        }
      }
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

int link_function_mips(
  struct _asm_context *asm_context,
  struct _imports *imports,
  const uint8_t *code,
  uint32_t function_offset,
  int size,
  uint8_t *obj_file,
  uint32_t obj_size)
{
  uint32_t opcode;
  uint32_t local_offset;
  int n;

  for (n = 0; n < size; n = n + 4)
  {
    if (asm_context->memory.endian == ENDIAN_LITTLE)
    {
      opcode = code[n + 0] |
              (code[n + 1] << 8) |
              (code[n + 2] << 16) |
              (code[n + 3] << 24);
    }
      else
    {
      opcode = code[n + 3] |
              (code[n + 2] << 8) |
              (code[n + 1] << 16) |
              (code[n + 0] << 24);
    }

    if ((opcode & 0xfc000000) == 0x0c000000)
    {
      //printf("jal detected @ 0x%04x function_offset=0x%04x rel=0x%04x\n", asm_context->address, function_offset, function_offset + n);

      local_offset = opcode & 0x03000000;

      // This needs to be from the same .o file as this function.
      const char *symbol = imports_obj_find_name_from_offset(
        obj_file, obj_size, function_offset + n, local_offset);

      if (symbol == NULL)
      {
        printf("Error: Couldn't find symbol name from offset.\n");
        return -1;
      }

      //printf("-> call to %s\n", symbol);

      if (asm_context->pass == 1)
      {
        if (linker_search_code_from_symbol(asm_context->linker, symbol) == 0)
        {
          printf("Error: Symbol not found %s\n", symbol);
          return -1;
        }
      }
        else
      {
        uint32_t address;

        if (symbols_lookup(&asm_context->symbols, symbol, &address) != 0)
        {
          printf("Error: Symbol not found %s\n", symbol);
          return -1;
        }

        opcode = opcode & 0xfc000000;
        opcode |= address >> 2;
      }
    }

    add_bin32(asm_context, opcode, IS_OPCODE);
  }

  return 0;
}

