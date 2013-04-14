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

#include "asm_common.h"
#include "asm_msp430.h"
#include "assembler.h"
#include "disasm_msp430.h"
#include "get_tokens.h"
#include "eval_expression.h"

enum
{
  OPTYPE_ERROR,
  OPTYPE_REGISTER,
  OPTYPE_INDEXED,
  OPTYPE_REGISTER_INDIRECT,
  OPTYPE_REGISTER_INDIRECT_INC,
  OPTYPE_SYMBOLIC,
  OPTYPE_IMMEDIATE,
  OPTYPE_ABSOLUTE,
};

struct _operand
{
  int reg;     // register
  int value;   // number value
  int type;    // OPTYPE
  int error;   // if expression can't be evaluated on pass 1
  int a;       // As or Ad
};

struct _data
{
  int data[2];
  int count;
};

#define CMD_SP_INC 0xdead0001
#define CMD_PC 0xdead0002
#define CMD_R3 0xdead0003
#define CMD_DST_DST 0xdead0004

static struct _aliases
{
  const char *instr;
  int operand_count;
  int opcode;
  const char *alt;
  int cmd;
} aliases[] =
{
  { "adc", 1, 0, "addc", 0 },
  { "br", 1, 0, "mov", CMD_PC },
  { "clrc", 0, 0xc312, NULL, 0 },
  { "clrn", 0, 0xc222, NULL, 0 },
  { "clrz", 0, 0xc322, NULL, 0 },
  { "dadc", 1, 0, "dadd", 0 },
  { "dec", 1, 0, "sub", 1 },
  { "decd", 1, 0, "sub", 2 },
  { "dint", 0, 0xc232, NULL, 0 },
  { "eint", 0, 0xd232, NULL, 0 },
  { "inc", 1, 0, "add", 1 },
  { "incd", 1, 0, "add", 2 },
  { "inv", 1, 0, "xor", -1 },
  { "nop", 0, 0x4303, NULL, CMD_R3 },
  { "pop", 1, 0, "mov", CMD_SP_INC },
  { "ret", 0, 0x4130, NULL, 0 },
  { "rla", 1, 0, "add", CMD_DST_DST },
  { "rlc", 1, 0, "addc", CMD_DST_DST },
  { "sbc", 1, 0, "subc", 0 },
  { "setc", 0, 0xd312, NULL, 0 },
  { "setn", 0, 0xd222, NULL, 0 },
  { "setz", 0, 0xd322, NULL, 0 },
  { "tst", 1, 0, "cmp", 0 },
  { "clr", 1, 0, "mov", 0 },
  { NULL, 0, 0, NULL, 0 },
};

static char *one_oper[] = { "rrc", "swpb", "rra", "sxt", "push", "call", NULL };
static char *jumps[] = { "jne", "jeq", "jlo", "jhs", "jn", "jge", "jl", "jmp", NULL };
static char *jumps_a[] = { "jnz", "jz", "jnc", "jc", NULL, NULL, NULL, NULL, NULL };
static char *two_oper[] = { "mov", "add", "addc", "subc", "sub", "cmp", "dadd", "bit",
                     "bic", "bis", "xor", "and", NULL };
static char *ms430x_ext[] = { "rrcx", "swpbx", "rrax", "sxtx", "pushx", "movx", "addx",
                    "addcx", "subcx", "subx", "cmpx", "daddx", "bitx", "bicx",
                    "bisx", "xorx", "andx", NULL };
static char *msp430x_shift[] = { "rrcm", "rram", "rlam", "rrum", NULL };
static char *msp430x_stack[] = { "pushm", "popm", NULL };
static char *msp430x_alu[] = { "mova", "cmpa", "adda", "suba", NULL };
static char *msp430x_rpt[] = { "rpt", "rptc", "rptz",  NULL };

static void print_operand_error(const char *s, int count, struct _asm_context *asm_context)
{
  printf("Error: Instruction '%s' takes %d operand%s at %s:%d\n", s, count, count==1?"":"s", asm_context->filename, asm_context->line);
}

