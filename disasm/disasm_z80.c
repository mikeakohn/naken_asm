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
#include "disasm_z80.h"
#include "table_z80.h"

#define READ_RAM(a) memory_read_m(memory, a)
//#define READ_RAM16(a) memory_read_m(memory, a)|(memory_read_m(memory, a+1)<<8)
#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)

int get_cycle_count_z80(unsigned short int opcode)
{
  return -1;
}

int disasm_z80(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
int opcode;
int opcode16;
int n;
char *reg8[] = { "b","c","d","e","h","l","(hl)","a" };
char *reg_ihalf[] = { "ixh","ixl","iyh","iyl" };

  *cycles_min=-1;
  *cycles_max=-1;

  opcode=READ_RAM(address);
  opcode16=READ_RAM16(address);

  n=0;
  while(table_z80[n].instr!=NULL)
  {
    if (table_z80[n].opcode==(opcode&table_z80[n].mask))
    {
      switch(table_z80[n].type)
      {
        case OP_A_REG8:
          sprintf(instruction, "%s a,%s", table_z80[n].instr, reg8[opcode&0x7]);
          return 1;
        case OP_REG8:
          sprintf(instruction, "%s %s", table_z80[n].instr, reg8[opcode&0x7]);
          return 1;
      }
    }
      else
    if (table_z80[n].opcode==(opcode16&table_z80[n].mask))
    {
      switch(table_z80[n].type)
      {
        case OP_A_REG_IHALF:
          n=((opcode&0x2000)>>12)|(opcode&1);
          sprintf(instruction, "%s a,%s", table_z80[n].instr, reg_ihalf[n]);
          return 2;
        case OP_A_INDEX:
          n=((opcode16&0x2000)>>13);
          char offset=READ_RAM(address+2);
          if (offset==0)
          {
            sprintf(instruction, "%s a,(%s)", table_z80[n].instr, (n==0)?"ix":"iy");
          }
            else
          if (offset>0)
          {
            sprintf(instruction, "%s a,(%s+%d)", table_z80[n].instr, (n==0)?"ix":"iy", offset);
          }
            else
          {
            sprintf(instruction, "%s a,(%s%d)", table_z80[n].instr, (n==0)?"ix":"iy", offset);
          }
          return 3;
      }
    }
    n++;
  }

  sprintf(instruction, "???");

  return 1;
}

void list_output_z80(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max;
char instruction[128];
char bytes[10];
int count;
int n;
//unsigned int opcode=memory_read_m(&asm_context->memory, address);

  fprintf(asm_context->list, "\n");
  count=disasm_z80(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);

  bytes[0]=0;
  for (n=0; n<count; n++)
  {
    char temp[4];
    sprintf(temp, "%02x ", memory_read_m(&asm_context->memory, address+n));
    strcat(bytes, temp);
  }

  fprintf(asm_context->list, "0x%04x: %-9s %-40s cycles: ", address, bytes, instruction);

  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }
}

void disasm_range_z80(struct _memory *memory, int start, int end)
{
char instruction[128];
char bytes[10];
int cycles_min=0,cycles_max=0;
int count;
int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start<=end)
  {
    count=disasm_z80(memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0]=0;
    for (n=0; n<count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", READ_RAM(start+n));
      strcat(bytes, temp);
    }

    if (cycles_min<1)
    {
      printf("0x%04x: %-9s %-40s ?\n", start, bytes, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: %-9s %-40s %d\n", start, bytes, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %-9s %-40s %d-%d\n", start, bytes, instruction, cycles_min, cycles_max);
    }

    start=start+count;
  }
}


