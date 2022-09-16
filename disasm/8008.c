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
#include <stdint.h>
#include <string.h>

#include "disasm/8008.h"
#include "table/8008.h"

static const char reg_name[] = { 'a', 'b', 'c', 'd', 'e', 'h', 'l', 'm' };

int get_cycle_count_8008(uint16_t opcode)
{
  return -1;
}

int disasm_8008(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int n;
  int reg, immediate, port;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read_m(memory, address);

  for (n = 0; table_8008[n].instr != NULL; n++)
  {
    if ((opcode & table_8008[n].mask) == table_8008[n].opcode)
    {
      switch (table_8008[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_8008[n].instr);
          return 1;
        }
        case OP_SREG:
        {
          reg = opcode & 0x7;
          sprintf(instruction, "%s %c", table_8008[n].instr, reg_name[reg]);
          return 1;
        }
        case OP_DREG:
        {
          reg = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s %c", table_8008[n].instr, reg_name[reg]);
          return 1;
        }
        case OP_DREG_NOT_A:
        {
          reg = (opcode >> 3) & 0x7;
          if (reg == 0) { continue; }
          sprintf(instruction, "%s %c", table_8008[n].instr, reg_name[reg]);
          return 1;
        }
        case OP_REG_REG:
        {
          int sreg = opcode & 0x7;
          int dreg = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s %c, %c",
            table_8008[n].instr,
            reg_name[dreg],
            reg_name[sreg]);
          return 1;
        }
        case OP_REG_M:
        {
          reg = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s %c, m", table_8008[n].instr, reg_name[reg]);
          return 1;
        }
        case OP_M_REG:
        {
          reg = opcode & 0x7;
          sprintf(instruction, "%s m, %c", table_8008[n].instr, reg_name[reg]);
          return 1;
        }
        case OP_M:
        {
          sprintf(instruction, "%s m", table_8008[n].instr);
          return 1;
        }
        case OP_ADDRESS:
        {
          immediate =
            memory_read_m(memory, address + 1) |
           (memory_read_m(memory, address + 2) << 8);
          sprintf(instruction, "%s 0x%04x", table_8008[n].instr, immediate);
          return 3;
        }
        case OP_IMMEDIATE:
        {
          immediate = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s 0x%02x", table_8008[n].instr, immediate);
          return 2;
        }
        case OP_M_IMMEDIATE:
        {
          immediate = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s m, 0x%02x", table_8008[n].instr, immediate);
          return 2;
        }
        case OP_REG_IMMEDIATE:
        {
          reg = (opcode >> 3) & 0x7;
          immediate = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s %c, 0x%02x",
            table_8008[n].instr,
            reg_name[reg],
            immediate);
          return 2;
        }
        case OP_SUB:
        {
          int a = (opcode >> 3) & 0x7;
          sprintf(instruction, "%s 0x%04x", table_8008[n].instr, a << 3);
          return 1;
        }
        case OP_PORT_MMM:
        {
          port = (opcode >> 1) & 0x7;
          sprintf(instruction, "%s %d", table_8008[n].instr, port);
          return 1;
        }
        case OP_PORT_MMM_NOT_0:
        {
          port = ((opcode >> 1) & 0x7) + 8;
          sprintf(instruction, "%s %d", table_8008[n].instr, port);
          return 1;
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

  return 2;
}

void list_output_8008(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  char temp[32];
  int count;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_8008(
      &asm_context->memory,
      start,
      instruction,
      &cycles_min,
      &cycles_max);

    char temp2[12];
    int n;

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      sprintf(temp2, " %02x", memory_read_m(&asm_context->memory, start + n));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-9s %-40s cycles: ",
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

void disasm_range_8008(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  char temp[32];
  int cycles_min = 0, cycles_max = 0;
  int count;
  //uint16_t opcode;

  printf("\n");

  printf("%-7s %-9s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- --------- ----------------------------------       ------\n");

  while (start <= end)
  {
    count = disasm_8008(memory, start, instruction, &cycles_min, &cycles_max);

    //opcode = memory_read_m(memory, start);
    char temp2[12];
    int n;

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      sprintf(temp2, " %02x", memory_read_m(memory, start + n));
      strcat(temp, temp2);
    }

    printf("0x%04x: %-9s %-40s ", start, temp, instruction);

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

    start = start + 2;
  }
}

