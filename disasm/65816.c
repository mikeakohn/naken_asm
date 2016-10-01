/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn, Joe Davisson
 *
 * 65816 by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disasm/65816.h"
#include "table/65816.h"

extern struct _table_65816 table_65816[];
extern struct _table_65816_opcodes table_65816_opcodes[];

int get_cycle_count_65816(uint16_t opcode)
{
  return -1;
}

int disasm_65816(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max, int bytes)
{
  char temp[128];
  uint8_t opcode = memory_read_m(memory, address);
  int offset;

  strcpy(instruction, table_65816[table_65816_opcodes[opcode].instr].name);

  switch(table_65816_opcodes[opcode].op)
  {
    case OP_NONE:
      break;
    case OP_IMMEDIATE8:
    case OP_IMMEDIATE16:
    case OP_ADDRESS8:
    case OP_ADDRESS16:
    case OP_ADDRESS24:
    case OP_INDEXED8_X:
    case OP_INDEXED8_Y:
    case OP_INDEXED16_X:
    case OP_INDEXED16_Y:
    case OP_INDEXED24_X:
    case OP_INDIRECT8:
    case OP_INDIRECT8_LONG:
    case OP_INDIRECT16:
    case OP_INDIRECT16_LONG:
    case OP_X_INDIRECT8:
    case OP_X_INDIRECT16:
    case OP_INDIRECT8_Y:
    case OP_INDIRECT8_Y_LONG:
    case OP_BLOCK_MOVE:
      strcat(instruction, " ???");
      break;
    case OP_RELATIVE:
      offset = (int8_t)memory_read_m(memory, address + 1);
      sprintf(temp, " 0x%04x (%d)", address + 2 + offset, offset);
      strcat(instruction, temp);
      break;
    case OP_RELATIVE_LONG:
    case OP_SP_RELATIVE:
    case OP_SP_INDIRECT_Y:
    default:
      strcat(instruction, " ???");
      break;
  }

  return 0;
}

void list_output_65816(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
#if 0
  unsigned int opcode=memory_read32_m(&asm_context->memory, address);

  opcode &= 0xFF;

  fprintf(asm_context->list, "\n");
#endif

  char instruction[128];
  char bytes[32];
  int cycles_min,cycles_max;
  //uint8_t opcode = memory_read_m(&asm_context->memory, start)
  int count = end - start;
  int n;

  disasm_65816(&asm_context->memory, start, instruction, &cycles_min, &cycles_max, end - start + 1);

  //strcpy(instruction, "???");

  bytes[0] = 0;
  for (n = 0; n < count; n++)
  {
    char temp[4];
    sprintf(temp, "%02x ", memory_read_m(&asm_context->memory, start + n));
    strcat(bytes, temp);
  }

  fprintf(asm_context->list, "0x%04x: %-16s %-40s cycles: ?\n", start, bytes, instruction);

#if 0
  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }
#endif
}

void disasm_range_65816(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end)
{
#if 0
char instruction[128];
//int vectors_flag=0;
int cycles_min=0,cycles_max=0;
int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start<=end)
  {
    num=READ_RAM(start)|(READ_RAM(start+1)<<8);

    int count=disasm_65816(memory, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min<1)
    {
      printf("0x%04x: 0x%02x %-40s ?\n", start, num & 0xFF, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: 0x%02x %-40s %d\n", start, num & 0xFF, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%02x %-40s %d-%d\n", start, num & 0xFF, instruction, cycles_min, cycles_max);
    }

    count-=1;
    while (count>0)
    {
      start=start+1;
      num=READ_RAM(start)|(READ_RAM(start+1)<<8);
      printf("0x%04x: 0x%02x\n", start, num & 0xFF);
      count-=1;
    }

    start=start+1;
  }
#endif
}

