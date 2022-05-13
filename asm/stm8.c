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
#include "asm/stm8.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "disasm/stm8.h"
#include "table/stm8.h"

enum
{
  NUM_SIZE_SHORT,
  NUM_SIZE_WORD,
  NUM_SIZE_EXTENDED,
  NUM_SIZE_UNKNOWN,
};

struct _operand
{
  uint8_t type;
  uint8_t reg;
  uint8_t num_size;
  int value;
};

int find_bigger_instruction(int n)
{
  switch (table_stm8_opcodes[n].type)
  {
    case OP_NUMBER8:
      if (table_stm8_opcodes[n+1].type == OP_NUMBER16)
      {
        return n + 1;
      }
      break;
    case OP_ADDRESS8:
      if (table_stm8_opcodes[n+1].type == OP_ADDRESS16)
      {
        return n + 1;
      }
      break;
    case OP_OFFSET8_INDEX_X:
      if (table_stm8_opcodes[n+1].type == OP_OFFSET16_INDEX_X)
      {
        return n + 1;
      }
      break;
    case OP_OFFSET8_INDEX_Y:
      if (table_stm8_opcodes[n+1].type == OP_OFFSET16_INDEX_Y)
      {
        return n + 1;
      }
      break;
    case OP_INDIRECT8:
      if (table_stm8_opcodes[n+1].type == OP_INDIRECT16)
      {
        return n + 1;
      }
      break;
    case OP_INDIRECT8_X:
      if (table_stm8_opcodes[n+1].type == OP_INDIRECT16_X)
      {
        return n + 1;
      }
      break;
    case OP_INDIRECT8_Y:
      if (table_stm8_opcodes[n+1].type == OP_INDIRECT16_Y)
      {
        return n + 1;
      }
      break;
    case OP_ADDRESS8_ADDRESS8:
      if (table_stm8_opcodes[n+1].type == OP_ADDRESS16_ADDRESS16)
      {
        return n + 1;
      }
      break;
    default:
      break;
  }

  return n;
}

static int ignore_expression(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  // Ignore all tokens until an ',' or ']' or EOL
  while (1)
  {
    token_type=tokens_get(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token,',') || IS_TOKEN(token,']') ||
        token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      tokens_push(asm_context, token, token_type);
      return 0;
    }
  }

  return -1;
}

static int get_num(struct _asm_context *asm_context, int instr_index, int *num, int *num_size)
{
  if (eval_expression(asm_context, num) != 0)
  {
    if (asm_context->pass == 1)
    {
      ignore_expression(asm_context);
      //*num_size = get_minimum_size(instr_index);
      *num_size = NUM_SIZE_UNKNOWN;
      memory_write(asm_context, asm_context->address, *num_size, asm_context->tokens.line);
      return 0;
    }

    print_error_illegal_expression(table_stm8[instr_index].instr, asm_context);
    return -1;
  }

  int override_size = memory_read(asm_context, asm_context->address);

  if (asm_context->pass == 2 && override_size != 0)
  {
    *num_size = override_size;
    return 0;
  }

  if (*num >= -128 && *num <= 0xff) { *num_size = NUM_SIZE_SHORT; return 0; }
  if (*num >= -32768 && *num <= 0xffff) { *num_size = NUM_SIZE_WORD; return 0; }
  if (*num >= -8388608 && *num <=0xffffff) { *num_size = NUM_SIZE_EXTENDED; return 0; }

  // FIXME - bad error message
  print_error_range("number", 0, 0xffffff, asm_context);

  return -1;
}

static int add_bin_void(struct _asm_context *asm_context, int n)
{
  int count = 1;

  if (table_stm8_opcodes[n].prefix != 0)
  {
    add_bin8(asm_context, table_stm8_opcodes[n].prefix, IS_OPCODE);
    count++;
  }
  add_bin8(asm_context, table_stm8_opcodes[n].opcode, IS_OPCODE);

  return count;
}

