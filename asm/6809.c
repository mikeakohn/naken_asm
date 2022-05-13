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
#include "asm/6809.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/6809.h"

#define CHECK_BYTES(a) \
  if (table_6809[n].bytes != a) \
  { \
    print_error_internal(asm_context, __FILE__, __LINE__); \
    return -1; \
  }

#define CHECK_BYTES_16(a) \
  if (table_6809_16[n].bytes != a) \
  { \
    print_error_internal(asm_context, __FILE__, __LINE__); \
    return -1; \
  }

struct _operand
{
  uint8_t count;
  uint8_t type;
  uint8_t index_reg;
  uint8_t is_indirect : 1;
  uint8_t use_long : 1;
  uint16_t reg_list;
  uint16_t reg_src;
  uint16_t reg_dst;
  int value;
};

enum
{
  OPERAND_NONE,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_DP_ADDRESS,
  OPERAND_REG_LIST,
  OPERAND_INDEX_REG,
  OPERAND_INDEX_REG_INC_1,
  OPERAND_INDEX_REG_INC_2,
  OPERAND_INDEX_REG_DEC_1,
  OPERAND_INDEX_REG_DEC_2,
  OPERAND_INDEX_OFFSET_REG,
  OPERAND_INDEX_OFFSET_PC,
  OPERAND_INDEX_INDIRECT_ADDRESS,
};

#define REG_FLAG_CC 0x01
#define REG_FLAG_A 0x02
#define REG_FLAG_B 0x04
#define REG_FLAG_DP 0x08
#define REG_FLAG_X 0x10
#define REG_FLAG_Y 0x20
#define REG_FLAG_U 0x40
#define REG_FLAG_PC 0x80

#define REG_FLAG_D 0x100
#define REG_FLAG_S 0x200

struct _aliases
{
  const char *name;
  const char *value;
};

static struct _aliases aliases[] =
{
  { "asl", "lsl" },
  { "bcc", "bhs" },
  { "bcs", "blo" },
  { "asla", "lsla" },
  { "aslb", "lslb" },
  { "lbcc", "lbhs" },
  { "lbcs", "lblo" },
  { NULL, NULL }
};

static void check_alias(char *instr)
{
  int n = 0;

  while (aliases[n].name != NULL)
  {
    if (strcmp(instr, aliases[n].name) == 0)
    {
      strcpy(instr, aliases[n].value);
      break;
    }

    n++;
  }
}

static int get_register(char *s)
{
  if (strcasecmp(s, "cc") == 0) { return REG_FLAG_CC; }
  if (strcasecmp(s, "a") == 0) { return REG_FLAG_A; }
  if (strcasecmp(s, "b") == 0) { return REG_FLAG_B; }
  if (strcasecmp(s, "dp") == 0) { return REG_FLAG_DP; }
  if (strcasecmp(s, "x") == 0) { return REG_FLAG_X; }
  if (strcasecmp(s, "y") == 0) { return REG_FLAG_Y; }
  if (strcasecmp(s, "u") == 0) { return REG_FLAG_U; }
  if (strcasecmp(s, "pc") == 0) { return REG_FLAG_PC; }

  // Can't be used with stack instruction
  if (strcasecmp(s, "d") == 0) { return REG_FLAG_D; }
  if (strcasecmp(s, "s") == 0) { return REG_FLAG_S; }

  return 0;
}

uint8_t get_reg_postbyte(int reg)
{
  switch (reg)
  {
    case REG_FLAG_D: return 0x0;
    case REG_FLAG_X: return 0x1;
    case REG_FLAG_Y: return 0x2;
    case REG_FLAG_U: return 0x3;
    case REG_FLAG_S: return 0x4;
    case REG_FLAG_PC: return 0x5;
    case REG_FLAG_A: return 0x8;
    case REG_FLAG_B: return 0x9;
    case REG_FLAG_CC: return 0xa;
    case REG_FLAG_DP: return 0xb;
    default: return 0xff;
  }
}

