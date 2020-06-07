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

#include "disasm/6800.h"
#include "table/6800.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) (memory_read_m(memory, a) << 8) | \
                       memory_read_m(memory, a + 1)

int get_cycle_count_6800(unsigned short int opcode)
{
  return -1;
}

int disasm_6800(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  int opcode;
  int size = 1;

  *cycles_min = -1;
  *cycles_max = -1;

  opcode = READ_RAM(address);

  switch(table_6800[opcode].operand_type)
  {
    case M6800_OP_UNDEF:
      strcpy(instruction, "???");
      break;
    case M6800_OP_NONE:
      strcpy(instruction, table_6800[opcode].instr);
      break;
    case M6800_OP_IMM8:
      sprintf(instruction, "%s #$%02x", table_6800[opcode].instr, READ_RAM(address+1));
      size = 2;
      break;
    case M6800_OP_IMM16:
      sprintf(instruction, "%s #$%04x", table_6800[opcode].instr, READ_RAM16(address + 1));
      size = 3;
      break;
    case M6800_OP_DIR_PAGE_8:
      sprintf(instruction, "%s $%02x", table_6800[opcode].instr, READ_RAM(address + 1));
      size = 2;
      break;
    case M6800_OP_ABSOLUTE_16:
      sprintf(instruction, "%s $%04x", table_6800[opcode].instr, READ_RAM16(address + 1));
      size = 3;
      break;
    case M6800_OP_NN_X:
      sprintf(instruction, "%s $%02x, X", table_6800[opcode].instr, READ_RAM(address + 1));
      size = 2;
      break;
    case M6800_OP_REL_OFFSET:
      sprintf(instruction, "%s $%04x, X  (%d)", table_6800[opcode].instr, (address + 2) + (int8_t)(READ_RAM(address + 1)), (int8_t)READ_RAM(address + 1));
      size = 2;
      break;
  }

  return size;
}

void list_output_6800(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  char bytes[10];
  int count;
  int n;

  fprintf(asm_context->list, "\n");

  while(start < end)
  {
    count = disasm_6800(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;
    for (n = 0; n < count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", memory_read_m(&asm_context->memory, start + n));
      strcat(bytes, temp);
    }

    fprintf(asm_context->list, "0x%04x: %-9s %-40s cycles: ", start, bytes, instruction);

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

void disasm_range_6800(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  char bytes[10];
  int cycles_min = 0,cycles_max = 0;
  int count;
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_6800(memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0] = 0;
    for (n = 0; n < count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", READ_RAM(start + n));
      strcat(bytes, temp);
    }

    if (cycles_min < 1)
    {
      printf("0x%04x: %-9s %-40s ?\n", start, bytes, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: %-9s %-40s %d\n", start, bytes, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %-9s %-40s %d-%d\n", start, bytes, instruction, cycles_min, cycles_max);
    }

    start = start + count;
  }
}