static int add_bin_num8(struct _asm_context *asm_context, int n, int num)
{
  int count = 2;

  if (table_stm8_opcodes[n].prefix != 0)
  {
    add_bin8(asm_context, table_stm8_opcodes[n].prefix, IS_OPCODE);
  }
  add_bin8(asm_context, table_stm8_opcodes[n].opcode, IS_OPCODE);
  add_bin8(asm_context, num, IS_OPCODE);

  return count;
}

static int add_bin_num16(struct _asm_context *asm_context, int n, int num)
{
  int count = 3;

  if (table_stm8_opcodes[n].prefix != 0)
  {
    add_bin8(asm_context, table_stm8_opcodes[n].prefix, IS_OPCODE);
  }
  add_bin8(asm_context, table_stm8_opcodes[n].opcode, IS_OPCODE);
  add_bin8(asm_context, (num >> 8) & 0xff, IS_OPCODE);
  add_bin8(asm_context, num & 0xff, IS_OPCODE);

  return count;
}

static int add_bin_num24(struct _asm_context *asm_context, int n, int num)
{
  int count = 4;

  if (table_stm8_opcodes[n].prefix != 0)
  {
    add_bin8(asm_context, table_stm8_opcodes[n].prefix, IS_OPCODE);
  }
  add_bin8(asm_context, table_stm8_opcodes[n].opcode, IS_OPCODE);
  add_bin8(asm_context, (num >> 16) & 0xff, IS_OPCODE);
  add_bin8(asm_context, (num >> 8) & 0xff, IS_OPCODE);
  add_bin8(asm_context, num & 0xff, IS_OPCODE);

  return count;
}

static int add_bin_bit(struct _asm_context *asm_context, int n, int num, int pos)
{
  int count = 3;

  if (table_stm8_opcodes[n].prefix != 0)
  {
    add_bin8(asm_context, table_stm8_opcodes[n].prefix, IS_OPCODE);
  }
  add_bin8(asm_context, table_stm8_opcodes[n].opcode | (pos << 1), IS_OPCODE);
  add_bin8(asm_context, (num >> 8) & 0xff, IS_OPCODE);
  add_bin8(asm_context, num & 0xff, IS_OPCODE);

  return count;
}

static int add_bin_bit_offset(struct _asm_context *asm_context, int n, int num, int pos, int offset)
{
  int count = 4;

  if (table_stm8_opcodes[n].prefix != 0)
  {
    add_bin8(asm_context, table_stm8_opcodes[n].prefix, IS_OPCODE);
  }
  add_bin8(asm_context, table_stm8_opcodes[n].opcode | (pos << 1), IS_OPCODE);
  add_bin8(asm_context, (num >> 8) & 0xff, IS_OPCODE);
  add_bin8(asm_context, num & 0xff, IS_OPCODE);
  add_bin8(asm_context, ((uint8_t)offset) & 0xff, IS_OPCODE);

  return count;
}

static int add_bin_num16_num8(struct _asm_context *asm_context, int n, int num1, int num2)
{
  int count = 4;

  if (table_stm8_opcodes[n].prefix != 0)
  {
    add_bin8(asm_context, table_stm8_opcodes[n].prefix, IS_OPCODE);
  }
  add_bin8(asm_context, table_stm8_opcodes[n].opcode, IS_OPCODE);
  add_bin8(asm_context, num2 & 0xff, IS_OPCODE);
  add_bin8(asm_context, (num1 >> 8) & 0xff, IS_OPCODE);
  add_bin8(asm_context, num1 & 0xff, IS_OPCODE);

  return count;
}

static int add_bin_num8_num8(struct _asm_context *asm_context, int n, int num1, int num2)
{
  int count = 3;

  if (table_stm8_opcodes[n].prefix != 0)
  {
    add_bin8(asm_context, table_stm8_opcodes[n].prefix, IS_OPCODE);
  }
  add_bin8(asm_context, table_stm8_opcodes[n].opcode, IS_OPCODE);
  add_bin8(asm_context, num2 & 0xff, IS_OPCODE);
  add_bin8(asm_context, num1 & 0xff, IS_OPCODE);

  return count;
}

