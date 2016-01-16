/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/6809.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/6809.h"

#define CHECK_BYTES(a) \
  if (m6809_table[n].bytes != a) \
  { \
    print_error_internal(asm_context, __FILE__, __LINE__); \
    return -1; \
  }

#define CHECK_BYTES_16(a) \
  if (m6809_table_16[n].bytes != a) \
  { \
    print_error_internal(asm_context, __FILE__, __LINE__); \
    return -1; \
  }

struct _operand
{
  uint8_t count;
  uint8_t type;
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

enum
{
  REG_D,
  REG_X,
  REG_Y,
  REG_U,
  REG_S,
  REG_PC,
  REG_A,
  REG_B,
  REG_CC,
  REG_DP,
};

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

  while(aliases[n].name != NULL)
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
  switch(reg)
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

  memset(&operand, 0, sizeof(operand));

  do
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      operand.type = OPERAND_NONE;
      break;
    }

    if (get_register(token) != 0)
    {
      while(1)
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

        eat_operand(asm_context);
        operand.value = 0x00;
      }

      operand.type = OPERAND_DP_ADDRESS;
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

        eat_operand(asm_context);
        operand.value = 0xffff;
      }
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

        eat_operand(asm_context);
        operand.value = 0xffff;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
      if (IS_NOT_TOKEN(token, ','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (expect_token_s(asm_context, "x") != 0) { return -1; }
      //operand.type = OPERAND_REG;
      operand.value = REG_X;
    }
  } while(0);

  n = 0;
  while(1)
  {
    if (m6809_table[n].instr == NULL) { break; }

    if (strcmp(instr_case, m6809_table[n].instr) == 0)
    {
      matched = 1;

      switch(m6809_table[n].operand_type)
      {
        case M6809_OP_INHERENT:
        {
          if (operand.type == OPERAND_NONE)
          {
            CHECK_BYTES(1);
            add_bin8(asm_context, m6809_table[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case M6809_OP_IMMEDIATE:
        {
          if (operand.type != OPERAND_NUMBER) { break; }
          if (m6809_table[n].bytes == 2)
          {
            if (check_range(asm_context, "Immediate", operand.value, -128, 0xff) == -1) { return -1; }
            value8 = (uint8_t)operand.value;
            add_bin8(asm_context, m6809_table[n].opcode, IS_OPCODE);
            add_bin8(asm_context, value8, IS_OPCODE);
            return 2;
          }
            else
          if (m6809_table[n].bytes == 3)
          {
            if (check_range(asm_context, "Immediate", operand.value, -32768, 0xffff) == -1) { return -1; }
            value16 = (uint16_t)operand.value;
            add_bin8(asm_context, m6809_table[n].opcode, IS_OPCODE);
            add_bin16(asm_context, value16, IS_OPCODE);
            return 3;
          }

          break;
        }
        case M6809_OP_EXTENDED:
        {
          if (operand.type != OPERAND_ADDRESS) { break; }
          if (m6809_table[n].bytes == 3)
          {
            if (check_range(asm_context, "Address", operand.value, 0, 0xffff) == -1) { return -1; }
            value16 = (uint16_t)operand.value;
            add_bin8(asm_context, m6809_table[n].opcode, IS_OPCODE);
            add_bin16(asm_context, value16, IS_OPCODE);
            return 3;
          }

          break;
        }
        case M6809_OP_RELATIVE:
        {
          if (operand.type != OPERAND_ADDRESS) { break; }
          if (m6809_table[n].bytes == 2)
          {
            uint32_t offset = operand.value - (asm_context->address + 2);
            if (check_range(asm_context, "Offset", offset, -128, 127) == -1) { return -1; }
            add_bin8(asm_context, m6809_table[n].opcode, IS_OPCODE);
            add_bin8(asm_context, (uint8_t)offset, IS_OPCODE);
            return 2;
          }

          break;
        }
        case M6809_OP_DIRECT:
        {
          if (operand.type != OPERAND_DP_ADDRESS) { break; }
          if (m6809_table[n].bytes == 2)
          {
            if (check_range(asm_context, "Address", operand.value, 0, 0xff) == -1) { return -1; }
            add_bin8(asm_context, m6809_table[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operand.value, IS_OPCODE);
            return 2;
          }

          break;
        }
        case M6809_OP_STACK:
        {
          if (operand.type != OPERAND_REG_LIST ||
              operand.reg_list > 0xff) { break; }
          if (m6809_table[n].bytes == 2)
          {
            add_bin8(asm_context, m6809_table[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operand.reg_list, IS_OPCODE);
            return 2;
          }

          break;
        }
        case M6809_OP_TWO_REG:
        {
          if (operand.type != OPERAND_REG_LIST ||
              operand.count != 2)
          {
            break;
          }

          if (m6809_table[n].bytes == 2)
          {
            uint8_t src = get_reg_postbyte(operand.reg_src);
            uint8_t dst = get_reg_postbyte(operand.reg_dst);

            if (src == 0xff) { break; }
            if (dst == 0xff) { break; }

            src = (src << 4) | dst;

            add_bin8(asm_context, m6809_table[n].opcode, IS_OPCODE);
            add_bin8(asm_context, src, IS_OPCODE);
            return 2;
          }

          break;
        }
        case M6809_OP_INDEXED:
        {
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
  while(1)
  {
    if (m6809_table_16[n].instr == NULL) { break; }

    if (strcmp(instr_case, m6809_table_16[n].instr) == 0)
    {
      matched = 1;

      switch(m6809_table_16[n].operand_type)
      {
        case M6809_OP_INHERENT:
        {
          if (operand.type == OPERAND_NONE)
          {
            CHECK_BYTES_16(1);
            add_bin16(asm_context, m6809_table_16[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case M6809_OP_IMMEDIATE:
        {
          if (operand.type != OPERAND_NUMBER) { break; }
          if (m6809_table_16[n].bytes == 4)
          {
            if (check_range(asm_context, "Immediate", operand.value, -32768, 0xffff) == -1) { return -1; }
            value16 = (uint16_t)operand.value;
            add_bin16(asm_context, m6809_table_16[n].opcode, IS_OPCODE);
            add_bin16(asm_context, value16, IS_OPCODE);
            return 4;
          }

          break;
        }
        case M6809_OP_EXTENDED:
        {
          if (operand.type != OPERAND_ADDRESS) { break; }
          if (m6809_table_16[n].bytes == 4)
          {
            if (check_range(asm_context, "Address", operand.value, 0, 0xffff) == -1) { return -1; }
            value16 = (uint16_t)operand.value;
            add_bin16(asm_context, m6809_table_16[n].opcode, IS_OPCODE);
            add_bin16(asm_context, value16, IS_OPCODE);
            return 4;
          }

          break;
        }
        case M6809_OP_RELATIVE:
        {
          if (operand.type != OPERAND_ADDRESS) { break; }
          if (m6809_table_16[n].bytes == 4)
          {
            uint32_t offset = operand.value - (asm_context->address + 2);
            //if (check_range(asm_context, "Offset", offset, -32768, 32767) == -1) { return -1; }
            add_bin16(asm_context, m6809_table_16[n].opcode, IS_OPCODE);
            add_bin16(asm_context, (uint16_t)offset, IS_OPCODE);
            return 4;
          }

          break;
        }
        case M6809_OP_DIRECT:
        {
          if (operand.type != OPERAND_DP_ADDRESS) { break; }
          if (m6809_table_16[n].bytes == 3)
          {
            if (check_range(asm_context, "Address", operand.value, 0, 0xff) == -1) { return -1; }
            add_bin16(asm_context, m6809_table_16[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operand.value, IS_OPCODE);
            return 3;
          }

          break;
        }
        case M6809_OP_INDEXED:
        {
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
    printf("Error: Unknown operands combo for '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}



