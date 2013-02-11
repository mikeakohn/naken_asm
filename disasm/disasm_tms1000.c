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
#include "disasm_tms1000.h"

#define READ_RAM(a) memory_read_m(memory, a)

int get_cycle_count_tms1000(unsigned short int opcode)
{
  return -1;
}

int disasm_tms1000(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
int bit_instr;
int opcode;
int n;

  *cycles_min=-1;
  *cycles_max=-1;

  opcode=get_opcode32(memory, address);

  n=0;
  while(tms_instr_table[n].instr!=NULL)
  {
    if (tms_instr_table[n].op1000==opcode)
    {
      sprintf(instruction, tms_instr_table[n].instr);
      return 1;
    }
    n++;
  }

  bit_instr=opcode>>2;
  if (bit_instr==0xc) { sprintf(instruction, "sbit %d", opcode&0x3); return 1; }
    else
  if (bit_instr==0xd) { sprintf(instruction, "rbit %d", opcode&0x3); return 1; }
    else
  if (bit_instr==0xe) { sprintf(instruction, "tbiti %d", opcode&0x3); return 1;}
    else
  if (bit_instr==0xf) { sprintf(instruction, "ldx %d", opcode&0x3); return 1; }

  bit_instr=opcode>>4;
  if (bit_instr==0x4) { sprintf(instruction, "tcy %d", opcode&0xf); return 1; }
    else
  if (bit_instr==0x6) { sprintf(instruction, "tcmiy %d", opcode&0xf); return 1;}
    else
  if (bit_instr==0x1) { sprintf(instruction, "ldp %d", opcode&0xf); return 1; }
    else
  if (bit_instr==0x7) { sprintf(instruction, "alec %d", opcode&0xf); return 1; }
    else
  if (bit_instr==0x5) { sprintf(instruction, "ylec %d", opcode&0xf); return 1; }

  bit_instr=opcode>>6;
  unsigned char branch_address=opcode&0x3f;
  if (branch_address&0x20!=0) { branch_address|=0xc0; }
  branch_address=(address+1)+((char)branch_address);

  if (bit_instr==0x2)
  { sprintf(instruction, "br %d", branch_address); return 1; }
    else
  if (bit_instr==0x3)
  { sprintf(instruction, "call %d", branch_address); return 1; }

  return 1;
}

void list_output_tms1000(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max;
char instruction[128];
unsigned int opcode=get_opcode32(&asm_context->memory, address);

  fprintf(asm_context->list, "\n");
  disasm_tms1000(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);
  fprintf(asm_context->list, "0x%04x: 0x%02x %-40s cycles: ", address, opcode, instruction);

  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

}

void disasm_range_tms1000(struct _memory *memory, int start, int end)
{
char instruction[128];
int cycles_min=0,cycles_max=0;
int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start<=end)
  {
    num=READ_RAM(start);

    disasm_tms1000(memory, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min<1)
    {
      printf("0x%04x: 0x%02x %-40s ?\n", start, num, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: 0x%02x %-40s %d\n", start, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%02x %-40s %d-%d\n", start, num, instruction, cycles_min, cycles_max);
    }

    start=start+4;
  }
}

