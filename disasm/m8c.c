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

#include "disasm/m8c.h"
#include "table/m8c.h"

int get_cycle_count_m8c(unsigned short int opcode)
{
  return -1;
}

int append_operand(
  struct _memory *memory,
  char *instruction,
  uint8_t operand,
  uint32_t address,
  uint8_t opcode)
{
  char temp[32];
  uint8_t data8;
  uint16_t data16;
  int16_t offset;

  switch (operand)
  {
    case OP_A:
      strcat(instruction, "A");
      return 0;
    case OP_X:
      strcat(instruction, "X");
      return 0;
    case OP_F:
      strcat(instruction, "F");
      return 0;
    case OP_SP:
      strcat(instruction, "SP");
      return 0;
    case OP_EXPR:
      data8 = memory_read_m(memory, address);
      sprintf(temp, "0x%02x", data8);
      strcat(instruction, temp);
      return 1;
    case OP_INDEX_EXPR:
      data8 = memory_read_m(memory, address);
      sprintf(temp, "[0x%02x]", data8);
      strcat(instruction, temp);
      return 1;
    case OP_INDEX_X_EXPR:
      data8 = memory_read_m(memory, address);
      sprintf(temp, "[X+0x%02x]", data8);
      strcat(instruction, temp);
      return 1;
#if 0
    case OP_INDEX_EXPR_INC:
      data8 = memory_read_m(memory, address);
      sprintf(temp, "[[0x%02x]++]", data8);
      strcat(instruction, temp);
      return 1;
#endif
    case OP_REG_INDEX_EXPR:
      data8 = memory_read_m(memory, address);
      sprintf(temp, "REG[0x%02x]", data8);
      strcat(instruction, temp);
      return 1;
    case OP_REG_INDEX_X_EXPR:
      data8 = memory_read_m(memory, address);
      sprintf(temp, "REG[X+0x%02x]", data8);
      strcat(instruction, temp);
      return 1;
    case OP_EXPR_S12:
      offset = memory_read_m(memory, address);
      offset |= (opcode & 0xf) << 8;
      if ((offset & 0x800) != 0) { offset |= 0xf000; }
      sprintf(temp, "0x%04x (offset=%d)", address + 1 + offset, offset);
      strcat(instruction, temp);
      return 1;
    case OP_EXPR_S12_JUMP:
      offset = memory_read_m(memory, address);
      offset |= (opcode & 0xf) << 8;
      if ((offset & 0x800) != 0) { offset |= 0xf000; }
      sprintf(temp, "0x%04x (offset=%d)", address + offset, offset);
      strcat(instruction, temp);
      return 1;
    case OP_EXPR_U16:
      data16 = (memory_read_m(memory, address) << 8) |
                memory_read_m(memory, address + 1);
      sprintf(temp, "0x%04x", data16);
      strcat(instruction, temp);
      return 2;
  }

  return 0;
}

int disasm_m8c(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  uint32_t op_address = address + 1;
  int n;

  *cycles_min = -1;
  *cycles_max = -1;

  uint8_t opcode = memory_read_m(memory, address);

  // memory_read_m(memory, address);

  n = 0;

  while (table_m8c[n].instr != NULL)
  {
    if (table_m8c[n].opcode != (opcode & table_m8c[n].mask))
    {
      n++;
      continue;
    }

    *cycles_min = table_m8c[n].cycles;

    strcpy(instruction, table_m8c[n].instr);

    if (table_m8c[n].operand_0 == OP_NONE)
    {
      return table_m8c[n].byte_count;
    }

    strcat(instruction, " ");

    op_address += append_operand(memory, instruction, table_m8c[n].operand_0, op_address, opcode);

    if (table_m8c[n].operand_1 == OP_NONE)
    {
      return table_m8c[n].byte_count;
    }

    strcat(instruction, ", ");

    op_address += append_operand(memory, instruction, table_m8c[n].operand_1, op_address, opcode);

    return table_m8c[n].byte_count;
  }

  strcpy(instruction, "???");

  return 1;
}

void list_output_m8c(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
  int cycles_min, cycles_max;
  char instruction[128];
  int opcode, count;
  char hex[80];
  char temp[16];
  int n;

  count = disasm_m8c(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

  hex[0] = 0;

  for (n = 0; n < count; n++)
  {
    opcode = memory_read_m(&asm_context->memory, start + n);

    sprintf(temp, "%02x ", opcode);
    strcat(hex, temp);
  }

  fprintf(asm_context->list, "0x%04x: %-12s %-30s cycles=%d\n", start, hex, instruction, cycles_min);
}

void disasm_range_m8c(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
  char instruction[128];
  int cycles_min = 0, cycles_max = 0;
  int opcode, count;
  char hex[80];
  char temp[16];
  int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start <= end)
  {
    count = disasm_m8c(memory, start, instruction, &cycles_min, &cycles_max);

     hex[0] = 0;

     for (n = 0; n < count; n++)
     {
       opcode = memory_read_m(memory, start + n);

       sprintf(temp, "%02x ", opcode);
       strcat(hex, temp);
     }

    printf("0x%04x: %-20s %-40s %d\n", start, hex, instruction, cycles_min);

    start += count;
  }
}

