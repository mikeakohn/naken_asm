/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm_common.h"
#include "disasm_680x0.h"
#include "table_680x0.h"

#define READ_RAM16(a) (memory_read_m(memory, a+1)<<8)|memory_read_m(memory, a);

int get_cycle_count_680x0(unsigned short int opcode)
{
  return -1;
}

int disasm_680x0(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
int count=1;
int opcode;
char temp[32];
//int value;
int n;

  opcode=READ_RAM16(address);

  strcpy(instruction, table_680x0[opcode].name);

  for (n=0; n<3; n++)
  {
    if (table_680x0[opcode].operands[n].type==OP_NONE) break;

    if (n==0) { strcat(instruction, " "); }
    else { strcat(instruction, ", "); }

    switch(table_680x0[opcode].operands[n].type)
    {
      case OP_D:
        //sprintf(temp, "d%d", table_680x0[opcode].range);
        strcat(instruction, temp);
        break;
      case OP_A:
        //sprintf(temp, "a%d", table_680x0[opcode].range);
        strcat(instruction, temp);
        break;
    }
  }

  //strcpy(instruction, "???");
  return count;
}

void list_output_680x0(struct _asm_context *asm_context, int address)
{
int cycles_min=-1,cycles_max=-1,count;
char instruction[128];
char temp[32];
char temp2[4];
int n;

  fprintf(asm_context->list, "\n");
  count=disasm_680x0(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);

  temp[0]=0;
  for (n=0; n<count; n++)
  {
    sprintf(temp2, "%02x ", memory_read_m(&asm_context->memory, address+n));
    strcat(temp, temp2);
  }

  fprintf(asm_context->list, "0x%04x: %-10s %-40s cycles: ", address, temp, instruction);

/*
  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }
*/
}

void disasm_range_680x0(struct _memory *memory, int start, int end)
{
char instruction[128];
char temp[32];
char temp2[4];
int cycles_min=0,cycles_max=0;
int count;
int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start<=end)
  {
    count=disasm_680x0(memory, start, instruction, &cycles_min, &cycles_max);

    temp[0]=0;
    for (n=0; n<count; n++)
    {
      sprintf(temp2, "%02x ", memory_read_m(memory, start+n));
      strcat(temp, temp2);
    }

    if (cycles_min<1)
    {
      printf("0x%04x: %-10s %-40s ?\n", start, temp, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: %-10s %-40s %d\n", start, temp, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %-10s %-40s %d-%d\n", start, temp, instruction, cycles_min, cycles_max);
    }

    start=start+count;
  }
}

