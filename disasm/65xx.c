/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 * 65xx file by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm/common.h"
#include "disasm/65xx.h"

#include "table/65xx.h"
extern struct _table_65xx table_65xx[];
extern struct _table_65xx_opcodes table_65xx_opcodes[];

#define READ_RAM(a) (memory_read_m(memory, a) & 0xFF)

// bytes for each addressing mode
static int op_bytes[] = { 1, 2, 2, 3, 2, 2, 3, 3, 3, 2, 2, 2 };

int get_cycle_count_65xx(unsigned short int opcode)
{
  return -1;
}

int disasm_65xx(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
unsigned int opcode;
//int n,r;
char temp[32];
char num[8];

int op;
int lo, hi;
int branch_address = 0;

  *cycles_min=-1;
  *cycles_max=-1;
  opcode=READ_RAM(address);

  sprintf(temp, " ");

  if(table_65xx_opcodes[opcode].instr != -1)
  {
    strcpy(instruction, table_65xx[table_65xx_opcodes[opcode].instr].name);
    op = table_65xx_opcodes[opcode].op;

    if(op_bytes[op] > 1)
    {
      if(op_bytes[op] == 2)
      {
        lo = READ_RAM(address + 1);

        // special case for branches
        if(op == OP_RELATIVE)
        {
          branch_address = (address + 2) + (signed char)lo;
          sprintf(num, "0x%04x", branch_address);
        }
          else
        {
          sprintf(num, "0x%02x", lo);
        }
      }
      else if(op_bytes[op] == 3)
      {
        lo = READ_RAM(address + 1);
        hi = READ_RAM(address + 2);
        sprintf(num, "0x%04x", (hi << 8) | lo);
      }

      switch(op)
      {
        case OP_NONE:
          sprintf(temp, " ");
          break;
        case OP_IMMEDIATE:
          sprintf(temp, " #%s", num);
          break;
        case OP_ADDRESS8:
          sprintf(temp, " %s", num);
          break;
        case OP_ADDRESS16:
          sprintf(temp, " %s", num);
          break;
        case OP_INDEXED8_X:
          sprintf(temp, " %s,x", num);
          break;
        case OP_INDEXED8_Y:
          sprintf(temp, " %s,y", num);
          break;
        case OP_INDEXED16_X:
          sprintf(temp, " %s,x", num);
          break;
        case OP_INDEXED16_Y:
          sprintf(temp, " %s,y", num);
          break;
        case OP_INDIRECT16:
          sprintf(temp, " (%s)", num);
          break;
        case OP_X_INDIRECT8:
          sprintf(temp, " (%s,x)", num);
          break;
        case OP_INDIRECT8_Y:
          sprintf(temp, " (%s),y", num);
          break;
        case OP_RELATIVE:
          sprintf(temp, " %s", num);
          break;
      }
    }

    // get cycle mode
    int min = table_65xx_opcodes[opcode].cycles_min;
    int max = table_65xx_opcodes[opcode].cycles_max;

    if(op == OP_RELATIVE)
    {
      // branch, see if we're in the same page
      int page1 = (address + 2) / 256;
      int page2 = branch_address / 256;
      if(page1 != page2)
        max += 2;
      else
        max += 1;
    }

    strcat(instruction, temp);

    *cycles_min = min;
    *cycles_max = max;
  }
    else
  {
    // Could not figure out this opcode so return instruction as ???
    strcpy(instruction, "???");
    sprintf(temp, " 0x%02x", opcode);
    strcat(instruction, temp);
    return 0;
  }

  // set this to the number of bytes the operation took up
  return op_bytes[op];
}

void list_output_65xx(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max;
char instruction[128];
unsigned int opcode=get_opcode32(&asm_context->memory, address);

  opcode &= 0xFF;

  fprintf(asm_context->list, "\n");
  disasm_65xx(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);

  fprintf(asm_context->list, "0x%04x: 0x%02x %-40s cycles: ", address, opcode, instruction);

  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

}

void disasm_range_65xx(struct _memory *memory, int start, int end)
{
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

    int count=disasm_65xx(memory, start, instruction, &cycles_min, &cycles_max);

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
}

