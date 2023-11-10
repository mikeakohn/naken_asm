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
#include <ctype.h>

#include "asm/common.h"
#include "asm/riscv.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/riscv.h"

#define MAX_OPERANDS 5

enum
{
  OPERAND_NONE,
  OPERAND_X_REGISTER,
  OPERAND_F_REGISTER,
  OPERAND_NUMBER,
  OPERAND_RM,
  OPERAND_REGISTER_OFFSET,
  // FIXME: What is this?
  OPERAND_IORW,
};

#define RM_RNE 0
#define RM_RTZ 1
#define RM_RDN 2
#define RM_RUP 3
#define RM_RMM 4

struct _operand
{
  void set(int type, int value)
  {
    this->type = type;
    this->value = value;
  }

  int value;
  int type;
  int16_t offset;
  bool force_long;
};

struct _modifiers
{
  int aq;
  int rl;
  int rm;
  int fence;
};

static const char *rm_string[] =
{
  "rne",
  "rtz",
  "rdn",
  "rup",
  "rmm",
};

static const char *fence_string[] =
{
  "sw",
  "sr",
  "so",
  "si",
  "pw",
  "pr",
  "po",
  "pi",
};

static int get_register_number(char *token)
{
  int num = 0;

  while (*token != 0)
  {
    if (*token < '0' || *token > '9') { return -1; }

    num = (num * 10) + (*token - '0');
    token++;

    if (num > 31) { return -1; }
  }

  return num;
}

static int get_x_register_riscv(char *token)
{
  if (token[0] != 'x' && token[0] != 'X')
  {
    if (strcasecmp(token, "zero") == 0) { return 0; }
    if (strcasecmp(token, "ra") == 0) { return 1; }
    if (strcasecmp(token, "sp") == 0) { return 2; }
    if (strcasecmp(token, "gp") == 0) { return 3; }
    if (strcasecmp(token, "tp") == 0) { return 4; }
    if (strcasecmp(token, "fp") == 0) { return 8; }

    if (token[2] == 0)
    {
      if (token[0] == 't')
      {
        if (token[1] >= '0' && token[1] <= '2')
        {
          return (token[1] - '0') + 5;
        }
          else
        if (token[1] >= '3' && token[1] <= '6')
        {
          return (token[1] - '0') - 3 + 28;
        }

      }
      else if (token[0] == 'a' && token[1] >= '0' && token[1] <= '7')
      {
        return (token[1] - '0') + 10;
      }
      else if (token[0] == 's' && token[1] >= '0' && token[1] <= '1')
      {
        return (token[1] - '0') + 8;
      }
#if 0
      else if (token[0] == 'v' && token[1] >= '0' && token[1] <= '1')
      {
        return (token[1] - '0') + 16;
      }
#endif
    }

    if (token[0] == 's')
    {
      int n = get_register_number(token + 1);
      if (n >= 2 && n <= 11) { return n - 2 + 18; }
    }

    return -1;
  }

  return get_register_number(token + 1);
}

static uint32_t permutate_branch(int32_t offset)
{
  uint32_t immediate;

  immediate = ((offset >> 12) & 0x1) << 31;
  immediate |= ((offset >> 11) & 0x1) << 7;
  immediate |= ((offset >> 5) & 0x3f) << 25;
  immediate |= ((offset >> 1) & 0xf) << 8;

  return immediate;
}

static uint32_t permutate_jal(int32_t offset)
{
  uint32_t immediate;

  immediate = ((offset >> 20) & 0x1) << 31;
  immediate |= ((offset >> 12) & 0xff) << 12;
  immediate |= ((offset >> 11) & 0x1) << 20;
  immediate |= ((offset >> 1) & 0x3ff) << 21;

  return immediate;
}

static int permutate_16(int i, int8_t *table, bool is_unsigned = true)
{
  int value = 0;
  int low = 16, high = 0;

  for (int n = 0; n < 11; n++)
  {
    const int bit = table[n];

    if (bit == -1) { continue; }

    if (bit > high) { high = bit; }
    if (bit < low)  { low = bit; }

    value |= ((i >> bit) & 1) << (12 - n);
  }

  int low_mask = (1 << low) - 1;
  if ((i & low_mask) != 0) { return -1; }

  if (is_unsigned)
  {
    if ((i >> (high + 1)) != 0) { return -2; }
  }

  return value;
}

