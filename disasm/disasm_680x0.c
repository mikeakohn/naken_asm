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
#include "disasm_680x0.h"
#include "table_680x0.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) (memory_read_m(memory, a)<<8)|memory_read_m(memory, a+1)
#define READ_RAM32(a) (memory_read_m(memory, a)<<24)|(memory_read_m(memory, a+1)<<16)|(memory_read_m(memory, a+2)<<8)|memory_read_m(memory, a+3)

#define SIZE(a,b) ((a>>b)&0x3)

enum
{
  SIZE_B=0,
  SIZE_W,
  SIZE_L,
};

//extern struct _table_680x0_no_operands table_680x0_no_operands[];
extern struct _table_680x0 table_680x0[];
extern char *table_680x0_condition_codes[];

static char sizes[] = { 'b','w','l','?' };

int get_cycle_count_680x0(unsigned short int opcode)
{
  return -1;
}

static int get_ea_680x0(struct _memory *memory, int address, char *ea, unsigned short int opcode, int pos, int size)
{
  // FIXME - is pos always 0?
  int reg=opcode&0x7;
  int mode=((opcode>>pos)>>3)&0x7;

  switch(mode)
  {
    case 0:
      sprintf(ea, "d%d", reg);
      return 2;
    case 1:
      sprintf(ea, "a%d", reg);
      return 2;
    case 2:
      sprintf(ea, "(a%d)", reg);
      return 2;
    case 3:
      sprintf(ea, "(a%d)+", reg);
      return 2;
    case 4:
      sprintf(ea, "-(a%d)", reg);
      return 2;
    case 5:
      sprintf(ea, "(%d,a%d)", (short int)READ_RAM16(address+2), reg);
      return 4;
    case 7:
      if (reg==0)
      {
        short int value=READ_RAM16(address+2);
        if (value>0) { sprintf(ea, "$%x", value); }
        else { sprintf(ea, "$%x", ((unsigned int)value)&0xffffff); }
        return 4;
      }
      else if (reg==1)
      {
        sprintf(ea, "$%x", READ_RAM32(address+2));
        return 6;
      }
      else if (reg==2)
      {
        sprintf(ea, "(%d,PC)", (short int)READ_RAM16(address+2));
        return 6;
      }
      else if (reg==4)
      {
        if (size==SIZE_B)
        {
          sprintf(ea, "#$%x", READ_RAM(address+3));
        }
          else
        if (size==SIZE_W)
        {
          sprintf(ea, "#$%x", READ_RAM16(address+2));
          return 4;
        }
          else
        if (size==SIZE_L)
        {
          sprintf(ea, "#$%x", READ_RAM32(address+2));
          return 6;
        }
          else
        {
          sprintf(ea, "???");
        }
      }
        else
      {
        sprintf(ea, "???");
      }
      return 2;
    default:
      break;

  }

  strcpy(ea,"???");

  return 2;
}

#if 0
static char get_size_680x0(unsigned short int opcode, int pos)
{
  char size[] = { 'b','w','l','?' };
  return size[(opcode>>pos)&0x3];
}
#endif

int disasm_680x0(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
//int count=2;
int opcode;
char ea[32];
int size;
int reg;
int mode,len;
unsigned int immediate;
//int value;
int n;

  *cycles_min=-1;
  *cycles_max=-1;

  opcode=READ_RAM16(address);

#if 0
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
#endif

  n=0;
  while(table_680x0[n].instr!=NULL)
  {
    if ((opcode&table_680x0[n].mask)==table_680x0[n].opcode)
    {
      switch(table_680x0[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", table_680x0[n].instr);
          return 2;
        case OP_SINGLE_EA:
          size=SIZE(opcode,6);
          get_ea_680x0(memory, address, ea, opcode, 0, size);
          if (size==3) { break; }
          sprintf(instruction, "%s.%c %s", table_680x0[n].instr, sizes[size], ea);
          return 2;
        case OP_SINGLE_EA_NO_SIZE:
          get_ea_680x0(memory, address, ea, opcode, 0, 0);
          sprintf(instruction, "%s %s", table_680x0[n].instr, ea);
          return 2;
        case OP_IMMEDIATE:
          size=SIZE(opcode,6);
          get_ea_680x0(memory, address, ea, opcode, 0, size);
          if (size==3) { break; }

          if (size==SIZE_B)
          {
            immediate=READ_RAM(address+3);
            sprintf(instruction, "%s.%c #$%02x, %s", table_680x0[n].instr, sizes[size], immediate, ea);
            return 4;
          }
            else
          if (size==SIZE_W)
          {
            immediate=READ_RAM16(address+2);
            sprintf(instruction, "%s.%c #$%04x, %s", table_680x0[n].instr, sizes[size], immediate, ea);
            return 4;
          }
            else
          {
            immediate=READ_RAM32(address+2);
            sprintf(instruction, "%s.%c #$%08x, %s", table_680x0[n].instr, sizes[size], immediate, ea);
            return 6;
          }
        case OP_SHIFT_EA:
          get_ea_680x0(memory, address, ea, opcode, 0, 0);
          sprintf(instruction, "%s %s", table_680x0[n].instr, ea);
          return 2;
        case OP_SHIFT:
          size=SIZE(opcode,6);
          if (size==3) { break; }
          if ((opcode&0x0020)==0)
          {
            immediate=(opcode>>9)&0x7;
            immediate=(immediate==0)?8:immediate;
            sprintf(instruction, "%s.%c #%d, d%d", table_680x0[n].instr, sizes[size], immediate, opcode&0x7);
          }
            else
          {
            immediate=(opcode>>9)&0x7;
            sprintf(instruction, "%s.%c d%d, d%d", table_680x0[n].instr, sizes[size], immediate, opcode&0x7);
          }
          return 2;
        case OP_REG_AND_EA:
          size=SIZE(opcode,6);
          get_ea_680x0(memory, address, ea, opcode, 0, size);
          if (size==3) { break; }
          reg=(opcode>>9)&0x7;
          mode=(opcode>>8)&0x1;
          if (mode==0)
          {
            sprintf(instruction, "%s.%c %s, d%d", table_680x0[n].instr, sizes[size], ea, reg);
          }
            else
          {
            sprintf(instruction, "%s.%c d%d, %s", table_680x0[n].instr, sizes[size], reg, ea);
          }
          return 2;
        case OP_VECTOR:
          sprintf(instruction, "%s #%d", table_680x0[n].instr, opcode&0xf);
          return 2;
        case OP_AREG:
          sprintf(instruction, "%s a%d", table_680x0[n].instr, opcode&0x7);
          return 2;
        case OP_REG:
        {
          char r=(opcode&0x8)==0?'d':'a';
          sprintf(instruction, "%s %c%d", table_680x0[n].instr, r, opcode&0x7);
          return 2;
        }
        case OP_EA_AREG:
          reg=(opcode>>9)&0x7;
          mode=(opcode>>6)&0x7;
          if (mode==3) { size=SIZE_W; }
          else if (mode==7) { size=SIZE_L; }
          else { break; }
          len=get_ea_680x0(memory, address, ea, opcode, 0, size);
          sprintf(instruction, "%s.%c %s, a%d", table_680x0[n].instr, sizes[size], ea, reg);
          return len;
        case OP_EA_DREG:
          reg=(opcode>>9)&0x7;
          mode=(opcode>>6)&0x7;
          if (mode>2) { break; }
          size=mode;
          len=get_ea_680x0(memory, address, ea, opcode, 0, size);
          sprintf(instruction, "%s.%c %s, d%d", table_680x0[n].instr, sizes[size], ea, reg);
          return len;
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

