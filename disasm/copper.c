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

#include "disasm/copper.h"
//#include "table/copper.h"

int get_cycle_count_copper(unsigned short int opcode)
{
  return -1;
}

int disasm_copper(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint16_t opcode;
  uint16_t data;

  opcode = memory_read16_m(memory, address);
  data = memory_read16_m(memory, address + 2);

  // FIXME: Not sure if the cycle counts is right..

  if ((opcode & 0xfe01) == 0)
  {
    // MOVE instruction.
    *cycles_min = 4;
    *cycles_max = 4;

    sprintf(instruction, "move 0x%02x, 0x%04x", opcode & 0x01ff, data);
  }
    else
  {
    char temp[8];
    int x, y, compare_x, compare_y, bfd;

    if ((data & 0x0001) == 0)
    {
      // WAIT instruction.
      *cycles_min = 3;
      *cycles_max = 6;

      strcpy(temp, "wait");
    }
      else
    {
      // SKIP instruction.
      *cycles_min = 4;
      *cycles_max = 4;

      strcpy(temp, "skip");
    }

    bfd = (data >> 15) & 1;
    x = (opcode >> 1) & 0x7f;
    y = (opcode >> 8) & 0xff;
    compare_x = (data >> 1) & 0x7f;
    compare_y = (data >> 8) & 0x7f;

    sprintf(instruction, "%s %s%d, %d, 0x%02x, 0x%02x",
      temp, (bfd == 1) ? "bfd, " : "", x, y, compare_x, compare_y);
  }

  return 4;
}

void list_output_copper(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  int opcode, data;

  fprintf(asm_context->list, "\n");

  disasm_copper(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);
  opcode = memory_read16_m(&asm_context->memory, start);
  data = memory_read16_m(&asm_context->memory, start + 2);

  if (cycles_min < 0)
  {
    fprintf(asm_context->list, "0x%04x: %04x %04x %-40s", start, opcode, data, instruction);
  }
    else
  if (cycles_min == cycles_max)
  {
    fprintf(asm_context->list, "0x%04x: %04x %04x %-40s cycles: %d\n", start, opcode, data, instruction, cycles_min);
  }
      else
  {
    fprintf(asm_context->list, "0x%04x: %04x %04x %-40s cycles: %d-%d\n", start, opcode, data, instruction, cycles_min, cycles_max);
  }
}

void disasm_range_copper(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int count;
  int opcode, data;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_copper(memory, start, instruction, &cycles_min, &cycles_max);
    opcode = memory_read16_m(memory, start);
    data = memory_read16_m(memory, start);

    if (cycles_min < 1)
    {
      printf("0x%04x: %04x %04x %-40s ?\n", start, opcode, data, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: %04x %04x %-40s %d\n", start, opcode, data, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %04x %04x %-40s %d-%d\n", start, opcode, data, instruction, cycles_min, cycles_max);
    }

    start += count;
  }
}

