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
#include <stdint.h>

#include "disasm_msp430.h"
#include "table_msp430.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define READ_RAM16(a) (memory_read_m(memory, a+1)<<8)|memory_read_m(memory, a)

static char *regs[] = { "PC", "SP", "SR", "CG", "r4", "r5", "r6", "r7", "r8",
                        "r9", "r10", "r11", "r12", "r13", "r14", "r15" };

// FIXME - let's move this somewhee more sane
int get_register_msp430(char *token)
{
  if (token[0]=='r' || token[0]=='R')
  {
    if (token[2]==0 && (token[1]>='0' && token[1]<='9'))
    {
      return token[1]-'0';
    }
      else
    if (token[3]==0 && token[1]=='1' && (token[2]>='0' && token[2]<='5'))
    {
      return 10+(token[2]-'0');
    }
  }

  if (strcasecmp(token, "pc")==0) return 0;
  if (strcasecmp(token, "sp")==0) return 1;
  if (strcasecmp(token, "sr")==0) return 2;
  if (strcasecmp(token, "cg")==0) return 3;

  return -1;
}

static int get_source_reg(struct _memory *memory, int address, int reg, int As, int bw, char *reg_str, uint16_t prefix)
{
int count=0;
int extra=0;

  if (prefix!=0xffff) extra=(prefix&0x0780)<<9;

  reg_str[0]=0;

  if (reg==0)
  {
    if (As==0)
    { strcat(reg_str, regs[reg]); }
      else
    if (As==1)
    {
      uint16_t a=(READ_RAM(address+3)<<8)|READ_RAM(address+2);
      count+=2;
      a=a+(address+count);
      sprintf(reg_str, "0x%04x", a|extra);
    }
      else
    if (As==2)
    { strcpy(reg_str, "@PC"); }
      else
    if (As==3)
    {
      uint16_t a=(READ_RAM(address+3)<<8)|READ_RAM(address+2);
      count+=2;
      if (bw==0)
      { sprintf(reg_str, "#0x%04x", a|extra); }
        else
      { sprintf(reg_str, "#0x%02x", a|extra); }
    }
  }
    else
  if (reg==2)
  {
    if (As==0)
    { strcat(reg_str, regs[reg]); }
      else
    if (As==1)
    {
      uint16_t a=(READ_RAM(address+3)<<8)|READ_RAM(address+2);
      count+=2;
      sprintf(reg_str, "&0x%04x", a|extra);
    }
      else
    if (As==2)
    { strcat(reg_str, "#4"); }
      else
    if (As==3)
    { strcat(reg_str, "#8"); }
  }
    else
  if (reg==3)
  {
    if (As==0)
    { strcat(reg_str, "#0"); }
      else
    if (As==1)
    { strcat(reg_str, "#1"); }
      else
    if (As==2)
    { strcat(reg_str, "#2"); }
      else
    if (As==3)
    { strcat(reg_str, "#-1"); }
  }
    else
  {
    if (As==0)
    {
      strcat(reg_str, regs[reg]);
    }
      else
    if (As==1)
    {
      uint16_t a=(READ_RAM(address+3)<<8)|READ_RAM(address+2);
      count+=2;
      sprintf(reg_str, "0x%x(%s)", a|extra, regs[reg]);
    }
      else
    if (As==2)
    {
      sprintf(reg_str, "@%s", regs[reg]);
    }
      else
    if (As==3)
    {
      sprintf(reg_str, "@%s+", regs[reg]);
    }
  }

  return count;
}

static int get_dest_reg(struct _memory *memory, int address, int reg, int Ad, char *reg_str, int count, uint16_t prefix)
{
int extra=0;

  if (prefix!=0xffff) extra=(prefix&0x000f)<<16;

  reg_str[0]=0;

  if (reg==0)
  {
    if (Ad==0)
    { strcat(reg_str, regs[reg]); }
      else
    if (Ad==1)
    {
      uint16_t a=(READ_RAM(address+count+3)<<8)|READ_RAM(address+(count+2));
      count+=2;
      a=a+(address+count);
      sprintf(reg_str, "0x%04x", a|extra);
    }
  }
    else
  if (reg==2)
  {
    if (Ad==0)
    { strcat(reg_str, regs[reg]); }
      else
    if (Ad==1)
    {
      uint16_t a=(READ_RAM(address+count+3)<<8)|READ_RAM(address+count+2);
      count+=2;
      sprintf(reg_str, "&0x%04x", a|extra);
    }
  }
    else
  if (reg==3)
  {
    strcat(reg_str, regs[reg]);
  }
    else
  {
    if (Ad==0)
    {
      strcat(reg_str, regs[reg]);
    }
      else
    if (Ad==1)
    {
      uint16_t a=(READ_RAM(address+count+3)<<8)|READ_RAM(address+count+2);
      count+=2;
      sprintf(reg_str, "0x%x(%s)", a|extra, regs[reg]);
    }
  }

  return count;
}

