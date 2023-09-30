/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, Joe Davisson
 *
 * 6502 file by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/6502.h"
#include "table/6502.h"

#define READ_RAM(a) (memory->read8(a) & 0xff)

// bytes for each addressing mode
static int op_bytes[] = { 1, 2, 2, 3, 2, 2, 3, 3, 3, 2, 2, 2, 3, 2, 3 };

int disasm_6502(
  Memory *memory,
  uint32_t address,
  char *instruction,
  int length,
  int flags,
  int *cycles_min,
  int *cycles_max)
{
  uint32_t opcode;
  //int n,r;
  char temp[32];
  char num[8];

  int op;
  int lo = 0, hi = 0;
  int branch_address = 0;

  *cycles_min = -1;
  *cycles_max = -1;
  opcode = READ_RAM(address);

  snprintf(temp, sizeof(temp), " ");

  if (table_6502_opcodes[opcode].instr != M65XX_ERROR)
  {
    strcpy(instruction, table_6502[table_6502_opcodes[opcode].instr].name);
    op = table_6502_opcodes[opcode].op;

    if (op_bytes[op] > 1)
    {
      if (op_bytes[op] == 2)
      {
        lo = READ_RAM(address + 1);

        // special case for branches
        if (op == OP_RELATIVE)
        {
          branch_address = (address + 2) + (int8_t)lo;
          snprintf(num, sizeof(num), "0x%04x", branch_address);
        }
          else
        {
          snprintf(num, sizeof(num), "0x%02x", lo);
        }
      }
        else
      if (op_bytes[op] == 3)
      {
        lo = READ_RAM(address + 1);
        hi = READ_RAM(address + 2);
        snprintf(num, sizeof(num), "0x%04x", (hi << 8) | lo);
      }

      switch (op)
      {
        case OP_NONE:
          snprintf(temp, sizeof(temp), " ");
          break;
        case OP_IMMEDIATE:
          snprintf(temp, sizeof(temp), " #%s", num);
          break;
        case OP_ADDRESS8:
          snprintf(temp, sizeof(temp), " %s", num);
          break;
        case OP_ADDRESS16:
          snprintf(temp, sizeof(temp), " %s", num);
          break;
        case OP_INDEXED8_X:
          snprintf(temp, sizeof(temp), " %s,x", num);
          break;
        case OP_INDEXED8_Y:
          snprintf(temp, sizeof(temp), " %s,y", num);
          break;
        case OP_INDEXED16_X:
          snprintf(temp, sizeof(temp), " %s,x", num);
          break;
        case OP_INDEXED16_Y:
          snprintf(temp, sizeof(temp), " %s,y", num);
          break;
        case OP_INDIRECT16:
          snprintf(temp, sizeof(temp), " (%s)", num);
          break;
        case OP_X_INDIRECT8:
          snprintf(temp, sizeof(temp), " (%s,x)", num);
          break;
        case OP_INDIRECT8_Y:
          snprintf(temp, sizeof(temp), " (%s),y", num);
          break;
        case OP_INDIRECT8:
          snprintf(temp, sizeof(temp), " (%s)", num);
          break;
        case OP_X_INDIRECT16:
          snprintf(temp, sizeof(temp), " (%s,x)", num);
          break;
        case OP_RELATIVE:
          snprintf(temp, sizeof(temp), " %s (offset=%d)", num, (int8_t)lo);
          break;
        case OP_ADDRESS8_RELATIVE:
          snprintf(temp, sizeof(temp), " 0x%02x, 0x%02x (offset=%d)", lo, address + 2 + (int8_t)hi, (int8_t)hi);
          break;
      }
    }

    // get cycle mode
    int min = table_6502_opcodes[opcode].cycles_min;
    int max = table_6502_opcodes[opcode].cycles_max;

    if (op == OP_RELATIVE)
    {
      // Branches are 2 cycles for no branch, 3 cycles when branching, or
      // 4 cycles if the branch ends up on a new page.
      int page1 = (address + 2) / 256;
      int page2 = branch_address / 256;

      if (page1 != page2) { max += 1; }
    }

    strcat(instruction, temp);

    *cycles_min = min;
    *cycles_max = max;
  }
    else
  {
    // Could not figure out this opcode so return instruction as ???
    strcpy(instruction, "???");
    snprintf(temp, sizeof(temp), " 0x%02x", opcode);
    strcat(instruction, temp);
    return 0;
  }

  // set this to the number of bytes the operation took up
  return op_bytes[op];
}

void list_output_6502(
  AsmContext *asm_context,
  uint32_t start,
  uint32_t end)
{
  int cycles_min,cycles_max;
  char instruction[128];
  char bytes[16];
  int count;
  int n;

  Memory *memory = &asm_context->memory;

  fprintf(asm_context->list, "\n");
  count = disasm_6502(
    memory,
    start,
    instruction,
    sizeof(instruction),
    asm_context->flags,
    &cycles_min,
    &cycles_max);

  bytes[0] = 0;
  for (n = 0; n < count; n++)
  {
    char temp[4];
    snprintf(temp, sizeof(temp), "%02x ", memory->read8(start + n));
    strcat(bytes, temp);
  }

  fprintf(asm_context->list, "0x%04x: %-16s %-35s cycles: ", start, bytes, instruction);

  if (cycles_min == cycles_max)
  {
    fprintf(asm_context->list, "%d\n", cycles_min);
  }
    else
  {
    fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max);
  }
}

void disasm_range_6502(
  Memory *memory,
  uint32_t flags,
  uint32_t start,
  uint32_t end)
{
  char instruction[128];
  //int vectors_flag=0;
  int cycles_min=0,cycles_max=0;
  int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while (start <= end)
  {
    num = READ_RAM(start)|(READ_RAM(start+1)<<8);

    int count = disasm_6502(
      memory,
      start,
      instruction,
      sizeof(instruction),
      0,
      &cycles_min,
      &cycles_max);

    if (cycles_min < 1)
    {
      printf("0x%04x: 0x%02x %-40s ?\n", start, num & 0xff, instruction);
    }
      else
    if (cycles_min == cycles_max)
    {
      printf("0x%04x: 0x%02x %-40s %d\n", start, num & 0xff, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%02x %-40s %d-%d\n", start, num & 0xff, instruction, cycles_min, cycles_max);
    }

    count -= 1;
    while (count > 0)
    {
      start = start + 1;
      num = READ_RAM(start) | (READ_RAM(start + 1) << 8);
      printf("0x%04x: 0x%02x\n", start, num & 0xff);
      count -= 1;
    }

    start = start + 1;
  }
}

