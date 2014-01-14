/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm_common.h"
#include "disasm_680x.h"
#include "table_680x.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)

extern struct _m680x_table m680x_table[];

int get_cycle_count_680x(unsigned short int opcode)
{
  return -1;
}

int disasm_680x(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
//int bit_instr;
int opcode;
int size=1;
//int n;

  *cycles_min=-1;
  *cycles_max=-1;

  opcode=READ_RAM(address);

  switch(m680x_table[opcode].operand_type)
  {
    case M6800_OP_UNDEF:
      strcpy(instruction, "???");
      break;
    case M6800_OP_NONE:
      strcpy(instruction, m680x_table[opcode].instr);
      break;
    case M6800_OP_IMM8:
      sprintf(instruction, "%s #$%02x", m680x_table[opcode].instr, READ_RAM(address+1));
      size=2;
      break;
    case M6800_OP_IMM16:
      sprintf(instruction, "%s #$%04x", m680x_table[opcode].instr, READ_RAM16(address+1));
      size=3;
      break;
    case M6800_OP_DIR_PAGE_8:
      sprintf(instruction, "%s $%02x", m680x_table[opcode].instr, READ_RAM(address+1));
      size=2;
      break;
    case M6800_OP_ABSOLUTE_16:
      sprintf(instruction, "%s $%04x", m680x_table[opcode].instr, READ_RAM16(address+1));
      size=3;
      break;
    case M6800_OP_NN_X:
      sprintf(instruction, "%s $%04x,X (%d)", m680x_table[opcode].instr, (address+2)+(char)(READ_RAM(address+1)), (char)(READ_RAM(address+1)));
      size=2;
      break;
    case M6800_OP_REL_OFFSET:
      sprintf(instruction, "%s $%04x,X  (%d)", m680x_table[opcode].instr, (address+2)+(char)(READ_RAM(address+1)), (char)READ_RAM(address+1));
      size=2;
      break;
  }

  return size;
}

void list_output_680x(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max;
char instruction[128];
char bytes[10];
int count;
int n;
//unsigned int opcode=memory_read_m(&asm_context->memory, address);

  fprintf(asm_context->list, "\n");
  count=disasm_680x(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);

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

void disasm_range_680x(struct _memory *memory, int start, int end)
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
    count=disasm_680x(memory, start, instruction, &cycles_min, &cycles_max);

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