static int one_operand(struct _memory *memory, int address, char *instruction, uint16_t opcode, uint16_t prefix)
{
//char *instr[] = { "rrc", "swpb", "rra", "sxt", "push", "call", "reti", "???" };
char ext[3] = { 0 };
int o;
int reg;
int As;
int count=2;
int bw=0;

  o=(opcode&0x0380)>>7;

  //strcpy(instruction, instr[o]);
  if (o==7) { return 2; }
  if (o==6) { return count; }

  if (prefix!=0xffff) { strcat(instruction, "x"); }

  As=(opcode&0x0030)>>4;
  reg=opcode&0x000f;

  if ((opcode&0x0040)==0)
  {
    if (((opcode>>7)&1)==0 && ((opcode>>7)&7)!=6)
    {
      strcpy(ext, ".w");
    }
  }
    else
  {
    if (o==1 || o==3 || o==5 || o==6) { strcpy(instruction, "???"); return 1; }
    strcpy(ext, ".b");
    bw=1;
  }

  if (prefix!=-1 && o<=5)
  {
    int al=((prefix>>5)&2)|bw;

    if ((o&1)==1)
    {
      if (al==0) { strcpy(ext, ".a"); }
      else if (al==1) { strcpy(ext, ".?"); }
      else if (al==2) { strcpy(ext, ".w"); }
      else if (al==3) { strcpy(ext, ".?"); }
    }
      else
    {
      if (al==0) { strcpy(ext, ".?"); }
      else if (al==1) { strcpy(ext, ".a"); }
      else if (al==2) { strcpy(ext, ".w"); }
      else if (al==3) { strcpy(ext, ".b"); }
    }
  }

  // FIXME - Add extension for MSP430X

  strcat(instruction, ext);
  strcat(instruction, " ");

  char reg_str[128];
  count+=get_source_reg(memory, address, reg, As, bw, reg_str, prefix);
  strcat(instruction, reg_str);

  return count;
}

static int relative_jump(struct _memory *memory, int address, char *instruction, uint16_t opcode, uint16_t prefix)
{
//char *instr[] = { "jne", "jeq", "jlo", "jhs", "jn", "jge", "jl", "jmp" };
int count=2;
int o;

  o=(opcode&0x1c00)>>10;
  if (o>7)
  {
    printf("WTF JMP?\n");
    return 1;
  }

  //strcpy(instruction, instr[o]);

  if (prefix!=0xffff) { strcat(instruction, "x"); }

  int offset=opcode&0x03ff;
  if ((offset&0x0200)!=0)
  {
    offset=-((offset^0x03ff)+1);
  }

  offset*=2;

  char token[128];
  sprintf(token, " 0x%04x  (offset: %d)", ((address+2)+offset)&0xffff, offset);
  strcat(instruction, token);

  return count;
}

