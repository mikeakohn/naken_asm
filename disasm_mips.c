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
#include "disasm_mips.h"

#define READ_RAM(a) memory_read_m(memory, a)

int get_cycle_count_mips(unsigned short int opcode)
{
  return -1;
}

int disasm_mips(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
const char *reg[32] = {
  "$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
  "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
  "$a0", "$a1", "$a2", "$a3", "$a4", "$a5", "$a6", "$a7",
  "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra" };
unsigned int opcode;
int function,format;
int n,r;
char temp[32];

  *cycles_min=-1;
  *cycles_max=-1;
  opcode=get_opcode32(memory, address);

  if ((opcode>>26)==0)
  {
    // R-Type Instruction [ op 6, rs 5, rt 5, rd 5, sa 5, function 6 ]
    function=opcode&0x3f;
    n=0;
    while(mips_r_table[n].instr!=NULL)
    {
      if (mips_r_table[n].function==function)
      {
        int rs=(opcode>>21)&0x1f;
        int rt=(opcode>>16)&0x1f;
        int rd=(opcode>>11)&0x1f;
        int sa=(opcode>>6)&0x1f;

        strcpy(instruction, mips_r_table[n].instr);

        for (r=0; r<3; r++)
        {
          if (mips_r_table[n].operand[r]==MIPS_OP_NONE) { break; }

          if (mips_r_table[n].operand[r]==MIPS_OP_RS)
          {
            sprintf(temp, "%s", reg[rs]);
          }
            else
          if (mips_r_table[n].operand[r]==MIPS_OP_RT)
          {
            sprintf(temp, "%s", reg[rt]);
          }
            else
          if (mips_r_table[n].operand[r]==MIPS_OP_RD)
          {
            sprintf(temp, "%s", reg[rd]);
          }
            else
          if (mips_r_table[n].operand[r]==MIPS_OP_SA)
          {
            sprintf(temp, "%s", reg[sa]);
          }
            else
          { temp[0]=0; }

          if (r!=0) { strcat(instruction, ", "); }
          else { strcat(instruction, " "); }

          strcat(instruction, temp);
        }

        break;
      }

      n++;
    }
  }
    else
  if ((opcode>>27)==1)
  {
    // J-Type Instruction [ op 6, target 26 ]
    unsigned int upper=(address+4)&0xf0000000;
    if ((opcode>>26)==2)
    {
      sprintf(instruction, "j 0x%08x", ((opcode&0x03ffffff)<<2)|upper);
    }
      else
    {
      sprintf(instruction, "jal 0x%08x", ((opcode&0x03ffffff)<<2)|upper);
    }
  }
    else
  if ((opcode>>28)==4)
  {
    // Coprocessor Instruction [ op 6, format 5, ft 5, fs 5, fd 5, funct 6 ]
    function=opcode&0x3f;
    format=(opcode>>21)&0x1f;
    n=0;
    while(mips_cop_table[n].instr!=NULL)
    {
      if (mips_cop_table[n].function==function &&
          mips_cop_table[n].format==format)
      {
        int ft=(opcode>>16)&0x1f;
        int fs=(opcode>>11)&0x1f;
        int fd=(opcode>>6)&0x1f;

        strcpy(instruction, mips_cop_table[n].instr);

        for (r=0; r<3; r++)
        {
          if (mips_cop_table[n].operand[r]==MIPS_COP_NONE) { break; }

          if (mips_cop_table[n].operand[r]==MIPS_COP_FD)
          {
            sprintf(temp, "$f%d", fd);
          }
            else
          if (mips_cop_table[n].operand[r]==MIPS_COP_FS)
          {
            sprintf(temp, "$f%d", fs);
          }
            else
          if (mips_cop_table[n].operand[r]==MIPS_COP_FT)
          {
            sprintf(temp, "$f%d", ft);
          }
            else
          { temp[0]=0; }

          if (r!=0) { strcat(instruction, ", "); }
          strcat(instruction, temp);
        }

        break;
      }

      n++;
    }
  }
    else
  {
    opcode=opcode>>26;
    // I-Type?  [ op 6, rs 5, rt 5, imm 16 ]
    n=0;
    while(mips_i_table[n].instr!=NULL)
    {
      if (mips_r_table[n].function==opcode)
      {
        int rs=(opcode>>21)&0x1f;
        int rt=(opcode>>16)&0x1f;
        int immediate=opcode&0xffff;

        if (mips_r_table[n].operand[2]==MIPS_OP_RT_IS_0)
        {
          if (rt!=0) { continue; }
        }
          else
        if (mips_r_table[n].operand[2]==MIPS_OP_RT_IS_1)
        {
          if (rt!=1) { continue; }
        }

        strcpy(instruction, mips_r_table[n].instr);

        for (r=0; r<3; r++)
        {
          if (mips_r_table[n].operand[r]==MIPS_OP_NONE) { break; }

          if (mips_r_table[n].operand[r]==MIPS_OP_RS)
          {
            sprintf(temp, "%s", reg[rs]);
          }
            else
          if (mips_r_table[n].operand[r]==MIPS_OP_RT)
          {
            sprintf(temp, "%s", reg[rt]);
          }
            else
          if (mips_r_table[n].operand[r]==MIPS_OP_IMMEDIATE)
          {
            sprintf(temp, "%x", immediate);
          }
            else
          if (mips_r_table[n].operand[r]==MIPS_OP_IMMEDIATE_RS)
          {
            sprintf(temp, "%x(%s)", immediate, reg[rs]);
          }
            else
          if (mips_r_table[n].operand[r]==MIPS_OP_LABEL)
          {
            // FIXME - this is probably an offset
            sprintf(temp, "%x", immediate);
          }
            else
          { temp[0]=0; }

          if (r!=0) { strcat(instruction, ", "); }
          strcat(instruction, temp);
        }

        break;
      }

      n++;
    }

    if (mips_i_table[n].instr!=NULL)
    {
      printf("Internal Error: Unknown MIPS opcode %08x, %s:%d\n", opcode, __FILE__, __LINE__);
      strcpy(instruction, "???");
    }
  }

  return 4;
}

