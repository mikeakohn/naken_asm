/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm_common.h"
#include "asm_stm8.h"
#include "assembler.h"
#include "disasm_stm8.h"
#include "tokens.h"
#include "eval_expression.h"
#include "table_stm8.h"

enum
{
  OPERAND_NONE,
  OPERAND_REG_A,
  OPERAND_REG_X,
  OPERAND_REG_Y,
  OPERAND_SP,
  OPERAND_NUMBER8,             // #$10
  OPERAND_NUMBER16,            // #$1000
  OPERAND_ADDRESS8,            // $10
  OPERAND_ADDRESS16,           // $1000
  OPERAND_ADDRESS24,           // REVIEW: Is this needed?
  OPERAND_INDEX_X,             // (X)
  OPERAND_INDEX_Y,             // (Y)
  OPERAND_OFFSET8_INDEX_X,     // ($10, X)
  OPERAND_OFFSET16_INDEX_X,    // ($1000,X)
  OPERAND_OFFSET8_INDEX_Y,     // ($10,Y)
  OPERAND_OFFSET16_INDEX_Y,    // ($1000,Y)
  OPERAND_OFFSET8_INDEX_SP,    // ($10,SP)
  OPERAND_OFFSET16_INDEX_SP,   // REVIEW: Is this needed?
  OPERAND_INDIRECT8,           // [$10.w]
  OPERAND_INDIRECT16,          // [$1000.w]
  OPERAND_INDIRECT8_X,         // ([$10.w],X)
  OPERAND_INDIRECT16_X,        // ([$1000.w],X)
  OPERAND_INDIRECT8_Y,         // ([$10.w].Y)
};

enum
{
  NUM_SIZE_SHORT,
  NUM_SIZE_WORD,
  NUM_SIZE_EXTENDED,
};

struct _operand
{
  uint8_t type;
  uint8_t reg;
  int value;
};

