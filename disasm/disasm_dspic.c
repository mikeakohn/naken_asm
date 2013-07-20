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

#if 0
#define EXTRACT_VALUE() ((opcode>>table_dspic[n].operands[r].bitpos)& \
                  ((1<<table_dspic[n].operands[r].bitlen)-1))
#define EXTRACT_ATTR() ((opcode>>table_dspic[n].operands[r].attrpos)& \
                  ((1<<table_dspic[n].operands[r].attrlen)-1))
#endif

static char accum[] = { 'A','B' };
static char *addr_modes[] = { "w%d", "[w%d]", "[w%d--]", "[w%d++]", "[--w%d]", "[++w%d]" };

#if 0
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
#endif

#if 0
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
#endif

#if 0
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
#endif


int get_cycle_count_dspic(unsigned short int opcode)
{
  return -1;
}

static void get_wd(char *temp, int reg, int attr, int reg2)
{
  switch(attr)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      sprintf(temp, addr_modes[attr], reg);
      break;
    case 6:
      sprintf(temp, "[w%d+w%d]", reg, reg2);
      break;
    default:
      strcpy(temp, "???");
      break;
  }
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
uint32_t opcode;
//opcode48;
int count=4;
int16_t offset;
//int value,attr;
int n,b,d,f,a,w,lit;

  //opcode=get24bits(memory, address);
  opcode=get_opcode32(memory, address);

  n=0;
  while(table_dspic[n].name!=NULL)
  {
    if ((opcode&table_dspic[n].mask)==table_dspic[n].opcode)
    {
#if 0
      if (table_dspic[n].bitlen==48)
      {
        opcode48=get_opcode32(memory, address+4);

        if ((opcode48&table_dspic[n].mask48)!=table_dspic[n].opcode48)
        {
          n++;
          continue;
        }

        count+=4;
      }
#endif

      strcpy(instruction, table_dspic[n].name);
      *cycles_min=table_dspic[n].cycles_min;
      *cycles_max=table_dspic[n].cycles_max;

      switch(table_dspic[n].type)
      {
        case OP_NONE:
          return 4;
        case OP_F:
          f=opcode&0xffff;
          sprintf(instruction, "%s 0x%04x", table_dspic[n].name, f);
          return 4;
        case OP_F_WREG:
          b=(opcode>>14)&1;
          d=(opcode>>13)&1;
          f=opcode&0x1fff;
          sprintf(instruction, "%s%s 0x%04x%s", table_dspic[n].name, (b==0?"":".b"), f, (d==0?", wreg":""));
          return 4;
        case OP_ACC:
          a=(opcode>>15)&1;
          sprintf(instruction, "%s %c", table_dspic[n].name, (a==0)?'A':'B');
          return 4;
        case OP_ACC_LIT4_WD:
          b=(opcode>>16)&1;  // .R ?
          a=(opcode>>15)&1;
          lit=(opcode>>7)&0xf;
          if ((lit&0x8)!=0) { lit=-((lit^0xf)+1); }
          get_wd(temp, opcode&0xf, (opcode>>4)&0x7, (opcode>>11)&0xf);

          if (lit==0)
          {
            sprintf(instruction, "%s%s %c, %s", table_dspic[n].name, (b==0)?"":".r", accum[a], temp);
          }
            else
          {
            sprintf(instruction, "%s%s %c, #%d, %s", table_dspic[n].name, (b==0)?"":".r", accum[a], lit, temp);
          }
          return 4;
        case OP_ACC_LIT6:
          a=(opcode>>15)&1;
          lit=opcode&0x3f;
          if ((lit&0x20)!=0) { lit=-((lit^0x3f)+1); }
          sprintf(instruction, "%s %c, #%d", table_dspic[n].name, accum[a], lit);
          return 4;
        case OP_ACC_WB:
          a=(opcode>>15)&1;
          w=opcode&0xf;
          sprintf(instruction, "%s %c, w%d", table_dspic[n].name, accum[a], w);
          return 4;
        case OP_BRA:
          offset=opcode&0xffff;
          sprintf(instruction, "%s 0x%04x (%d)", table_dspic[n].name, (address/2)+2+offset, offset);
          return 4;
        case OP_CP0_WS:
          b=(opcode>>11)&1;
          get_wd(temp, opcode&0xf, (opcode>>4)&0x7, 0);
          sprintf(instruction, "%s%s %s", table_dspic[n].name, (b==0)?"":".b", temp);
          return 4;
        case OP_CP_F:
          b=(opcode>>14)&1;
          f=opcode&0x1fff;
          sprintf(instruction, "%s%s 0x%04x", table_dspic[n].name, (b==0)?"":".b", f);
          return 4;
        case OP_D_WNS_WND_1:
          w=((opcode>>1)&0x7)*2;
          get_wd(temp, (opcode>>7)&0xf, (opcode>>11)&0x7, 0);
          sprintf(instruction, "%s.d w%d, %s", table_dspic[n].name, w, temp);
          return 4;
        case OP_D_WNS_WND_2:
          w=((opcode>>8)&0x7)*2;
          get_wd(temp, opcode&0xf, (opcode>>4)&0x7, 0);
          sprintf(instruction, "%s.d %s, w%d", table_dspic[n].name, temp, w);
          return 4;
        case OP_F_BIT4:
          f=opcode&0x1fff;
          lit=(opcode>>13)&0x7;
          sprintf(instruction, "%s.b 0x%x, #%d", table_dspic[n].name, f, lit);
          return 4;
        case OP_F_BIT4_2:
          get_wd(temp, opcode&0xf, (opcode>>4)&0x7, 0);
          lit=(opcode>>12)&0xf;
          b=(opcode>>11)&1;
          sprintf(instruction, "%s.%c %s, #%d", table_dspic[n].name, (b==0)?'c':'z', temp, lit);
          return 4;
        case OP_F_WND:
          f=((opcode>>4)&0x7ffff)<<1;
          w=opcode&0xf;
          sprintf(instruction, "%s 0x%04x, w%d", table_dspic[n].name, f, w);
          return 4;
        case OP_GOTO:
          f=(opcode&0xffff)|(get_opcode32(memory, address+4)<<16);
          sprintf(instruction, "%s 0x%04x, w%d", table_dspic[n].name, f, w);
          return 8;
        case OP_LIT1:
          lit=opcode&0x1;
          sprintf(instruction, "%s #%d", table_dspic[n].name, lit);
          return 4;
        case OP_LIT10_WN:
          lit=(opcode>>4)&0x3ff;
          b=(opcode>>14)&0x1;
          w=opcode&0xf;
          sprintf(instruction, "%s%s #%x, w%d", table_dspic[n].name, (b==0)?"":".b", lit, w);
          return 4;
        case OP_LIT14:
          lit=opcode&0x3fff;
          sprintf(instruction, "%s #%d", table_dspic[n].name, lit);
          return 4;
        case OP_LIT14_EXPR:
          lit=opcode&0x3fff;
          offset=get_opcode32(memory, address+4);
          sprintf(instruction, "%s #%d, 0x%04x (%d)", table_dspic[n].name, lit, ((address/2)+2)+offset, offset);
          return 8;
        case OP_LIT16_WND:
          lit=(opcode>>4)&0xffff;
          w=opcode&0xf;
          sprintf(instruction, "%s #0x%02x, w%d", table_dspic[n].name, lit, w);
          return 4;
        case OP_LIT8_WND:
          lit=(opcode>>4)&0xff;
          w=opcode&0xf;
          sprintf(instruction, "%s #0x%02x, w%d", table_dspic[n].name, lit, w);
          return 4;
        case OP_LNK_LIT14:
          lit=opcode&0x3fff;
          sprintf(instruction, "%s #0x%02x", table_dspic[n].name, lit);
          return 4;
        case OP_N_WM_WN_ACC_AX_WY:
        case OP_POP_D_WND:
        case OP_POP_S:
        case OP_POP_WD:
        case OP_PUSH_S:
        case OP_PUSH_WNS:
        case OP_SS_WB_WS_WND:
        case OP_SU_WB_LIT5_WND:
        case OP_SU_WB_WS_WND:
        case OP_S_WM_WN:
        case OP_US_WB_WS_WND:
        case OP_UU_WB_LIT5_WND:
        case OP_UU_WB_WS_WND:
        case OP_U_WM_WN:
        case OP_WB_LIT4_WND:
        case OP_WB_LIT5:
        case OP_WB_LIT5_WD:
        case OP_WB_WN:
        case OP_WB_WNS_WND:
        case OP_WB_WS:
        case OP_WB_WS_WD:
        case OP_WD:
        case OP_WM_WM_ACC_WX_WY:
        case OP_WM_WM_ACC_WX_WY_WXD:
        case OP_WM_WN:
        case OP_WM_WN_ACC_WX_WY:
        case OP_WM_WN_ACC_WX_WY_AWB:
        case OP_WN:
        case OP_WN_EXPR:
        case OP_WNS_F:
        case OP_WNS_WD_LIT10:
        case OP_WNS_WND:
        case OP_WS_BIT4:
        case OP_WS_BIT4_2:
        case OP_WS_LIT10_WND:
        case OP_WS_LIT4_ACC:
        case OP_WS_PLUS_WB:
        case OP_WS_WB:
        case OP_WS_WB_WD_WB:
        case OP_WS_WD:
        case OP_WS_WND:
        case OP_A_WX_WY_AWB:
        default:
          strcpy(instruction, "???");
          break;
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