void list_output_mips(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max,count;
char instruction[128];
unsigned int opcode=get_opcode32(&asm_context->memory, address);

  fprintf(asm_context->list, "\n");
  count=disasm_mips(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);
  fprintf(asm_context->list, "0x%04x: 0x%08x %-40s cycles: ", address, opcode, instruction);

  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

}

void disasm_range_mips(struct _memory *memory, int start, int end)
{
// Are these correct and the same for all MSP430's?
char *vectors[16] = { "", "", "", "", "", "",
                      "", "", "", "",
                      "", "", "", "",
                      "",
                      "Reset/Watchdog/Flash" };
char instruction[128];
int vectors_flag=0;
int cycles_min=0,cycles_max=0;
int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start<=end)
  {
    if (start>=0xffe0 && vectors_flag==0)
    {
      printf("Vectors:\n");
      vectors_flag=1;
    }

    num=READ_RAM(start)|(READ_RAM(start+1)<<8);

    disasm_mips(memory, start, instruction, &cycles_min, &cycles_max);

    if (vectors_flag==1)
    {
      printf("0x%04x: 0x%04x  Vector %2d {%s}\n", start, num, (start-0xffe0)/2, vectors[(start-0xffe0)/2]);
      start+=2;
      continue;
    }

    if (cycles_min<1)
    {
      printf("0x%04x: 0x%04x %-40s ?\n", start, num, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: 0x%04x %-40s %d\n", start, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%04x %-40s %d-%d\n", start, num, instruction, cycles_min, cycles_max);
    }

#if 0
    count-=4;
    while (count>0)
    {
      start=start+4;
      num=READ_RAM(start)|(READ_RAM(start+1)<<8);
      printf("0x%04x: 0x%04x\n", start, num);
      count-=4;
    }
#endif

    start=start+4;
  }
}

