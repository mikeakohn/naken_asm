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
#include "disasm_805x.h"
#include "table_805x.h"

#define READ_RAM(a) memory_read_m(memory, a)

int get_cycle_count_805x(unsigned short int opcode)
{
  return -1;
}

int disasm_805x(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
int count=1;
int opcode;
char temp[32];
int n;

  opcode=READ_RAM(address);

  strcpy(instruction, table_805x[opcode].name);

  for (n=0; n<3; n++)
  {
    if (table_805x[opcode].op[n]==OP_NONE) break;

    if (n==0) { strcat(instruction, " "); }
    else { strcat(instruction, ", "); }

    switch(table_805x[opcode].op[n])
    {
      case OP_REG:
        sprintf(temp, "R%d", table_805x[opcode].range);
        strcat(instruction, temp);
        break;
      case OP_AT_REG:
        sprintf(temp, "@R%d", table_805x[opcode].range);
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
        sprintf(temp, "#%d", READ_RAM(address+count));
        count++;
        break;
      case OP_DATA_16:
        sprintf(temp, "#%d", READ_RAM(address+count)|(READ_RAM(address+count+1)<<8));
        count=3;
        break;
      case OP_SLASH_BIT_ADDR:
        sprintf(temp, "/%d", READ_RAM(address+count));
        count++;
        break;
      case OP_CODE_ADDR:
        sprintf(temp, "%d", READ_RAM(address+count)|(READ_RAM(address+count+1)<<8));
        count=3;
        break;
      case OP_PAGE:
        sprintf(temp, "%d", READ_RAM(address+count)|(table_805x[opcode].range<<8));
        count++;
        break;
      case OP_BIT_ADDR:
      case OP_RELADDR:
      case OP_IRAM_ADDR:
        sprintf(temp, "%d", READ_RAM(address+count));
        count++;
        break;
    }
  }

  strcpy(instruction, "???");
  return count;
}

void list_output_805x(struct _asm_context *asm_context, int address)
{
int cycles_min=-1,cycles_max=-1,count;
char instruction[128];
unsigned int opcode=memory_read_m(&asm_context->memory, address);

  fprintf(asm_context->list, "\n");
  count=disasm_805x(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);
  fprintf(asm_context->list, "0x%04x: 0x%08x %-40s cycles: ", address, opcode, instruction);

/*
  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }
*/
}

void disasm_range_805x(struct _memory *memory, int start, int end)
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

    disasm_805x(memory, start, instruction, &cycles_min, &cycles_max);

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

