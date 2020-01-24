/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/86000.h"
#include "table/86000.h"

int get_cycle_count_86000(unsigned short int opcode)
{
  return -1;
}

int disasm_86000(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int opcode, value, immediate, reg, bit;
  int8_t offset;
  int16_t offset16;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read_m(memory, address);

  strcpy(instruction, "???");

  n = 0;
 
  while (table_86000[n].name != NULL)
  {
    if ((opcode & table_86000[n].mask) == table_86000[n].opcode)
    {
      *cycles_min = table_86000[n].cycles;
      *cycles_max = table_86000[n].cycles;

      switch (table_86000[n].type)
      {
        case OP_NONE:
          strcpy(instruction, table_86000[n].name);
          return 1;
        case OP_ADDRESS:
          value = ((opcode & 1) << 8) |memory_read_m(memory, address + 1);
          sprintf(instruction, "%s 0x%02x", table_86000[n].name, value);
          return 2;
        case OP_IMMEDIATE:
          immediate = memory_read_m(memory, address + 1);

          sprintf(instruction, "%s #0x%02x",
            table_86000[n].name,
            immediate);
          return 2;
        case OP_AT_REG:
          reg = opcode & 0x3;
          sprintf(instruction, "%s #0x%02x", table_86000[n].name, reg);
          return 1;
        case OP_ADDRESS_RELATIVE8:
          value = memory_read_m(memory, address + 1);
          offset = memory_read_m(memory, address + 2);

          sprintf(instruction, "%s 0x%02x, 0x%04x (offset=%d)",
            table_86000[n].name,
            value,
            address + 3 + offset,
            offset);
          return 3;
        case OP_IMMEDIATE_RELATIVE8:
          immediate = memory_read_m(memory, address + 1);
          offset = memory_read_m(memory, address + 2);

          sprintf(instruction, "%s #0x%02x, 0x%04x (offset=%d)",
            table_86000[n].name,
            immediate,
            address + 3 + offset,
            offset);
          return 3;
        case OP_AT_REG_IMMEDIATE_RELATIVE8:
          reg = opcode & 0x3;
          immediate = memory_read_m(memory, address + 1);
          offset = memory_read_m(memory, address + 2);

          sprintf(instruction, "%s @r%d, #0x%02x, %04x (offset=%d)",
            table_86000[n].name,
            reg,
            immediate,
            address + 3 + offset,
            offset);
          return 3;
        case OP_ADDRESS_BIT_RELATIVE8:
          bit = opcode & 0x7;
          value = ((opcode >> 4) & 1) | memory_read_m(memory, address + 1);
          offset = memory_read_m(memory, address + 2);

          sprintf(instruction, "%s #0x%02x, %d, 0x%04x (offset=%d)",
            table_86000[n].name,
            value,
            bit,
            address + 3 + offset,
            offset);
          return 3;
        case OP_RELATIVE8:
          offset = memory_read_m(memory, address + 1);

          sprintf(instruction, "%s 0x%04x (offset=%d)",
            table_86000[n].name,
            address + 2 + offset,
            offset);
          return 2;
        case OP_RELATIVE16:
          offset16 = (memory_read_m(memory, address + 2) << 8) |
                      memory_read_m(memory, address + 1);

          sprintf(instruction, "%s 0x%04x (offset=%d)",
            table_86000[n].name,
            address + 2 + offset16,
            offset16);
          return 3;
        case OP_ADDRESS12:
          value = ((opcode & 0x10) << 7) |
                  ((opcode & 0x07) << 8) |
                  memory_read_m(memory, address + 1);

          sprintf(instruction, "%s 0x%04x",
            table_86000[n].name,
            value);
          return 2;
        case OP_ADDRESS16:
          value = (memory_read_m(memory, address + 1) << 8) |
                   memory_read_m(memory, address + 2);

          sprintf(instruction, "%s 0x%04x",
            table_86000[n].name,
            value);
          return 3;
        case OP_ADDRESS_BIT:
          bit = opcode & 0x7;
          value = (((opcode >> 4) & 1) << 8) | memory_read_m(memory, address + 1);

          sprintf(instruction, "%s 0x%04x, %d",
            table_86000[n].name,
            value,
            bit);
          return 2;
        case OP_AT_REG_RELATIVE8:
          reg = opcode & 0x3;
          offset = memory_read_m(memory, address + 1);

          sprintf(instruction, "%s @r%d, 0x%04x (offset=%d)",
            table_86000[n].name,
            reg,
            address + 2 + offset,
            offset);
          return 2;
        case OP_IMMEDIATE_ADDRESS:
          value = ((opcode & 1) << 8) | memory_read_m(memory, address + 1);
          immediate = memory_read_m(memory, address + 2);

          sprintf(instruction, "%s #0x%02x, 0x%04x",
            table_86000[n].name,
            immediate,
            value);
          return 3;
        case OP_IMMEDIATE_AT_REG:
          reg = opcode & 0x3;
          immediate = memory_read_m(memory, address + 2);

          sprintf(instruction, "%s @r%d, 0x%04x",
            table_86000[n].name,
            immediate,
            address);
          return 2;
      }
    }

    n++;
  }

  return 1;
}

void list_output_86000(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min = -1, cycles_max = -1, count;
  char instruction[128];
  char temp[32];
  char temp2[4];
  int n;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_86000(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      sprintf(temp2, "%02x ", memory_read_m(&asm_context->memory, start + n));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-10s %-40s cycles: ", start, temp, instruction);

    if (cycles_min < 0)
    {
      fprintf(asm_context->list, "?");
    }
      else
    {
      fprintf(asm_context->list, "%d", cycles_min);
    }

    start += count;
  }
}

void disasm_range_86000(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  char temp[32];
  char temp2[4];
  int cycles_min = 0, cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_86000(memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      sprintf(temp2, "%02x ", memory_read_m(memory, start + n));
      strcat(temp, temp2);
    }

    printf("0x%04x: %-10s %-40s cycles: ", start, temp, instruction);

    if (cycles_min < 0)
    {
      printf("?");
    }
      else
    {
      printf("%d", cycles_min);
    }

    start += count;
  }
}

