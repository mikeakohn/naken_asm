/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn, Joe Davisson
 *
 * 6502 by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/common.h"
#include "asm/6502.h"
#include "common/assembler.h"
//#include "disasm/6502.h"
#include "common/tokens.h"
#include "common/eval_expression.h"

#include "table/6502.h"

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
  else
  {
    tokens_push(asm_context, token, *token_type);
  }

  // obtain number
  if (eval_expression(asm_context, num) != 0)
  {
    if (asm_context->pass == 1)
      ignore_operand(asm_context);
    else
      return -1;
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
  else
  {
    tokens_push(asm_context, token, *token_type);
  }

  // obtain address
  if (eval_expression(asm_context, num) != 0)
  {
    if (asm_context->pass == 1)
      ignore_operand(asm_context);
    else
      return -1;

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

    if (*num > 0xff)
      *size = 16;

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

  return 0;
}

extern struct _table_6502 table_6502[];
extern struct _table_6502_opcodes table_6502_opcodes[];

// bytes for each addressing mode
static int op_bytes[] = { 1, 2, 2, 3, 2, 2, 3, 3, 3, 2, 2, 2, 3, 2, 3 };

int parse_instruction_6502(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  char temp[256];
  int token_type;
  int opcode;
  int op;
  int instr_enum;
  int offset = 0;
  int num;
  int size;
  int bytes;
  int i;

  // make lowercase
  lower_copy(instr_case, instr);

  // get instruction from string
  instr_enum = -1;

  for(i = 0; i < table_6502_len; i++)
  {
    if (strcmp(instr_case, table_6502[i].name) == 0)
    {
      instr_enum = i;
      break;
    }
  }

  // no matching instruction
  if (instr_enum == -1)
  {
    print_error_unknown_operand_combo(instr_case, asm_context);
    return -1;
  }

  // get default addressing mode
  op = table_6502[instr_enum].op;

  // start with unknown number/address size
  size = 0;

  // parse
  while (1)
  {
    if (GET_TOKEN() == TOKEN_EOL) { break; }

    // dot suffix
    if (IS_TOKEN(token, '.'))
    {
      if (GET_TOKEN() == TOKEN_EOL) { break; }

      if (IS_TOKEN(token, 'b') || IS_TOKEN(token, 'B'))
      {
        size = 8;
      }
      else if (IS_TOKEN(token, 'w') || IS_TOKEN(token, 'W'))
      {
        size = 16;
      }
      else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (GET_TOKEN() == TOKEN_EOL) { break; }
    }

    if (op == OP_RELATIVE)
    {
      if (IS_TOKEN(token, '#'))
      {
        if (GET_TOKEN() == TOKEN_EOL) { break; }

        if (get_num(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        if (num < -128 || num > 0xff)
        {
          print_error("8-bit constant out of range.", asm_context);
          return -1;
        }

        num = (uint8_t)num;
      }
      else
      {
        tokens_push(asm_context, token, token_type);

        if (GET_TOKEN() == TOKEN_EOL) { break; }

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
    else
    {
      if (IS_TOKEN(token, '#'))
      {
        op = OP_IMMEDIATE;

        if (GET_TOKEN() == TOKEN_EOL) { break; }

        if (get_num(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        if (num < -128 || num > 0xff)
        {
          print_error("8-bit constant out of range.", asm_context);
          return -1;
        }

        num = (uint8_t)num;
      }
      else if (IS_TOKEN(token, '('))
      {
        if (GET_TOKEN() == TOKEN_EOL) { break; }

        if (get_address(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        if (GET_TOKEN() == TOKEN_EOL) { break; }

        if (IS_TOKEN(token, ','))
        {
          if (GET_TOKEN() == TOKEN_EOL) { break; }

          if (IS_TOKEN(token, 'x') || IS_TOKEN(token, 'X'))
          {
            if (GET_TOKEN() == TOKEN_EOL) { break; }

            if (IS_TOKEN(token, ')'))
            {
#if 0
              if (num < 0 || num > 0xff || size == 16)
              {
                print_error("Address out of range.", asm_context);
                return -1;
              }
#endif

              op = OP_X_INDIRECT8;
            }
            else
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
        }
        else if (IS_TOKEN(token, ')'))
        {
          op = OP_INDIRECT16;

          if (GET_TOKEN() == TOKEN_EOL) { break; }

          if (IS_TOKEN(token, ','))
          {
            if (GET_TOKEN() == TOKEN_EOL) { break; }

            if (IS_TOKEN(token, 'y') || IS_TOKEN(token, 'Y'))
            {
              if (num < 0 || num > 0xff || size == 16)
              {
                print_error("Address out of range.", asm_context);
                return -1;
              }

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
      }
      else
      {
        tokens_push(asm_context, token, token_type);

        if (GET_TOKEN() == TOKEN_EOL) { break; }

        if (get_address(asm_context, token, &token_type, &num, &size) == -1)
        {
          return -1;
        }

        if (num < 0 || num > 0xffff)
        {
          print_error("Address out of range.", asm_context);
          return -1;
        }

        // forward label
        if (num == 0)
        {
          int worst_case = memory_read(asm_context, asm_context->address);
          if (worst_case == 1) { size = 16; }
        }

        if (size == 8)
        {
          if (num > 0xff)
          {
            print_error("Zero-page address out of range.", asm_context);
            return -1;
          }

          op = OP_ADDRESS8;
        }
        else if (size == 16)
        {
          if (num > 0xffff)
          {
            print_error("Absolute address out of range.", asm_context);
            return -1;
          }

          op = OP_ADDRESS16;
        }

        if (GET_TOKEN() == TOKEN_EOL) { break; }

        if (IS_TOKEN(token, ','))
        {
          if (GET_TOKEN() == TOKEN_EOL) { break; }

          if (IS_TOKEN(token, 'x') || IS_TOKEN(token, 'X'))
          {
            op = OP_INDEXED8_X;

            if (num > 0xff || size == 16)
              op = OP_INDEXED16_X;

            if (num > 0xffff)
            {
              print_error("Address out of range.", asm_context);
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else if (IS_TOKEN(token, 'y') || IS_TOKEN(token, 'Y'))
          {
            op = OP_INDEXED8_Y;

            if (num > 0xff || size == 16)
              op = OP_INDEXED16_Y;

            if (num > 0xffff)
            {
              print_error("Address out of range.", asm_context);
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else
          {
            tokens_push(asm_context, token, token_type);

            if (asm_context->pass == 1)
            {
              ignore_operand(asm_context);
              offset = 0;
            }
              else
            {
              int address;

              if (eval_expression(asm_context, &address) != 0)
              {
                return -1;
              }

              offset = address - (asm_context->address + 2);
            }

            op = OP_ADDRESS8_RELATIVE;
            break;
            //print_error_unexp(token, asm_context);
            //return -1;
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

  for(i = 0; i < 256; i++)
  {
    if (table_6502_opcodes[i].instr == instr_enum)
    {
      if (table_6502_opcodes[i].op == op)
      {
        opcode = i;
        break;
      }
      else if (op == OP_INDIRECT16)
      {
        // FIXME - If any instruction has both OP_INDIRECT8 and OP_INDIRECT16
        // modes, this will be incorrect.
        if (table_6502_opcodes[i].op == OP_INDIRECT8)
        {
          op = OP_INDIRECT8;
          opcode = i;
          break;
        }
      }
      else if (op == OP_X_INDIRECT8)
      {
        // FIXME - This could also possibly collide with different opcodes.
        if (table_6502_opcodes[i].op == OP_X_INDIRECT16)
        {
          op = OP_X_INDIRECT16;
          opcode = i;
          break;
        }
      }
      else if (op == OP_ADDRESS8)
      {
        if (table_6502_opcodes[i].op == OP_ADDRESS16)
        {
          op = OP_ADDRESS16;
          opcode = i;
          break;
        }
      }
      else if (op == OP_INDEXED8_Y)
      {
        if (table_6502_opcodes[i].op == OP_INDEXED16_Y)
        {
          op = OP_INDEXED16_Y;
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

  // Make sure all numbers are checked to be in bounds
  switch (op)
  {
    case OP_ADDRESS8:
    case OP_INDEXED8_X:
    case OP_INDEXED8_Y:
    case OP_X_INDIRECT8:
    case OP_INDIRECT8_Y:
    case OP_INDIRECT8:
      if (num < 0 || num > 0xff)
      {
        print_error_range("Address", 0, 0xff, asm_context);
        return -1;
      }
      break;
    case OP_ADDRESS16:
    case OP_INDEXED16_X:
    case OP_INDEXED16_Y:
    case OP_INDIRECT16:
    case OP_X_INDIRECT16:
      if (num < 0 || num > 0xffff)
      {
        print_error_range("Address", 0, 0xffff, asm_context);
        return -1;
      }
      break;
    case OP_ADDRESS8_RELATIVE:
      if (num < 0 || num > 0xffff)
      {
        print_error_range("Address", 0, 0xffff, asm_context);
        return -1;
      }

      if (offset < -128 || offset > 127)
      {
        print_error_range("Offset", -128, 127, asm_context);
        return -1;
      }

      add_bin8(asm_context, opcode & 0xff, IS_OPCODE);
      add_bin8(asm_context, num & 0xff, IS_OPCODE);
      add_bin8(asm_context, offset & 0xff, IS_OPCODE);
      return 3;
    default:
      break;
  }

  // write output
  bytes = op_bytes[op];

  add_bin8(asm_context, opcode & 0xff, IS_OPCODE);

  if (bytes > 1)
  {
    add_bin8(asm_context, num & 0xff, IS_OPCODE);
  }

  if (bytes > 2)
  {
    add_bin8(asm_context, (num >> 8) & 0xff, IS_OPCODE);
  }

  return bytes;
}

