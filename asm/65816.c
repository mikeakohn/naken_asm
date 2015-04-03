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
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  char temp[256];
//  int token_type;
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
    if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
      break;

    if(op == OP_RELATIVE)
    {
      num = (uint8_t)num;
    }
    else if(op == OP_RELATIVE_LONG)
    {
      num = (uint16_t)num;
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
        op = OP_NUMBER16;

        if(eval_expression(asm_context, &num) != 0)
        {
          if(asm_context->pass == 1)
          {
            eat_operand(asm_context);
          }
          else
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }

        num = (uint16_t)num;
      }
      else if(IS_TOKEN(token, '('))
      {
        if(eval_expression(asm_context, &num) != 0)
        {
          if(asm_context->pass == 1)
          {
            eat_operand(asm_context);
          }
          else
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }

        if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
          break;

        if(IS_TOKEN(token, ','))
        {
          if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
            break;

          if(IS_TOKEN(token, 'x') || IS_TOKEN(token, 'X'))
          {
            if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
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
            num = (uint8_t)num;
            op = OP_SP_INDIRECT_Y;

            if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
              break;

            if(IS_NOT_TOKEN(token, ')'))
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
              break;

            if(IS_NOT_TOKEN(token, ','))
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
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
        if(eval_expression(asm_context, &num) != 0)
        {
          if(asm_context->pass == 1)
          {
            eat_operand(asm_context);
          }
          else
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }

        if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
          break;

        if(IS_TOKEN(token, ']'))
        {
          num = (uint8_t)num;
          op = OP_INDIRECT8_LONG;

          if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
            break;

          if(IS_TOKEN(token, ','))
          {
            if(tokens_get(asm_context, token, TOKENLEN) == TOKEN_EOL)
              break;

            if(IS_TOKEN(token, 'y'))
              op = OP_INDIRECT8_Y_LONG;
          }
        }
      }
    }
  }

  opcode = -1;

  for(i = 0; i < 256; i++)
  {
    if( (table_65816_opcodes[i].instr == instr_enum) &&
        (table_65816_opcodes[i].op == op) )
    {
      opcode = i;
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

    // special cases:
    // OP_RELATIVE
    // OP_RELATIVE_LONG
    // OP_BLOCK_MOVE

    // if #
    //  OP_NUMBER16

    // if (
    //   if ,x)
    //     OP_X_INDIRECT8
    //   if ,s),y
    //     OP_SP_INDIRECT_Y
    //   if )
    //     if ,y
    //       OP_INDIRECT8_Y
    //     else
    //       OP_INDIRECT8
    //       OP_INDIRECT16

    // if [
    //   if ]
    //     if ,y
    //       OP_INDIRECT8_Y_LONG
    //     else
    //       OP_INDIRECT8_LONG

    // if ,x
    //   OP_INDEXED8_X
    //   OP_INDEXED16_X
    //   OP_INDEXED24_X

    // if ,y
    //   OP_INDEXED8_Y
    //   OP_INDEXED16_Y
    //   OP_INDEXED24_X

    // if ,s
    //   OP_SP_RELATIVE

    // OP_ADDRESS8
    // OP_ADDRESS16
    // OP_ADDRESS24

    // if instruction only
    //   OP_NONE

