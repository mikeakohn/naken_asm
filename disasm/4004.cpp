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
#include <stdint.h>
#include <string.h>

#include "disasm/4004.h"
#include "table/4004.h"

int disasm_4004(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int *cycles_min,
  int *cycles_max)
{
  int opcode;
  int n;
  int r, p, a, c, d;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory->read8(address);

  n = 0;
  while (table_4004[n].instr != NULL)
  {
    if ((opcode & table_4004[n].mask) == table_4004[n].opcode)
    {
      *cycles_min = 16;
      *cycles_max = 16;

      switch (table_4004[n].type)
      {
        case OP_NONE:
        {
          *cycles_min = 8;
          *cycles_max = 8;
          strcpy(instruction, table_4004[n].instr);
          return 1;
        }
        case OP_R:
        case OP_DATA:
        {
          *cycles_min = 8;
          *cycles_max = 8;
          r = opcode & 0xf;
          snprintf(instruction, length, "%s %d", table_4004[n].instr, r);
          return 1;
        }
        case OP_P:
        {
          *cycles_min = 8;
          *cycles_max = 8;
          p = opcode & 0xe;
          snprintf(instruction, length, "%s %d", table_4004[n].instr, p);
          return 1;
        }
        case OP_ADDR12:
        {
          a = ((opcode & 0xf) << 8) | memory->read8(address + 1);
          snprintf(instruction, length, "%s 0x%x", table_4004[n].instr, a);
          return 2;
        }
        case OP_P_DATA:
        {
          p = opcode & 0xe;
          d = memory->read8(address + 1);
          snprintf(instruction, length, "%s %d 0x%x", table_4004[n].instr, p, d);
          return 2;
        }
        case OP_R_ADDR8:
        {
          r = opcode & 0xf;
          a = memory->read8(address + 1);
          snprintf(instruction, length, "%s %d 0x%x", table_4004[n].instr, r, a);
          return 2;
        }
        case OP_COND:
        {
          c = opcode & 0xf;
          a = memory->read8(address + 1);
          snprintf(instruction, length, "%s %d 0x%x", table_4004[n].instr, c, a);
          return 2;
        }
        case OP_COND_ALIAS:
        {
          a = memory->read8(address + 1);
          snprintf(instruction, length, "%s 0x%x", table_4004[n].instr, a);
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

void list_output_4004(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min, cycles_max;
  uint32_t opcode;
  char instruction[128];
  char temp[32];
  int count;

  Memory *memory = &asm_context->memory;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_4004(
      memory,
      start,
      instruction,
      sizeof(instruction),
      &cycles_min,
      &cycles_max);

    opcode = memory->read8(start);
    snprintf(temp, sizeof(temp), "%02x", opcode);

    if (count == 2)
    {
      char temp2[4];
      snprintf(temp2, sizeof(temp2), " %02x", memory->read8(start + 1));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-6s %-40s cycles: ", start, temp, instruction);

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

void disasm_range_4004(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  uint16_t opcode;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    disasm_4004(
      memory,
      start,
      instruction,
      sizeof(instruction),
      &cycles_min,
      &cycles_max);

    opcode = memory->read16(start);

    printf("0x%04x: 0x%04x %-40s ", start / 2, opcode, instruction);

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