int parse_index(struct _asm_context *asm_context, char *instr, struct _operand *operand)
{
  char token[TOKENLEN];
  int token_type;
  int modifier = 0;

  operand->type = OPERAND_INDEX_REG;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token,'-'))
  {
    modifier--;
    operand->type = OPERAND_INDEX_REG_DEC_1;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token,'-'))
    {
      modifier--;
      operand->type = OPERAND_INDEX_REG_DEC_2;
      token_type = tokens_get(asm_context, token, TOKENLEN);
    }
  }

  if (strcasecmp(token, "x") == 0) { operand->index_reg = 0; }
  else if (strcasecmp(token, "y") == 0) { operand->index_reg = 1; }
  else if (strcasecmp(token, "u") == 0) { operand->index_reg = 2; }
  else if (strcasecmp(token, "s") == 0) { operand->index_reg = 3; }
  else
  {
    print_error_illegal_operands(instr, asm_context);
    return -1;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token,'+'))
  {
    if (modifier < 0)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    modifier++;
    operand->type = OPERAND_INDEX_REG_INC_1;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token,'+'))
    {
      modifier++;
      operand->type = OPERAND_INDEX_REG_INC_2;
      token_type = tokens_get(asm_context, token, TOKENLEN);
    }
  }

  if (operand->is_indirect && IS_TOKEN(token,']'))
  {
    tokens_push(asm_context, token, token_type);
    return 0;
  }

  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  return 0;
}

