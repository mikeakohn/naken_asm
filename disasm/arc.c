/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/arc.h"
#include "table/arc.h"

#define READ_RAM16(a) ((memory_read_m(memory, a + 0) << 8) | \
                       (memory_read_m(memory, a + 1)))

#define READ_RAM32(a) ((memory_read_m(memory, a + 0) << 24) | \
                       (memory_read_m(memory, a + 1) << 16) | \
                       (memory_read_m(memory, a + 2) << 8) | \
                       (memory_read_m(memory, a + 3)))

int map16_bit_register(int r)
{
  if (r <= 3) { return r; }

  return r + 8;
}

int get_cycle_count_arc(unsigned short int opcode)
{
  return -1;
}

int disasm_arc(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  uint32_t opcode;
  int n, c, b, f, u;
  int limm;

  opcode = (memory_read16_m(memory, address) << 16) |
            memory_read16_m(memory, address + 2);

  *cycles_min = -1;
  *cycles_max = -1;

  n = 0;
  while(table_arc[n].instr != NULL)
  {
    if ((opcode & table_arc[n].mask) == table_arc[n].opcode)
    {
      switch(table_arc[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_arc[n].instr);
          return 4;
        }
        case OP_B_C:
        {
          c = (opcode >> 6) & 0x3f;
          b = (((opcode >> 12) & 0x7) << 3) | ((opcode >> 24) & 0x7);
          f = (opcode >> 15) & 0x1;

          sprintf(instruction, "%s%s r%d, r%d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, c);

          return 4;
        }
        case OP_B_U6:
        {
          u = (opcode >> 6) & 0x3f;
          b = (((opcode >> 12) & 0x7) << 3) | ((opcode >> 24) & 0x7);
          f = (opcode >> 15) & 0x1;

          sprintf(instruction, "%s%s r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, u);

          return 4;
        }
        case OP_B_LIMM:
        {
          b = (((opcode >> 12) & 0x7) << 3) | ((opcode >> 24) & 0x7);
          f = (opcode >> 15) & 0x1;

          limm = (memory_read16_m(memory, address + 4) << 16) |
                  memory_read16_m(memory, address + 6);

          sprintf(instruction, "%s%s r%d, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            b, limm);

          return 8;
        }
        case OP_0_C:
        {
          c = (opcode >> 6) & 0x3f;
          f = (opcode >> 15) & 0x1;

          sprintf(instruction, "%s%s 0, r%d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            c);

          return 4;
        }
        case OP_0_U6:
        {
          u = (opcode >> 6) & 0x3f;
          f = (opcode >> 15) & 0x1;

          sprintf(instruction, "%s%s 0, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            u);

          return 4;
        }
        case OP_0_LIMM:
        {
          b = (((opcode >> 12) & 0x7) << 3) | ((opcode >> 24) & 0x7);
          f = (opcode >> 15) & 0x1;

          limm = (memory_read16_m(memory, address + 4) << 16) |
                  memory_read16_m(memory, address + 6);

          sprintf(instruction, "%s%s 0, %d",
            table_arc[n].instr,
            f == 0 ? "" : ".f",
            limm);

          return 8;
        }
        default:
        {
          break;
        }
      }
    }

    n++;
  }

  opcode = memory_read16_m(memory, address);

  n = 0;
  while(table_arc16[n].instr != NULL)
  {
    if ((opcode & table_arc16[n].mask) == table_arc16[n].opcode)
    {
      switch(table_arc16[n].type)
      {
        case OP_NONE:
        {
          strcpy(instruction, table_arc16[n].instr);
          return 2;
        }
        case OP_B_C:
        {
          c = map16_bit_register((opcode >> 5) & 0x7);
          b = map16_bit_register((opcode >> 8) & 0x7);

          sprintf(instruction, "%s r%d, r%d",
            table_arc16[n].instr,
            b, c);

          return 2;
        }
        default:
        {
          break;
        }
      }
    }

    n++;
  }

  strcpy(instruction, "???");

  return 4;
}

void list_output_arc(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint32_t opcode;
  int count, n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    struct _memory *memory = &asm_context->memory;

    count = disasm_arc(memory, start, instruction, &cycles_min, &cycles_max);

    if (count < 4)
    {
      opcode = memory_read16_m(memory, start);
      fprintf(asm_context->list, "0x%04x: %04x     %-40s\n", start, opcode, instruction);
    }
      else
    {
      opcode = (memory_read16_m(memory, start) << 16) |
                memory_read16_m(memory, start + 2);
      fprintf(asm_context->list, "0x%04x: %08x %-40s\n", start, opcode, instruction);
    }

    for (n = 4; n < count; n = n + 4)
    {
      opcode = (memory_read16_m(memory, start + n) << 16) |
                memory_read16_m(memory, start + n + 2);

      fprintf(asm_context->list, "        %08x\n", opcode);
    }

    start += count;
  }
}

void disasm_range_arc(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min, cycles_max;
  uint16_t opcode;
  int count;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_arc(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = (memory_read16_m(memory, start) << 16) |
              memory_read16_m(memory, start + 2);

    printf("0x%04x: %04x %-40s\n", start, opcode, instruction);

    start = start + count;
  }
}

