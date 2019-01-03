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
#include <stdint.h>
#include <string.h>

#include "disasm/sweet16.h"
#include "table/sweet16.h"

int get_cycle_count_sweet16(uint16_t opcode)
{
  return -1;
}

int disasm_sweet16(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int opcode;
  int data;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = memory_read_m(memory, address);

  n = 0;

  while(table_sweet16[n].instr != NULL)
  {
    if ((opcode & table_sweet16[n].mask) == table_sweet16[n].opcode)
    {
      switch(table_sweet16[n].type)
      {
        case SWEET16_OP_NONE:
        {
          strcpy(instruction, table_sweet16[n].instr);
          return 1;
        }
        case SWEET16_OP_REG:
        {
          sprintf(instruction, "%s r%d", table_sweet16[n].instr, opcode & 0xf);
          return 1;
        }
        case SWEET16_OP_AT_REG:
        {
          sprintf(instruction, "%s @r%d", table_sweet16[n].instr, opcode & 0xf);
          return 1;
        }
        case SWEET16_OP_EA:
        {
          int8_t offset = memory_read_m(memory, address + 1);
          sprintf(instruction, "%s 0x%02x (offset=%d)",
             table_sweet16[n].instr, address + 2 + offset, offset);
          return 2;
        }
        case SWEET16_OP_REG_VALUE:
        {
          data = ((memory_read_m(memory, address + 2) << 8) |
                   memory_read_m(memory, address + 1));
          sprintf(instruction, "%s r%d, 0x%04x",
            table_sweet16[n].instr, opcode & 0xf, data);
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

void list_output_sweet16(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  uint32_t opcode;
  char instruction[128];
  char temp[32];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_sweet16(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read_m(&asm_context->memory, start);
    sprintf(temp, "%02x", opcode);

    for (n = 1; n < count; n++)
    {
      char temp2[4];
      sprintf(temp2, " %02x", memory_read_m(&asm_context->memory, start + n));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-8s %-40s", start, temp, instruction);

    start += count;
  }
}

void disasm_range_sweet16(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0,cycles_max = 0;
  int count;
  uint16_t opcode;

  printf("\n");

  printf("%-7s %-5s %-40s\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_sweet16(memory, start, instruction, &cycles_min, &cycles_max);

    opcode = memory_read16_m(memory, start);

    printf("0x%04x: 0x%04x %-40s", start / 2, opcode, instruction);

    start = start + count;
  }
}

