/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disasm/1802.h"
#include "table/1802.h"

int get_cycle_count_1802(uint16_t opcode)
{
  return -1;
}

int disasm_1802(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int data;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read_m(memory, address);

  if (opcode == 0x68)
  {
    opcode = memory_read_m(memory, address + 1);

    n = 0;
    while (table_1802_16[n].instr != NULL)
    {
      if ((opcode & table_1802_16[n].mask) == table_1802_16[n].opcode)
      {
        // According to Wikipedia, every machine cycle is 8 clock cycles.
        *cycles_min = table_1802_16[n].cycles * 8;
        *cycles_max = *cycles_min;

        switch (table_1802_16[n].type)
        {
          case RCA1802_OP_NONE:
          {
            strcpy(instruction, table_1802_16[n].instr);
            return 2;
          }
          case RCA1802_OP_REG:
          {
            sprintf(instruction, "%s %d", table_1802_16[n].instr, opcode & 0xf);
            return 2;
          }
          case RCA1802_OP_IMMEDIATE:
          {
            data = memory_read_m(memory, address + 2);
            sprintf(instruction, "%s 0x%02x", table_1802_16[n].instr, data);
            return 3;
          }
          case RCA1802_OP_BRANCH:
          {
            data = memory_read_m(memory, address + 2);
            sprintf(instruction, "%s 0x%04x",
              table_1802_16[n].instr, (address & 0xff00) | data);
            return 3;
          }
          case RCA1802_OP_REG_BRANCH:
          {
            data = memory_read_m(memory, address + 2);
            sprintf(instruction, "%s %d, 0x%04x",
              table_1802_16[n].instr, opcode & 0xf, (address & 0xff00) | data);
            return 3;
          }
        }
      }

      n++;
    }

    strcpy(instruction, "???");
    return 1;
  }

  n = 0;
  while (table_1802[n].instr != NULL)
  {
    if ((opcode & table_1802[n].mask) == table_1802[n].opcode)
    {
      // According to Wikipedia, every machine cycle is 8 clock cycles.
      *cycles_min = table_1802[n].cycles * 8;
      *cycles_max = *cycles_min;

      switch (table_1802[n].type)
      {
        case RCA1802_OP_NONE:
        {
          strcpy(instruction, table_1802[n].instr);
          return 1;
        }
        case RCA1802_OP_REG:
        {
          sprintf(instruction, "%s %d", table_1802[n].instr, opcode & 0xf);
          return 1;
        }
        case RCA1802_OP_NUM_1_TO_7:
        {
          sprintf(instruction, "%s %d", table_1802[n].instr, opcode & 0x7);
          return 1;
        }
        case RCA1802_OP_IMMEDIATE:
        {
          data = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s 0x%02x", table_1802[n].instr, data);
          return 2;
        }
        case RCA1802_OP_BRANCH:
        {
          data = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s 0x%04x",
            table_1802[n].instr, (address & 0xff00) | data);
          return 2;
        }
        case RCA1802_OP_LONG_BRANCH:
        {
          data = ((memory_read_m(memory, address + 1) << 8) |
                   memory_read_m(memory, address + 2));
          sprintf(instruction, "%s 0x%04x",
            table_1802[n].instr, data);
          return 3;
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

  return 1;
}

void list_output_1802(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  char temp[32];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_1802(
      &asm_context->memory,
      start,
      instruction,
      &cycles_min,
      &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      char temp2[4];
      sprintf(temp2, " %02x", memory_read_m(&asm_context->memory, start + n));
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

void disasm_range_1802(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  char temp[32];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-8s %-9s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("-------  --------- ------------------------------           ------\n");

  while (start <= end)
  {
    count = disasm_1802(memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      char temp2[4];
      sprintf(temp2, " %02x", memory_read_m(memory, start + n));
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

