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
#define READ_RAM32(a) (memory_read_m(memory, a)<<24)|(memory_read_m(memory, a+1)<<16)|(memory_read_m(memory, a+2)<<8)|memory_read_m(memory, a+3);

extern struct _table_680x0_no_operands table_680x0_no_operands[];
extern struct _table_680x0 table_680x0[];
extern char *table_680x0_condition_codes[];

int get_cycle_count_680x0(unsigned short int opcode)
{
  return -1;
}

static int get_ea_680x0(char *ea, unsigned short int opcode, int pos)
{
  opcode=(opcode>>pos)&0x3f;

  switch(opcode>>3)
  {
    case 0:
      sprintf(ea, "d%d", opcode&0x7);
      return 2;
    case 1:
      sprintf(ea, "a%d", opcode&0x7);
      return 2;
    case 2:
      sprintf(ea, "(a%d)", opcode&0x7);
      return 2;
    case 3:
      sprintf(ea, "(a%d)+", opcode&0x7);
      return 2;
    case 4:
      sprintf(ea, "-(a%d)", opcode&0x7);
      return 2;

  }

  strcpy(ea,"???");

  return 2;
}

static char get_size_680x0(unsigned short int opcode, int pos)
{
  char size[] = { 'b','w','l','?' };
  return size[(opcode>>pos)&0x3];
}

int disasm_680x0(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
//int count=2;
int opcode;
char ea[32];
char size;
unsigned int immediate;
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

  n=0;
  while(table_680x0[n].instr!=NULL)
  {
    if ((opcode&table_680x0[n].mask)==table_680x0[n].opcode)
    {
      switch(table_680x0[n].type)
      {
        case OP_SINGLE_EA:
          get_ea_680x0(ea, opcode, 0);
          size=get_size_680x0(opcode, 6);
          sprintf(instruction, "%s.%c %s", table_680x0[n].instr, size, ea);
          return 2;
        case OP_SINGLE_EA_NO_SIZE:
          get_ea_680x0(ea, opcode, 0);
          sprintf(instruction, "%s %s", table_680x0[n].instr, ea);
          return 2;
        case OP_IMMEDIATE:
          get_ea_680x0(ea, opcode, 0);
          size=get_size_680x0(opcode, 6);

          if (size=='b')
          {
            immediate=READ_RAM(address+3);
            sprintf(instruction, "%s.%c #$%02x, %s", table_680x0[n].instr, size, immediate, ea);
            return 4;
          }
            else
          if (size=='w')
          {
            immediate=READ_RAM16(address+2);
            sprintf(instruction, "%s.%c #$%04x, %s", table_680x0[n].instr, size, immediate, ea);
            return 4;
          }
            else
          {
            immediate=READ_RAM32(address+2);
            sprintf(instruction, "%s.%c #$%08x, %s", table_680x0[n].instr, size, immediate, ea);
            return 6;
          }
        case OP_SHIFT_EA:
          get_ea_680x0(ea, opcode, 0);
          sprintf(instruction, "%s %s", table_680x0[n].instr, ea);
          return 2;
        case OP_SHIFT:
          size=get_size_680x0(opcode, 6);
          if ((opcode&0x0020)==0)
          {
            immediate=(opcode>>9)&0x7;
            immediate=(immediate==0)?8:immediate;
            sprintf(instruction, "%s.%c #%d, d%d", table_680x0[n].instr, size, immediate, opcode&0x7);
          }
            else
          {
            immediate=(opcode>>9)&0x7;
            sprintf(instruction, "%s.%c d%d, d%d", table_680x0[n].instr, size, immediate, opcode&0x7);
          }
        default:
          return -1;
      }
    }

    n++;
  }

  if ((opcode&0xf0f8)==0x50c8)
  {
    short int offset=READ_RAM16(address+2);
    sprintf(instruction, "db%s d%d, $%x (%d)", table_680x0_condition_codes[(opcode>>8)&0xf], opcode&0x7, (address+4)+offset, offset);
    return 4;
  }

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

  fprintf(asm_context->list, "0x%04x: %04x %-40s\n", address, (memory_read_m(&asm_context->memory, address)<<8)|memory_read_m(&asm_context->memory, address+1), instruction);

  for (n=2; n<count; n+=2)
  {
    fprintf(asm_context->list, "        %04x\n", (memory_read_m(&asm_context->memory, address+n)<<8)|memory_read_m(&asm_context->memory, address+n+1));
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

