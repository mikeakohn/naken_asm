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
int n,r,i;
char *reg8[] = { "b","c","d","e","h","l","(hl)","a" };
char *reg_ihalf[] = { "ixh","ixl","iyh","iyl" };
char *reg16[] = { "bc","de","hl","sp" };
char *reg_xy[] = { "ix","iy" };
char *cond[] = { "nz","z","nc","c", "po","pe","p","m" };
char offset;

  *cycles_min=-1;
  *cycles_max=-1;

  opcode=READ_RAM(address);
  opcode16=READ_RAM16(address);

  n=0;
  while(table_z80[n].instr!=NULL)
  {
    if (table_z80[n].opcode==(opcode&table_z80[n].mask))
    {
      *cycles_min=table_z80[n].cycles;
      *cycles_max=table_z80[n].cycles+table_z80[n].cycles_max;

      switch(table_z80[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", table_z80[n].instr);
          return 1;
        case OP_A_REG8:
          sprintf(instruction, "%s a,%s", table_z80[n].instr, reg8[opcode&0x7]);
          return 1;
        case OP_REG8:
          sprintf(instruction, "%s %s", table_z80[n].instr, reg8[opcode&0x7]);
          return 1;
        case OP_A_NUMBER8:
          sprintf(instruction, "%s a,%d", table_z80[n].instr, READ_RAM(address+1));
          return 2;
        case OP_HL_REG16_1:
          sprintf(instruction, "%s hl,%s", table_z80[n].instr, reg16[(opcode>>4)&0x3]);
          return 1;
        case OP_A_INDEX_HL:
          sprintf(instruction, "%s a,(hl)", table_z80[n].instr);
          return 1;
        case OP_INDEX_HL:
          sprintf(instruction, "%s (hl)", table_z80[n].instr);
          return 1;
        case OP_NUMBER8:
          sprintf(instruction, "%s %d", table_z80[n].instr, READ_RAM(address+1));
          return 2;
        case OP_ADDRESS:
          sprintf(instruction, "%s 0x%04x", table_z80[n].instr, READ_RAM(address+1)|(READ_RAM(address+2)<<8));
          return 3;
        case OP_COND_ADDRESS:
          r=(opcode>>3)&0x7;
          sprintf(instruction, "%s %s,x0%04x", table_z80[n].instr, cond[r], READ_RAM(address+1)|(READ_RAM(address+2)<<8));
          return 3;
        case OP_REG8_V2:
          r=(opcode>>3)&0x7;
          sprintf(instruction, "%s %s", table_z80[n].instr, reg8[r]);
          return 1;
        case OP_REG16:
          r=(opcode>>4)&0x3;
          sprintf(instruction, "%s %s", table_z80[n].instr, reg16[r]);
          return 1;
        case OP_INDEX_SP_HL:
          sprintf(instruction, "%s (sp),hl", table_z80[n].instr);
          return 1;
        case OP_AF_AF_TICK:
          sprintf(instruction, "%s af,af'", table_z80[n].instr);
          return 1;
        case OP_DE_HL:
          sprintf(instruction, "%s de,hl", table_z80[n].instr);
          return 1;
        case OP_A_INDEX_N:
          sprintf(instruction, "%s a,(%d)", table_z80[n].instr, READ_RAM(address+1));
          return 2;
      }
    }
      else
    if (table_z80[n].opcode==(opcode16&table_z80[n].mask))
    {
      *cycles_min=table_z80[n].cycles;
      *cycles_max=table_z80[n].cycles+table_z80[n].cycles_max;

      switch(table_z80[n].type)
      {
        case OP_NONE16:
          sprintf(instruction, "%s", table_z80[n].instr);
          return 2;
        case OP_NONE24:
          if (READ_RAM(address+2)==0)
          {
            sprintf(instruction, "%s", table_z80[n].instr);
            return 3;
          }
          break;
        case OP_A_REG_IHALF:
          r=((opcode16&0x2000)>>12)|(opcode16&1);
          sprintf(instruction, "%s a,%s", table_z80[n].instr, reg_ihalf[r]);
          return 2;
        case OP_A_INDEX:
          r=((opcode16&0x2000)>>13)&0x1;
          offset=READ_RAM(address+2);
          if (offset==0)
          {
            sprintf(instruction, "%s a,(%s)", table_z80[n].instr, reg_xy[r]);
          }
            else
          if (offset>0)
          {
            sprintf(instruction, "%s a,(%s+%d)", table_z80[n].instr, reg_xy[r], offset);
          }
            else
          {
            sprintf(instruction, "%s a,(%s%d)", table_z80[n].instr, reg_xy[r], offset);
          }
          return 3;
        case OP_HL_REG16_2:
          sprintf(instruction, "%s hl,%s", table_z80[n].instr, reg16[(opcode16>>4)&0x3]);
          return 2;
        case OP_XY_REG16:
        {
          char *xy=((opcode16>>13)&0x1)==0?"ix":"iy";
          r=(opcode16>>4)&0x3;
          if (r==2)
          {
            sprintf(instruction, "%s %s,%s", table_z80[n].instr, xy, xy);
          }
            else
          {
            sprintf(instruction, "%s %s,%s", table_z80[n].instr, xy, reg16[r]);
          }
          return 2;
        }
        case OP_REG_IHALF:
          r=((opcode16&0x2000)>>12)|(opcode16&1);
          sprintf(instruction, "%s %s", table_z80[n].instr, reg_ihalf[r]);
          return 2;
        case OP_INDEX:
          r=((opcode16&0x2000)>>13);
          offset=READ_RAM(address+2);
          if (offset==0)
          {
            sprintf(instruction, "%s (%s)", table_z80[n].instr, reg_xy[r]);
          }
            else
          if (offset>0)
          {
            sprintf(instruction, "%s (%s+%d)", table_z80[n].instr, reg_xy[r], offset);
          }
            else
          {
            sprintf(instruction, "%s (%s%d)", table_z80[n].instr, reg_xy[r], offset);
          }
          return 3;
        case OP_BIT_REG8:
          r=opcode16&0x7;
          i=(opcode16>>3)&0x7;
          sprintf(instruction, "%s %d,%s", table_z80[n].instr, i, reg8[r]);
          return 2;
        case OP_BIT_INDEX_HL:
          i=(opcode16>>3)&0x7;
          sprintf(instruction, "%s %d,(hl)", table_z80[n].instr, i);
          return 2;
        case OP_BIT_INDEX:
          r=(opcode16>>13)&0x1;
          offset=READ_RAM(address+2);
          i=READ_RAM(address+3);
          if ((i>>6)==1)
          {
            i=(i>>3)&0x7;
            if (offset==0)
            {
              sprintf(instruction, "%s %d,(%s)", table_z80[n].instr, i, reg_xy[r]);
            }
              else
            if (offset>0)
            {
              sprintf(instruction, "%s %d,(%s+%d)", table_z80[n].instr, i, reg_xy[r], offset);
            }
              else
            {
              sprintf(instruction, "%s %d,(%s%d)", table_z80[n].instr, i, reg_xy[r], offset);
            }
            return 4;
          }
          break;
        case OP_REG_IHALF_V2:
          r=((opcode16&0x2000)>>12)|((opcode16>>3)&1);
          sprintf(instruction, "%s %s", table_z80[n].instr, reg_ihalf[r]);
          return 2;
        case OP_XY:
          r=(opcode16>>13)&0x1;
          sprintf(instruction, "%s %s", table_z80[n].instr, reg_xy[r]);
          return 2;
        case OP_INDEX_SP_XY:
          r=(opcode16>>13)&0x1;
          sprintf(instruction, "%s (sp),%s", table_z80[n].instr, reg_xy[r]);
          return 2;
        case OP_IM_NUM:
          r=(opcode16>>3)&0x3;
          if (r==0 || r==1) { r=0; } else { r--; }
          sprintf(instruction, "%s %d", table_z80[n].instr, r);
          return 2;
        case OP_REG8_INDEX_C:
          r=(opcode16>>3)&0x7;
          sprintf(instruction, "%s %s,(c)", table_z80[n].instr, reg8[r]);
          return 2;
        case OP_F_INDEX_C:
          sprintf(instruction, "%s f,(c)", table_z80[n].instr);
          return 2;
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
char bytes[16];
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


