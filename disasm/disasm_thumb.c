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
#include "disasm_thumb.h"
#include "table_thumb.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) (memory_read_m(memory, a))|(memory_read_m(memory, a+1)<<8)

int get_cycle_count_thumb(unsigned short int opcode)
{
  return -1;
}

static void get_rlist(char *s, int rlist)
{
int i,comma;
char temp[32];

  s[0]=0;

  for (i=0; i<8; i++)
  {
    if ((rlist&(1<<i))!=0)
    {
      if (comma==0)
      {
        sprintf(temp, " r%d", i);
        comma=1;
      }
        else
      {
        sprintf(temp, ", r%d", i);
        comma=1;
      }

      strcat(s, temp);
    }
  }
}

int disasm_thumb(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
uint16_t opcode;
int rd,rs,rn,offset;
int h1,h2;
int immediate;
char temp[128];
int n;

  *cycles_min=-1;
  *cycles_max=-1;

  opcode=READ_RAM16(address);

  n=0;
  while(table_thumb[n].instr!=NULL)
  {
    if (table_thumb[n].opcode==(opcode&table_thumb[n].mask))
    {
      *cycles_min=table_thumb[n].cycles;
      *cycles_max=table_thumb[n].cycles;

      switch(table_thumb[n].type)
      {
        case OP_SHIFT:
          rd=opcode&0x7;
          rs=(opcode>>3)&0x7;
          offset=(opcode>>6)&0x1f;
          sprintf(instruction, "%s r%d, r%d, #%d", table_thumb[n].instr, rd, rs, offset);
          return 2;
        case OP_ADD_SUB:
          immediate=(opcode>>10)&1;
          rd=opcode&0x7;
          rs=(opcode>>3)&0x7;
          if (immediate==0)
          {
            rn=(opcode>>6)&7;
            sprintf(instruction, "%s r%d, r%d, r%d", table_thumb[n].instr, rd, rs, rn);
          }
            else
          {
            offset=(opcode>>6)&7;
            sprintf(instruction, "%s r%d, r%d, #%d", table_thumb[n].instr, rd, rs, offset);
          }
          return 2;
        case OP_IMM:
          immediate=opcode&0xff;
          rd=(opcode>>8)&0x7;
          sprintf(instruction, "%s r%d, #0x%02x", table_thumb[n].instr, rd, immediate);
          return 2;
        case OP_ALU:
          rd=opcode&0x7;
          rs=(opcode>>3)&0x7;
          sprintf(instruction, "%s r%d, r%d", table_thumb[n].instr, rd, rs);
          return 2;
        case OP_HI:
          rd=opcode&0x7;
          rs=(opcode>>3)&0x7;
          h1=(opcode>>7)&0x1;
          h2=(opcode>>6)&0x1;
          sprintf(instruction, "%s r%d, r%d", table_thumb[n].instr, rd+(h1*8), rs+(h2*8));
          return 2;
        case OP_HI_BX:
          rs=(opcode>>3)&0x7;
          h2=(opcode>>6)&0x1;
          sprintf(instruction, "%s r%d", table_thumb[n].instr, rs+(h2*8));
          return 2;
        case OP_PC_RELATIVE_LOAD:
          rd=(opcode>>8)&0x7;
          offset=(opcode&0xff)<<2;
          sprintf(instruction, "%s r%d, [PC, #%d]  (0x%x)", table_thumb[n].instr, rd, offset, ((address+4)&0xfffffffc)+offset);
          return 2;
        case OP_LOAD_STORE:
          rd=opcode&0x7;
          rs=(opcode>>3)&0x7;  // rb
          rn=(opcode>>6)&0x7;  // ro
          sprintf(instruction, "%s r%d, [r%d, r%d]", table_thumb[n].instr, rd, rs, rn);
          return 2;
        case OP_LOAD_STORE_SIGN_EXT_HALF_WORD:
          rd=opcode&0x7;
          rs=(opcode>>3)&0x7;  // rb
          rn=(opcode>>6)&0x7;  // ro
          sprintf(instruction, "%s r%d, [r%d, r%d]", table_thumb[n].instr, rd, rs, rn);
          return 2;
        case OP_LOAD_STORE_IMM_OFFSET_WORD:
          rd=opcode&0x7;
          rs=(opcode>>3)&0x7;  // rb
          offset=(opcode>>6)&0x1f;
          sprintf(instruction, "%s r%d, [r%d, #%d]", table_thumb[n].instr, rd, rs, offset<<2);
          return 2;
        case OP_LOAD_STORE_IMM_OFFSET:
          rd=opcode&0x7;
          rs=(opcode>>3)&0x7;  // rb
          offset=(opcode>>6)&0x1f;
          sprintf(instruction, "%s r%d, [r%d, #%d]", table_thumb[n].instr, rd, rs, offset);
          return 2;
        case OP_LOAD_STORE_IMM_OFFSET_HALF_WORD:
          rd=opcode&0x7;
          rs=(opcode>>3)&0x7;  // rb
          offset=(opcode>>6)&0x1f;
          sprintf(instruction, "%s r%d, [r%d, #%d]", table_thumb[n].instr, rd, rs, offset<<1);
          return 2;
        case OP_LOAD_STORE_SP_RELATIVE:
          rd=(opcode>>8)&0x7;
          offset=opcode&0xff;
          sprintf(instruction, "%s r%d, [SP, #%d]", table_thumb[n].instr, rd, offset<<2);
          return 2;
        case OP_LOAD_ADDRESS:
          rd=(opcode>>8)&0x7;
          offset=opcode&0xff;
          rs=(opcode>>11)&0x1;  // SP (0=PC,1=SP)
          sprintf(instruction, "%s r%d, %s, #%d", table_thumb[n].instr, rd, (rs==0)?"PC":"SP",offset<<2);
          return 2;
        case OP_ADD_OFFSET_TO_SP:
          rs=(opcode>>11)&0x1;  // S (0=positive,1=negative)
          offset=opcode&0xff;
          sprintf(instruction, "%s SP, #%s%d", table_thumb[n].instr, (rs==0)?"":"-",offset<<2);
          return 2;
        case OP_PUSH_POP_REGISTERS:
          rs=opcode&0xff;      // Rlist
          rn=opcode>>8&0x1;    // PC or LR
          get_rlist(temp, rs);
          if (rn==1)
          {
            if (temp[0]!=0) { strcat(temp, ","); }
            if (((opcode>>11)&1)==0) { strcat(temp, " LR"); }
            else { strcat(temp, " PC"); }
          }
          sprintf(instruction, "%s {%s }", table_thumb[n].instr, temp);
          return 2;
        default:
          strcpy(instruction, "???");
          return 2;
      }
    }
    n++;
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_thumb(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max;
char instruction[128];
char bytes[10];
int count;
int n;
//unsigned int opcode=memory_read_m(&asm_context->memory, address);

  fprintf(asm_context->list, "\n");
  count=disasm_thumb(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);

  bytes[0]=0;
  // count should always be 2
  if (count==2)
  {
    char temp[8];
    sprintf(temp, "%04x ", memory_read_m(&asm_context->memory, address)|(memory_read_m(&asm_context->memory, address+1)<<8));
    strcat(bytes, temp);
  }
    else
  {
    for (n=0; n<count; n++)
    {
      char temp[4];
      sprintf(temp, "%02x ", memory_read_m(&asm_context->memory, address+n));
      strcat(bytes, temp);
    }
  }

  fprintf(asm_context->list, "0x%04x: %-9s %-40s cycles: ", address, bytes, instruction);

  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }
}

void disasm_range_thumb(struct _memory *memory, int start, int end)
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
    count=disasm_thumb(memory, start, instruction, &cycles_min, &cycles_max);

    bytes[0]=0;
    // count should always be 2
    if (count==2)
    {
      char temp[8];
      sprintf(temp, "%04x ", READ_RAM16(start));
      strcat(bytes, temp);
    }
      else
    {
      for (n=0; n<count; n++)
      {
        char temp[4];
        sprintf(temp, "%02x ", READ_RAM(start+n));
        strcat(bytes, temp);
      }
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