int parse_index_with_offset(struct _asm_context *asm_context, char *instr, struct _operand *operand)
{
  char token[TOKENLEN];
  int token_type;

  operand->type = OPERAND_INDEX_OFFSET_REG;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (strcasecmp(token, "x") == 0) { operand->index_reg = 0; }
  else if (strcasecmp(token, "y") == 0) { operand->index_reg = 1; }
  else if (strcasecmp(token, "u") == 0) { operand->index_reg = 2; }
  else if (strcasecmp(token, "s") == 0) { operand->index_reg = 3; }
  else if (strcasecmp(token, "pc") == 0)
  {
    operand->type = OPERAND_INDEX_OFFSET_PC;
    //operand->index_reg = 4;
  }
  else
  {
    print_error_illegal_operands(instr, asm_context);
    return -1;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (operand->is_indirect && IS_TOKEN(token,']'))
  {
    tokens_push(asm_context, token, token_type);
    return 0;
  }

  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  return 0;
}

static int check_indexed(struct _asm_context *asm_context, struct _operand *operand)
{
  uint8_t post_byte = operand->index_reg << 5;

  if (operand->type == OPERAND_INDEX_OFFSET_PC)
  {
    post_byte = 0x8c;
    if (operand->is_indirect == 1) { post_byte |= 0x10; }

    if (operand->value < -128 || operand->value > 127 || operand->use_long == 1)
    {
      add_bin8(asm_context, post_byte | 0x01, IS_OPCODE);
      add_bin16(asm_context, operand->value & 0xffff, IS_OPCODE);
      return 2;
    }
      else
    {
      add_bin8(asm_context, post_byte, IS_OPCODE);
      add_bin8(asm_context, operand->value & 0xff, IS_OPCODE);
      return 1;
    }
  }

  if (operand->is_indirect == 1) { post_byte |= 0x10; }

  if (operand->type == OPERAND_INDEX_OFFSET_REG)
  {
    if (operand->value < -128 || operand->value > 127 || operand->use_long == 1)
    {
      add_bin8(asm_context, post_byte | 0x89, IS_OPCODE);
      add_bin16(asm_context, operand->value & 0xffff, IS_OPCODE);
      return 2;
    }
      else
    if (operand->value < -16 || operand->value > 15 || operand->use_long == 1 || operand->is_indirect)
    {
      add_bin8(asm_context, post_byte | 0x88, IS_OPCODE);
      add_bin8(asm_context, operand->value & 0xff, IS_OPCODE);
      return 1;
    }
      else
    {
      add_bin8(asm_context, post_byte | (operand->value & 0x1f), IS_OPCODE);
      return 0;
    }

    return 0;
  }
    else
  if (operand->type == OPERAND_INDEX_REG)
  {
    add_bin8(asm_context, post_byte | 0x84, IS_OPCODE);
    return 0;
  }
    else
  if (operand->type == OPERAND_INDEX_REG_INC_1)
  {
    if (operand->is_indirect == 1) { return -1; }
    add_bin8(asm_context, post_byte | 0x80, IS_OPCODE);
    return 0;
  }
    else
  if (operand->type == OPERAND_INDEX_REG_INC_2)
  {
    add_bin8(asm_context, post_byte | 0x81, IS_OPCODE);
    return 0;
  }
    else
  if (operand->type == OPERAND_INDEX_REG_DEC_1)
  {
    if (operand->is_indirect == 1) { return -1; }
    add_bin8(asm_context, post_byte | 0x82, IS_OPCODE);
    return 0;
  }
    else
  if (operand->type == OPERAND_INDEX_REG_DEC_2)
  {
    add_bin8(asm_context, post_byte | 0x83, IS_OPCODE);
    return 0;
  }
    else
  if (operand->type == OPERAND_REG_LIST && operand->count == 2)
  {
    switch (operand->reg_src)
    {
      case REG_FLAG_A: post_byte |= 0x86; break;
      case REG_FLAG_B: post_byte |= 0x85; break;
      case REG_FLAG_D: post_byte |= 0x8b; break;
      default: return -1;
    }

    switch (operand->reg_dst)
    {
      case REG_FLAG_X: post_byte |= 0x00; break;
      case REG_FLAG_Y: post_byte |= 0x20; break;
      case REG_FLAG_U: post_byte |= 0x40; break;
      case REG_FLAG_S: post_byte |= 0x60; break;
      default: return -1;
    }

    if (operand->is_indirect == 1) { post_byte |= 0x10; }

    add_bin8(asm_context, post_byte, IS_OPCODE);
    return 0;
  }
    else
  if (operand->type == OPERAND_INDEX_INDIRECT_ADDRESS)
  {
    add_bin8(asm_context, 0x9f, IS_OPCODE);
    add_bin16(asm_context, operand->value & 0xffff, IS_OPCODE);
    return 2;
  }

  return -1;
}

int parse_instruction_6809(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  int token_type;
  struct _operand operand;
  uint8_t value8;
  uint16_t value16;
  int matched = 0;
  int n;

  lower_copy(instr_case, instr);
  check_alias(instr_case);

//printf("%s %d\n", instr, asm_context->line);

  memset(&operand, 0, sizeof(operand));

  do
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      operand.type = OPERAND_NONE;
      break;
    }

    if (IS_TOKEN(token,','))
    {
      if (parse_index(asm_context, instr, &operand) == -1) { return -1; }
    }
      else
    if (IS_TOKEN(token,'['))
    {
      operand.is_indirect = 1;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,','))
      {
        if (parse_index(asm_context, instr, &operand) == -1) { return -1; }
      }
        else
      if (get_register(token) != 0)
      {
        operand.type = OPERAND_REG_LIST;
        operand.reg_src = get_register(token);

        if (expect_token(asm_context, ',') == -1) { return -1; }

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (get_register(token) != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operand.reg_dst = get_register(token);
      }
        else
      {
        tokens_push(asm_context, token, token_type);
        if (eval_expression(asm_context, &operand.value) != 0)
        {
          if (asm_context->pass == 2)
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }

          operand.use_long = 1;
          ignore_operand(asm_context);
          operand.value = 0xffff;
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token,']'))
        {
          operand.type = OPERAND_INDEX_INDIRECT_ADDRESS;
          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
          break;
        }

        if (IS_NOT_TOKEN(token, ','))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        if (parse_index_with_offset(asm_context, instr, &operand) != 0)
        {
          return -1;
        }
      }

      if (expect_token(asm_context, ']') == -1) { return -1; }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }
      else
    if (get_register(token) != 0)
    {
      while (1)
      {
        n = get_register(token);

        if (n == 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operand.reg_list |= n;

        if (operand.count == 0) { operand.reg_src = n; }
        if (operand.count == 1) { operand.reg_dst = n; }
        operand.count++;

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
        if (IS_NOT_TOKEN(token,','))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);
      }

      operand.type = OPERAND_REG_LIST;
    }
      else
    if (IS_TOKEN(token,'>'))
    {
      if (eval_expression(asm_context, &operand.value) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        ignore_operand(asm_context);
        operand.value = 0x00;
      }

      operand.type = OPERAND_DP_ADDRESS;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }
      else
    if (token_type == TOKEN_POUND)
    {
      operand.type = OPERAND_NUMBER;
      if (eval_expression(asm_context, &operand.value) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        ignore_operand(asm_context);
        operand.value = 0xffff;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      break;
    }
      else
    {
      operand.type = OPERAND_ADDRESS;
      tokens_push(asm_context, token, token_type);
      if (eval_expression(asm_context, &operand.value) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        operand.use_long = 1;
        ignore_operand(asm_context);
        operand.value = 0xffff;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

      if (IS_NOT_TOKEN(token, ','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (parse_index_with_offset(asm_context, instr, &operand) != 0)
      {
        return -1;
      }
    }
  } while (0);

//printf("%s %d\n", instr, operand.type);

  n = 0;
  while (1)
  {
    if (table_6809[n].instr == NULL) { break; }

    if (strcmp(instr_case, table_6809[n].instr) == 0)
    {
      matched = 1;

      switch (table_6809[n].operand_type)
      {
        case M6809_OP_INHERENT:
        {
          if (operand.type == OPERAND_NONE)
          {
            CHECK_BYTES(1);
            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case M6809_OP_IMMEDIATE:
        {
          if (operand.type != OPERAND_NUMBER) { break; }
          if (table_6809[n].bytes == 2)
          {
            if (check_range(asm_context, "Immediate", operand.value, -128, 0xff) == -1) { return -1; }
            value8 = (uint8_t)operand.value;
            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);
            add_bin8(asm_context, value8, IS_OPCODE);
            return 2;
          }
            else
          if (table_6809[n].bytes == 3)
          {
            if (check_range(asm_context, "Immediate", operand.value, -32768, 0xffff) == -1) { return -1; }
            value16 = (uint16_t)operand.value;
            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);
            add_bin16(asm_context, value16, IS_OPCODE);
            return 3;
          }

          break;
        }
        case M6809_OP_EXTENDED:
        {
          if (operand.type != OPERAND_ADDRESS) { break; }
          if (table_6809[n].bytes == 3)
          {
            if (check_range(asm_context, "Address", operand.value, 0, 0xffff) == -1) { return -1; }
            value16 = (uint16_t)operand.value;
            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);
            add_bin16(asm_context, value16, IS_OPCODE);
            return 3;
          }

          break;
        }
        case M6809_OP_RELATIVE:
        {
          if (operand.type != OPERAND_ADDRESS) { break; }
          if (table_6809[n].bytes == 2)
          {
            int32_t offset = operand.value - (asm_context->address + 2);
            if (asm_context->pass == 2)
            {
              if (check_range(asm_context, "Offset", offset, -128, 127) == -1)
              {
                return -1;
              }
            }
            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)offset, IS_OPCODE);
            return 2;
          }

          break;
        }
        case M6809_OP_LONG_RELATIVE:
        {
          if (operand.type != OPERAND_ADDRESS) { break; }
          if (table_6809[n].bytes == 3)
          {
            int32_t offset = operand.value - (asm_context->address + 3);
            if (asm_context->pass == 2)
            {
              if (check_range(asm_context, "Offset", offset, -32768, 32767) == -1)
              {
                return -1;
              }
            }
            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)(offset >> 8), IS_OPCODE);
            add_bin8(asm_context, (uint8_t)(offset & 0xff), IS_OPCODE);
            return 3;
          }

          break;
        }
        case M6809_OP_DIRECT:
        {
          if (operand.type != OPERAND_DP_ADDRESS) { break; }
          if (table_6809[n].bytes == 2)
          {
            if (check_range(asm_context, "Address", operand.value, 0, 0xff) == -1) { return -1; }
            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operand.value, IS_OPCODE);
            return 2;
          }

          break;
        }
        case M6809_OP_STACK:
        {
          if (operand.type != OPERAND_REG_LIST ||
              operand.reg_list > 0xff) { break; }
          if (table_6809[n].bytes == 2)
          {
            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operand.reg_list, IS_OPCODE);
            return 2;
          }

          break;
        }
        case M6809_OP_TWO_REG:
        {
          if (operand.type != OPERAND_REG_LIST || operand.count != 2)
          {
            break;
          }

          if (table_6809[n].bytes == 2)
          {
            uint8_t src = get_reg_postbyte(operand.reg_src);
            uint8_t dst = get_reg_postbyte(operand.reg_dst);

            if (src == 0xff) { break; }
            if (dst == 0xff) { break; }

            src = (src << 4) | dst;

            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);
            add_bin8(asm_context, src, IS_OPCODE);
            return 2;
          }

          break;
        }
        case M6809_OP_INDEXED:
        {
          if (operand.type == OPERAND_INDEX_REG ||
              operand.type == OPERAND_INDEX_REG_INC_1 ||
              operand.type == OPERAND_INDEX_REG_INC_2 ||
              operand.type == OPERAND_INDEX_REG_DEC_1 ||
              operand.type == OPERAND_INDEX_REG_DEC_2 ||
              operand.type == OPERAND_INDEX_OFFSET_REG ||
              operand.type == OPERAND_INDEX_OFFSET_PC ||
              operand.type == OPERAND_INDEX_INDIRECT_ADDRESS ||
             (operand.type == OPERAND_REG_LIST && operand.count == 2))
          {
            add_bin8(asm_context, table_6809[n].opcode, IS_OPCODE);

            int count = check_indexed(asm_context, &operand);

            if (count >= 0) { return count + 2; }
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
    n++;
  }

  n = 0;
  while (1)
  {
    if (table_6809_16[n].instr == NULL) { break; }

    if (strcmp(instr_case, table_6809_16[n].instr) == 0)
    {
      matched = 1;

      switch (table_6809_16[n].operand_type)
      {
        case M6809_OP_INHERENT:
        {
          if (operand.type == OPERAND_NONE)
          {
            CHECK_BYTES_16(2);
            add_bin16(asm_context, table_6809_16[n].opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case M6809_OP_IMMEDIATE:
        {
          if (operand.type != OPERAND_NUMBER) { break; }
          if (table_6809_16[n].bytes == 4)
          {
            if (check_range(asm_context, "Immediate", operand.value, -32768, 0xffff) == -1) { return -1; }
            value16 = (uint16_t)operand.value;
            add_bin16(asm_context, table_6809_16[n].opcode, IS_OPCODE);
            add_bin16(asm_context, value16, IS_OPCODE);
            return 4;
          }

          break;
        }
        case M6809_OP_EXTENDED:
        {
          if (operand.type != OPERAND_ADDRESS) { break; }
          if (table_6809_16[n].bytes == 4)
          {
            if (check_range(asm_context, "Address", operand.value, 0, 0xffff) == -1) { return -1; }
            value16 = (uint16_t)operand.value;
            add_bin16(asm_context, table_6809_16[n].opcode, IS_OPCODE);
            add_bin16(asm_context, value16, IS_OPCODE);
            return 4;
          }

          break;
        }
        case M6809_OP_RELATIVE:
        {
          if (operand.type != OPERAND_ADDRESS) { break; }
          if (table_6809_16[n].bytes == 4)
          {
            int32_t offset = operand.value - (asm_context->address + 4);
            //if (check_range(asm_context, "Offset", offset, -32768, 32767) == -1) { return -1; }
            add_bin16(asm_context, table_6809_16[n].opcode, IS_OPCODE);
            add_bin16(asm_context, (uint16_t)offset, IS_OPCODE);
            return 4;
          }

          break;
        }
        case M6809_OP_DIRECT:
        {
          if (operand.type != OPERAND_DP_ADDRESS) { break; }
          if (table_6809_16[n].bytes == 3)
          {
            if (check_range(asm_context, "Address", operand.value, 0, 0xff) == -1) { return -1; }
            add_bin16(asm_context, table_6809_16[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operand.value, IS_OPCODE);
            return 3;
          }

          break;
        }
        case M6809_OP_INDEXED:
        {
          if (operand.type == OPERAND_INDEX_REG ||
              operand.type == OPERAND_INDEX_REG_INC_1 ||
              operand.type == OPERAND_INDEX_REG_INC_2 ||
              operand.type == OPERAND_INDEX_REG_DEC_1 ||
              operand.type == OPERAND_INDEX_REG_DEC_2 ||
              operand.type == OPERAND_INDEX_OFFSET_REG ||
              operand.type == OPERAND_INDEX_OFFSET_PC ||
              operand.type == OPERAND_INDEX_INDIRECT_ADDRESS ||
             (operand.type == OPERAND_REG_LIST && operand.count == 2))
          {
            add_bin16(asm_context, table_6809_16[n].opcode, IS_OPCODE);

            int count = check_indexed(asm_context, &operand);

            if (count >= 0) { return count + 3; }
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
    n++;
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