static int get_f_register_riscv(char *token)
{
  int n;

  if (token[0] != 'f') { return -1; }

  if (token[1] == 's')
  {
    n = get_register_number(token + 2);
    if (n >= 0 && n <= 1) { return n + 8; }
    if (n >= 2 && n <= 11) { return n - 2 + 18; }
    return -1;
  }

#if 0
  if (token[1] == 'v')
  {
    n = get_register_number(token + 2);
    if (n >= 0 && n <= 1) { return n + 16; }
    return -1;
  }
#endif

  if (token[1] == 'a')
  {
    n = get_register_number(token + 2);
    if (n >= 0 && n <= 7) { return n + 10; }
    return -1;
  }

  if (token[1] == 't')
  {
    n = get_register_number(token + 2);
    if (n >= 0 && n <= 7) { return n + 0; }
    if (n >= 8 && n <= 11) { return n - 8 + 28; }
    return -1;
  }

  return get_register_number(token + 1);
}

static uint32_t find_opcode(const char *instr_case)
{
  int n;

  for (n = 0; table_riscv[n].instr != NULL; n++)
  {
    if (strcmp(instr_case, table_riscv[n].instr) == 0)
    {
      return table_riscv[n].opcode;
    }
  }

  return 0xffffffff;
}

static int write_long_li(
  AsmContext *asm_context,
  int64_t value,
  uint32_t opcode_lui,
  uint32_t opcode_add)
{
  uint32_t lower = (value & 0xfff) << 20;
  uint32_t upper = (value & 0x00000800) == 0 ?
    value & 0xfffff000 :
    (value + 0x00001000) & 0xfffff000;

  add_bin32(asm_context, opcode_lui | upper, IS_OPCODE);
  add_bin32(asm_context, opcode_add | lower, IS_OPCODE);

  return 0;
}

static int get_operands_li32(
  AsmContext *asm_context,
  struct _operand *operands,
  char *instr,
  char *instr_case)
{
  int32_t num;

  // On pass 1, if data size is unknown have to assume this can't be
  // done with a single instruction.
  int force_long = asm_context->memory_read(asm_context->address);
  num = operands[1].value;

  if (force_long)
  {
    uint64_t temp = num;
    uint64_t mask = temp & 0xffffffff00000000ULL;

    if (mask != 0xffffffff00000000ULL && mask != 0)
    {
      print_error_range(asm_context, "Constant", -0x80000000LL, 0xffffffff);
      return -1;
    }
  }

  // If data size was unknown on pass 1, force_long.
  if (asm_context->pass == 2)
  {
    force_long = asm_context->memory_read(asm_context->address);
  }

  // Apply operands to memory.
  uint32_t opcode_lui  = find_opcode("lui")  | (operands[0].value << 7);
  //uint32_t opcode_ori  = find_opcode("ori")  | (operands[0].value << 7);
  uint32_t opcode_addi = find_opcode("addi") | (operands[0].value << 7);

  if (force_long == 1)
  {
    opcode_addi |= (operands[0].value << 15);
    write_long_li(asm_context, num, opcode_lui, opcode_addi);
    return 8;
  }
    else
  if (num >= -2048 && num <= 2047)
  {
    add_bin32(asm_context, opcode_addi | ((num & 0xfff) << 20), IS_OPCODE);
    return 4;
  }
    else
  if ((num & 0x00000fff) == 0)
  {
    add_bin32(asm_context, opcode_lui | (num & 0xfffff000), IS_OPCODE);
    return 4;
  }
    else
  {
    opcode_addi |= (operands[0].value << 15);
    write_long_li(asm_context, num, opcode_lui, opcode_addi);
    return 8;
  }
}

#if 0
static int get_operands_li64(
  AsmContext *asm_context,
  struct _operand *operands,
  char *instr,
  char *instr_case)
{
  int force_long = asm_context->memory_read(asm_context->address);

  //int64_t num = var.get_int64();
  int64_t num = operands[1].value;

  if (force_long)
  {
    //uint64_t temp = var.get_int64();
    uint64_t temp = num;
    uint64_t mask = temp & 0xffffffff00000000ULL;

    if (mask != 0xffffffff00000000ULL && mask != 0)
    {
      print_error_range(asm_context, "Constant", -0x80000000LL, 0xffffffff);
      return -1;
    }
  }

  print_error_internal(asm_context, __FILE__, __LINE__);
  return -1;
}
#endif

static int get_operands_li(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  char *instr,
  char *instr_case)
{
  if (operand_count != 2)
  {
    print_error_opcount(asm_context, instr);
    return -1;
  }

  if (operands[0].type != OPERAND_X_REGISTER ||
      operands[1].type != OPERAND_NUMBER)
  {
    print_error_illegal_operands(asm_context, instr);
    return -1;
  }

#if 0
  if (asm_context->flags == RISCV64)
  {
    return get_operands_li64(asm_context, operands, instr, instr_case);
  }
    else
#endif
  {
    return get_operands_li32(asm_context, operands, instr, instr_case);
  }
}

