/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm_common.h"
#include "disasm_stm8.h"

#define READ_RAM(a) memory_read_m(memory, a)

int get_cycle_count_stm8(unsigned short int opcode)
{
  return -1;
}

int disasm_stm8(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
unsigned char opcode;
int function,format;
int n,r;
char temp[32];

  *cycles_min=-1;
  *cycles_max=-1;

  opcode=READ_RAM(address);

  if (opcode==0x90)
  {
    opcode=READ_RAM(address+1);
printf("opcode=0x%02x\n", opcode);
    n=0;
    while(stm8_x_y[n].instr!=NULL)
    {
//printf("%02x %02x\n", stm8_x_y[n].opcode, opcode);
      if (stm8_x_y[n].opcode==opcode)
      {
        sprintf(instruction, "%s Y", stm8_x_y[n].instr);
        return 2;
      }

      n++;
    }

    strcpy(instruction, "???");
    return 1;
  }

  n=0;
  while(stm8_single[n].instr!=NULL)
  {
    if (stm8_single[n].opcode==opcode)
    {
      strcpy(instruction, stm8_single[n].instr);
      //add_bin(asm_context, stm8_single[n].opcode, IS_OPCODE);
      return 1;
    }

    n++;
  }

  n=0;
  while(stm8_x_y[n].instr!=NULL)
  {
    if (stm8_x_y[n].opcode==opcode)
    {
      sprintf(instruction, "%s X", stm8_x_y[n].instr);
      return 1;
    }

    n++;
  }

  strcpy(instruction, "???");

  return 1;
}

void list_output_stm8(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max,count;
char instruction[128];
//unsigned int opcode=READ_RAM(&asm_context->memory, address);
//unsigned int opcode=0;
int n;

  fprintf(asm_context->list, "\n");
  count=disasm_stm8(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);
  fprintf(asm_context->list, "0x%04x:", address);
  for (n = 0; n < 5; n++)
  {
    if (n < count)
    {
      fprintf(asm_context->list, " %02x", memory_read_m(&asm_context->memory, address+n));
    }
      else
    {
      fprintf(asm_context->list, "   ");
    }
  }
  fprintf(asm_context->list, " %-40s cycles: ", instruction);

  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

}

void disasm_range_stm8(struct _memory *memory, int start, int end)
{
char instruction[128];
int cycles_min=0,cycles_max=0;
int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start<=end)
  {
    num=READ_RAM(start)|(READ_RAM(start+1)<<8);

    disasm_stm8(memory, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min<1)
    {
      printf("0x%04x: 0x%08x %-40s ?\n", start, num, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: 0x%08x %-40s %d\n", start, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%08x %-40s %d-%d\n", start, num, instruction, cycles_min, cycles_max);
    }

#if 0
    count-=4;
    while (count>0)
    {
      start=start+4;
      num=READ_RAM(start)|(READ_RAM(start+1)<<8);
      printf("0x%04x: 0x%04x\n", start, num);
      count-=4;
    }
#endif

    start=start+4;
  }
}