static int process_operand(struct _asm_context *asm_context, struct _operand *operand, struct _data *data, int bw, int al, int is_dest)
{
  if (operand->type==OPTYPE_IMMEDIATE)
  {
    if (bw==1 && al==1)
    {
      if (operand->value>0xff || operand->value<-128)
      {
        print_error("Constant larger than 8 bit.", asm_context);
        return -1;
      }

      operand->value=(((unsigned short int)operand->value)&0xff);
    }
      else
    if (bw==0 && al==1)
    {
      if (operand->value>0xffff || operand->value<-32768)
      {
        print_error("Constant larger than 16 bit.", asm_context);
        return -1;
      }

      operand->value=(((unsigned int)operand->value)&0xffff);
    }
      else
    if (bw==1 && al==0)
    {
      if (operand->value>0xfffff || operand->value<-524288)
      {
        print_error("Constant larger than 20 bit.", asm_context);
        return -1;
      }

      operand->value=(((unsigned int)operand->value)&0xffff);
    }
  }
    else
  if (operand->type>=OPTYPE_ABSOLUTE ||
      operand->type>=OPTYPE_INDEXED ||
      operand->type>=OPTYPE_SYMBOLIC)
  {
    if (operand->value>0xffff || operand->value<-32768)
    {
      print_error("Constant larger than 16 bit.", asm_context);
      return -1;
    }

    operand->value=(((unsigned int)operand->value)&0xffff);
  }

  switch(operand->type)
  {
    case OPTYPE_REGISTER:
      operand->a=0;
      break;
    case OPTYPE_INDEXED:
      operand->a=1;
      data->data[data->count++]=operand->value;
      break;
    case OPTYPE_REGISTER_INDIRECT:
      if (is_dest==1)
      {
        if (asm_context->pass==2)
        {
          printf("Warning: Addressing mode of @r%d being changed to 0(r%d) at %s:%d.\n", operand->reg, operand->reg, asm_context->filename, asm_context->line);
        }

        operand->a=1;
        operand->value=0;
      }
        else
      {
        operand->a=2;
      }
      break;
    case OPTYPE_REGISTER_INDIRECT_INC:
      if (is_dest==1)
      {
        printf("Error: Indirect autoincrement not allowed for dest operand at %s:%d.\n", asm_context->filename, asm_context->line);
        return -1;
      }
      operand->a=3;
      break;
    case OPTYPE_SYMBOLIC:
      operand->a=1;
      operand->reg=0;
      data->data[data->count]=(operand->value-(asm_context->address+(data->count*2+2)))&0xffff;
      data->count++;
      break;
    case OPTYPE_IMMEDIATE:
      if (memory_read(asm_context, asm_context->address)!=0 ||
          operand->error!=0)
      {
        operand->a=3;
        operand->reg=0;
        data->data[data->count++]=operand->value;
      }
        else
      if (operand->value==0) { operand->reg=3; operand->a=0; }
      else if (operand->value==1) { operand->reg=3; operand->a=1; }
      else if (operand->value==2) { operand->reg=3; operand->a=2; }
      else if (operand->value==4) { operand->reg=2; operand->a=2; }
      else if (operand->value==8) { operand->reg=2; operand->a=3; }
        else
      if (operand->value==0xff && bw==1)
      {
        operand->a=3;
        operand->reg=3;
      }
        else
      if (operand->value==0xffff && bw==0)
      {
        operand->a=3;
        operand->reg=3;
      }
        else
      {
        operand->a=3;
        operand->reg=0;
        data->data[data->count++]=operand->value;
      }
      break;
    case OPTYPE_ABSOLUTE:
      operand->a=1;
      operand->reg=2;
      data->data[data->count++]=operand->value;
      break;
    default:
      printf("Internal Error: Unknown optype\n");
      break;
  }

  return 0;
}

static int add_instruction(struct _asm_context *asm_context, struct _data *data, int error, int opcode)
{
int n;

  //asm_context->instruction_count++;

  if (asm_context->pass==1)
  {
    if (error==1) { add_bin(asm_context, 1, IS_DATA); }
    else { add_bin(asm_context, 0, IS_DATA); }
  }
    else
  {
    add_bin(asm_context, opcode, IS_OPCODE);
  }

  for (n=0; n<data->count; n++)
  {
    add_bin(asm_context, data->data[n], IS_DATA);
  }

  return 0;
}

