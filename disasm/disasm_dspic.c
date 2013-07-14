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
#include "disasm_dspic.h"
#include "table_dspic.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define EXTRACT_VALUE() ((opcode>>dspic_table[n].operands[r].bitpos)& \
                  ((1<<dspic_table[n].operands[r].bitlen)-1))
#define EXTRACT_ATTR() ((opcode>>dspic_table[n].operands[r].attrpos)& \
                  ((1<<dspic_table[n].operands[r].attrlen)-1))

static char *addr_modes[] = { "w%d", "[w%d]", "[w%d--]", "[w%d++]", "[--w%d]", "[++w%d]" };

int get_dspic_flag_value(int flag)
{
  switch(flag)
  {
    case FLAG_B: return 1;  // sometimes not a flag (mov) :(
    case FLAG_W: return 0;
    case FLAG_C: return 0;
    case FLAG_Z: return 1;
    case FLAG_N: return 0;  // not a flag
    case FLAG_D: return 0;  // not a flag (pop,push)
    case FLAG_U: return 0;  // not a flag (div)
    case FLAG_SW: return 0; // not a flag? (div)
    case FLAG_SD: return 0; // not a flag? (div)
    case FLAG_UW: return 0;  // MISTAKE?
    case FLAG_UD: return 0;  // not a flag
    case FLAG_S:  return 0;  // not a flag (div,pop,push)
    case FLAG_SS: return 0;  // not a flag (mul)
    case FLAG_SU: return 0;  // not a flag (mul)
    case FLAG_US: return 0;  // not a flag (mul)
    case FLAG_UU: return 0;  // not a flag (mul)
    case FLAG_R: return 0;   // not a flag (sac)
  }
  return 0;
}

char *get_dspic_flag_str(int flag)
{
  switch(flag)
  {
    case FLAG_B: return ".b";
    case FLAG_W: return ".w";
    case FLAG_C: return ".c";
    case FLAG_Z: return ".z";
    case FLAG_N: return ".n";
    case FLAG_D: return ".d";
    case FLAG_U: return ".u";
    case FLAG_SW: return ".sw";
    case FLAG_SD: return ".sd";
    case FLAG_UW: return ".uw";
    case FLAG_UD: return ".ud";
    case FLAG_S:  return ".s";
    case FLAG_SS: return ".ss";
    case FLAG_SU: return ".su";
    case FLAG_US: return ".us";
    case FLAG_UU: return ".uu";
    case FLAG_R: return ".r";
  }

  return "";
}

int convert_dspic_flag_combo(int value, int flags)
{
  switch(flags)
  {
    case FLAG_NONE:
      if (value==0) return 0;
      return -1;
    case FLAG_B|FLAG_W:
      if (value==1) return FLAG_B;
      return -1;
    case FLAG_C|FLAG_Z:
      if (value==1) return FLAG_Z;
      return FLAG_C;
    case FLAG_S|FLAG_SW|FLAG_SD:
      if (value==0) return FLAG_S;
      return FLAG_SD;
  }

  printf("Internal Error: Not implemented %s:%d\n", __FILE__, __LINE__);

  return -1;
}


int get_cycle_count_dspic(unsigned short int opcode)
{
  return -1;
}

#if 0
static int get24bits(struct _memory *memory, int address)
{
int data;

  //data=READ_RAM(address)<<16;
  //data|=READ_RAM(address+1)<<8;
  //data|=READ_RAM(address+2);
  data=READ_RAM(address+2)<<16;
  data|=READ_RAM(address+1)<<8;
  data|=READ_RAM(address);

  return data;
}
#endif

#if 0
static void get_reg_string(char *temp, int mode, int reg)
{
  sprintf(temp, addr_modes[mode], reg);
}
#endif

