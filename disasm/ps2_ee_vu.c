/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/ps2_ee_vu.h"

#define READ_RAM(a) memory_read_m(memory, a)

int get_cycle_count_ps2_ee_vu(uint32_t opcode)
{
  return -1;
}

int disasm_ps2_ee_vu(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint32_t opcode;
  int function, format, operation;
  int n, r;
  char temp[32];

  *cycles_min = -1;
  *cycles_max = -1;
  opcode = memory_read32_m(memory, address);

  instruction[0] = 0;

  n = 0;
  while(table_ps2_ee_vu[n].instr != NULL)
  {
    if (table_ps2_ee_vu[n].opcode == (opcode & table_ps2_ee_vu[n].mask))
    {
      strcpy(instruction, table_ps2_ee_vu[n].instr);

      for (r = 0; r < table_ps2_ee_vu[n].operand_count; r++)
      {
        if (r != 0) { strcat(instruction, ","); }

        switch(table_ps2_ee_vu[n].operand[r])
        {
          default:
            strcpy(temp, " ?");
            break;
        }

        strcat(instruction, temp);
      }

      return 4;
    }

    n++;
  }

  return 4;
}

void list_output_ps2_ee_vu(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  uint32_t opcode;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    opcode = memory_read32_m(&asm_context->memory, start);

    disasm_ps2_ee_vu(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    fprintf(asm_context->list, "0x%08x: 0x%08x %-40s cycles: ", start, opcode, instruction);

    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

    start += 4;
  }
}

void disasm_range_ps2_ee_vu(struct _memory *memory, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0,cycles_max = 0;
  int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start < end)
  {
    // FIXME - Need to check endian
    num = READ_RAM(start) |
          (READ_RAM(start + 1) << 8) |
          (READ_RAM(start + 2) << 16) |
          (READ_RAM(start + 3) << 24);

    disasm_ps2_ee_vu(memory, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min < 1)
    {
      printf("0x%04x: 0x%08x %-40s ?\n", start, num, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: 0x%08x %-40s %d\n", start, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%08x %-40s %d-%d\n", start, num, instruction, cycles_min, cycles_max);
    }

    start += 4;
  }
}

