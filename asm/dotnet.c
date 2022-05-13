/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/common.h"
#include "asm/dotnet.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/dotnet.h"

int parse_instruction_dotnet(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  int n;

  lower_copy(instr_case, instr);

  n = 0;
  while (table_dotnet[n].instr != NULL)
  {
    if (strcmp(table_dotnet[n].instr, instr_case) != 0)
    {
      n++;
      continue;
    }

    switch (table_dotnet[n].type)
    {
      case DOTNET_OP_NONE:
        add_bin8(asm_context, table_dotnet[n].opcode, IS_OPCODE);
        return 1;
      default:
        n++;
        continue;
    }
  }

  n = 0;
  while (table_dotnet_fe[n].instr != NULL)
  {
    if (strcmp(table_dotnet_fe[n].instr, instr_case) != 0)
    {
      n++;
      continue;
    }

    switch (table_dotnet[n].type)
    {
      case DOTNET_OP_NONE:
        add_bin8(asm_context, 0xfe, IS_OPCODE);
        add_bin8(asm_context, table_dotnet_fe[n].opcode, IS_OPCODE);
        return 2;
      default:
        n++;
        continue;
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

