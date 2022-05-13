/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn, Joe Davisson
 *
 * 65816 by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/common.h"
#include "asm/65816.h"
#include "common/assembler.h"
#include "disasm/65816.h"
#include "common/tokens.h"
#include "common/eval_expression.h"

#include "table/65816.h"

#define GET_TOKEN() (token_type = tokens_get(asm_context, token, TOKENLEN))

static int get_num(
  struct _asm_context *asm_context,
  char *token,
  int *token_type,
  int *num,
  int *size)
{
  char modifier = 0;

  // check for modifiers
  if (IS_TOKEN(token, '<'))
  {
    modifier = '<';
  }
  else if (IS_TOKEN(token, '>'))
  {
    modifier = '>';
  }
  else if (IS_TOKEN(token, '^'))
  {
    modifier = '^';
  }
  else
  {
    tokens_push(asm_context, token, *token_type);
  }

  // obtain number
  if (eval_expression(asm_context, num) != 0)
  {
    if (asm_context->pass == 1)
    {
      ignore_operand(asm_context);
    }
    else
    {
      return -1;
    }
  }

  // extract single byte
  if (modifier == '<')
  {
    *num &= 0xff;
    *size = 8;
  }
  else if (modifier == '>')
  {
    *num >>= 8;
    *num &= 0xff;
    *size = 8;
  }
  else if (modifier == '^')
  {
    *num >>= 16;
    *num &= 0xff;
    *size = 8;
  }

  return 0;
}

static int get_address(
  struct _asm_context *asm_context,
  char *token,
  int *token_type,
  int *num,
  int *size)
{
  char modifier = 0;
  int worst_case = 0;

  // check for modifiers
  if (IS_TOKEN(token, '<'))
  {
    modifier = '<';
  }
  else if (IS_TOKEN(token, '!'))
  {
    modifier = '!';
  }
  else if (IS_TOKEN(token, '>'))
  {
    modifier = '>';
  }
  else
  {
    tokens_push(asm_context, token, *token_type);
  }

  // obtain address
  if (eval_expression(asm_context, num) != 0)
  {
    if (asm_context->pass == 1)
    {
      ignore_operand(asm_context);
    }
    else
    {
      return -1;
    }

    // Store a flag in this address to remind on pass 2 that this
    // instruction can't use zero page.
    memory_write(asm_context, asm_context->address, 1, asm_context->tokens.line);
    worst_case = 1;
  }

  // On pass 2, figure out if the size of the operand was unknown in pass 1
  worst_case = memory_read(asm_context, asm_context->address);

  // try to guess addressing mode if one hasn't been forced
  if (*size == 0)
  {
    *size = 8;

    if (*num > 0xff) { *size = 16; }

    if (*num > 0xffff)
    {
      if(worst_case == 1)
      {
        printf("Use jml/jsl aliases or .l modifier to force 24-bit addressing.\n");
        return -1;
      }
      else
      {
        *size = 24;
      }
    }

    if (*size == 8 && worst_case == 1) { *size = 16; }
  }

  if (modifier == '<')
  {
    // force direct-page mode
    *num &= 0xff;
    *size = 8;
  }
  else if (modifier == '!')
  {
    // force absolute mode
    *num &= 0xffff;
    *size = 16;
  }
  else if (modifier == '>')
  {
    // force absolute-long mode
    *num &= 0xffffff;
    *size = 24;
  }

  return 0;
}

extern struct _table_65816 table_65816[];
extern struct _table_65816_opcodes table_65816_opcodes[];

// bytes for each addressing mode
static int op_bytes[] =
{
  1, 2, 3, 2, 3, 4, 2, 2, 3, 3, 4, 2,
  2, 3, 3, 2, 3, 2, 2, 3, 2, 3, 2, 2
};

int parse_directive_65816(
  struct _asm_context *asm_context,
  const char *directive)
{
  return 1;
}

