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
#include <string.h>

#include "disasm/8051.h"
#include "table/8051.h"

#define READ_RAM(a) memory_read_m(memory, a)

int get_cycle_count_8051(uint32_t opcode)
{
  return -1;
}

int disasm_8051(
  struct _memory *memory,
  uint32_t address,
  char *instruction,
  int *cycles_min,
  int *cycles_max)
{
  int count = 1;
  int opcode;
  char temp[32];
  int value;
  int n;

  opcode = READ_RAM(address);

  // The mov iram_addr, iram_addr instruction didn't fit the format of
  // the table and how the rest of the instructions were done.
  if (opcode == 0x85)
  {
    sprintf(instruction, "%s 0x%02x, 0x%02x",
      table_8051[opcode].name,
      READ_RAM(address + 1),
      READ_RAM(address + 2));
    return 3;
  }

  strcpy(instruction, table_8051[opcode].name);

  for (n = 0; n < 3; n++)
  {
    if (table_8051[opcode].op[n] == OP_NONE) break;

    if (n == 0)
    {
      strcat(instruction, " ");
    }
      else
    {
      strcat(instruction, ", ");
    }

    switch (table_8051[opcode].op[n])
    {
      case OP_REG:
        sprintf(temp, "R%d", table_8051[opcode].range);
        strcat(instruction, temp);
        break;
      case OP_AT_REG:
        sprintf(temp, "@R%d", table_8051[opcode].range);
        strcat(instruction, temp);
        break;
      case OP_A:
        strcat(instruction, "A");
        break;
      case OP_C:
        strcat(instruction, "C");
        break;
      case OP_AB:
        strcat(instruction, "AB");
        break;
      case OP_DPTR:
        strcat(instruction, "DPTR");
        break;
      case OP_AT_A_PLUS_DPTR:
        strcat(instruction, "@A+DPTR");
        break;
      case OP_AT_A_PLUS_PC:
        strcat(instruction, "@A+PC");
        break;
      case OP_AT_DPTR:
        strcat(instruction, "@DPTR");
        break;
      case OP_DATA:
        sprintf(temp, "#0x%02x", READ_RAM(address + count));
        strcat(instruction, temp);
        count++;
        break;
      case OP_DATA_16:
        sprintf(temp, "#0x%04x", READ_RAM(address + count + 1) | (READ_RAM(address + count) << 8));
        strcat(instruction, temp);
        count = 3;
        break;
      case OP_CODE_ADDR:
        sprintf(temp, "0x%04x", READ_RAM(address + count + 1) | (READ_RAM(address + count) << 8));
        strcat(instruction, temp);
        count = 3;
        break;
      case OP_RELADDR:
        value = READ_RAM(address + count);
        sprintf(temp, "0x%04x", (address + count + 1) + ((char)value));
        strcat(instruction, temp);
        count++;
        break;
      case OP_SLASH_BIT_ADDR:
        value = READ_RAM(address + count);
        if ((value & 0x80) != 0)
        {
          sprintf(temp, "/0x%02x.%d [0x%02x]",
            value & 0xf8, value & 0x7, value);
        }
          else
        {
          sprintf(temp, "/0x%02x.%d [0x%02x]",
            ((value & 0xf8) >> 3) + 0x20, value & 0x7, value);
        }
        strcat(instruction, temp);
        count++;
        break;
      case OP_BIT_ADDR:
        value = READ_RAM(address + count);
        if ((value & 0x80) != 0)
        {
          sprintf(temp, "0x%02x.%d [0x%02x]",
            value & 0xf8, value & 0x07, value);
        }
          else
        {
          sprintf(temp, "0x%02x.%d [0x%02x]",
            ((value & 0xf8) >> 3) + 0x20, value & 0x07, value);
        }
        strcat(instruction, temp);
        count++;
        break;
      case OP_PAGE:
        sprintf(temp, "0x%04x",
          (address & 0xf800) |
          READ_RAM(address + count) |
          (table_8051[opcode].range << 8));
        strcat(instruction, temp);
        count++;
        break;
      case OP_IRAM_ADDR:
        sprintf(temp, "0x%02x", READ_RAM(address + count));
        strcat(instruction, temp);
        count++;
        break;
    }
  }

  return count;
}

void list_output_8051(
  struct _asm_context *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min = -1, cycles_max = -1, count;
  char instruction[128];
  char temp[32];
  char temp2[4];
  int n;

  fprintf(asm_context->list, "\n");

  while (start < end)
  {
    count = disasm_8051(&asm_context->memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;
    for (n = 0; n < count; n++)
    {
      sprintf(temp2, "%02x ", memory_read_m(&asm_context->memory, start + n));
      strcat(temp, temp2);
    }

    fprintf(asm_context->list, "0x%04x: %-10s %-40s cycles: ", start, temp, instruction);

#if 0
    if (cycles_min == cycles_max)
    { fprintf(asm_context->list, "%d\n", cycles_min); }
      else
    { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }
#endif

    start += count;
  }
}

void disasm_range_8051(
  struct _memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
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
    count = disasm_8051(memory, start, instruction, &cycles_min, &cycles_max);

    temp[0] = 0;

    for (n = 0; n < count; n++)
    {
      sprintf(temp2, "%02x ", memory_read_m(memory, start+n));
      strcat(temp, temp2);
    }

    if (cycles_min < 1)
    {
      printf("0x%04x: %-10s %-40s ?\n", start, temp, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: %-10s %-40s %d\n", start, temp, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %-10s %-40s %d-%d\n", start, temp, instruction, cycles_min, cycles_max);
    }

    start += count;
  }
}

