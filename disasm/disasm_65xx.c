/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 * 65xx file by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm_common.h"
#include "disasm_65xx.h"

#include "table_65xx.h"
extern struct _opcodes_65xx opcodes_65xx[];
extern struct _cycles_65xx cycles_65xx[];

#define READ_RAM(a) (memory_read_m(memory, a) & 0xFF)

// addressing modes
enum
{
  MODE_ABSOLUTE,
  MODE_ABSOLUTE_X_INDEXED,
  MODE_ABSOLUTE_Y_INDEXED,
  MODE_IMMEDIATE,
  MODE_IMPLIED,
  MODE_INDIRECT,
  MODE_X_INDEXED_INDIRECT,
  MODE_INDIRECT_Y_INDEXED,
  MODE_RELATIVE,
  MODE_ZEROPAGE,
  MODE_ZEROPAGE_X_INDEXED,
  MODE_ZEROPAGE_Y_INDEXED
};

// bytes each mode takes
static int mode_bytes[] = { 3, 3, 3, 2, 1, 3, 2, 2, 2, 2, 2, 2 };

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

int index, mode;
int lo, hi;
int branch_address = 0;
int found = 0;

  *cycles_min=-1;
  *cycles_max=-1;
  opcode=READ_RAM(address);

  for(index = 0; index < 56; index++)
  {
    for(mode = 0; mode < 12; mode++)
    {
      if(opcodes_65xx[index].opcode[mode] == opcode)
      {
        found = 1;
        break;
      }
    }
    if(found == 1)
      break;
  }

  sprintf(temp, " ");

  if(found)
  {
    strcpy(instruction, opcodes_65xx[index].name);

    if(mode_bytes[mode] > 1)
    {
      if(mode_bytes[mode] == 2)
      {
        lo = READ_RAM(address + 1);

        // special case for branches
        if(mode == MODE_RELATIVE)
        {
          branch_address = (address + 2) + (signed char)lo;
          sprintf(num, "0x%04x", branch_address);
        }
          else
        {
          sprintf(num, "0x%02x", lo);
        }
      }

      if(mode_bytes[mode] == 3)
      {
        lo = READ_RAM(address + 1);
        hi = READ_RAM(address + 2);
        sprintf(num, "0x%04x", (hi << 8) | lo);
      }

      switch(mode)
      {
        case MODE_ABSOLUTE:
          sprintf(temp, " %s", num);
          break;
        case MODE_ABSOLUTE_X_INDEXED:
          sprintf(temp, " %s,x", num);
          break;
        case MODE_ABSOLUTE_Y_INDEXED:
          sprintf(temp, " %s,y", num);
          break;
        case MODE_IMMEDIATE:
          sprintf(temp, " #%s", num);
          break;
        case MODE_IMPLIED:
          sprintf(temp, " ");
          break;
        case MODE_INDIRECT:
          sprintf(temp, " (%s)", num);
          break;
        case MODE_X_INDEXED_INDIRECT:
          sprintf(temp, " (%s,x)", num);
          break;
        case MODE_INDIRECT_Y_INDEXED:
          sprintf(temp, " (%s),y", num);
          break;
        case MODE_RELATIVE:
          sprintf(temp, " %s", num);
          break;
        case MODE_ZEROPAGE:
          sprintf(temp, " %s", num);
          break;
        case MODE_ZEROPAGE_X_INDEXED:
          sprintf(temp, " %s,x", num);
          break;
        case MODE_ZEROPAGE_Y_INDEXED:
          sprintf(temp, " %s,y", num);
          break;
      }
    }

    // get cycle mode
    int min = cycles_65xx[opcode].cycles;
    int max = min;

    if(mode == MODE_RELATIVE)
    {
      // branch, see if we're in the same page
      int page1 = (address + 2) / 256;
      int page2 = branch_address / 256;
      if(page1 != page2)
        max += 2;
      else
        max += 1;
    }
      else
    {
      max += cycles_65xx[opcode].inc;
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
  return mode_bytes[mode];
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
      printf("hi1 0x%04x: 0x%02x %-40s ?\n", start, num & 0xFF, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("hi2 0x%04x: 0x%02x %-40s %d\n", start, num & 0xFF, instruction, cycles_min);
    }
      else
    {
      printf("hi3 0x%04x: 0x%02x %-40s %d-%d\n", start, num & 0xFF, instruction, cycles_min, cycles_max);
    }

    count-=1;
    while (count>0)
    {
      start=start+1;
      num=READ_RAM(start)|(READ_RAM(start+1)<<8);
      printf("hi4 0x%04x: 0x%02x\n", start, num & 0xFF);
      count-=1;
    }

    start=start+1;
  }
}

