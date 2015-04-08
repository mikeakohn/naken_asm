/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn, Joe Davisson
 *
 * 65816 by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/65816.h"
#include "common/assembler.h"
#include "disasm/65816.h"
#include "common/tokens.h"
#include "common/eval_expression.h"

#include "table/65816.h"

#define GET_TOKEN() \
  (token_type = tokens_get(asm_context, token, TOKENLEN))

#define GET_NUM() \
  if(eval_expression(asm_context, &num) != 0) \
  { \
    if(asm_context->pass == 1) \
    { \
      eat_operand(asm_context); \
    } \
    else \
    { \
      print_error_unexp(token, asm_context); \
      return -1; \
    } \
  }

extern struct _table_65816 table_65816[];
extern struct _table_65816_opcodes table_65816_opcodes[];

// bytes for each addressing mode
static int op_bytes[] =
{
  1, 3, 2, 3, 4, 2, 2, 3, 3, 4, 2,
  2, 3, 2, 3, 2, 2, 3, 2, 3, 2, 2
};

int parse_instruction_65816(struct _asm_context *asm_context, char *instr)
{
//FIXME add ! prefix to force absolute address (16-bit) ?
//FIXME add @ prefix to force long address (24-bit) ?
//FIXME add < > ^ prefixes to immediate mode options ?
//FIXME add support for M and X flags

  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  char temp[256];
  int token_type;
  int opcode;
  int op;
  int instr_enum;
  int num;
  int i;

  // make lowercase
  lower_copy(instr_case, instr);

  // get instruction from string
  instr_enum = -1;

  for(i = 0; i < 92; i++)
  {
    if(strcmp(instr_case, table_65816[i].name) == 0)
    {
      instr_enum = i;
      break;
    }
  }

  // no matching instruction
  if(instr_enum == -1)
  {
   print_error_unexp(token, asm_context);
   return -1;
  }

  // get default op, if any
  op = table_65816[instr_enum].op;

  // parse
  while(1)
  {
    if(GET_TOKEN() == TOKEN_EOL)
      break;

    if(op == OP_RELATIVE)
    {
      GET_NUM();

      if(asm_context->pass == 2)
      {
        // calculate branch offset, need to add 2 to current
        // address, since thats where the program counter would be
        num -= (asm_context->address + 2);

        if(num < -128 || num > 127)
        {
          print_error("Relative branch out of range", asm_context);
          return -1;
        }

        num = (uint8_t)num;
      }
    }
    else if(op == OP_RELATIVE_LONG)
    {
      GET_NUM();

      if(asm_context->pass == 2)
      {
        // calculate branch offset, need to add 3 to current
        // address, since thats where the program counter would be
        num -= (asm_context->address + 3);

        if(num < -32768 || num > 32767)
        {
          print_error("Relative long branch out of range", asm_context);
          return -1;
        }

        num = (uint16_t)num;
      }
    }
    else if(op == OP_BLOCK_MOVE)
    {
      print_error("Block move not implemented yet.", asm_context);
      return -1;
    }
    else
    {
      if(IS_TOKEN(token, '#'))
      {
        GET_NUM();
        num = (uint16_t)num;
        op = OP_NUMBER16;
      }
      else if(IS_TOKEN(token, '('))
      {
        GET_NUM();

        if(GET_TOKEN() == TOKEN_EOL)
          break;

        if(IS_TOKEN(token, ','))
        {
          if(GET_TOKEN() == TOKEN_EOL)
            break;

          if(IS_TOKEN(token, 'x') || IS_TOKEN(token, 'X'))
          {
            if(GET_TOKEN() == TOKEN_EOL)
              break;

            if(IS_TOKEN(token, ')'))
            {
              if(num > 0xFF)
                op = OP_X_INDIRECT16;
              else
                op = OP_X_INDIRECT8;
            }
            else
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else if(IS_TOKEN(token, 's') || IS_TOKEN(token, 'S'))
          {
            op = OP_SP_INDIRECT_Y;

            if(GET_TOKEN() == TOKEN_EOL)
              break;

            if(IS_NOT_TOKEN(token, ')'))
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            if(GET_TOKEN() == TOKEN_EOL)
              break;

            if(IS_NOT_TOKEN(token, ','))
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            if(GET_TOKEN() == TOKEN_EOL)
              break;

            if(IS_NOT_TOKEN(token, 'y') && IS_NOT_TOKEN(token, 'Y'))
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
        }
      }
      else if(IS_TOKEN(token, '['))
      {
        GET_NUM();

        if(GET_TOKEN() == TOKEN_EOL)
          break;

        if(IS_TOKEN(token, ']'))
        {
          op = OP_INDIRECT8_LONG;

          if(GET_TOKEN() == TOKEN_EOL)
            break;

          if(IS_TOKEN(token, ','))
          {
            if(GET_TOKEN() == TOKEN_EOL)
              break;

            if(IS_TOKEN(token, 'y') || IS_TOKEN(token, 'Y'))
              op = OP_INDIRECT8_Y_LONG;
          }
        }
      }
      else
      {
        tokens_push(asm_context, token, token_type);
        GET_NUM();
        num = (uint32_t)num;
        op = OP_ADDRESS8;

        if(num > 0xFF)
          op = OP_ADDRESS16;

        if(num > 0xFFFF)
          op = OP_ADDRESS24;

        if(num > 0xFFFFFF)
        {
          print_error("Address out of range.", asm_context);
          print_error_unexp(token, asm_context);
          return -1;
        }

        if(GET_TOKEN() == TOKEN_EOL)
          break;

        if(IS_TOKEN(token, ','))
        {
          if(GET_TOKEN() == TOKEN_EOL)
            break;

          if(IS_TOKEN(token, 'x') || IS_TOKEN(token, 'X'))
          {
            op = OP_INDEXED8_X;

            if(num > 0xFF)
              op = OP_INDEXED16_X;

            if(num > 0xFFFF)
              op = OP_INDEXED24_X;

            if(num > 0xFFFFFF)
            {
              print_error("Address out of range.", asm_context);
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else if(IS_TOKEN(token, 'y') || IS_TOKEN(token, 'Y'))
          {
            op = OP_INDEXED8_Y;

            if(num > 0xFF)
              op = OP_INDEXED16_Y;

            if(num > 0xFFFF)
            {
              print_error("Absolute long not supported for Y indexing.",
                          asm_context);
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
          else if(IS_TOKEN(token, 's'))
          {
            op = OP_SP_RELATIVE;

            if(num > 0xFF)
            {
              print_error("Address out of range.", asm_context);
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
        }
      }
    }
  }

  opcode = -1;

  for(i = 0; i < 256; i++)
  {
    if(table_65816_opcodes[i].instr == instr_enum)
    {
      if(table_65816_opcodes[i].op == op)
      {
        opcode = i;
        break;
      }
      else if(op == OP_ADDRESS8)
      {
        if(table_65816_opcodes[i].op == OP_ADDRESS16)
        {
          op = OP_ADDRESS16;
          opcode = i;
          break;
        }
      }
      else if(op == OP_ADDRESS16)
      {
        if(table_65816_opcodes[i].op == OP_ADDRESS24)
        {
          op = OP_ADDRESS24;
          opcode = i;
          break;
        }
      }
    }
  }

  if(asm_context->pass == 2 && opcode == -1)
  {
    sprintf(temp, "No instruction found for addressing mode %d", op);
    print_error(temp, asm_context);
    return -1;
  }

  add_bin8(asm_context, opcode & 0xFF, IS_OPCODE);

  if(op_bytes[op] > 1)
    add_bin8(asm_context, num & 0xFF, IS_OPCODE);

  if(op_bytes[op] > 2)
    add_bin8(asm_context, (num >> 8) & 0xFF, IS_OPCODE);

  if(op_bytes[op] > 3)
    add_bin8(asm_context, (num >> 16) & 0xFF, IS_OPCODE);

  return op_bytes[op];
}