static int get_operands_call(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  char *instr,
  bool is_call)
{
  if (operand_count != 1)
  {
    print_error_opcount(asm_context, instr);
    return -1;
  }

  if (operands[0].type != OPERAND_NUMBER)
  {
    print_error_illegal_operands(asm_context, instr);
    return -1;
  }

  int offset = operands[0].value - (asm_context->address + 4);

  // call offset         : auipc x6, offset[31:12]
  //                       jalr x1, x6, offset[11:0]
  // tail offset         : auipc x6, offset[31:12]
  //                       jalr x0, x6, offset[11:0]
  const uint32_t opcode_auipc = 0x00000017 | (6 << 7);
  uint32_t opcode_jalr  = 0x00000067;

  int force_long = asm_context->memory_read(asm_context->address);

  if (is_call) { opcode_jalr |= (1 << 7); }
  if (offset < -2048 || offset > 2047) { force_long = 1; }

  if (force_long == 1)
  {
    write_long_li(asm_context, offset, opcode_auipc | (6 << 15), opcode_jalr);
    return 8;
  }
    else
  {
    add_bin32(asm_context, opcode_jalr | ((offset & 0xfff) << 20), IS_OPCODE);
    return 4;
  }
}

#if 0
static int compute_alias(
  AsmContext *asm_context,
  struct _operand *operands,
  int &operand_count,
  char *instr,
  char *instr_case)
{
  char token[TOKENLEN];
  //int token_type;

  bool has_two_reg = false;

  if (operand_count == 2 &&
      operands[0].type == OPERAND_X_REGISTER &&
      operands[1].type == OPERAND_X_REGISTER)
  {
    has_two_reg = true;
  }

  return 0;
}
#endif

static int get_operands(
  AsmContext *asm_context,
  struct _operand *operands,
  char *instr,
  char *instr_case,
  struct _modifiers *modifiers)
{
  char token[TOKENLEN];
  int token_type;
  int operand_count = 0;
  int n;

  // FIXME: Should probably change RISC-V to accept "." in instructions.
  if (IS_TOKEN(instr, 'c') || IS_TOKEN(instr, 'C'))
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token, '.'))
    {
      strcat(instr, ".");
      token_type = tokens_get(asm_context, token, TOKENLEN);
      strcat(instr, token);
      lower_copy(instr_case, instr);
    }
      else
    {
      tokens_push(asm_context, token, token_type);
    }
  }

  while (true)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    // FIXME - FILL IN
#if 0
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) break;
    if (IS_NOT_TOKEN(token, ',') || operand_count == 5)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
#endif

    if (operand_count == 0 && IS_TOKEN(token, '.'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      n = 0;
      while (token[n] != 0) { token[n] = tolower(token[n]); n++; }

      if (strcasecmp(token, "aq") == 0) { modifiers->aq = 1; continue; }
      if (strcasecmp(token, "rl") == 0) { modifiers->rl = 1; continue; }

      strcat(instr_case, ".");
      strcat(instr_case, token);
      continue;
    }

    do
    {
      // Check for registers
      n = get_x_register_riscv(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_X_REGISTER;
        operands[operand_count].value = n;
        break;
      }

      n = get_f_register_riscv(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_F_REGISTER;
        operands[operand_count].value = n;
        break;
      }

      // Check for FPU rounding mode
      for (n = 0; n < 5; n++)
      {
        if (strcasecmp(token, rm_string[n]) == 0)
        {
          operands[operand_count].type = OPERAND_RM;
          modifiers->rm = n;
          break;
        }
      }

      if (n != 5) { break; }

      // Check for fence's operands
      for (n = 0; n < 8; n++)
      {
        if (strcasecmp(token, fence_string[n]) == 0)
        {
          modifiers->fence |= 1 << n;
          operand_count--;  // sorry :(
          break;
        }
      }

      if (n != 8) { break; }

      if (strcasecmp(token, "iorw") == 0)
      {
        operands[0].type = OPERAND_IORW;
        break;
      }

      // Check if this is (reg)
      if (IS_TOKEN(token, '('))
      {
        char token[TOKENLEN];
        int token_type;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        n = get_x_register_riscv(token);

        if (n != -1)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token, ')'))
          {
            print_error_unexp(asm_context, token);
            return -1;
          }

          operands[operand_count].offset = 0;
          operands[operand_count].type = OPERAND_REGISTER_OFFSET;
          operands[operand_count].value = n;

          break;
        }

        tokens_push(asm_context, token, token_type);
      }

      // Assume this is just a number
      operands[operand_count].type = OPERAND_NUMBER;

