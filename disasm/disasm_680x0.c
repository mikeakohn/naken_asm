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

#define READ_RAM(a) memory_read_m(memory, a);
#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1);

extern struct _table_680x0_no_operands table_680x0_no_operands[];

int get_cycle_count_680x0(unsigned short int opcode)
{
  return -1;
}

int disasm_680x0(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
//int count=2;
int opcode;
//char temp[32];
//int value;
int n;

  *cycles_min=-1;
  *cycles_max=-1;

  opcode=READ_RAM16(address);

  n=0;
  while(table_680x0_no_operands[n].instr!=NULL)
  {
    if (opcode==table_680x0_no_operands[n].opcode)
    {
      sprintf(instruction, "%s", table_680x0_no_operands[n].instr);
      return 2;
    }
    n++;
  }

  //strcpy(instruction, table_680x0[opcode].name);

#if 0
  for (n=0; n<3; n++)
  {
    //if (table_680x0[opcode].operands[n].type==OP_NONE) break;

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
#endif

  strcpy(instruction, "???");
  return -1;
}

void list_output_680x0(struct _asm_context *asm_context, int address)
{
int cycles_min=-1,cycles_max=-1;
int count;
char instruction[128];
//char temp[32];
//char temp2[4];
int n;

  fprintf(asm_context->list, "\n");
  count=disasm_680x0(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);

#if 0
  temp[0]=0;
  for (n=0; n<count; n++)
  {
    sprintf(temp2, "%02x ", memory_read_m(&asm_context->memory, address+n));
    strcat(temp, temp2);
  }
#endif

  fprintf(asm_context->list, "0x%04x: %04x %-40s", address, (memory_read_m(&asm_context->memory, address)<<8)|memory_read_m(&asm_context->memory, address+1), instruction);

  for (n=2; n<count; n+=2)
  {
    fprintf(asm_context->list, "        %04x", (memory_read_m(&asm_context->memory, address+n)<<8)|memory_read_m(&asm_context->memory, address+n+1));
  }

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

