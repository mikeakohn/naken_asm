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
#include "asm/webasm.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/webasm.h"

int parse_instruction_webasm(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  int n;

  lower_copy(instr_case, instr);

  n = 0;
  while(table_webasm[n].instr != NULL)
  {
    if (strcmp(table_webasm[n].instr, instr_case) != 0)
    {
      n++;
      continue;
    }

    switch(table_webasm[n].type)
    {
      case WEBASM_OP_NONE:
        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);
        return 1;
      default:
        n++;
        continue;
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

