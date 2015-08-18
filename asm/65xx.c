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
#include "asm/65xx.h"
#include "common/assembler.h"
//#include "disasm/65xx.h"
#include "common/tokens.h"
#include "common/eval_expression.h"

#include "table/65xx.h"

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

extern struct _table_65xx table_65xx[];
extern struct _table_65xx_opcodes table_65xx_opcodes[];

// bytes for each addressing mode
static int op_bytes[] = { 1, 2, 2, 3, 2, 2, 3, 3, 3, 2, 2, 2 };

int parse_instruction_65xx(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  char temp[256];
  int token_type;
  int opcode;
  int op;
  int instr_enum;
  int num;
  int size;
  int bytes;
  int i;

  // make lowercase
  lower_copy(instr_case, instr);

  // get instruction from string
  instr_enum = -1;

  for(i = 0; i < 56; i++)
  {
    if(strcmp(instr_case, table_65xx[i].name) == 0)
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

  // get default addressing mode
  op = table_65xx[instr_enum].op;

  size = 0;

  // parse
  while(1)
  {
    if(GET_TOKEN() == TOKEN_EOL)
      break;

    if(IS_TOKEN(token, '.'))
    {
      if(GET_TOKEN() == TOKEN_EOL)
        break;

      if(IS_TOKEN(token, 'b') || IS_TOKEN(token, 'B'))
      {
        size = 8;
      }
      else if(IS_TOKEN(token, 'w') || IS_TOKEN(token, 'W'))
      {
        size = 16;
      }
      else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
 
      if(GET_TOKEN() == TOKEN_EOL)
        break;
    }

    if(op == OP_RELATIVE)
    {
      if(IS_TOKEN(token, '#'))
      {
        GET_NUM();

        if(num < -128 || num > 0xFF)
        {
          print_error("8-bit constant out of range.", asm_context);
          return -1;
        }

        num = (uint8_t)num;
      }
      else
      {
        tokens_push(asm_context, token, token_type);
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
    }
    else
    {
      if(IS_TOKEN(token, '#'))
      {
        op = OP_IMMEDIATE;
        GET_NUM();

        if(num < -128 || num > 0xFF)
        {
          print_error("8-bit constant out of range.", asm_context);
          return -1;
        }

        num = (uint8_t)num;
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
              if(num < 0 || num > 0xFF)
              {
                print_error("Address out of range.", asm_context);
                return -1;
              }

              op = OP_X_INDIRECT8;
            }
            else
            {
              print_error_unexp(token, asm_context);
              return -1;
            }
          }
        }
        else if(IS_TOKEN(token, ')'))
        {
          op = OP_INDIRECT16;

          if(GET_TOKEN() == TOKEN_EOL)
            break;

          if(IS_TOKEN(token, ','))
          {
            if(GET_TOKEN() == TOKEN_EOL)
              break;

            if(IS_TOKEN(token, 'y') || IS_TOKEN(token, 'Y'))
              op = OP_INDIRECT8_Y;
          }
        }
      }
      else
      {
        tokens_push(asm_context, token, token_type);
        GET_NUM();

        if(num < 0 || num > 0xFFFF)
        {
          print_error("Address out of range.", asm_context);
          return -1;
        }

        op = OP_ADDRESS8;

        if(num > 0xFF)
          op = OP_ADDRESS16;

        if(size == 8)
        {
          if(num > 0xFF)
          {
            print_error("Zero-page address out of range.", asm_context);
            return -1;
          }

          op = OP_ADDRESS8;
        }
        else if(size == 16)
        {
          if(num > 0xFFFF)
          {
            print_error("Absolute address out of range.", asm_context);
            return -1;
          }

          op = OP_ADDRESS16;
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
    if(table_65xx_opcodes[i].instr == instr_enum)
    {
      if(table_65xx_opcodes[i].op == op)
      {
        opcode = i;
        break;
      }
      else if(op == OP_ADDRESS8)
      {
        if(table_65xx_opcodes[i].op == OP_ADDRESS16)
        {
          op = OP_ADDRESS16;
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

  bytes = op_bytes[op];

  add_bin8(asm_context, opcode & 0xFF, IS_OPCODE);

  if(bytes > 1)
    add_bin8(asm_context, num & 0xFF, IS_OPCODE);

  if(bytes > 2)
    add_bin8(asm_context, (num >> 8) & 0xFF, IS_OPCODE);

  return bytes;
}