#if 0
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
#endif

static int get_prefix(struct _asm_context *asm_context, int zc)
{
char token[TOKENLEN];
//int token_type;
int num;

  get_token(asm_context, token, TOKENLEN);
  if (IS_TOKEN(token,'#'))
  {
    if (eval_expression(asm_context, &num)!=0)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (num<1 || num>16)
    {
      print_error("Constant can only be between 1 and 16", asm_context);
      return -1;
    }
    return 0x1800|(zc<<8)|num;
  }
    else
  {
    num=get_register_msp430(token);
    if (num>=0)
    {
      return 0x1880|(zc<<8)|num;
    }
  }

  print_error("expecting register or immediate", asm_context);
  return -1;
}

int parse_instruction_msp430(struct _asm_context *asm_context, char *instr)
{
struct _operand operands[3];
struct _data data;
int operand_count=0;
char token[TOKENLEN];
char instr_lower_mem[TOKENLEN];
char *instr_lower;
int token_type;
int size=0;
int num,n;
int bw=0,al=1;
int opcode;
int msp430x=0;
int prefix=0;

  lower_copy(instr_lower_mem, instr);
  instr_lower=instr_lower_mem;

  // Not sure if this is a good area for this.  If there isn't an instruction
  // here then it pads for no reason.
  if ((asm_context->address&0x01)!=0)
  {
    if (asm_context->pass==2)
    {
      printf("Warning: Instruction doesn't start on 16 bit boundary at %s:%d.  Padding with a 0.\n", asm_context->filename, asm_context->line);
    }
    memory_write_inc(asm_context, 0, DL_NO_CG);
  }

  // check for RPT prefix
  n=0;
  while(msp430x_rpt[n]!=NULL)
  {
    if (strcmp(instr_lower,msp430x_rpt[n])==0)
    {
      prefix=get_prefix(asm_context, n&1);
      if (prefix==-1) return -1;
      token_type=get_token(asm_context, instr, TOKENLEN);
      if (token_type!=TOKEN_EOL)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      asm_context->line++;
      while(1)
      {
        token_type=get_token(asm_context, instr, TOKENLEN);
        if (token_type==TOKEN_EOL) break;
        if (token_type==TOKEN_EOF)
        {
          print_error("Unexpected end of file", asm_context);
          return -1;
        }
      }

      lower_copy(instr_lower_mem, instr);
      msp430x=1;
      break;
    }

    n++;
  }

  memset(&operands, 0, sizeof(operands));
  data.count=0;

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }

    if (operand_count==3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    operands[operand_count].reg=-1;
    num=0;

    if (operand_count==0)
    {
      if (IS_TOKEN(token,'.'))
      {
        token_type=get_token(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token,'b') || IS_TOKEN(token,'B')) { size=8; }
        else if (IS_TOKEN(token,'w') || IS_TOKEN(token,'W')) { size=16; }
        else if (IS_TOKEN(token,'a') || IS_TOKEN(token,'A')) { size=20; }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        token_type=get_token(asm_context, token, TOKENLEN);
        if (token_type<0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
    }

    if (IS_TOKEN(token,'#'))
    {
      operands[operand_count].type=OPTYPE_IMMEDIATE;

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
          operands[operand_count].error=1;
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
      operands[operand_count].value=num;
    }
      else
    if (IS_TOKEN(token,'@'))
    {
      operands[operand_count].type=OPTYPE_REGISTER_INDIRECT;
      token_type=get_token(asm_context, token, TOKENLEN);
      num=get_register_msp430(token);

      if (num<0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].reg=num;

      token_type=get_token(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,'+'))
      {
        operands[operand_count].type=OPTYPE_REGISTER_INDIRECT_INC;
      }
        else
      {
        pushback(asm_context, token, token_type);
      }
    }
      else
    if (IS_TOKEN(token,'&'))
    {
      operands[operand_count].type=OPTYPE_ABSOLUTE;

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
          operands[operand_count].error=1;
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }

      operands[operand_count].value=num;
    }
      else
    {
      num=get_register_msp430(token);
      if (num>=0)
      {
        operands[operand_count].type=OPTYPE_REGISTER;
        operands[operand_count].reg=num;
      }
        else
      {
        operands[operand_count].type=OPTYPE_SYMBOLIC;

        if (asm_context->pass==1)
        {
          // In pass 1 it will always be 2 words long, so who cares
          eat_operand(asm_context);
        }
          else
        {
          pushback(asm_context, token, token_type);
          if (eval_expression(asm_context, &num)!=0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].value=num;

          token_type=get_token(asm_context, token, TOKENLEN);
          if (IS_TOKEN(token,'('))
          {
            operands[operand_count].type=OPTYPE_INDEXED;

            token_type=get_token(asm_context, token, TOKENLEN);
            {
              int reg=get_register_msp430(token);

              if (reg<0)
              {
                print_error_unexp(token, asm_context);
                return -1;
              }

              operands[operand_count].reg=reg;
            }

            token_type=get_token(asm_context, token, TOKENLEN);
            if (IS_NOT_TOKEN(token,')'))
            {
              print_error_unexp(token, asm_context);
            }
          }
            else
          {
            pushback(asm_context, token, token_type);
          }
        }
      }
    }

    operands[operand_count++].value=num;

    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
  printf("-------- instr=%s bw=%d  al=%d\n", instr, bw, al);
  for (n=0; n<operand_count; n++)
  {
    printf("operand %d: value=%d type=%d error=%d\n", n, operands[n].value, operands[n].type, operands[n].error);
  }
