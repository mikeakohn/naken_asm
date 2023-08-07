/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "asm/pdk15.h"
#include "asm/pdk_parse.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "table/pdk15.h"

int parse_instruction_pdk15(AsmContext *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int n;
  uint16_t opcode;
  int matched = 0;

  if (asm_context->pass == 1)
  {
    ignore_line(asm_context);

    add_bin16(asm_context, 0, IS_OPCODE);
    return 2;
  }

  memset(operands, 0, sizeof(operands));

  operand_count = pdk_parse(asm_context, operands);

  if (operand_count == -1) { return -1; }

  lower_copy(instr_case, instr);

  for (n = 0; table_pdk15[n].instr != NULL; n++)
  {
    if (strcmp(table_pdk15[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch (table_pdk15[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0) { continue; }

          add_bin16(asm_context, table_pdk15[n].opcode, IS_OPCODE);

          return 2;
        }
        case OP_A:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_A)
          {
            continue;
          }

          add_bin16(asm_context, table_pdk15[n].opcode, IS_OPCODE);

          return 2;
        }
        case OP_IO_A:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_IMMEDIATE ||
              operands[1].type != OPERAND_A)
          {
            continue;
          }

          if (operands[0].value < 0 || operands[0].value > 0x7f)
          {
            print_error_range(asm_context, "IO", 0, 0x7f);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[0].value & 0x7f);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_A_IO:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_A ||
              operands[1].type != OPERAND_IMMEDIATE)
          {
            continue;
          }

          if (operands[1].value < 0 || operands[1].value > 0x7f)
          {
            print_error_range(asm_context, "IO", 0, 0x7f);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[1].value & 0x7f);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_K8:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_IMMEDIATE)
          {
            continue;
          }

          if (operands[0].value < -128 || operands[0].value > 255)
          {
            print_error_range(asm_context, "Literal", -128, 255);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[0].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_M7:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_ADDRESS)
          {
            continue;
          }

          if (operands[0].value < 0 || operands[0].value > 0xff)
          {
            print_error_range(asm_context, "Address", 0, 0xff);
            return -1;
          }

          if ((operands[0].value & 1) != 0)
          {
            print_error_align(asm_context, 2);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[0].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_M8:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_ADDRESS)
          {
            continue;
          }

          if (operands[0].value < 0 || operands[0].value > 0xff)
          {
            print_error_range(asm_context, "Address", 0, 0xff);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[0].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_A_M8:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_A ||
              operands[1].type != OPERAND_ADDRESS)
          {
            continue;
          }

          if (operands[1].value < 0 || operands[1].value > 0xff)
          {
            print_error_range(asm_context, "Address", 0, 0xff);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[1].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_M8_A:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_ADDRESS ||
              operands[1].type != OPERAND_A)
          {
            continue;
          }

          if (operands[0].value < 0 || operands[0].value > 0xff)
          {
            print_error_range(asm_context, "Address", 0, 0xff);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[0].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_A_M7:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_A ||
              operands[1].type != OPERAND_ADDRESS)
          {
            continue;
          }

          if (operands[1].value < 0 || operands[1].value > 0xff)
          {
            print_error_range(asm_context, "Address", 0, 0xff);
            return -1;
          }

          if ((operands[1].value & 1) != 0)
          {
            print_error_align(asm_context, 2);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[1].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_M7_A:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_ADDRESS ||
              operands[1].type != OPERAND_A)
          {
            continue;
          }

          if (operands[0].value < 0 || operands[0].value > 0xff)
          {
            print_error_range(asm_context, "Address", 0, 0xff);
            return -1;
          }

          if ((operands[0].value & 1) != 0)
          {
            print_error_align(asm_context, 2);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[0].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_IO_N:
        case OP_M_N:
        {
          if (operand_count != 1) { continue; }

          if (table_pdk15[n].type == OP_IO_N)
          {
            if (operands[0].type != OPERAND_BIT_OFFSET) { continue; }
          }
            else
          {
            if (operands[0].type != OPERAND_ADDRESS_BIT_OFFSET) { continue; }
          }

          if (operands[0].value < 0 || operands[0].value > 0x7f)
          {
            print_error_range(
              asm_context,
              table_pdk15[n].type == OP_IO_N ? "IO" : "Address",
              0, 0x7f);
            return -1;
          }

          if (operands[0].bit < 0 || operands[0].bit > 8)
          {
            print_error_range(asm_context, "Bit offset", 0, 8);
            return -1;
          }

          opcode =
            table_pdk15[n].opcode |
           (operands[0].bit << 7) |
            operands[0].value;
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_A_K:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_A ||
              operands[1].type != OPERAND_IMMEDIATE)
          {
            continue;
          }

          if (operands[1].value < -128 || operands[1].value > 0xff)
          {
            print_error_range(asm_context, "Address", -128, 0xff);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[1].value & 0xff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_K12:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_IMMEDIATE)
          {
            continue;
          }

          if (operands[0].value < -2048 || operands[0].value > 4095)
          {
            print_error_range(asm_context, "Literal", -2048, 4095);
            return -1;
          }

          opcode = table_pdk15[n].opcode | (operands[0].value & 0x7ff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        default:
          break;
      }
    }
  }

  if (matched == 1)
  {
    print_error_unknown_operand_combo(asm_context, instr);
  }
    else
  {
    print_error_unknown_instr(asm_context, instr);
  }

  return -1;
}