static int two_operand(struct _memory *memory, int address, char *instruction, uint16_t opcode, uint16_t prefix)
{
//char *instr[] = { "mov", "add", "addc", "subc", "sub", "cmp", "dadd", "bit",
//                  "bic", "bis", "xor", "and" };
char ext[3] = { 0 };
int o;
int Ad,As;
int count=0;
int bw=0;

  // FIXME - is this needed anymore?
  o=opcode>>12;
#if 0
  if (o<4 || o>15)
  {
    strcpy(instruction, "???");
    return 1;
  }
#endif

  o=o-4;

  if ((opcode&0x0040)==0)
  { strcpy(ext, ".w"); bw=0; }
    else
  { strcpy(ext, ".b"); bw=1; }

  if (prefix==0xffff)
  {
    char instr[32];
    strcpy(instr, instruction);
    if ((opcode&0x00ff)==0x0003)
    { sprintf(instruction, "nop   --  %s", instr); }
      else
    if (opcode==0x4130)
    { sprintf(instruction, "ret   --  %s", instr); }
      else
    if ((opcode&0xff30)==0x4130)
    { sprintf(instruction, "pop.%c r%d   --  %s", bw==0?'w':'b', opcode&0x000f, instr[o]); }
      else
    if (opcode==0xc312)
    { sprintf(instruction, "clrc  --  %s", instr); }
      else
    if (opcode==0xc222)
    { sprintf(instruction, "clrn  --  %s", instr); }
      else
    if (opcode==0xc322)
    { sprintf(instruction, "clrz  --  %s", instr); }
      else
    if (opcode==0xc232)
    { sprintf(instruction, "dint  --  %s", instr); }
      else
    if (opcode==0xd312)
    { sprintf(instruction, "setc  --  %s", instr); }
      else
    if (opcode==0xd222)
    { sprintf(instruction, "setn  --  %s", instr); }
      else
    if (opcode==0xd322)
    { sprintf(instruction, "setz  --  %s", instr); }
      else
    if (opcode==0xd232)
    { sprintf(instruction, "eint  --  %s", instr); }
  }
    else
  {
    strcat(instruction, "x");

    int al=((prefix>>5)&2)|bw;

    if (al==0) { strcpy(ext, ".?"); }
    else if (al==1) { strcpy(ext, ".a"); }
    else if (al==2) { strcpy(ext, ".w"); }
    else if (al==3) { strcpy(ext, ".b"); }
  }

  Ad=(opcode&0x0080)>>7;
  As=(opcode&0x0030)>>4;

  strcat(instruction, ext);
  strcat(instruction, " ");

  char reg_str[128];
  count=get_source_reg(memory, address, (opcode&0x0f00)>>8, As, bw, reg_str, prefix);
  strcat(instruction, reg_str);

  strcat(instruction, ", ");
  count=get_dest_reg(memory, address, opcode&0x000f, Ad, reg_str, count, prefix);
  strcat(instruction, reg_str);

  //if (prefix!=0xffff) { count+=2; }

  return count+2;
}

//#ifdef SUPPORT_MSP430X
static int get_20bit(struct _memory *memory, int address, unsigned int opcode)
{
  return ((opcode>>8)&0x0f)<<16|
         (READ_RAM(address+3)<<8)|
         (READ_RAM(address+2));
}

#if 0
static int twenty_bit_zero(struct _memory *memory, int address, char *instruction, uint16_t opcode)
{
char *instr[] = { "rrcm", "rram", "rlam", "rrum" };
char *instr2[] = { "mova", "cmpa", "adda", "suba" };
int o;

  o=(opcode>>4)&0x0f;

  switch(o)
  {
  case 0:
    sprintf(instruction, "mova @r%d, r%d", (opcode>>8)&0x0f, (opcode&0x0f));
    return 2;
  case 1:
    sprintf(instruction, "mova @r%d+, r%d", (opcode>>8)&0x0f, (opcode&0x0f));
    return 2;
  case 2:
    sprintf(instruction, "mova &%05x, r%d", get_20bit(memory,address,opcode), (opcode&0x0f));
    return 4;
  case 3:
    sprintf(instruction, "mova %d(r%d), r%d", (READ_RAM(address+3)<<8)|READ_RAM(address+2),(opcode>>8)&0xff, (opcode&0x0f));
    return 4;
  case 4:
  case 5:
    sprintf(instruction, "%s.%c #%d, r%d", instr[(opcode>>8)&3], ((opcode&64)==0)?'a':'w', (opcode>>10)&3, opcode&0x000f);
    return 2;
  case 6:
    sprintf(instruction, "mova r%d, &0x%05x", (opcode>>8)&0x0f, ((opcode&0x0f)<<16)|(READ_RAM(address+3)<<8)|READ_RAM(address+2));
    return 4;
  case 7:
    sprintf(instruction, "mova r%d, %d(r%d)", (opcode>>8)&0x0f, (READ_RAM(address+3)<<8)|READ_RAM(address+2),  opcode&0x0f);
    return 4;
  case 8:
  case 9:
  case 10:
  case 11:
    sprintf(instruction, "%s #%d, r%d", instr2[(opcode>>4)&3], get_20bit(memory,address,opcode), opcode&0x0f);
    return 4;
  case 12:
  case 13:
  case 14:
  case 15:
    sprintf(instruction, "%s r%d, r%d", instr2[(opcode>>4)&3], (opcode>>8)&0x0f, opcode&0x0f);
    return 2;
  default:
    strcpy(instruction, "???");
    return 2;
  }
}
#endif

