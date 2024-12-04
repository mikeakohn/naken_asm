/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

// Apollo Guidance Computer.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/agc.h"
#include "table/agc.h"

int disasm_agc(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  uint16_t opcode;
  int n;
  int count;
  bool is_extra_code = false;

  opcode = memory->read16(address);

  // Opcode is EXTRA.
  if (opcode == 000006)
  {
     is_extra_code = true;
     opcode = memory->read16(address + 2);
  }

  count = is_extra_code ? 4 : 2;

  *cycles_min = -1;
  *cycles_max = -1;

  for (n = 0; table_agc[n].instr != NULL; n++)
  {
    if (((opcode & table_agc[n].mask) == table_agc[n].opcode) &&
        table_agc[n].is_extra_code == is_extra_code)
    {
      *cycles_min = table_agc[n].cycles;
      *cycles_max = table_agc[n].cycles;

      switch (table_agc[n].type)
      {
        case AGC_OP_NONE:
        {
          snprintf(instruction, length, "%s", table_agc[n].instr);
          return count;
        }
        case AGC_OP_K10:
        {
          snprintf(instruction, length, "%s 0%o",
            table_agc[n].instr,
            opcode & 0x3ff);

          return count;
        }
        case AGC_OP_K12:
        {
          snprintf(instruction, length, "%s 0%o",
            table_agc[n].instr,
            opcode & 0xfff);

          return count;
        }
        case AGC_OP_IO:
        {
          snprintf(instruction, length, "%s 0%o",
            table_agc[n].instr,
            opcode & 0x1ff);

          return count;
        }
        case AGC_OP_NDX:
        {
          if (is_extra_code)
          {
            snprintf(instruction, length, "%s 0%o",
              table_agc[n].instr,
              opcode & 0xfff);
          }
            else
          {
            snprintf(instruction, length, "%s 0%o",
              table_agc[n].instr,
              opcode & 0x3ff);
          }

          return count;
        }
        default:
        {
          //print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }
  }

  strcpy(instruction, "???");

  return count;
}

void list_output_agc(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  char temp[32];
  int cycles_min, cycles_max;
  int count;

  Memory *memory = &asm_context->memory;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_agc(
      memory,
      start,
      instruction,
      sizeof(instruction),
      asm_context->flags,
      &cycles_min,
      &cycles_max);

    if (count == 2)
    {
      snprintf(temp, sizeof(temp), "      %05o ", memory->read16(start));
    }
      else
    {
      snprintf(temp, sizeof(temp), "%05o %05o ",
        memory->read16(start + 0),
        memory->read16(start + 2));
    }

    fprintf(asm_context->list, "0%04o: %-11s %-40s  %d\n",
      start / 2,
      temp,
      instruction,
      cycles_min);

    start += count;
  }
}

void disasm_range_agc(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  char temp[32];
  int cycles_min, cycles_max;
  int count;

  printf("\n");

  printf("%-7s %-11s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ----------- ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_agc(
      memory,
      start,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    if (count == 2)
    {
      snprintf(temp, sizeof(temp), "      %05o", memory->read16(start));
    }
      else
    {
      snprintf(temp, sizeof(temp), "%05o %05o",
        memory->read16(start + 0),
        memory->read16(start + 2));
    }

    printf("0%04o: %-11s %-40s  %d\n",
      start / 2,
      temp,
      instruction,
      cycles_min);

    start = start + count;
  }
}