#if 0
      if (asm_context->pass == 1)
      {
        ignore_operand(asm_context);
        operands[operand_count].value = 0;
      }
      else
      {
#endif
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &n) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          operands[operand_count].force_long = true;
          asm_context->memory_write(asm_context->address, 1);
          n = 0;
        }
          else
        {
          print_error_unexp(asm_context, token);
          return -1;
        }
      }

      operands[operand_count].value = n;

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token, '('))
      {
        if (operands[operand_count].value < -32768 ||
            operands[operand_count].value > 32767)
        {
          print_error_range(asm_context, "Offset", -32768, 32767);
          return -1;
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);

        n = get_x_register_riscv(token);
        if (n == -1)
        {
          print_error_unexp(asm_context, token);
          return -1;
        }

        operands[operand_count].offset = (uint16_t)operands[operand_count].value;
        operands[operand_count].type = OPERAND_REGISTER_OFFSET;
        operands[operand_count].value = n;

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token, ')'))
        {
          print_error_unexp(asm_context, token);
          return -1;
        }
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }

      //}

      //break;
    } while (false);

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token, ',') || operand_count == 5)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }

    if (operand_count == MAX_OPERANDS)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  return operand_count;
}

int parse_instruction_riscv(AsmContext *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count;
  int matched = 0;
  uint32_t opcode;
  struct _modifiers modifiers;
  int n;

  memset(&modifiers, 0, sizeof(modifiers));
  modifiers.rm = -1;

  lower_copy(instr_case, instr);

  memset(&operands, 0, sizeof(operands));

  operand_count = get_operands(asm_context, operands, instr, instr_case, &modifiers);

  if (operand_count < 0) { return -1; }

  if (strcmp(instr_case, "li") == 0)
  {
    return get_operands_li(asm_context, operands, operand_count, instr, instr_case);
  }

  if (strcmp(instr_case, "call") == 0)
  {
    return get_operands_call(asm_context, operands, operand_count, instr, true);
  }

  if (strcmp(instr_case, "tail") == 0)
  {
    return get_operands_call(asm_context, operands, operand_count, instr, false);
  }

#if 0
  n = compute_alias(asm_context, operands, operand_count, instr, instr_case);

  if (n != 0)
  {
    print_error_unknown_operand_combo(asm_context, instr);
    return -1;
  }
#endif

  for (n = 0; table_riscv[n].instr != NULL; n++)
  {
    if (strcmp(table_riscv[n].instr, instr_case) == 0)
    {
      matched = 1;

      if ((asm_context->address % 4) != 0)
      {
        print_error_align(asm_context, 4);
        return -1;
      }

      // If aq or rl was set, make sure it was only done on the
      // right instructions.
      if (modifiers.aq != 0 || modifiers.rl != 0)
      {
        if (table_riscv[n].type != OP_LR && table_riscv[n].type != OP_STD_EXT)
        {
          continue;
        }
      }

      if (modifiers.rm != -1)
      {
        if (operands[operand_count - 1].type != OPERAND_RM ||
            (table_riscv[n].type != OP_FP_FP_RM &&
             table_riscv[n].type != OP_R_FP_RM &&
             table_riscv[n].type != OP_FP_R_RM &&
             table_riscv[n].type != OP_FP_FP_FP_RM &&
             table_riscv[n].type != OP_FP_FP_FP_FP_RM))
        {
          continue;
        }

        operand_count--;
      }

      if (modifiers.rm == -1)
      {
        if (table_riscv[n].type != OP_ALIAS_JAL &&
            table_riscv[n].type != OP_ALIAS_JALR)
        {
          modifiers.rm = 7;
        }
      }

      // If the fence operands were used, make sure they only happened
      // with the fence instruction.

      if (modifiers.fence != 0 && table_riscv[n].type != OP_FENCE)
      {
        continue;
      }

      switch (table_riscv[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode;
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_R_TYPE:
        {
          if (operand_count != 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_R_R:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_I_TYPE:
        {
          if (operand_count != 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[2].value < -2048 || operands[2].value > 0xfff)
          {
            print_error_range(asm_context, "Immediate", -2048, 0xfff);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_UI_TYPE:
        {
          if (operand_count != 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[2].value < 0 || operands[2].value >= 4096)
          {
            print_error_range(asm_context, "Immediate", 0, 4095);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_SB_TYPE:
        {
          if (operand_count != 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          int32_t offset = 0;
          uint32_t immediate;

          if (asm_context->pass == 2)
          {
            //offset = (uint32_t)operands[2].value - (asm_context->address + 4);
            offset = (uint32_t)operands[2].value - asm_context->address;

            if ((offset & 0x1) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }

            if (offset < -4096 || offset >= 4095)
            {
              print_error_range(asm_context, "Offset", -4096, 4095);
              return -1;
            }
          }

          immediate = permutate_branch(offset);

          opcode = table_riscv[n].opcode |
                  (immediate |
                  (operands[1].value << 20) |
                  (operands[0].value << 15));
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_U_TYPE:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[1].value < -(1 << 19) || operands[1].value >= (1 << 20))
          {
            print_error_range(asm_context, "Immediate", -(1 << 19), (1 << 20) - 1);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 12) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_UJ_TYPE:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          int32_t offset = 0;
          uint32_t immediate;

          if (asm_context->pass == 2)
          {
            offset = (uint32_t)operands[1].value - asm_context->address;
            //offset = (uint32_t)operands[1].value - (asm_context->address + 4);
            //address = operands[1].value;

            if ((offset & 0x1) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }

            const int low = -(1 << 20);
            const int high = (1 << 20) - 1;

            if (offset < low || offset > high)
            {
              print_error_range(asm_context, "Offset", low, high);
              return -1;
            }
          }

          offset &= 0x1fffff;

          immediate = permutate_jal(offset);

          opcode = table_riscv[n].opcode | immediate | (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_SHIFT:
        {
          if (operand_count != 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[2].value < 0 || operands[2].value > 31)
          {
            print_error_range(asm_context, "Immediate", 0, 31);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FENCE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }
          opcode = table_riscv[n].opcode | (modifiers.fence << 20);
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case OP_FFFF:
        {
          if (operand_count != 0)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }
          opcode = table_riscv[n].opcode;
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case OP_READ:
        {
          if (operand_count != 1)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RD_INDEX_R:
        case OP_FD_INDEX_R:
        {
          int offset;
          int rd, rs1;

          if ((table_riscv[n].type == OP_RD_INDEX_R &&
               operands[0].type != OPERAND_X_REGISTER) ||
              (table_riscv[n].type == OP_FD_INDEX_R &&
               operands[0].type != OPERAND_F_REGISTER))
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operand_count == 2)
          {
            if (asm_context->pass == 1)
            {
              add_bin32(asm_context, table_riscv[n].opcode, IS_OPCODE);
              return 4;
            }

            if (operands[1].type != OPERAND_REGISTER_OFFSET)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }

            offset = operands[1].offset;
          }
            else
          if (operand_count == 3)
          {
            if (operands[1].type != OPERAND_X_REGISTER ||
                operands[2].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }

            offset = operands[2].value;
          }
            else
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          rd = operands[0].value;
          rs1 = operands[1].value;

          if (offset < -2048 || offset > 2047)
          {
            print_error_range(asm_context, "Offset", -2048, 2047);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  ((offset & 0xfff) << 20) |
                  (rs1 << 15) |
                  (rd << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RS_INDEX_R:
        case OP_FS_INDEX_R:
        {
          int offset;
          int rs1, rs2;

          if ((table_riscv[n].type == OP_RS_INDEX_R &&
               operands[0].type != OPERAND_X_REGISTER) ||
              (table_riscv[n].type == OP_FS_INDEX_R &&
               operands[0].type != OPERAND_F_REGISTER))
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operand_count == 2)
          {
            if (asm_context->pass == 1)
            {
              add_bin32(asm_context, table_riscv[n].opcode, IS_OPCODE);
              return 4;
            }

            if (operands[1].type != OPERAND_REGISTER_OFFSET)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }

            offset = operands[1].offset;
          }
            else
          if (operand_count == 3)
          {
            if (operands[1].type != OPERAND_X_REGISTER ||
                operands[2].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }

            offset = operands[2].value;
          }
            else
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          rs2 = operands[0].value;
          rs1 = operands[1].value;

          if (offset < -2048 || offset > 2047)
          {
            print_error_range(asm_context, "Offset", -2048, 2047);
            return -1;
          }

          offset = offset & 0xfff;

          opcode = table_riscv[n].opcode |
                (((offset >> 5) & 0x7f) << 25) |
                  (rs2 << 20) |
                  (rs1 << 15) |
                 ((offset & 0x1f) << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_LR:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_REGISTER_OFFSET ||
              operands[1].offset != 0)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (modifiers.aq == 1) { opcode |= (1 << 26); }
          if (modifiers.rl == 1) { opcode |= (1 << 25); }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_STD_EXT:
        {
          if (operand_count != 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[2].type == OPERAND_X_REGISTER)
          {
            operands[2].type = OPERAND_REGISTER_OFFSET;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_REGISTER_OFFSET ||
              operands[2].offset != 0)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          // FIXME - The docs say rs2 and rs1 are reversed. gnu-as is like this.
          opcode = table_riscv[n].opcode |
                  (operands[1].value << 20) |
                  (operands[2].value << 15) |
                  (operands[0].value << 7);

          if (modifiers.aq == 1) { opcode |= (1 << 26); }
          if (modifiers.rl == 1) { opcode |= (1 << 25); }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_R_FP:
        case OP_R_FP_RM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_R_FP_RM)
          {
            opcode |= (modifiers.rm << 12);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_R_FP_FP:
        {
          if (operand_count != 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER ||
              operands[2].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FP:
        {
          if (operand_count != 1)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[0].value << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FP_FP:
        case OP_FP_FP_RM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_FP_FP_RM)
          {
            opcode |= (modifiers.rm << 12);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FP_R:
        case OP_FP_R_RM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_FP_R_RM)
          {
            opcode |= (modifiers.rm << 12);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FP_FP_FP:
        case OP_FP_FP_FP_RM:
        {
          if (operand_count != 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER ||
              operands[2].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_FP_FP_FP_RM)
          {
            opcode |= (modifiers.rm << 12);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FP_FP_FP_FP_RM:
        {
          if (operand_count != 4)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER ||
              operands[2].type != OPERAND_F_REGISTER ||
              operands[3].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[3].value << 27) |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_FP_FP_FP_FP_RM)
          {
            opcode |= (modifiers.rm << 12);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_ALIAS_RD_RS1:
        case OP_ALIAS_RD_RS2:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_ALIAS_RD_RS1)
          {
            opcode |= operands[1].value << 15;
          }
            else
          {
            opcode |= operands[1].value << 20;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_ALIAS_FP_FP:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_ALIAS_BR_RS_X0:
        case OP_ALIAS_BR_X0_RS:
        {
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          int32_t offset = 0;
          uint32_t immediate;

          if (asm_context->pass == 2)
          {
            //offset = (uint32_t)operands[1].value - (asm_context->address + 4);
            offset = (uint32_t)operands[1].value - (asm_context->address);

            if ((offset & 0x1) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }

            if (offset < -4096 || offset >= 4095)
            {
              print_error_range(asm_context, "Offset", -4096, 4095);
              return -1;
            }
          }

          immediate = permutate_branch(offset);

          opcode = table_riscv[n].opcode | immediate;

          if (table_riscv[n].type == OP_ALIAS_BR_RS_X0)
          {
            opcode |= operands[0].value << 15;
          }
            else
          {
            opcode |= operands[0].value << 20;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_ALIAS_BR_RS_RT:
        {
          if (operand_count != 3)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          int32_t offset = 0;
          uint32_t immediate;

          if (asm_context->pass == 2)
          {
            offset = (uint32_t)operands[2].value - asm_context->address;

            if ((offset & 0x1) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }

            if (offset < -4096 || offset >= 4095)
            {
              print_error_range(asm_context, "Offset", -4096, 4095);
              return -1;
            }
          }

          immediate = permutate_branch(offset);

          opcode = table_riscv[n].opcode |
                  (immediate |
                  (operands[0].value << 20) |
                  (operands[1].value << 15));
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_ALIAS_JAL:
        {
          if (operand_count != 1) { continue; }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          int32_t offset = 0;
          uint32_t immediate;

          if (asm_context->pass == 2)
          {
            offset = (uint32_t)operands[0].value - asm_context->address;

            if ((offset & 0x1) != 0)
            {
              print_error_illegal_operands(asm_context, instr);
              return -1;
            }

            const int low = -(1 << 20);
            const int high = (1 << 20) - 1;

            if (offset < low || offset > high)
            {
              print_error_range(asm_context, "Offset", low, high);
              return -1;
            }
          }

          offset &= 0x1fffff;

          immediate = permutate_jal(offset);

          opcode = table_riscv[n].opcode | immediate;
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_ALIAS_JALR:
        {
          if (operand_count != 1) { continue; }

          if (operands[0].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv[n].opcode | (operands[0].value << 15);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        default:
          break;
      }
    }
  }

  for (n = 0; table_riscv_comp[n].instr != NULL; n++)
  {
    if (strcmp(table_riscv_comp[n].instr, instr_case) == 0)
    {
      int immediate, offset;
      bool is_op_error = false;

      switch (table_riscv_comp[n].type)
      {
        case OP_NONE:
          if (operand_count != 0)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          opcode = table_riscv_comp[n].opcode;
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_RD_NZUIMM:
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value < 8 || operands[0].value > 15)
          {
            print_error_illegal_register(asm_context, instr);
            return -1;
          }

          if ((operands[1].value % 4) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          if (check_range(asm_context, "Immediate", operands[1].value, 0, 1024) == -1) { return -1; }

          immediate = permutate_16(operands[1].value, RiscvPerm::nzuimm);
          opcode = table_riscv_comp[n].opcode |
            immediate |
            ((operands[0].value - 8) << 2);

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_UIMM53_76:
        case OP_COMP_UIMM548_76:
        case OP_COMP_UIMM53_26:
          if (operand_count != 2)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if (operands[1].type != OPERAND_REGISTER_OFFSET)
          {
            is_op_error = true;
          }

          if ((table_riscv_comp[n].flags & RISCV_FP) == 0)
          {
            if (operands[0].type != OPERAND_X_REGISTER) { is_op_error = true; }
          }
            else
          {
            if (operands[0].type != OPERAND_F_REGISTER) { is_op_error = true; }
          }

          if (is_op_error)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (table_riscv_comp[n].type == OP_COMP_UIMM53_76)
          {
            immediate = permutate_16(operands[1].offset, RiscvPerm::uimm53_76);
          }
            else
          if (table_riscv_comp[n].type == OP_COMP_UIMM548_76)
          {
            immediate = permutate_16(operands[1].offset, RiscvPerm::uimm548_76);
          }
            else
          if (table_riscv_comp[n].type == OP_COMP_UIMM53_26)
          {
            immediate = permutate_16(operands[1].offset, RiscvPerm::uimm53_26);
          }

          if (immediate == -1)
          {
            print_error(asm_context, "Operand alignemnt.");
            return -1;
          }

          if (immediate == -2)
          {
            print_error(asm_context, "Out of range.");
            return -1;
          }

          opcode = table_riscv_comp[n].opcode |
            immediate |
            ((operands[1].value - 8) << 7) |
            ((operands[0].value - 8) << 2);

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_JUMP:
          if (operand_count != 1 ||
              operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          offset = asm_context->pass == 1 ?
            4 : operands[0].value - asm_context->address;

          if (check_range(asm_context, "Offset", operands[1].value, 0, 1024) == -1) { return -1; }

          immediate = permutate_16(offset, RiscvPerm::jump, false);

          if (immediate == -1)
          {
            print_error(asm_context, "Operand alignemnt.");
            return -1;
          }

          opcode = table_riscv_comp[n].opcode | immediate;
          add_bin16(asm_context, opcode, IS_OPCODE);
         
          return 2;
        case OP_COMP_9_46875:
          if (operand_count != 1 ||
              operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "Immediate", operands[0].value, -512, 511) == -1) { return -1; }

          immediate = 0;

          if (asm_context->pass == 1)
          {
            immediate = permutate_16(operands[0].value, RiscvPerm::imm9_46875, false);
          }

          if (immediate < 0)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv_comp[n].opcode | immediate;
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_RD_NZIMM5:
        case OP_COMP_RD_IMM5:
        case OP_COMP_RD_17_1612:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value == 0)
          {
            print_error_illegal_register(asm_context, instr);
            return -1;
          }

          immediate = 0;

          if (table_riscv_comp[n].type == OP_COMP_RD_NZIMM5)
          {
            immediate = permutate_16(operands[1].value, RiscvPerm::nzimm5, false);
          }
            else
          if (table_riscv_comp[n].type == OP_COMP_RD_IMM5)
          {
            immediate = permutate_16(operands[1].value, RiscvPerm::imm5, false);
          }
            else
          if (table_riscv_comp[n].type == OP_COMP_RD_17_1612)
          {
            if (operands[0].value == 2)
            {
              print_error_illegal_register(asm_context, instr);
              return -1;
            }

            immediate = permutate_16(operands[1].value, RiscvPerm::imm17_1612, false);
          }

          if (immediate < 0)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv_comp[n].opcode |
            (operands[0].value << 7) |
            immediate;
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_RD_NZ5_40:
        case OP_COMP_RD_5_40:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value < 8 || operands[0].value > 15)
          {
            print_error_illegal_register(asm_context, instr);
            return -1;
          }

          immediate = permutate_16(operands[1].value, RiscvPerm::imm5);

          if (immediate < 0)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (table_riscv_comp[n].type == OP_COMP_RD_NZ5_40 &&
              operands[1].value == 0)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          opcode = table_riscv_comp[n].opcode |
            ((operands[0].value - 8) << 7) |
            immediate;
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_RD:
        case OP_COMP_RD32:
          if (operand_count != 1 || operands[0].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (table_riscv_comp[n].type == OP_COMP_RD)
          {
            if (operands[0].value < 8 || operands[0].value > 15)
            {
              print_error_illegal_register(asm_context, instr);
              return -1;
            }

            operands[0].value -= 8;
          }

          opcode = table_riscv_comp[n].opcode |
            (operands[0].value << 7);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_RD_RS2:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (operands[0].value < 8 || operands[0].value > 15 ||
              operands[1].value < 8 || operands[1].value > 15)
          {
            print_error_illegal_register(asm_context, instr);
            return -1;
          }

          opcode = table_riscv_comp[n].opcode |
            ((operands[0].value - 8) << 7) |
            ((operands[1].value - 8) << 2);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_BRANCH:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          offset = asm_context->pass == 1 ?
            4 : operands[1].value - asm_context->address;

          if (check_range(asm_context, "Offset", operands[1].value, -256, 256) == -1) { return -1; }

          immediate = permutate_16(offset, RiscvPerm::branch, false);

          if (immediate == -1)
          {
            print_error(asm_context, "Operand alignemnt.");
            return -1;
          }

          opcode = table_riscv_comp[n].opcode |
            ((operands[0].value - 8) << 7) |
            immediate;
          add_bin16(asm_context, opcode, IS_OPCODE);
         
          return 2;
        case OP_COMP_RD_5_4386:
        case OP_COMP_RD_5_496:
        case OP_COMP_RD_5_4276:
          if (operand_count != 2 ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if ((table_riscv_comp[n].flags & RISCV_FP) == 0)
          {
            if (operands[0].type != OPERAND_X_REGISTER) { is_op_error = true; }
            if (operands[0].value == 0) { is_op_error = true; }
          }
            else
          {
            if (operands[0].type != OPERAND_F_REGISTER) { is_op_error = true; }
          }

          if (is_op_error)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (table_riscv_comp[n].type == OP_COMP_RD_5_4386)
          {
            immediate = permutate_16(operands[1].value, RiscvPerm::uimm5_4386);
          }
            else
          if (table_riscv_comp[n].type == OP_COMP_RD_5_496)
          {
            immediate = permutate_16(operands[1].value, RiscvPerm::uimm5_496);
          }
            else
          if (table_riscv_comp[n].type == OP_COMP_RD_5_4276)
          {
            immediate = permutate_16(operands[1].value, RiscvPerm::uimm5_4276);
          }

          if (immediate == -1)
          {
            print_error(asm_context, "Operand alignemnt.");
            return -1;
          }

          if (immediate == -2)
          {
            print_error(asm_context, "Out of range.");
            return -1;
          }

          opcode = table_riscv_comp[n].opcode |
            immediate |
            (operands[0].value << 7);

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_RS1_RS2:
          if (operand_count != 2 ||
              operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          opcode = table_riscv_comp[n].opcode |
            (operands[0].value << 7) |
            (operands[1].value << 2);

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        case OP_COMP_5386_RS2:
        case OP_COMP_5496_RS2:
        case OP_COMP_5276_RS2:
          if (operand_count != 2 ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          if ((table_riscv_comp[n].flags & RISCV_FP) == 0)
          {
            if (operands[0].type != OPERAND_X_REGISTER) { is_op_error = true; }
          }
            else
          {
            if (operands[0].type != OPERAND_F_REGISTER) { is_op_error = true; }
          }

          if (is_op_error)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (table_riscv_comp[n].type == OP_COMP_5386_RS2)
          {
            immediate = permutate_16(operands[1].value, RiscvPerm::uimm5386);
          }
            else
          if (table_riscv_comp[n].type == OP_COMP_5496_RS2)
          {
            immediate = permutate_16(operands[1].value, RiscvPerm::uimm5496);
          }
            else
          if (table_riscv_comp[n].type == OP_COMP_5276_RS2)
          {
            immediate = permutate_16(operands[1].value, RiscvPerm::uimm5276);
          }

          if (immediate == -1)
          {
            print_error(asm_context, "Operand alignemnt.");
            return -1;
          }

          if (immediate == -2)
          {
            print_error(asm_context, "Out of range.");
            return -1;
          }

          opcode = table_riscv_comp[n].opcode |
            immediate |
            (operands[0].value << 2);

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        default:
          break;
      }
    }
  }

  if (matched == 1)
  {
    print_error_unknown_operand_combo(asm_context, instr);
  }
    else
  {
    print_error_unknown_instr(asm_context, instr);
  }

  return -1;
}