#if 0
static int twenty_bit_call(struct _memory *memory, int address, char *instruction, uint16_t opcode)
{
  if ((opcode&0x00ff)==0) { strcpy(instruction, "reti"); return 2; }

  int o=(opcode>>6)&0x03;
  int mode=(opcode>>4)&0x03;

  if (o==0x01) // calla source, As=mode
  {
    int count=2;
    int reg=opcode&0x000f;
    char reg_str[128];

    // mode=As 
    count+=get_source_reg(memory, address, reg, mode, 0, reg_str, -1);
    sprintf(instruction, "calla %s", reg_str);
    return count;
  }
    else
  if (o==0x02)
  {
    if (mode==0) // calla &abs20
    {
      sprintf(instruction, "calla &%d", get_20bit(memory,address,opcode));
      return 4;
    }
      else
    if (mode==1) // calla x(PC)
    {
      sprintf(instruction, "calla %d(PC)", get_20bit(memory,address,opcode));
      return 4;
    }
      else
    if (mode==3) // calla #immediate
    {
      sprintf(instruction, "calla #%d", get_20bit(memory,address,opcode));
      return 4;
    }
  }

  strcpy(instruction, "???");
  return 2;
}
#endif

#if 0
static int twenty_bit_stack(struct _memory *memory, int address, char *instruction, uint16_t opcode)
{
char temp[8];
int n=((opcode>>4)&0xf)+1;
int is_push=(opcode&0x0200)==0?1:0;

  sprintf(instruction, "%s", (is_push==1)?"pushm":"popm");
  strcat(instruction, (opcode&0x0100)==0?".a ":".w ");
  sprintf(temp, "#%d, ", n);
  strcat(instruction, temp);
  if (is_push==1)
  {
    sprintf(temp, "r%d", (opcode&0xf));
  }
    else
  {
    //sprintf(temp, "r%d", (opcode&0xf)-n+1);
    sprintf(temp, "r%d", (opcode&0xf)-1+n);
  }
  strcat(instruction, temp);

  return 2;
}
#endif

int get_cycle_count(uint16_t opcode)
{
int src_reg,dst_reg;
int Ad,As;

  if ((opcode&0xfc00)==0x1000)
  {
    // One operand
    int o=(opcode&0x0380)>>7;
    As=(opcode&0x0030)>>4;
    src_reg=opcode&0x000f;

    if (((opcode&0x0040)!=0) && (o==1 || o==3 || o==5 || o==6)) { return -1; }

    if (src_reg==3 || (src_reg==2 && (As&2)==2)) { As=0; }

    if (o==6) { return 5; }    // RETI
    if (o==7) { return -1; }   // Illegal

    if (o==5) // CALL
    {
      if (As==1) return 5;        // x(Rn), EDE, &EDE
      if (As==2) return 4;        // @Rn
      if (As==3)
      {
        if (src_reg==0) return 5; // #value
        return 5;                 // @Rn+
      }

      return 4;                   // Rn
    }

    if (o==4) // PUSH
    {
      if (As==1) return 5;        // x(Rn), EDE, &EDE
      if (As==2) return 4;        // @Rn
      if (As==3)
      {
        if (src_reg==0) return 4; // #value
        return 5;                 // @Rn+
      }

      return 3;                   // Rn
    }

    // RRA, RRC, SWPB, SXT
    if (As==1) return 4;        // x(Rn), EDE, &EDE
    if (As==2) return 3;        // @Rn
    if (As==3)
    {
      if (src_reg==0) return -1; // #value
      return 3;                  // @Rn+
    }

    return 1;                   // Rn
  }
    else
  if ((opcode&0xe000)==0x2000)
  {
    // Jump
    return 2; 
  }
    else
//#ifdef SUPPORT_MSP430X
  if ((opcode&0x1400)==0x1400)
  {
    int mul=(((opcode&0x0100)>>8)^1)+1;
    int n=((opcode>>4)&0x000f)+1;
    return 2+(n*mul);
  }
    else
//#endif
  {
    // Two operand
    Ad=(opcode&0x0080)>>7;
    As=(opcode&0x0030)>>4;
    src_reg=(opcode>>8)&0x000f;
    dst_reg=opcode&0x000f;

    if (src_reg==3 || (src_reg==2 && (As&2)==2)) { As=0; }
    if (dst_reg==3) { Ad=0; }

    if ((opcode>>12)<4) return -1;

    // Cycle counts
    if (As==1) //Src EDE, &EDE, x(Rn)
    {
      if (Ad==1) return 6; // Dest x(Rn) and TONI and &TONI
      return 3;  // Dest
    }
      else
    if (As==3)  // #value and @Rn+
    {
      if (dst_reg==0) return 3;   // Dest PC
      if (Ad==0) return 2;        // Dest Rm
      return 5;                   // Dest x(Rm), EDE, &EDE
    }
      else
    if (As==2)  // @Rn
    {
      if (dst_reg==0) return 2;   // Dest PC
      if (Ad==0) return 2;        // Dest Rm
      return 5;                   // Dest x(Rm), EDE, &EDE
    }
      else     // Rn
    {
      if (dst_reg==0) return 2;   // Dest PC
      if (Ad==0) return 1;        // Dest Rm
      return 4;                   // Dest x(Rm), EDE, &EDE
    }
  }


  return -1;
}