int disasm_dspic(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
char temp[32];
int opcode,opcode48;
int count=4;
int value,attr;
int n,r;

  //opcode=get24bits(memory, address);
  opcode=get_opcode32(memory, address);

  n=0;
  while(dspic_table[n].name!=NULL)
  {
    if ((opcode&dspic_table[n].mask)==dspic_table[n].opcode)
    {
      if (dspic_table[n].bitlen==48)
      {
        opcode48=get_opcode32(memory, address+4);

        if ((opcode48&dspic_table[n].mask48)!=dspic_table[n].opcode48)
        {
          n++;
          continue;
        }

        count+=4;
      }

      strcpy(instruction, dspic_table[n].name);
      *cycles_min=dspic_table[n].cycles_min;
      *cycles_max=dspic_table[n].cycles_max;

      if (dspic_table[n].flags!=0)
      {
        int flag=-1;
        if (dspic_table[n].flag_pos==-1)
        {
          flag=dspic_table[n].default_flag;
        }
          else
        {
          flag=(opcode>>(dspic_table[n].flag_pos))&1;
          flag=convert_dspic_flag_combo(flag, dspic_table[n].flags);
        }

        strcat(instruction, get_dspic_flag_str(flag));
      }
//printf("n=%d args=%d\n", n, dspic_table[n].args);

      for (r=0; r<dspic_table[n].args; r++)
      {
        temp[0]=0;

        switch(dspic_table[n].operands[r].type)
        {
          case OP_BRA:
          {
            short int distance=EXTRACT_VALUE();
            sprintf(temp, "0x%02x  (%d)", ((address+4)+(distance*4))/2, distance);
            break;
          }
          case OP_F:
          case OP_EXPR:
          {
            value=EXTRACT_VALUE();
            sprintf(temp, "0x%02x", value);
            break;
          }
          case OP_LIT:
          {
            value=EXTRACT_VALUE();
            sprintf(temp, "#%d", value);
            break;
          }
          case OP_ACC:
          {
            value=EXTRACT_VALUE();
            if (value==0) { strcpy(temp, "A"); }
            else { strcpy(temp, "B"); }
            break;
          }
          case OP_WREG:
          {
            value=EXTRACT_VALUE();
            if (dspic_table[n].operands[r].optional==1)
            {
              if (value==1) strcpy(temp, "wreg");
            }
            break;
          }
          case OP_WN:
          case OP_WM:
          case OP_WB:
          case OP_WNS:
          case OP_WND:
          {
            value=EXTRACT_VALUE();
            sprintf(temp, "w%d", value);
            break;
          }
          case OP_WD:
          case OP_WS:
          case OP_W_INDEX:
          {
            value=EXTRACT_VALUE();
            attr=EXTRACT_ATTR();
            if (attr>=6)
            {
              if ((opcode>>19)==0xf)
              {
                sprintf(temp, "[r%d + r%d]", value, (opcode>>15)&0xf);
              }
                else
              {
                strcpy(temp, "???");
              }
            }
              else
            {
              sprintf(temp, addr_modes[attr], value);
            }

            break;
          }
          case OP_WS_LIT:
          case OP_WD_LIT:
          {
            value=EXTRACT_VALUE();
            attr=EXTRACT_ATTR();

            sprintf(temp, "[w%d + #%d]", value, attr);

            break;
          }
          case OP_WXD:
          case OP_WYD:
          {
            value=EXTRACT_VALUE();
            sprintf(temp, "w%d", value+4);
            break;
          }
          case OP_MULT2:
          {
            value=EXTRACT_VALUE();
            sprintf(temp, "w%d*w%d", value+4, value+4);
            break;
          }
          case OP_MULT3:
          {
            const char *mul3[] = { "w4*w5", "w4*w6", "w4*w7", "?", "w5*w6",
                                   "w5*w7", "w6*w7", "?" };
            value=EXTRACT_VALUE();
            sprintf(temp, "%s", mul3[value]);
            break;
          }
          case OP_PREFETCH_ED_X:
          case OP_PREFETCH_ED_Y:
          case OP_PREFETCH_X:
          case OP_PREFETCH_Y:
          {
            int reg;
            value=EXTRACT_VALUE();
            if (value==0x4)
            {
              strcpy(temp, "<no prefetch?>");
            }
              else
            {
              reg=(value>>3)+8;
              value=value&0x7;
              if (dspic_table[n].operands[r].type==OP_PREFETCH_ED_Y) reg+=2;
              if (value==0x4)
              {
                sprintf(temp, "[w%d + w12]", reg);
              }
                else
              {
                if ((value&0x7)!=0)
                {
                  value=((value^0x7)+1);
                  sprintf(temp, "[w%d]-=%d", reg, value*2);
                }
                  else
                {
                  sprintf(temp, "[w%d]+=%d", reg, value*2);
                }
              }
              if (dspic_table[n].operands[r].type==OP_PREFETCH_X ||
                 dspic_table[n].operands[r].type==OP_PREFETCH_Y)
              {
                char temp2[8];
                attr=EXTRACT_ATTR();
                sprintf(temp2, ", w%d", attr+4);
                strcat(temp, temp2);
              }
            }
          }
          case OP_ACC_WB:
            value=EXTRACT_VALUE();
            if (value==0) { strcpy(temp, "w13"); }
            else if (value==1) { strcpy(temp, "[w13]+=2"); }
            else if (value==3) { strcpy(temp, "?"); }
            break;
          case OP_EXPR_DO:
            value=get_opcode32(memory, address+4);
            sprintf(temp, "%d", value);
            //count+=2;
            break;
          case OP_EXPR_GOTO:
            value=(opcode&0xffff)|(get_opcode32(memory, address+4)<<16);
            sprintf(temp, "0x%04x", value);
            //count+=2;
            break;
          default:
            strcpy(temp, "<error>");
            break;
        }

        if (temp[0]!=0)
        {
          if (r==0) { strcat(instruction, " "); }
          else { strcat(instruction, ", "); }
          strcat(instruction, temp);
        }
      }

      return count;
    }

    n++;
  }

  strcpy(instruction, "???");
  return 4;
}

