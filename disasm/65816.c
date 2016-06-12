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

int get_cycle_count_65816(uint16_t opcode)
{
  return -1;
}

int disasm_65816(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max)
{
  return -1;
}

void list_output_65816(struct _asm_context *asm_context, uint32_t start, uint32_t end)
{
#if 0
  int cycles_min,cycles_max;
  unsigned int opcode=memory_read32_m(&asm_context->memory, address);

  opcode &= 0xFF;

  fprintf(asm_context->list, "\n");
  disasm_65816(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);
#endif
  char instruction[128];
  char bytes[32];
  //uint8_t opcode = memory_read_m(&asm_context->memory, start)
  int count = end - start;
  int n;

  strcpy(instruction, "???");

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

