/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/java.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/java.h"

static int parse_index(struct _asm_context *asm_context, const char *instr)
{
  int index;

  if (eval_expression(asm_context, &index) != 0)
  {
    if (asm_context->pass == 1)
    {
      ignore_line(asm_context);
      return 0;
    }

    print_error_illegal_expression(instr, asm_context);
    return -1;
  }

  return 0;
}

int parse_instruction_java(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  int n;

  lower_copy(instr_case, instr);

  for (n = 0; n < 255; n++)
  {
    if (table_java[n].op_type == JAVA_OP_ILLEGAL) { continue; }

    if (strcmp(table_java[n].instr, instr_case) == 0)
    {
      switch(table_java[n].op_type)
      {
        case JAVA_OP_NONE:
          add_bin8(asm_context, n, IS_OPCODE);

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          return 1;
        case JAVA_OP_CONSTANT_INDEX:
        case JAVA_OP_FIELD_INDEX:
        case JAVA_OP_INTERFACE_INDEX:
        case JAVA_OP_LOCAL_INDEX:
        case JAVA_OP_METHOD_INDEX:
        case JAVA_OP_CLASS_INDEX:
        case JAVA_OP_SPECIAL_INDEX:
        case JAVA_OP_STATIC_INDEX:
        case JAVA_OP_VIRTUAL_INDEX:
          return parse_index(asm_context, instr);

        case JAVA_OP_CLASS_INDEX_TYPE:
        case JAVA_OP_INDEX_LOCAL_CONST:
        case JAVA_OP_ARRAY_TYPE:
        case JAVA_OP_CONSTANT16:
        case JAVA_OP_CONSTANT8:
        case JAVA_OP_OFFSET16:
        case JAVA_OP_OFFSET32:
          return -1;
        case JAVA_OP_WARN:
          printf("Warning: %s is reserved  %s:%d.\n",
            instr, asm_context->tokens.filename, asm_context->tokens.line);
          add_bin8(asm_context, n, IS_OPCODE);
          return 1;
      }
    }
  } 

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