void list_output_dspic(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max,count;
char instruction[128];
unsigned int opcode;

  //address=address*2;
  opcode=get_opcode32(&asm_context->memory, address);

  fprintf(asm_context->list, "\n");
  count=disasm_dspic(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);

  fprintf(asm_context->list, "0x%04x: 0x%06x %-40s cycles: ", address/2, opcode, instruction);

  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

  if (count==8)
  {
    fprintf(asm_context->list, "0x%04x: 0x%02x%02x%02x\n",
      //(address+4)/2,
      (address/2)+2,
      memory_read(asm_context, address+4),
      memory_read(asm_context, address+5),
      memory_read(asm_context, address+6));
  }
}

void disasm_range_dspic(struct _memory *memory, int start, int end)
{
char instruction[128];
int cycles_min=0,cycles_max=0;
int num;

  printf("\n");

  printf("%-7s %-10s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ---------- ----------------------------------       ------\n");

  while(start<=end)
  {
    num=READ_RAM(start)|(READ_RAM(start+1)<<8)|(READ_RAM(start+2)<<16)|(READ_RAM(start+3)<<24);

    int count=disasm_dspic(memory, start, instruction, &cycles_min, &cycles_max);

    if (cycles_min<1)
    {
      printf("0x%04x: 0x%08x %-40s ?\n", start/2, num, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: 0x%08x %-40s %d\n", start/2, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%08x %-40s %d-%d\n", start/2, num, instruction, cycles_min, cycles_max);
    }

    count-=4;
    while (count>0)
    {
      start=start+4;
      // num=READ_RAM(start)|(READ_RAM(start+1)<<8);
      num=READ_RAM(start)|(READ_RAM(start+1)<<8)|(READ_RAM(start+2)<<16)|(READ_RAM(start+3)<<24);
      printf("0x%04x: 0x%08x\n", start/2, num);
      count-=4;
    }

    start=start+4;
  }
}