static int disasm_msp430_a(struct _memory *memory, int address, char *instruction, int *cycles, int prefix)
{
uint16_t opcode;
int op;

printf("wtf?\n");

  instruction[0]=0;
  //opcode=(memory[address+1]<<8)|memory[address];
  opcode=(READ_RAM(address+1)<<8)|READ_RAM(address);
  *cycles=get_cycle_count(opcode);

  // Check single operand
  op=opcode>>7;
  if ((((op>>3)==0x04) && (op&0x07)<=5) || opcode==0x1300)
  {
    return one_operand(memory, address, instruction, opcode, prefix);
  }

  // Check conditional jmp
  if ((opcode>>13)==1)
  {
    return relative_jump(memory, address, instruction, opcode, prefix);
  }

  // Two operand instruction
  op=opcode>>12;
  if (op>=4 && op<=15)
  {
    //printf("two operand\n");
    return two_operand(memory, address, instruction, opcode, prefix);
  }

  if (prefix!=-1)
  {
    // We already have a 20 bit instruction in the works. This is wrong.
    strcpy(instruction, "???");
    return 2;
  }

  // 20 bit moves, adds, shifts, compares
  if ((opcode&0xf000)==0x0000)
  {
    return twenty_bit_zero(memory, address, instruction, opcode);
  }

  // 20 bit call and returns
  if ((opcode&0xff00)==0x1300)
  {
    return twenty_bit_call(memory, address, instruction, opcode);
  }

  // 20 bit push and pop
  if ((opcode&0xfc00)==0x1400)
  {
    return twenty_bit_stack(memory, address, instruction, opcode);
  }

  // 20 bit prefix to 16 bit instructions 
  if ((opcode&0xf800)==0x1800)
  {
    int count=disasm_msp430_a(memory, address+2, instruction, cycles, opcode);
    *cycles=-1;
    return count;
  }

  strcpy(instruction, "???");
  return 2;
}

#if 0
int disasm_msp430(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
  int count=disasm_msp430_a(memory, address, instruction, cycles_min, -1);
  *cycles_max=*cycles_min;

  return count;
}
#endif

