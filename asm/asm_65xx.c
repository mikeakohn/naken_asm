/**
 *  naken_asm MSP430 assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 * 65xx file by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm_common.h"
#include "asm_65xx.h"
#include "assembler.h"
#include "disasm_65xx.h"
#include "get_tokens.h"
#include "eval_expression.h"

#include "table_65xx.h"
extern struct _opcodes_65xx opcodes_65xx[];

// addressing modes
enum
{
  MODE_ABSOLUTE,
  MODE_ABSOLUTE_X_INDEXED,
  MODE_ABSOLUTE_Y_INDEXED,
  MODE_IMMEDIATE,
  MODE_IMPLIED,
  MODE_INDIRECT,
  MODE_X_INDEXED_INDIRECT,
  MODE_INDIRECT_Y_INDEXED,
  MODE_RELATIVE,
  MODE_ZEROPAGE,
  MODE_ZEROPAGE_X_INDEXED,
  MODE_ZEROPAGE_Y_INDEXED
};

// bytes each mode takes
static int mode_bytes[] = { 3, 3, 3, 2, 1, 3, 2, 2, 2, 2, 2, 2 };


int parse_instruction_65xx(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  char temp[80];
  int token_type;
  int opcode;
  int mode;
  int num;
  int index;
  int i;

  // make lower case
  lower_copy(instr_case, instr);

  // get instruction index
  index = -1;

  for(i = 0; i < 56; i++)
  {
    if(strcmp(instr_case, opcodes_65xx[i].name) == 0)
    {
      index = i;
      break;
    }
  }

  // no instruction found
  if(index == -1)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  mode = MODE_IMPLIED;

  token_type=get_token(asm_context, token, TOKENLEN);
  if (token_type==TOKEN_EOL) { goto skip; }

  if(IS_TOKEN(token, '#'))
  {
    mode = MODE_IMMEDIATE;

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

    num &= 0xFFFF;

    // out of byte range
    if(num < 0 || num > 0xFF)
    {
      print_error_unexp(token, asm_context);
    }
  }
    else
  if(IS_TOKEN(token, '('))
  {
    mode = MODE_INDIRECT;
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

    num &= 0xFFFF;

    token_type = get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { goto skip; }

    if(IS_TOKEN(token, ','))
    {
      mode = MODE_X_INDEXED_INDIRECT;
    }
      else
    if(IS_TOKEN(token, ')'))
    {
      mode = MODE_INDIRECT;
      token_type = get_token(asm_context, token, TOKENLEN);
        if (token_type==TOKEN_EOL) { goto skip; }
      if(IS_TOKEN(token, ','))
      {
        mode = MODE_INDIRECT_Y_INDEXED;
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
    pushback(asm_context, token, token_type);

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

    num &= 0xFFFF;
    if(num >= 256)
      mode = MODE_ABSOLUTE;
    else
      mode = MODE_ZEROPAGE;

    token_type = get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { goto skip; }

    if(IS_TOKEN(token, ','))
    {
      token_type = get_token(asm_context, token, TOKENLEN);
      if (token_type==TOKEN_EOL) { goto skip; }

      if(IS_TOKEN(token, 'x') || IS_TOKEN(token, 'X'))
      {
        if(num >= 256)
          mode = MODE_ABSOLUTE_X_INDEXED;
        else
          mode = MODE_ZEROPAGE_X_INDEXED;
      }
        else
      if(IS_TOKEN(token, 'y') || IS_TOKEN(token, 'Y'))
      {
        if(num >= 256)
          mode = MODE_ABSOLUTE_Y_INDEXED;
        else
          mode = MODE_ZEROPAGE_Y_INDEXED;
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }
  }

skip:
  // branches are in table positions 3 - 10
  if(index >= 3 && index <= 10)
  {
    mode = MODE_RELATIVE;
    if(asm_context->pass == 2)
    {
      // calculate branch offset, need to add 2 to current
      // address, since thats where the program counter would be
      num -= (asm_context->address + 2);
      num &= 0xFF;
      if(num < 0 || num > 0xFF)
       {
        print_error("Branch out of range", asm_context);
        return -1;
      }
      num &= 0xFF;
    }
  }

  // see if theres an opcode for this instruction and mode
  opcode = opcodes_65xx[index].opcode[mode];
  if(opcode == 0xFF)
  {
    // try again without zeropage
    if(mode == MODE_ZEROPAGE)
    {
      mode = MODE_ABSOLUTE;
      opcode = opcodes_65xx[index].opcode[mode];
    }
    if(mode == MODE_ZEROPAGE_X_INDEXED)
    {
      mode = MODE_ABSOLUTE_X_INDEXED;
      opcode = opcodes_65xx[index].opcode[mode];
    }
    if(mode == MODE_ZEROPAGE_Y_INDEXED)
    {
      mode = MODE_ABSOLUTE_Y_INDEXED;
      opcode = opcodes_65xx[index].opcode[mode];
    }
  }

  if(asm_context->pass == 2 && opcode == 0xFF)
  {
    sprintf(temp, "No instruction found for addressing mode %d", mode);
    print_error(temp, asm_context);
    return -1;
  }

  // warn if indirect JMP bug will happen
  if((opcode == 0x6C) && ((num & 0xFF) == 0xFF))
  {
    print_error("Warning: Indirect JMP to upper page boundary (6502 bug)", asm_context);
  }

  // write opcode
  memory_write_inc(asm_context, opcode & 0xFF, asm_context->line);

  // write low byte first, if any
  if(mode_bytes[mode] > 1)
    memory_write_inc(asm_context, num & 0xFF, DL_NO_CG);

  // then high byte, if any
  if(mode_bytes[mode] > 2)
    memory_write_inc(asm_context, (num >> 8) & 0xFF, DL_NO_CG);

  return mode_bytes[mode];
}

