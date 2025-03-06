/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/pdp11.h"
#include "table/pdp11.h"

int disasm_pdp11(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int n;
  int i, z, a;

  opcode = memory->read16(address);

  for (int n = 0; table_pdp11[n].instr != NULL; n++)
  {
    if ((opcode & table_pdp11[n].mask) == table_pdp11[n].opcode)
    {
      switch (table_pdp11[n].type)
      {
        case OP_NONE:
        {
          snprintf(instruction, length, "%s", table_pdp11[n].instr);
          return 2;
        }
        default:
        {
          //print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }

    n++;
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_pdp11(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  uint32_t opcode;
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int count;

  Memory *memory = &asm_context->memory;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_pdp11(
      memory,
      start,
      instruction,
      sizeof(instruction),
      asm_context->flags,
      &cycles_min,
      &cycles_max);

    opcode = memory->read16(start);

    fprintf(asm_context->list, "0%04o: %04o %s\n", start / 2, opcode, instruction);

    start += count;
  }
}

void disasm_range_pdp11(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  uint16_t opcode;

  printf("\n");

  printf("%-5s  %-5s %s\n", "Addr", "Opcode", "Instruction");
  printf("-----  ------ ----------------------------------\n");

  while (start <= end)
  {
    disasm_pdp11(
      memory,
      start,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    opcode = memory->read16(start);

    printf("0%04o: %04o   %s\n", start / 2, opcode, instruction);

    start = start + 2;
  }
}