static int add_bin_num16_num16(struct _asm_context *asm_context, int n, int num1, int num2)
{
  int count = 5;

  if (table_stm8_opcodes[n].prefix != 0)
  {
    add_bin8(asm_context, table_stm8_opcodes[n].prefix, IS_OPCODE);
    count++;
  }
  add_bin8(asm_context, table_stm8_opcodes[n].opcode, IS_OPCODE);
  add_bin8(asm_context, (num2 >> 8) & 0xff, IS_OPCODE);
  add_bin8(asm_context, num2 & 0xff, IS_OPCODE);
  add_bin8(asm_context, (num1 >> 8) & 0xff, IS_OPCODE);
  add_bin8(asm_context, num1 & 0xff, IS_OPCODE);

  return count;
}

int parse_instruction_stm8(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  char token[TOKENLEN];
  struct _operand operands[3];
  int operand_count;
  int has_unknown_size;
  int instr_enum;
  int instr_index;
  int token_type;
  int num_size;
  int n;

  lower_copy(instr_case, instr);
  operand_count = 0;
  memset(operands, 0, sizeof(operands));

  if (strcmp(instr_case, "sla") == 0)
  {
    // Alias SLA as SLL
    strcpy(instr_case, "sll");
  }
    else
  if (strcmp(instr_case, "slaw") == 0)
  {
    // Alias SLAW as SLLW
    strcpy(instr_case, "sllw");
  }

  // Find instruction
  n = 0;
  while (table_stm8[n].instr != NULL)
  {
    if (strcmp(instr_case, table_stm8[n].instr) == 0) { break; }
    n++;
  }

  if (table_stm8[n].instr == NULL)
  {
    print_error_unknown_instr(instr, asm_context);
    return -1;
  }

  instr_index = n;
  instr_enum = table_stm8[n].instr_enum;

  // Parse operands
  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (operand_count != 0)
    {
      if (IS_NOT_TOKEN(token,','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
    }

    if (strcasecmp(token,"A") == 0)
    {
      operands[operand_count].type = OP_REG_A;
    }
      else
    if (strcasecmp(token,"X") == 0)
    {
      operands[operand_count].type = OP_REG_X;
    }
      else
    if (strcasecmp(token,"Y") == 0)
    {
      operands[operand_count].type = OP_REG_Y;
    }
      else
    if (strcasecmp(token,"XL") == 0)
    {
      operands[operand_count].type = OP_REG_XL;
    }
      else
    if (strcasecmp(token,"YL") == 0)
    {
      operands[operand_count].type = OP_REG_YL;
    }
      else
    if (strcasecmp(token,"XH") == 0)
    {
      operands[operand_count].type = OP_REG_XH;
    }
      else
    if (strcasecmp(token,"YH") == 0)
    {
      operands[operand_count].type = OP_REG_YH;
    }
      else
    if (strcasecmp(token,"CC") == 0)
    {
      operands[operand_count].type = OP_REG_CC;
    }
      else
    if (strcasecmp(token,"SP") == 0)
    {
      operands[operand_count].type = OP_SP;
    }
      else
    if (IS_TOKEN(token,'#'))
    {
      if (get_num(asm_context, instr_index, &n, &num_size) != 0)
      {
        return -1;
      }

      switch (num_size)
      {
        case NUM_SIZE_SHORT:
          operands[operand_count].type = OP_NUMBER8; break;
        case NUM_SIZE_WORD:
          operands[operand_count].type = OP_NUMBER16; break;
        default:
          // FIXME - bad error message
          //print_error_range(instr, 0, 0xff, asm_context);
          //return -1;
          operands[operand_count].type = OP_NUMBER8; break;
          break;
      }

      operands[operand_count].value = n;
      operands[operand_count].num_size = num_size;
    }
      else
    if (IS_TOKEN(token,'('))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,'['))
      {
        if (get_num(asm_context, instr_index, &n, &num_size) != 0)
        {
          return -1;
        }

        operands[operand_count].value = n;
        operands[operand_count].num_size = num_size;

        uint8_t is_w = 1;
        uint8_t is_e = 0;
        uint8_t skip_case = 0;

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token,'.'))
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (strcasecmp(token, "e") == 0)
          {
            // REVIEW: Look over why these are set like this.
            is_w = 0;
            is_e = 1;
          }
            else
          if (strcasecmp(token, "w") != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
          else
        {
          tokens_push(asm_context, token, token_type);
        }

        if (expect_token(asm_context, ']') == -1) { return -1; }
        if (expect_token(asm_context, ',') == -1) { return -1; }

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (strcasecmp(token,"X") == 0)
        {
          operands[operand_count].type = OP_INDIRECT8_X;

          if (is_e == 1)
          {
            operands[operand_count].type = OP_INDIRECT16_E_X;
            skip_case = 1;
            if (check_range(asm_context, "address", operands[operand_count].value, 0, 0xffff) == -1) { return -1; }
          }
        }
          else
        if (strcasecmp(token,"Y") == 0)
        {
          operands[operand_count].type = OP_INDIRECT8_Y;

          if (is_e == 1)
          {
            operands[operand_count].type = OP_INDIRECT16_E_Y;
            skip_case = 1;
            if (check_range(asm_context, "address", operands[operand_count].value, 0, 0xffff) == -1) { return -1; }
          }
        }
          else
        {
          print_error_unexp(token, asm_context);
        }

        if (skip_case == 0)
        {
          switch (num_size)
          {
            case NUM_SIZE_SHORT:
              if (is_w == 0 && is_e != 1)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
              break;
            case NUM_SIZE_WORD:
              operands[operand_count].type++;
              if (is_w == 0) { operands[operand_count].type++; }
              break;
            default:
              // FIXME - bad error message
              //print_error_range(instr, 0, 0xffff, asm_context);
              //return -1;
              break;
          }
        }
      }
        else
      if (strcasecmp(token,"X") == 0)
      {
        operands[operand_count].type = OP_INDEX_X;
      }
        else
      if (strcasecmp(token,"Y") == 0)
      {
        operands[operand_count].type = OP_INDEX_Y;
      }
        else
      {
        tokens_push(asm_context, token, token_type);

        if (get_num(asm_context, instr_index, &n, &num_size) != 0)
        {
          return -1;
        }

        operands[operand_count].value = n;
        operands[operand_count].num_size = num_size;

        if (expect_token(asm_context, ',') == -1) { return -1; }

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (strcasecmp(token,"X") == 0)
        {
          operands[operand_count].type = OP_OFFSET8_INDEX_X;
        }
          else
        if (strcasecmp(token,"Y") == 0)
        {
          operands[operand_count].type = OP_OFFSET8_INDEX_Y;
        }
          else
        if (strcasecmp(token,"SP") == 0)
        {
          operands[operand_count].type = OP_OFFSET8_INDEX_SP;
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        switch (num_size)
        {
          case NUM_SIZE_SHORT: break;
          case NUM_SIZE_WORD: operands[operand_count].type++; break;
          case NUM_SIZE_EXTENDED: operands[operand_count].type += 2; break;
          default:
            // FIXME - bad error message
            //print_error_range(instr, 0, 0xff, asm_context);
            //return -1;
            break;
        }
      }

      if (expect_token(asm_context, ')') == -1) { return -1; }
    }
      else
    if (IS_TOKEN(token,'['))
    {
      if (get_num(asm_context, instr_index, &n, &num_size) != 0)
      {
        return -1;
      }

      switch (num_size)
      {
        case NUM_SIZE_SHORT:
          operands[operand_count].type = OP_INDIRECT8; break;
        case NUM_SIZE_WORD:
          operands[operand_count].type = OP_INDIRECT16; break;
        default:
          // FIXME - bad error message
          //print_error_range(instr, 0, 0xff, asm_context);
          //return -1;
          operands[operand_count].type = OP_INDIRECT8; break;
          break;
      }

      operands[operand_count].value = n;
      operands[operand_count].num_size = num_size;

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'.'))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (strcasecmp(token, "e") == 0)
        {
          operands[operand_count].type = OP_INDIRECT16_E;
        }
          else
        if (strcasecmp(token, "w") != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
      if (expect_token(asm_context, ']') == -1) { return -1; }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (get_num(asm_context, instr_index, &n, &num_size) != 0)
      {
        return -1;
      }

      switch (num_size)
      {
        case NUM_SIZE_SHORT:
          operands[operand_count].type = OP_ADDRESS8; break;
        case NUM_SIZE_WORD:
          operands[operand_count].type = OP_ADDRESS16; break;
        case NUM_SIZE_EXTENDED:
          operands[operand_count].type = OP_ADDRESS24; break;
        default:
          // FIXME - bad error message
          //print_error_range(instr, 0, 0xff, asm_context);
          //return -1;
          operands[operand_count].type = OP_ADDRESS8; break;
      }

      operands[operand_count].value = n;
      operands[operand_count].num_size = num_size;
    }

    operand_count++;
  }

  // Check for an unknown size
  has_unknown_size = 0;
  for (n = 0; n < operand_count; n ++)
  {
    if (operands[n].num_size == NUM_SIZE_UNKNOWN)
    {
      has_unknown_size = 1;
    }
  }

  // Get opcodes
  n = 0;
  while (table_stm8_opcodes[n].instr_enum != STM8_NONE)
  {
    if (table_stm8_opcodes[n].instr_enum == instr_enum)
    {
      if (table_stm8_opcodes[n].dest != OP_NONE &&
          operands[0].type != table_stm8_opcodes[n].dest)
      {
        n++;
        continue;
      }

      if (table_stm8_opcodes[n].src != OP_NONE &&
          operands[1].type != table_stm8_opcodes[n].src)
      {
        n++;
        continue;
      }

      if (has_unknown_size == 1)
      {
        // Check if the next instruction is a bigger version
        if (table_stm8_opcodes[n].instr_enum ==
            table_stm8_opcodes[n+1].instr_enum)
        {
          n = find_bigger_instruction(n);
        }
      }

      switch (table_stm8_opcodes[n].type)
      {
        case OP_NONE:
        {
          if (operand_count == 0)
          {
            return add_bin_void(asm_context, n);
          }
          break;
        }
        case OP_NUMBER8:
        {
          if (operand_count <= 2 && operands[0].type == OP_NUMBER8)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_NUMBER8)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_NUMBER16:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_NUMBER16 ||
               operands[0].type == OP_NUMBER8))
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 &&
              (operands[1].type == OP_NUMBER16 ||
               operands[1].type == OP_NUMBER8))
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_ADDRESS8:
        {
          if (operand_count <= 2 && operands[0].type == OP_ADDRESS8)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_ADDRESS8)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_ADDRESS16:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_ADDRESS16 ||
               operands[0].type == OP_ADDRESS8))
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 &&
              (operands[1].type == OP_ADDRESS16 ||
               operands[1].type == OP_ADDRESS8))
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_ADDRESS24:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_ADDRESS24 ||
               operands[0].type == OP_ADDRESS16 ||
               operands[0].type == OP_ADDRESS8))
          {
            return add_bin_num24(asm_context, n, operands[0].value);
          }
          if (operand_count == 2 &&
              (operands[1].type == OP_ADDRESS24 ||
               operands[1].type == OP_ADDRESS16 ||
               operands[1].type == OP_ADDRESS8))
          {
            return add_bin_num24(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDEX_X:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_INDEX_X ||
               operands[1].type == OP_INDEX_X))
          {
            return add_bin_void(asm_context, n);
          }
          break;
        }
        case OP_OFFSET8_INDEX_X:
        {
          if (operand_count <= 2 && operands[0].type == OP_OFFSET8_INDEX_X)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_OFFSET8_INDEX_X)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_OFFSET16_INDEX_X:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_OFFSET16_INDEX_X ||
               operands[0].type == OP_OFFSET8_INDEX_X))
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 &&
              (operands[1].type == OP_OFFSET16_INDEX_X ||
               operands[1].type == OP_OFFSET8_INDEX_X))
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_OFFSET24_INDEX_X:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_OFFSET24_INDEX_X ||
               operands[0].type == OP_OFFSET16_INDEX_X ||
               operands[0].type == OP_OFFSET8_INDEX_X))
          {
            return add_bin_num24(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 &&
              (operands[1].type == OP_OFFSET24_INDEX_X ||
               operands[1].type == OP_OFFSET16_INDEX_X ||
               operands[1].type == OP_OFFSET8_INDEX_X))
          {
            return add_bin_num24(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDEX_Y:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_INDEX_Y ||
               operands[1].type == OP_INDEX_Y))
          {
            return add_bin_void(asm_context, n);
          }
          break;
        }
        case OP_OFFSET8_INDEX_Y:
        {
          if (operand_count <= 2 && operands[0].type == OP_OFFSET8_INDEX_Y)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_OFFSET8_INDEX_Y)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_OFFSET16_INDEX_Y:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_OFFSET16_INDEX_Y ||
               operands[0].type == OP_OFFSET8_INDEX_Y))
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 &&
              (operands[1].type == OP_OFFSET16_INDEX_Y ||
               operands[1].type == OP_OFFSET8_INDEX_Y))
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_OFFSET24_INDEX_Y:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_OFFSET24_INDEX_Y ||
               operands[0].type == OP_OFFSET16_INDEX_Y ||
               operands[0].type == OP_OFFSET8_INDEX_Y))
          {
            return add_bin_num24(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 &&
              (operands[1].type == OP_OFFSET24_INDEX_Y ||
               operands[1].type == OP_OFFSET16_INDEX_Y ||
               operands[1].type == OP_OFFSET8_INDEX_Y))
          {
            return add_bin_num24(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_OFFSET8_INDEX_SP:
        {
          if (operand_count <= 2 && operands[0].type == OP_OFFSET8_INDEX_SP)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_OFFSET8_INDEX_SP)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT8:
        {
          if (operand_count <= 2 && operands[0].type == OP_INDIRECT8)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_INDIRECT8)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT16:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_INDIRECT16 ||
               operands[0].type == OP_INDIRECT8))
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 &&
              (operands[1].type == OP_INDIRECT16 ||
               operands[1].type == OP_INDIRECT8))
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT16_E:
        {
          if (operand_count <= 2 && operands[0].type == OP_INDIRECT16_E)
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_INDIRECT16_E)
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT8_X:
        {
          if (operand_count <= 2 && operands[0].type == OP_INDIRECT8_X)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_INDIRECT8_X)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT16_X:
        {
          if (operand_count <= 2 &&
              (operands[0].type == OP_INDIRECT16_X ||
               operands[0].type == OP_INDIRECT8_X))
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 &&
              (operands[1].type == OP_INDIRECT16_X ||
               operands[1].type == OP_INDIRECT8_X))
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT16_E_X:
        {
          if (operand_count <= 2 && operands[0].type == OP_INDIRECT16_E_X)
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_INDIRECT16_E_X)
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT8_Y:
        {
          if (operand_count <= 2 && operands[0].type == OP_INDIRECT8_Y)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_INDIRECT8_Y)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT16_E_Y:
        {
          if (operand_count <= 2 && operands[0].type == OP_INDIRECT16_E_Y)
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OP_INDIRECT16_E_Y)
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_ADDRESS_BIT:
        {
          if (operand_count == 2 &&
              (operands[0].type == OP_ADDRESS8 ||
               operands[0].type == OP_ADDRESS16) &&
              operands[1].type == OP_NUMBER8)
          {
            if (operands[1].value < 0 || operands[1].value > 7)
            {
              print_error_range("Bit", 0, 7, asm_context);
              return -1;
            }

            return add_bin_bit(asm_context, n, operands[0].value, operands[1].value);
          }
          break;
        }
        case OP_ADDRESS_BIT_LOOP:
        {
          if (operand_count == 3 &&
              (operands[0].type == OP_ADDRESS8 ||
               operands[0].type == OP_ADDRESS16 ||
               operands[0].type == OP_ADDRESS24) &&
              operands[1].type == OP_NUMBER8 &&
              (operands[2].type == OP_ADDRESS8 ||
               operands[2].type == OP_ADDRESS16))
          {
            if (operands[1].value < 0 || operands[1].value > 7)
            {
              print_error_range("Bit", 0, 7, asm_context);
              return -1;
            }

            int address = asm_context->address + 4;
            if (table_stm8_opcodes[n].prefix != 0) { address++; }
            int offset = (asm_context->pass == 1) ? 0 : operands[2].value - address;

            if (offset < -128 || offset > 127)
            {
              print_error_range("Offset", -128, 127, asm_context);
              return -1;
            }

            return add_bin_bit_offset(asm_context, n, operands[0].value, operands[1].value, offset);
          }
          break;
        }
        case OP_RELATIVE:
        {
          if (operand_count == 1 &&
              (operands[0].type == OP_ADDRESS8 ||
               operands[0].type == OP_ADDRESS16 ||
               operands[0].type == OP_ADDRESS24))
          {
            int address = asm_context->address + 2;
            if (table_stm8_opcodes[n].prefix != 0) { address++; }
            int offset = operands[0].value - address;

            if (asm_context->pass == 1)
            {
              offset = 0;
            }
              else
            {
              if (offset < -128 || offset > 127)
              {
                print_error_range("Offset", -128, 127, asm_context);
                return -1;
              }
            }

            return add_bin_num8(asm_context, n, offset);
          }
          break;
        }
        case OP_SINGLE_REGISTER:
        {
          if (operand_count == 1)
          {
            return add_bin_void(asm_context, n);
          }
          break;
        }
        case OP_TWO_REGISTERS:
        {
          if (operand_count == 2)
          {
            return add_bin_void(asm_context, n);
          }
          break;
        }
        case OP_ADDRESS16_NUMBER8:
        {
          if (operand_count == 2 &&
              (operands[0].type == OP_ADDRESS8 ||
               operands[0].type == OP_ADDRESS16) &&
              operands[1].type == OP_NUMBER8)
          {
            return add_bin_num16_num8(asm_context, n, operands[0].value, operands[1].value);
          }
          break;
        }
        case OP_ADDRESS8_ADDRESS8:
        {
          if (operand_count == 2 &&
              operands[0].type == OP_ADDRESS8 &&
              operands[1].type == OP_ADDRESS8)
          {
            return add_bin_num8_num8(asm_context, n, operands[0].value, operands[1].value);
          }
          break;
        }
        case OP_ADDRESS16_ADDRESS16:
        {
          if (operand_count == 2 &&
              (operands[0].type == OP_ADDRESS8 ||
               operands[0].type == OP_ADDRESS16) &&
              (operands[1].type == OP_ADDRESS8 ||
               operands[1].type == OP_ADDRESS16))
          {
            return add_bin_num16_num16(asm_context, n, operands[0].value, operands[1].value);
          }
          break;
        }
        default:
          printf("Internal error %s:%d\n", __FILE__, __LINE__);
          return -1;
          break;
      }
    }

    n++;
  }

  print_error_unknown_operand_combo(instr, asm_context);

  return -1;
}

