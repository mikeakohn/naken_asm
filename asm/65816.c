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

// bytes for each mode
/*
static int mode_bytes[] =
{
  1, 3, 2, 3, 4, 2, 2, 3, 3, 4, 2,
  2, 3, 2, 3, 2, 2, 3, 2, 3, 2, 2
};
*/

int parse_instruction_65816(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  int token_type;
//  int opcode;
//  int mode;
  int enum_value;
  int i;

  // make lowercase
  lower_copy(instr_case, instr);

  // get instruction from string
  enum_value = -1;

  for(i = 0; i < 92; i++)
  {
    if(strcmp(instr_case, table_65816[i].name) == 0)
    {
      enum_value = i;
      break;
    }
  }

  // no matching instruction
  if(enum_value == -1)
  {
   print_error_unexp(token, asm_context);
   return -1;
  }

  // reset mode to default
//  mode = OP_NONE;

  // parse
  while((token_type = tokens_get(asm_context, token, TOKENLEN)) != TOKEN_EOL)
  {
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
  }

  return -1;
}