#endif

  // Do aliases first
  n=0;
  while(aliases[n].instr!=NULL)
  {
    if (strcmp(instr_lower,aliases[n].instr)==0)
    {
      if (aliases[n].operand_count!=operand_count)
      {
        print_operand_error(instr, aliases[n].operand_count, asm_context);
        return -1;
      }

      if (aliases[n].operand_count==0)
      {
        add_bin(asm_context, aliases[n].opcode, IS_OPCODE);
        return 0;
      }

      switch(aliases[n].cmd)
      {
        case CMD_SP_INC:
          memcpy(&operands[1], &operands[0], sizeof(struct _operand));
          memset(&operands[0], 0, sizeof(struct _operand));
          operands[0].reg=1;
          operands[0].type=OPTYPE_REGISTER_INDIRECT_INC;
          break;
        case CMD_PC:
          operands[1].reg=0;
          operands[1].type=OPTYPE_REGISTER;
          break;
        case CMD_R3:
          memset(&operands[0], 0, sizeof(struct _operand)*2);
          operands[0].value=0;
          operands[0].type=OPTYPE_IMMEDIATE;
          operands[1].reg=3;
          operands[1].type=OPTYPE_REGISTER;
          break;
        case CMD_DST_DST:
          memcpy(&operands[1], &operands[0], sizeof(struct _operand));
          break;
        default:
          memcpy(&operands[1], &operands[0], sizeof(struct _operand));
          memset(&operands[0], 0, sizeof(struct _operand));
          operands[0].value=aliases[n].cmd;
          operands[0].type=OPTYPE_IMMEDIATE;
          break;
      }

      operand_count=2;
      instr_lower=(char *)aliases[n].alt;
      break;
    }

    n++;
  }

  // Check for MSP430X version of MSP430 instruction
  n=0;
  while(ms430x_ext[n]!=NULL)
  {
    if (strcmp(instr_lower,ms430x_ext[n])==0)
    {
      msp430x=1;
      instr_lower[strlen(instr_lower)-1]=0;
      int src19_16=0;
      int dst19_16=0;

      if (operand_count>0)
      {
        src19_16=(((unsigned int)operands[0].value)&0xf0000)>>16;
      }

      if (operand_count>1)
      {
        dst19_16=(((unsigned int)operands[1].value)&0xf0000)>>16;
      }

      if (prefix==0)
      {
        add_bin(asm_context, 0x1800|(src19_16<<7)|(al<<6)|(dst19_16), IS_OPCODE);
      }
        else
      {
        if (src19_16!=0 || dst19_16!=0)
        {
          print_error("Constants out of 16 bit range with RPT", asm_context);
          return -1;
        }
        add_bin(asm_context, prefix|(al<<6), IS_OPCODE);
      }
      break;
    }

    n++;
  }

  // Set bw and al
  if (size==8) { bw=1; al=1; }
  else if (size==16) { bw=0; al=1; }
  else if (size==20) { bw=1; al=0; }
  else
  {
    if (msp430x==0) { bw=0; al=0; }
    else { bw=1; al=0; }
  }

  if (ms430x_ext[n]!=NULL && prefix!=0)
  {
    print_error("instruction doesn't support RPT", asm_context);
    return -1;
  }

  if (msp430x==0 && size>16)
  {
    print_error("instruction doesn't support .a", asm_context);
    return -1;
  }

  // One operand instructions
  n=0;
  while(one_oper[n]!=NULL)
  {
    if (strcmp(instr_lower,one_oper[n])==0)
    {
      if ((n%1)==1 && size==8)
      {
        printf("Error: Instruction '%s' can't be used with .b at %s:%d\n", instr, asm_context->filename, asm_context->line);
        return -1;
      }

      if (operand_count!=1)
      {
        print_operand_error(instr, 1, asm_context);
        return -1;
      }

      if (process_operand(asm_context, &operands[0], &data, bw, al, 0)<0)
      {
        return -1;
      }

      opcode=0x1000|(n<<7)|(bw<<6)|(operands[0].a<<4)|operands[0].reg;
      add_instruction(asm_context, &data, operands[0].error, opcode);

      return 0;
    }

    n++;
  }

  // Jumps
  n=0;
  while(jumps[n]!=NULL)
  {
    int offset;

    if (strcmp(instr_lower,jumps[n])==0 || (jumps_a[n]!=NULL && strcmp(instr_lower,jumps_a[n])==0))
    {
      if (operand_count!=1)
      {
        print_operand_error(instr, 1, asm_context);
        return -1;
      }

      if (asm_context->pass==1)
      {
        offset=asm_context->address;
      }
        else
      {
        if (operands[0].type!=OPTYPE_SYMBOLIC)
        {
          print_error("Expecting a branch address", asm_context);
          return -1;
        }

        offset=operands[0].value;
      }

      if ((offset&1)==1)
      {
        print_error("Jump offset is odd", asm_context);
        return -1;
      }

      offset=(offset-(asm_context->address+2))/2;

      if (offset>511)
      {
        printf("Error: Jump offset off by %d at %s:%d.\n", offset-511, asm_context->filename, asm_context->line);
        return -1;
      }

      if (offset<-512)
      {
        printf("Error: Jump offset off by %d at %s:%d.\n", (-offset)-512, asm_context->filename, asm_context->line);
        return -1;
      }

      opcode=0x2000|(n<<10);
      opcode|=((unsigned int)offset)&0x03ff;
      add_instruction(asm_context, &data, 0, opcode);

      return 0;
    }

    n++;
  }

  // Two operand instructions
  n=0;
  while(two_oper[n]!=NULL)
  {
    if (strcmp(instr_lower,two_oper[n])==0)
    {
      if (operand_count!=2)
      {
        print_operand_error(instr, 2, asm_context);
        return -1;
      }

      if (process_operand(asm_context, &operands[0], &data, bw, al, 0)<0)
      {
        return -1;
      }

      if (process_operand(asm_context, &operands[1], &data, bw, al, 1)<0)
      {
        return -1;
      }

      opcode=((n+4)<<12)|(operands[0].reg<<8)|(operands[1].a<<7)|(bw<<6)|
             (operands[0].a<<4)|(operands[1].reg);
      add_instruction(asm_context, &data, operands[0].error, opcode);

      return 0;
    }

    n++;
  }

  // Do reti
  if (strcmp(instr_lower, "reti")==0)
  {
    if (size!=0)
    {
      print_error("reti doesn't take a size", asm_context);
      return -1;
    }

    if (operand_count!=0)
    {
      print_operand_error(instr, 0, asm_context);
      return -1;
    }

    add_bin(asm_context, 0x1300, IS_OPCODE);
    return 0;
  }

  // MSP430X CALLA
  if (strcmp(instr_lower,"calla")==0)
  {
    if (operand_count!=1)
    {
      print_error("calla takes exactly one operand", asm_context);
      return -1;
    }

    if (size!=0)
    {
      print_error("calla doesn't take a size flag", asm_context);
      return -1;
    }

    int mode=0;
    int op=2;
    int value=operands[0].value;
    if (value>0xfffff || value<-524288)
    {
      print_error("Constant larger than 20 bit.", asm_context);
      return -1;
    }

    switch(operands[0].type)
    {
      case OPTYPE_ABSOLUTE:
        break;
      case OPTYPE_INDEXED:
        if (operands[0].reg!=0) { op=0; }
        else { mode=1; }
        break;
      case OPTYPE_IMMEDIATE:
        mode=3;
        break;
      case OPTYPE_REGISTER:
      case OPTYPE_REGISTER_INDIRECT:
      case OPTYPE_REGISTER_INDIRECT_INC:
        op=1;
      default:
        op=0;
        break;
    }

    opcode=0x1300|(op<<6);;

    if (op==0)
    {
      opcode|=(operands[0].a<<4)|(operands[0].reg);
      add_bin(asm_context, opcode, IS_OPCODE);
    }
      else
    if (op==1)
    {
      opcode|=(mode<<4)|((((unsigned int)value)&0xf0000)>>16);
      add_bin(asm_context, opcode, IS_OPCODE);
      add_bin(asm_context, ((unsigned int)value)&0xffff, IS_DATA);
    }
      else
    {
      print_error("Unknown addressing mode for calla", asm_context);
      return -1;
    }

    return 0;
  }

  // MSP430X SHIFT
  n=0;
  while(msp430x_shift[n]!=NULL)
  {
    if (strcmp(instr_lower,msp430x_shift[n])==0)
    {
      if (operand_count!=2 || operands[0].type!=OPTYPE_IMMEDIATE ||
          operands[1].type!=OPTYPE_REGISTER)
      {
        print_error("rotate expects an immediate and register", asm_context);
        return -1;
      }

      if (operands[0].value<1 || operands[0].value>16)
      {
        print_error("rotate can only shift between 1 and 4", asm_context);
        return -1;
      }

      if (al!=0 || bw!=1)
      {
        print_error("msp430x rotate can only be 16 or 20 bit", asm_context);
        return -1;
      }

      opcode=((operands[0].value-1)<<10)|(n<<8)|(1<<6)|(al<<4)|operands[1].value;
      add_bin(asm_context, opcode, IS_OPCODE);
      return 0;
    }

    n++;
  }

  // MSP430X ALU
  n=0;
  while(msp430x_alu[n]!=NULL)
  {
    if (strcmp(instr_lower,msp430x_alu[n])==0)
    {
      if (operand_count!=2)
      {
        print_error("Instruction takes exactly two operands", asm_context);
        return -1;
      }

      if (size!=0)
      {
        print_error("instruction doesn't take a size", asm_context);
        return -1;
      }

      int value=operands[0].value;
      if (value>0xfffff || value<-524288)
      {
        print_error("Constant larger than 20 bit.", asm_context);
        return -1;
      }

      if (operands[0].type==OPTYPE_IMMEDIATE &&
          operands[1].type==OPTYPE_REGISTER)
      {
        opcode=((((unsigned int)value)&0xf0000)>>8)|(2<<6)|(n<<4)|operands[1].reg;
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, ((unsigned int)value)&0xffff, IS_DATA);
      }
        else
      if (operands[0].type==OPTYPE_REGISTER &&
          operands[1].type==OPTYPE_REGISTER)
      {
        opcode=(operands[0].reg<<8)|(3<<6)|(n<<4)|operands[1].reg;
        add_bin(asm_context, opcode, IS_OPCODE);
      }
        else
      if (n!=0)
      {
        if (n==0) { break; }  // this is a MOVA
        print_error("Unknown addressing mode", asm_context);
        return -1;
      }

      return 0;
    }

    n++;
  }

  // MSP430X PUSH AND POP
  n=0;
  while(msp430x_stack[n]!=NULL)
  {
    if (strcmp(instr_lower,msp430x_stack[n])==0)
    {
      if (operand_count!=2 || operands[0].type!=OPTYPE_IMMEDIATE ||
          operands[1].type!=OPTYPE_REGISTER)
      {
        print_error("Excpecting an immediate and register", asm_context);
        return -1;
      }

      if (operands[0].value<1 || operands[0].value>16)
      {
        print_error("Constant can only be between 1 and 16", asm_context);
        return -1;
      }

      int reg;
      if (n==0) { reg=operands[1].reg; }
      else if (n==1) { reg=(operands[1].reg-n+1)&0x0f; }
      else { printf("Internal error at %s:%d\n", __FILE__, __LINE__); return -1; }

      opcode=0x1400|(n<<9)|(al<<8)|(operands[0].value<<4)|reg;
      add_bin(asm_context, opcode, IS_OPCODE);
      return 0;
    }

    n++;
  }

  // MSP430X MOVA
  if (strcmp(instr_lower,"mova")==0)
  {
    if (operand_count!=2)
    {
      print_error("Instruction takes exactly two operands", asm_context);
      return -1;
    }

    if (size!=0)
    {
      print_error("mova doesn't take a size", asm_context);
      return -1;
    }

    if (operands[1].type==OPTYPE_REGISTER)
    {
      opcode=operands[1].reg;

      if (operands[0].type==OPTYPE_REGISTER_INDIRECT)
      {
        opcode|=(operands[0].reg<<8);
        add_bin(asm_context, opcode, IS_OPCODE);
        return 0;
      }
      if (operands[0].type==OPTYPE_REGISTER_INDIRECT_INC)
      {
        opcode|=(1<<4)|(operands[0].reg<<8);
        add_bin(asm_context, opcode, IS_OPCODE);
        return 0;
      }
      if (operands[0].type==OPTYPE_ABSOLUTE)
      {
        if (operands[0].value>0xfffff || operands[0].value<-524288)
        {
          print_error("Constant larger than 20 bit.", asm_context);
          return -1;
        }

        opcode|=(2<<4)|(((unsigned int)operands[0].value&0xf0000)>>16);
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, operands[0].value&0xffff, IS_DATA);
      }
      if (operands[0].type==OPTYPE_INDEXED)
      {
        if (operands[0].value>0xffff || operands[0].value<-32768)
        {
          print_error("Constant larger than 16 bit.", asm_context);
          return -1;
        }

        opcode|=(3<<4)|(operands[0].reg<<8);
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, operands[0].value&0xffff, IS_DATA);
        return 0;
      }
    }
      else
    if (operands[0].type==OPTYPE_REGISTER)
    {
      opcode=0x0040|(operands[0].reg<<8);

      if (operands[1].type==OPTYPE_ABSOLUTE)
      {
        if (operands[1].value>0xfffff || operands[1].value<-524288)
        {
          print_error("Constant larger than 20 bit.", asm_context);
          return -1;
        }
        opcode|=0x0020|((operands[1].value&0xf0000)>>16);
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, operands[1].value&0xffff, IS_DATA);
        return 0;
      }

      if (operands[1].type==OPTYPE_INDEXED)
      {
        if (operands[1].value>0xffff || operands[1].value<-32768)
        {
          print_error("Constant larger than 16 bit.", asm_context);
          return -1;
        }
        opcode|=0x0030|operands[1].reg;
        add_bin(asm_context, opcode, IS_OPCODE);
        add_bin(asm_context, operands[1].value&0xffff, IS_DATA);
        return 0;
      }
    }

    print_error("Unknown addressing mode for mova", asm_context);
    return -1;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}


