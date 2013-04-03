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
#include "disasm_avr8.h"
#include "table_avr8.h"

//#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) memory_read_m(memory, a)|(memory_read_m(memory, a+1)<<8)

int get_cycle_count_avr8(unsigned short int opcode)
{
  return -1;
}

int disasm_avr8(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
int opcode;
int n;
int rd,rr,k;

  *cycles_min=-1;
  *cycles_max=-1;

  opcode=READ_RAM16(address);

  n=0;
  while(table_avr8[n].instr!=NULL)
  {
    if ((opcode&table_avr8[n].mask)==table_avr8[n].opcode)
    {
      *cycles_min=table_avr8[n].cycles_min;
      *cycles_max=table_avr8[n].cycles_max;

      switch(table_avr8[n].type)
      {
        case OP_NONE:
          sprintf(instruction, "%s", table_avr8[n].instr);
          return 2;
        case OP_BRANCH_S_K:
          sprintf(instruction, "%s s,addr", table_avr8[n].instr);
          return 2;
        case OP_BRANCH_K:
          sprintf(instruction, "%s addr", table_avr8[n].instr);
          return 2;
        case OP_TWO_REG:
          rd=(opcode>>4)&0x1f;
          rr=((opcode&0x200)>>5)|((opcode)&0xf);
          sprintf(instruction, "%s r%d, r%d", table_avr8[n].instr, rd, rr);
          return 2;
        case OP_REG_IMM:
          rd=((opcode>>4)&0xf)+16;
          k=((opcode&0xf00)>>4)|(opcode&0xf);
          sprintf(instruction, "%s r%d, 0x%x", table_avr8[n].instr, rd, k);
          return 2;
        case OP_ONE_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_BIT:
          rd=(opcode>>4)&0x1f;
          k=opcode&0x7;
          sprintf(instruction, "%s r%d, %d", table_avr8[n].instr, rd, k);
          return 2;
        case OP_REG_IMM_WORD:
          rd=(((opcode>>4)&0x3)<<1)+24;
          k=((opcode&0xc0)>>2)|(opcode&0xf);
          sprintf(instruction, "%s r%d, %d", table_avr8[n].instr, rd, k);
          return 2;
        case OP_IOREG_BIT:
          rd=(opcode>>3)&0x1f;
          k=opcode&0x7;
          sprintf(instruction, "%s 0x%x, %d", table_avr8[n].instr, rd, k);
          return 2;
        case OP_SREG_BIT:
          k=(opcode>>4)&0x7;
          sprintf(instruction, "%s %d", table_avr8[n].instr, k);
          return 2;
        case OP_REG_4:
          rd=((opcode>>4)&0xf)+16;
          sprintf(instruction, "%s r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_IN:
          rd=(opcode>>4)&0xf;
          k=((opcode&0x600)>>5)|(opcode&0xf);
          sprintf(instruction, "%s r%d, 0x%x", table_avr8[n].instr, rd, k);
          return 2;
        case OP_OUT:
          rd=(opcode>>4)&0xf;
          k=((opcode&0x600)>>5)|(opcode&0xf);
          sprintf(instruction, "%s 0x%x, r%d", table_avr8[n].instr, k, rd);
          return 2;
        case OP_MOVW:
          rd=((opcode>>4)&0xf)<<1;
          rr=(opcode&0xf)<<1;
          sprintf(instruction, "%s r%d, r%d", table_avr8[n].instr, rd, rr);
          return 2;
        case OP_RELATIVE:
          k=opcode&0xfff;
          if (k&800) { k=-(((~k)&0xfff)+1); }
          sprintf(instruction, "%s 0x%x (%d)", table_avr8[n].instr, address+k, k);
          return 2;
        case OP_JUMP:
          k=(((opcode>>3)|(opcode&0x1))<<8)|READ_RAM16(address+2);
          sprintf(instruction, "%s 0x%x", table_avr8[n].instr, k);
          return 2;
        case OP_SPM_Z_PLUS:
          sprintf(instruction, "%s Z+", table_avr8[n].instr);
          return 2;
        case OP_REG_X:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d, X", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_Y:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d, Y", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_Z:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d, Z", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_X_PLUS:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d, X+", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_Y_PLUS:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d, Y+", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_Z_PLUS:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d, Z+", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_MINUS_X:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d, -X", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_MINUS_Y:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d, -Y", table_avr8[n].instr, rd);
          return 2;
        case OP_REG_MINUS_Z:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s r%d, -Z", table_avr8[n].instr, rd);
          return 2;
        case OP_X_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s X, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_Y_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s Y, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_Z_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s Z, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_X_PLUS_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s X+, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_Y_PLUS_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s Y+, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_Z_PLUS_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s Z+, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_MINUS_X_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s -X, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_MINUS_Y_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s -Y, r%d", table_avr8[n].instr, rd);
          return 2;
        case OP_MINUS_Z_REG:
          rd=(opcode>>4)&0x1f;
          sprintf(instruction, "%s -Z, r%d", table_avr8[n].instr, rd);
          return 2;

        default:
          sprintf(instruction, "%s", table_avr8[n].instr);
          return 2;
      }
    }
    n++;
  }

  strcpy(instruction, "???");

  return 2;
}

void list_output_avr8(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max;
char instruction[128];
int count,opcode;
int n;

  fprintf(asm_context->list, "\n");
  count=disasm_avr8(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);

  opcode=memory_read_m(&asm_context->memory, address)|(memory_read_m(&asm_context->memory, address+1)<<8);

  fprintf(asm_context->list, "0x%04x: %04x %-40s cycles: ", address, opcode, instruction);

  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

  for (n=2; n<count; n+=2)
  {
    opcode=memory_read_m(&asm_context->memory, address+n)|(memory_read_m(&asm_context->memory, address+n+1)<<8);
    printf("     %04x\n", opcode);
  }
}

void disasm_range_avr8(struct _memory *memory, int start, int end)
{
char instruction[128];
int cycles_min=0,cycles_max=0;
int count,opcode;
int n;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start<=end)
  {
    count=disasm_avr8(memory, start, instruction, &cycles_min, &cycles_max);

    //opcode=(memory_read_m(memory, address)<<8)|memory_read_m(>memory, address+1);
    opcode=READ_RAM16(start);

    if (cycles_min<1)
    {
      printf("0x%04x: %04x %-40s ?\n", start, opcode, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: %04x %-40s %d\n", start, opcode, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: %04x %-40s %d-%d\n", start, opcode, instruction, cycles_min, cycles_max);
    }

    for (n=2; n<count; n+=2)
    {
      opcode=READ_RAM16(start+n);
      printf("     %04x\n", opcode);
    }

    start=start+count;
  }
}