int disasm_msp430(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
uint16_t opcode;
uint16_t prefix=-1;
char *prefix_str="";
int dst,src,num,wa;
int count=0;
int n;

  instruction[0]=0;
  opcode=READ_RAM16(address);
  // FIXME - this doesn't work for MSP430X
  *cycles_min=get_cycle_count(opcode);
  *cycles_max=*cycles_min;

  // 20 bit prefix to 16 bit instructions 
  if ((opcode&0xf800)==0x1800)
  {
    prefix=opcode;
    opcode=READ_RAM16(address+2);
    address+=2;
    count=2;
  }

  n=0;
  while(table_msp430[n].instr!=NULL)
  {
    if ((opcode&table_msp430[n].mask)==table_msp430[n].opcode)
    {
      switch(table_msp430[n].type)
      {
        case OP_NONE:
          strcpy(instruction, table_msp430[n].instr);
          break;
        case OP_ONE_OPERAND:
          strcpy(instruction, table_msp430[n].instr);
          count+=one_operand(memory, address, instruction, opcode, prefix);
          break;
        case OP_JUMP:
          strcpy(instruction, table_msp430[n].instr);
          count+=relative_jump(memory, address, instruction, opcode, prefix);
          break;
        case OP_TWO_OPERAND:
          strcpy(instruction, table_msp430[n].instr);
          count+=two_operand(memory, address, instruction, opcode, prefix);
          break;
        case OP_MOVA_AT_REG_REG:
          src=(opcode>>8)&0xf;
          dst=opcode&0xf;
          sprintf(instruction, "mova @%s, %s", regs[src], regs[dst]);
          return 2;
        case OP_MOVA_AT_REG_PLUS_REG:
          src=(opcode>>8)&0xf;
          dst=opcode&0xf;
          sprintf(instruction, "mova @%s+, %s", regs[src], regs[dst]);
          return 2;
        case OP_MOVA_ABS20_REG:
          num=(((opcode>>8)&0xf)<<16)|(READ_RAM16(address+2));
          dst=opcode&0xf;
          sprintf(instruction, "mova &0x%x, %s", num, regs[dst]);
          return 4;
        case OP_MOVA_INDIRECT_REG:
          num=READ_RAM16(address+2);
          src=(opcode>>8)&0xf;
          dst=opcode&0xf;
          sprintf(instruction, "mova 0x%x(%s), %s", num, regs[src], regs[dst]);
          return 4;
        case OP_SHIFT20:
          num=((opcode>>10)&0x3)+1;
          wa=(opcode>>4)&0x1;
          dst=opcode&0xf;
          *cycles_min=num; *cycles_max=num;
          sprintf(instruction, "%s.%c #%d, %s", table_msp430[n].instr, (wa==0)?'w':'a', num, regs[dst]);
          return 2;
        case OP_MOVA_REG_ABS:
          num=((opcode&0xf)<<16)|READ_RAM16(address+2);
          src=(opcode>>8)&0xf;
          sprintf(instruction, "mova %s, &0x%x", regs[src], num);
          return 4;
        case OP_MOVA_REG_INDIRECT:
          num=READ_RAM16(address+2);
          src=(opcode>>8)&0xf;
          dst=opcode&0xf;
          sprintf(instruction, "mova %s, 0x%x(%s)", regs[src], num, regs[dst]);
          return 4;
        case OP_IMMEDIATE_REG:
        case OP_REG_REG:
        case OP_CALLA_SOURCE:
        case OP_CALLA_ABS20:
        case OP_CALLA_INDIRECT_PC:
        case OP_CALLA_IMMEDIATE:
        case OP_PUSH:
        case OP_POP:
        default:
          sprintf(instruction, "%s << wtf", table_msp430[n].instr);
          break;
      }

      break;
    }

    n++;
  }

  if (table_msp430[n].instr==NULL) { strcpy(instruction, "???"); }

  return count;
}

void list_output_msp430(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max,count;
int num;
char instruction[128];

  fprintf(asm_context->list, "\n");
  count=disasm_msp430(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);
  num=memory_read(asm_context, address)|memory_read(asm_context, address+1)<<8;
  if (cycles_min<0)
  {
    fprintf(asm_context->list, "0x%04x: 0x%04x %-40s cycles: ?\n", address, num, instruction, cycles_min);
  }
    else
  {
    fprintf(asm_context->list, "0x%04x: 0x%04x %-40s cycles: %d\n", address, num, instruction, cycles_min);
  }
  count-=2;

  while(count>0)
  {
    address+=2;
    num=memory_read(asm_context, address)|memory_read(asm_context, address+1)<<8;
    fprintf(asm_context->list, "0x%04x: 0x%04x\n", address, num);
    count-=2;
  }
}

void disasm_range_msp430(struct _memory *memory, int start, int end)
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

    int count=disasm_msp430(memory, start, instruction, &cycles_min, &cycles_max);

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

    count-=2;
    while (count>0)
    {
      start=start+2;
      num=READ_RAM(start)|(READ_RAM(start+1)<<8);
      printf("0x%04x: 0x%04x\n", start, num);
      count-=2;
    }

    start=start+2;
  }
}