int parse_instruction_65816(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  char temp[256];
  int token_type = 0;
  int opcode = 0;
  int op = 0;
  int instr_enum = 0;
  int num = 0;
  int size = 0;
  int bytes = 0;
  int i = 0;
  int src = 0, dst = 0;

  // make lowercase
  lower_copy(instr_case, instr);

  // get instruction from string
  instr_enum = -1;

  if (strcmp(instr_case, "jml") == 0)
  {
    instr_enum = M65816_JMP;
    op = OP_NONE;
    size = 24;
  }
  else if (strcmp(instr_case, "jsl") == 0)
  {
    instr_enum = M65816_JSR;
    op = OP_NONE;
    size = 24;
  }
  else
  {
    // search instruction table
    for (i = 0; i < 90; i++)
    {
      if (strcmp(instr_case, table_65816[i].name) == 0)
      {
        instr_enum = i;
        break;
      }
    }

    // no matching instruction
    if (instr_enum == -1)
    {
      printf("No matching instruction for \"%s\".\n", instr_case);
      return -1;
    }

    // get default addressing mode
    op = table_65816[instr_enum].op;

    // start with unknown number/address size
    size = 0;
  }

  // parse
  while (1)
  {
    GET_TOKEN();
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

    // dot suffix
    if (IS_TOKEN(token, '.'))
    {
      GET_TOKEN();
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

      if (IS_TOKEN(token, 'b') || IS_TOKEN(token, 'B'))
      {
        size = 8;
      }
      else if (IS_TOKEN(token, 'w') || IS_TOKEN(token, 'W'))
      {
        size = 16;
      }
      else if (IS_TOKEN(token, 'l') || IS_TOKEN(token, 'L'))
      {
        size = 24;
      }
      else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      GET_TOKEN();
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
    }

    if (op == OP_RELATIVE)
    {
      if (IS_TOKEN(token, '#'))
      {
        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (get_num(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        if (num < -128 || num > 127)
        {
          print_error("8-bit signed constant out of range.", asm_context);
          return -1;
        }

        num = (uint8_t)num;
      }
      else
      {
        tokens_push(asm_context, token, token_type);

        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (get_num(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        if (asm_context->pass == 2)
        {
          // calculate branch offset, need to add 2 to current
          // address, since thats where the program counter would be
          num -= (asm_context->address + 2);

          if (num < -128 || num > 127)
          {
            print_error("Relative branch out of range", asm_context);
            return -1;
          }

          num = (uint8_t)num;
        }
      }
    }
    else if (op == OP_RELATIVE_LONG)
    {
      if (IS_TOKEN(token, '#'))
      {
        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (get_num(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        if (num < -32768 || num > 32767)
        {
          print_error("16-bit signed constant out of range.", asm_context);
          return -1;
        }

        num = (uint16_t)num;
      }
      else
      {
        tokens_push(asm_context, token, token_type);

        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (get_num(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        if (asm_context->pass == 2)
        {
          // calculate branch offset, need to add 3 to current
          // address, since thats where the program counter would be
          num -= (asm_context->address + 3);

          if (num < -32768 || num > 32767)
          {
            print_error("Relative long branch out of range", asm_context);
            return -1;
          }

          num = (uint16_t)num;
        }
      }
    }
    else if (op == OP_BLOCK_MOVE)
    {
      tokens_push(asm_context, token, token_type);

      GET_TOKEN();
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

      if (get_address(asm_context, token, &token_type, &num, &size) == -1)
      {
        return -1;
      }

      src = num;

      if (src < 0 || src > 0xff)
      {
        print_error("Block move source address out of range.", asm_context);
        return -1;
      }

      GET_TOKEN();
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

      if (IS_NOT_TOKEN(token, ','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      GET_TOKEN();
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

      tokens_push(asm_context, token, token_type);

      GET_TOKEN();
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

      if (get_address(asm_context, token, &token_type, &num, &size) == -1)
      {
        return -1;
      }

      dst = num;

      if (dst < 0 || dst > 0xff)
      {
        print_error("Block move destination address out of range.", asm_context);
        return -1;
      }

      num = src + (dst << 8);
    }
    else
    {
      if (IS_TOKEN(token, '#'))
      {
        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (get_num(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        op = OP_IMMEDIATE16;

        // value was forced with .b, .w, or .l
	if (size == 8)
        {
          if (num < -128 || num > 0xff)
          {
            print_error("8-bit constant out of range.", asm_context);
            return -1;
          }

          num = (uint8_t)num;
        }
        else if (size == 16)
        {
          if (num < -32768 || num > 0xffff)
          {
            print_error("16-bit constant out of range.", asm_context);
            return -1;
          }

          num = (uint16_t)num;
        }
        else if (size == 24)
        {
          print_error("Cannot force long value in immediate mode.", asm_context);
          return -1;
        }
        else
        {
          // otherwise default to 16-bit
          if (num < -32768 || num > 0xffff)
          {
            print_error("16-bit constant out of range.", asm_context);
            return -1;
          }

          num = (uint16_t)num;
        }
      }
      else if (IS_TOKEN(token, '('))
      {
        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (get_address(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (IS_TOKEN(token, ','))
        {
          GET_TOKEN();
          if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

          if (IS_TOKEN(token, 'x') || IS_TOKEN(token, 'X'))
          {
            GET_TOKEN();
            if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

            if (IS_TOKEN(token, ')'))
            {
              if (num < 0 || num > 0xffff)
              {
                print_error("Address out of range.", asm_context);
                return -1;
              }

              op = num > 0xff ? OP_X_INDIRECT16 : OP_X_INDIRECT8;
            }
            else
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else if (IS_TOKEN(token, 's') || IS_TOKEN(token, 'S'))
          {
            op = OP_SP_INDIRECT_Y;

            GET_TOKEN();
            if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

            if (IS_NOT_TOKEN(token, ')'))
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            GET_TOKEN();
            if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

            if (IS_NOT_TOKEN(token, ','))
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            GET_TOKEN();
            if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

            if (IS_NOT_TOKEN(token, 'y') && IS_NOT_TOKEN(token, 'Y'))
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
        else if (IS_TOKEN(token, ')'))
        {
          op = OP_INDIRECT8;

          GET_TOKEN();
          if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

          if (IS_TOKEN(token, ','))
          {
            GET_TOKEN();
            if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

            if (IS_TOKEN(token, 'y') || IS_TOKEN(token, 'Y'))
            {
              op = OP_INDIRECT8_Y;
            }
            else
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
        else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
      else if (IS_TOKEN(token, '['))
      {
        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (get_address(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (IS_TOKEN(token, ']'))
        {
          op = OP_INDIRECT8_LONG;

          GET_TOKEN();
          if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

          if (IS_TOKEN(token, ','))
          {
            GET_TOKEN();
            if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

            if (IS_TOKEN(token, 'y') || IS_TOKEN(token, 'Y'))
            {
              op = OP_INDIRECT8_Y_LONG;
            }
            else
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
      }
      else
      {
        tokens_push(asm_context, token, token_type);

        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (get_address(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        if (num < 0 || num > 0xffffff)
        {
          print_error("Address out of range.", asm_context);
          return -1;
        }

        if (size == 8)
        {
          if (num > 0xff)
          {
            print_error("Direct-page address out of range.", asm_context);
            return -1;
          }

          op = OP_ADDRESS8;
        }
        else if (size == 16)
        {
          if (num > 0xffff)
          {
            print_error("Absolute address out of range, use .l modifier.", asm_context);
            return -1;
          }

          op = OP_ADDRESS16;
        }
        else if (size == 24)
        {
          if (num > 0xffffff)
          {
            print_error("Absolute long address out of range.", asm_context);
            return -1;
          }

          op = OP_ADDRESS24;
        }

        // forward label, assume short
        if (num == 0) { size = 16; }

        GET_TOKEN();
        if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

        if (IS_TOKEN(token, ','))
        {
          GET_TOKEN();
          if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }

          if (IS_TOKEN(token, 'x') || IS_TOKEN(token, 'X'))
          {
            op = OP_INDEXED8_X;

            if (size == 16 || num > 0xff)
            {
              op = OP_INDEXED16_X;
            }

            if (size == 24 || num > 0xffff)
            {
              op = OP_INDEXED24_X;
            }

            if (num > 0xffffff)
            {
              print_error("Address out of range.", asm_context);
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else if (IS_TOKEN(token, 'y') || IS_TOKEN(token, 'Y'))
          {
            op = OP_INDEXED8_Y;

            if (size == 16 || num > 0xff) { op = OP_INDEXED16_Y; }

            if (size == 24 || num > 0xffff)
            {
              print_error("Absolute long not supported for Y indexing.",
                          asm_context);
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else if (IS_TOKEN(token, 's'))
          {
            op = OP_SP_RELATIVE;

            if (num > 0xff)
            {
              print_error("Address out of range.", asm_context);
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
        else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
    }
  }

  // find opcode in table
  opcode = -1;

  for (i = 0; i < 256; i++)
  {
    if (table_65816_opcodes[i].instr == instr_enum)
    {
      if (table_65816_opcodes[i].op == op)
      {
        opcode = i;
        break;
      }
      else if (op == OP_IMMEDIATE16)
      {
        if (table_65816_opcodes[i].op == OP_IMMEDIATE8)
        {
          op = OP_IMMEDIATE8;
          opcode = i;
          break;
        }
      }
      else if (op == OP_ADDRESS8)
      {
        if (table_65816_opcodes[i].op == OP_ADDRESS16)
        {
          op = OP_ADDRESS16;
          opcode = i;
          break;
        }
      }
      else if (op == OP_INDIRECT8)
      {
        if (table_65816_opcodes[i].op == OP_INDIRECT16)
        {
          op = OP_INDIRECT16;
          opcode = i;
          break;
        }
      }
      else if (op == OP_INDIRECT8_LONG)
      {
        if (table_65816_opcodes[i].op == OP_INDIRECT16_LONG)
        {
          op = OP_INDIRECT16_LONG;
          opcode = i;
          break;
        }
      }
      else if (op == OP_X_INDIRECT8)
      {
        if (table_65816_opcodes[i].op == OP_X_INDIRECT16)
        {
          op = OP_X_INDIRECT16;
          opcode = i;
          break;
        }
      }
    }
  }

  if (asm_context->pass == 2 && opcode == -1)
  {
    sprintf(temp, "No instruction found for addressing mode %d", op);
    print_error(temp, asm_context);
    return -1;
  }

  // fix for .b in immediate mode
  if (size == 8 && op == OP_IMMEDIATE16)
  {
    bytes = op_bytes[OP_IMMEDIATE8];
  }
  else
  {
    bytes = op_bytes[op];
  }

  // write output
  add_bin8(asm_context, opcode & 0xff, IS_OPCODE);

  if (bytes > 1)
  {
    add_bin8(asm_context, num & 0xff, IS_OPCODE);
  }

  if (bytes > 2)
  {
    add_bin8(asm_context, (num >> 8) & 0xff, IS_OPCODE);
  }

  if (bytes > 3)
  {
    add_bin8(asm_context, (num >> 16) & 0xff, IS_OPCODE);
  }

  return bytes;
}

