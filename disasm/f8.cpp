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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disasm/f8.h"
#include "table/f8.h"

int disasm_f8(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int data;
  int n;
  int8_t disp;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory->read8(address);

  for (n = 0; table_f8[n].instr != NULL; n++)
  {
    if ((opcode & table_f8[n].mask) == table_f8[n].opcode)
    {
      // According to Wikipedia, every machine cycle is 8 clock cycles.
      *cycles_min = table_f8[n].cycles_min;
      *cycles_max = table_f8[n].cycles_max;

      switch (table_f8[n].type)
      {
        case F8_OP_NONE:
        {
          strcpy(instruction, table_f8[n].instr);
          return 1;
        }
        case F8_OP_DATA3:
        {
          snprintf(instruction, length, "%s %d",
            table_f8[n].instr,
            opcode & 0x7);
          return 1;
        }
        case F8_OP_DATA4:
        {
          snprintf(instruction, length, "%s %d",
            table_f8[n].instr,
            opcode & 0xf);
          return 1;
        }
        case F8_OP_DATA8:
        {
          data = memory->read8(address + 1);

          snprintf(instruction, length, "%s %d",
            table_f8[n].instr,
            data);
          return 2;
        }
        case F8_OP_ADDR:
        {
          data = memory->read16(address + 1);

          snprintf(instruction, length, "%s %d",
            table_f8[n].instr,
            data);
          return 3;
        }
        case F8_OP_DISP:
        {
          disp = memory->read8(address + 1);

          snprintf(instruction, length, "%s 0x%04x (offset=%d)",
            table_f8[n].instr,
            address + 1 + disp,
            disp);
          return 2;
        }
        case F8_OP_DATA3_DISP:
        {
          disp = memory->read8(address + 1);

          snprintf(instruction, length, "%s %d, 0x%04x (offset=%d)",
            table_f8[n].instr,
            opcode & 7,
            address + 1 + disp,
            disp);
          return 2;
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

  return 1;
}

void list_output_f8(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  char temp[32];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  Memory *memory = &asm_context->memory;

  while (start < end)
  {
    count = disasm_f8(
      memory,
      start,
      instruction,
      sizeof(instruction),
      asm_context->flags,
      &cycles_min,
      &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      char temp2[4];
      snprintf(temp2, sizeof(temp2), " %02x", memory->read8(start + n));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-8s %-40s cycles: ",
      start, temp, instruction);

    if (cycles_min == 0)
    {
      fprintf(asm_context->list, "?\n");
    }
      else
    if (cycles_min == cycles_max)
    {
      fprintf(asm_context->list, "%d\n", cycles_min);
    }
      else
    {
      fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max);
    }

    start += count;
  }
}

void disasm_range_f8(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  char temp[32];
  int cycles_min = 0, cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-8s %-9s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("-------  --------- ------------------------------           ------\n");

  while (start <= end)
  {
    count = disasm_f8(
      memory,
      start,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      char temp2[4];
      snprintf(temp2, sizeof(temp2), " %02x", memory->read8(start + n));
      strcat(temp, temp2);
    }

    printf("0x%04x: %-10s %-40s ", start, temp, instruction);

    if (cycles_min == 0)
    {
      printf("?\n");
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("%d\n", cycles_min);
    }
      else
    {
      printf("%d-%d\n", cycles_min, cycles_max);
    }

    start = start + count;
  }
}