static int get_num(struct _asm_context *asm_context, char *instr, int *num, int *num_size)
{
  if (eval_expression(asm_context, num) != 0)
  {
    print_error_illegal_expression(instr, asm_context);
    return -1;
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

int parse_instruction_stm8(struct _asm_context *asm_context, char *instr)
{
char instr_case[TOKENLEN];
char token[TOKENLEN];
struct _operand operands[2];
int operand_count;
int instr_enum;
int token_type;
int num_size;
int n;

  lower_copy(instr_case, instr);
  operand_count = 0;
  memset(operands, 0, sizeof(operands));

  // Find instruction
  n = 0;
  while(table_stm8[n].instr != NULL)
  {
    if (strcmp(instr_case, table_stm8[n].instr) == 0) { break; }
    n++;
  }

  if (table_stm8[n].instr == NULL)
  {
    print_error_unknown_instr(instr, asm_context);
    return -1;
  }

  instr_enum = table_stm8[n].id;

  // Parse operands
  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    { 
      break;
    }

    if (operand_count == 2)
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
      operands[operand_count].type = OPERAND_REG_A;
    }
      else
    if (strcasecmp(token,"X") == 0)
    {
      operands[operand_count].type = OPERAND_REG_X;
    }
      else
    if (strcasecmp(token,"Y") == 0)
    {
      operands[operand_count].type = OPERAND_REG_Y;
    }
      else
    if (strcasecmp(token,"SP") == 0)
    {
      operands[operand_count].type = OPERAND_SP;
    }
      else
    if (IS_TOKEN(token,'#'))
    {
      if (get_num(asm_context, instr, &n, &num_size) != 0)
      {
        return -1;
      }

      switch(num_size)
      {
        case NUM_SIZE_SHORT:
          operands[operand_count].type = OPERAND_NUMBER8; break;
        case NUM_SIZE_WORD:
          operands[operand_count].type = OPERAND_NUMBER16; break;
        default:
           // FIXME - bad error message
           print_error_range(instr, 0, 0xff, asm_context);
           return -1;
      }

      operands[operand_count].value = n;
    }
      else
    if (IS_TOKEN(token,'('))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,'['))
      {
        if (get_num(asm_context, instr, &n, &num_size) != 0)
        {
          return -1;
        }

        operands[operand_count].value = n;

        if (expect_token(asm_context, '.') == -1) { return -1; }
        if (expect_token(asm_context, 'w') == -1) { return -1; }
        if (expect_token(asm_context, ']') == -1) { return -1; }
        if (expect_token(asm_context, ',') == -1) { return -1; }

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (strcasecmp(token,"X") == 0)
        {
          operands[operand_count].type = OPERAND_INDIRECT8_X;
        }
          else
        if (strcasecmp(token,"Y") == 0)
        {
          operands[operand_count].type = OPERAND_INDIRECT8_Y;
        }
          else
        {
          print_error_unexp(token, asm_context);
        }

        switch(num_size)
        {
          case NUM_SIZE_SHORT: break;
          case NUM_SIZE_WORD: operands[operand_count].type++; break;
          default:
             // FIXME - bad error message
             print_error_range(instr, 0, 0xff, asm_context);
             return -1;
        }
      }
        else
      if (strcasecmp(token,"X") == 0)
      {
        operands[operand_count].type = OPERAND_INDEX_X;
      }
        else
      if (strcasecmp(token,"Y") == 0)
      {
        operands[operand_count].type = OPERAND_INDEX_Y;
      }
        else
      {
        tokens_push(asm_context, token, token_type);

        if (get_num(asm_context, instr, &n, &num_size) != 0)
        {
          return -1;
        }

        operands[operand_count].value = n;

        if (expect_token(asm_context, ',') == -1) { return -1; }

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (strcasecmp(token,"X") == 0)
        {
          operands[operand_count].type = OPERAND_OFFSET8_INDEX_X;
        }
          else
        if (strcasecmp(token,"Y") == 0)
        {
          operands[operand_count].type = OPERAND_OFFSET8_INDEX_Y;
        }
          else
        if (strcasecmp(token,"SP") == 0)
        {
          operands[operand_count].type = OPERAND_OFFSET8_INDEX_SP;
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        switch(num_size)
        {
          case NUM_SIZE_SHORT: break;
          case NUM_SIZE_WORD: operands[operand_count].type++; break;
          default:
             // FIXME - bad error message
             print_error_range(instr, 0, 0xff, asm_context);
             return -1;
        }
      }

      if (expect_token(asm_context, ')') == -1) { return -1; }
    }
      else
    if (IS_TOKEN(token,'['))
    {
      if (get_num(asm_context, instr, &n, &num_size) != 0)
      {
        return -1;
      }

      switch(num_size)
      {
        case NUM_SIZE_SHORT:
          operands[operand_count].type = OPERAND_INDIRECT8; break;
        case NUM_SIZE_WORD:
          operands[operand_count].type = OPERAND_INDIRECT16; break;
        default:
           // FIXME - bad error message
           print_error_range(instr, 0, 0xff, asm_context);
           return -1;
      }

      operands[operand_count].value = n;

      if (expect_token(asm_context, '.') == -1) { return -1; }
      if (expect_token(asm_context, 'w') == -1) { return -1; }
      if (expect_token(asm_context, ']') == -1) { return -1; }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (get_num(asm_context, instr, &n, &num_size) != 0)
      {
        return -1;
      }

      switch(num_size)
      {
        case NUM_SIZE_SHORT:
          operands[operand_count].type = OPERAND_ADDRESS8; break;
        case NUM_SIZE_WORD:
          operands[operand_count].type = OPERAND_ADDRESS16; break;
        default:
           // FIXME - bad error message
           print_error_range(instr, 0, 0xff, asm_context);
           return -1;
      }

      operands[operand_count].value = n;
    }

    operand_count++;
  }

  // Get opcodes
  n = 0;
  while(table_stm8_opcodes[n].instr_enum != STM8_NONE)
  {
    if (table_stm8_opcodes[n].instr_enum == instr_enum)
    {
      if (table_stm8_opcodes[n].dest != REG_NONE &&
          operands[0].type != table_stm8_opcodes[n].dest)
      {
        n++;
        continue;
      }

      if (table_stm8_opcodes[n].src != REG_NONE &&
          operands[0].type != table_stm8_opcodes[n].src)
      {
        n++;
        continue;
      }

      switch(table_stm8_opcodes[n].type)
      {
        case OP_NUMBER:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_NUMBER8)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_NUMBER8)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_ADDRESS8:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_ADDRESS8)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_ADDRESS8)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_ADDRESS16:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_ADDRESS16)
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_ADDRESS16)
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDEX_X:
        {
          if (operand_count == 2 &&
              (operands[0].type == OPERAND_INDEX_X ||
               operands[1].type == OPERAND_INDEX_X))
          {
            return add_bin_void(asm_context, n);
          }
          break;
        }
        case OP_OFFSET8_INDEX_X:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_OFFSET8_INDEX_X)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_OFFSET8_INDEX_X)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_OFFSET16_INDEX_X:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_OFFSET16_INDEX_X)
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_OFFSET16_INDEX_X)
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDEX_Y:
        {
          if (operand_count == 2 &&
              (operands[0].type == OPERAND_INDEX_Y ||
               operands[1].type == OPERAND_INDEX_Y))
          {
            return add_bin_void(asm_context, n);
          }
          break;
        }
        case OP_OFFSET8_INDEX_Y:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_OFFSET8_INDEX_Y)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_OFFSET8_INDEX_Y)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_OFFSET16_INDEX_Y:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_OFFSET16_INDEX_Y)
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_OFFSET16_INDEX_Y)
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_OFFSET8_INDEX_SP:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_OFFSET8_INDEX_SP)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_OFFSET8_INDEX_SP)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT8:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_INDIRECT8)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_INDIRECT8)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT16:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_INDIRECT16)
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_INDIRECT16)
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT8_X:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_INDIRECT8_X)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_INDIRECT8_X)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT16_X:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_INDIRECT16_X)
          {
            return add_bin_num16(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_INDIRECT16_X)
          {
            return add_bin_num16(asm_context, n, operands[1].value);
          }
          break;
        }
        case OP_INDIRECT8_Y:
        {
          if (operand_count == 2 && operands[0].type == OPERAND_INDIRECT8_Y)
          {
            return add_bin_num8(asm_context, n, operands[0].value);
          }
            else
          if (operand_count == 2 && operands[1].type == OPERAND_INDIRECT8_Y)
          {
            return add_bin_num8(asm_context, n, operands[1].value);
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

