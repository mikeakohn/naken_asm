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
#include <ctype.h>

#include "asm_common.h"
#include "asm_dspic.h"
#include "assembler.h"
#include "disasm_dspic.h"
#include "get_tokens.h"
#include "eval_expression.h"
#include "table_dspic.h"

enum
{
  OPTYPE_ERROR,
  OPTYPE_REGISTER,
  OPTYPE_WREG,
  OPTYPE_LIT,
  OPTYPE_NUM,
  //OPTYPE_INDEX,
  //OPTYPE_INDEX_PRE_INC,
  //OPTYPE_INDEX_PRE_DEC,
  //OPTYPE_INDEX_POST_INC,
  //OPTYPE_INDEX_POST_DEC,
  OPTYPE_ACCUM,
  OPTYPE_W_PLUS_LIT,
  //OPTYPE_W_PLUS_W,   <-- hmmm
  OPTYPE_W_OP_EQ_NUM,
  //OPTYPE_W_MINUS_EQ_KX,
  //OPTYPE_W_PLUS_EQ_KX,
  //OPTYPE_W_MINUS_EQ_KY,
  //OPTYPE_W_PLUS_EQ_KY,
  OPTYPE_W_MUL_W,
};

int is_condition(char *token)
{
char *cond[] = {
  "c", "ge", "geu", "gt", "gtu", "le",
  "leu", "lt", "ltu", "n", "nc", "nn",
  "nov", "nz", "oa", "ob", "ov", "sa",
  "sb", "z", NULL };
int n;

  n=0;
  while(cond[n]!=NULL)
  {
    if (strcasecmp(cond[n], token)==0) return 1;
    n++;
  }

  return 0;
}

#define REG_NORMAL 0
#define REG_INDIRECT 1
#define REG_INDIRECT_POST_INC 2
#define REG_INDIRECT_POST_DEC 3
#define REG_INDIRECT_PRE_INC 4
#define REG_INDIRECT_PRE_DEC 5
#define REG_INDIRECT_W_PLUS_W 6

struct _operand
{
  int value;
  int type;
  int attribute;
  int reg2;
};

/*
struct _instruction
{
  char *name;
  int opcode;
  int operand_count;
};
*/

static void add_bin_dspic(struct _asm_context *asm_context, unsigned int b, int flags)
{
int line=asm_context->line;
int address=asm_context->address*2; 

  memory_write(asm_context, address++, b&0xff, line);
  memory_write(asm_context, address++, (b>>8)&0xff, line);
  memory_write(asm_context, address++, (b>>16)&0xff, line);
  memory_write(asm_context, address++, (b>>24)&0xff, line);

  asm_context->address+=2; 
}

static int get_register_dspic(char *token)
{
  if (token[0]=='w' || token[0]=='W')
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

  if (strcasecmp(token, "wreg")==0) return 0;
  if (strcasecmp(token, "sp")==0) return 15;

  return -1;
}

// generated with
// cat dspic_asm.c | grep BRA | grep define | awk '{ print "  else if (strcasecmp(token, \""$2"\")==0) { return "$2"; }" }'
#if 0
int get_condition(char *token)
{
char s[TOKENLEN];

  lower_copy(s, token);

  if (strcmp(s, "BRA_C")==0) { return BRA_C; }
  else if (strcmp(s, "BRA_GE")==0) { return BRA_GE; }
  else if (strcmp(s, "BRA_GEU")==0) { return BRA_GEU; }
  else if (strcmp(s, "BRA_GT")==0) { return BRA_GT; }
  else if (strcmp(s, "BRA_GTU")==0) { return BRA_GTU; }
  else if (strcmp(s, "BRA_LE")==0) { return BRA_LE; }
  else if (strcmp(s, "BRA_LEU")==0) { return BRA_LEU; }
  else if (strcmp(s, "BRA_LT")==0) { return BRA_LT; }
  else if (strcmp(s, "BRA_LTU")==0) { return BRA_LTU; }
  else if (strcmp(s, "BRA_N")==0) { return BRA_N; }
  else if (strcmp(s, "BRA_NC")==0) { return BRA_NC; }
  else if (strcmp(s, "BRA_NN")==0) { return BRA_NN; }
  else if (strcmp(s, "BRA_NOV")==0) { return BRA_NOV; }
  else if (strcmp(s, "BRA_NZ")==0) { return BRA_NZ; }
  else if (strcmp(s, "BRA_OA")==0) { return BRA_OA; }
  else if (strcmp(s, "BRA_OB")==0) { return BRA_OB; }
  else if (strcmp(s, "BRA_OV")==0) { return BRA_OV; }
  else if (strcmp(s, "BRA_SA")==0) { return BRA_SA; }
  else if (strcmp(s, "BRA_SB")==0) { return BRA_SB; }
  else if (strcmp(s, "BRA_Z")==0) { return BRA_Z; }

  return BRA_UNCOND;
}
#endif

static int check_range(int bitlen, int num)
{
  int low=-(1<<(bitlen-1));
  int high=(1<<bitlen)-1;
  if (num>high || num<low) { return -1; }
  return 0;
}

int parse_instruction_dspic(struct _asm_context *asm_context, char *instr)
{
struct _operand operands[5];
int operand_count=0;
char token[TOKENLEN];
char instr_case[TOKENLEN];
int flag=0;
int token_type;
int matched,wrong_op;
int range_error=-1;
int opcode=0;
int extra24=-1;
int num;
int n;

  memset(&operands, 0, sizeof(operands));
  lower_copy(instr_case, instr);

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }

    if (operand_count==3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    num=0;

    if (operand_count==0)
    {
      if (strcasecmp(instr_case, "bra")==0)
      {
        if (is_condition(token)==1)
        {
          char *s=token;
          while(*s!=0) { *s=tolower(*s); s++; }
          strcat(instr_case, " ");
          strcat(instr_case, token);
          token_type=get_token(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token,','))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          token_type=get_token(asm_context, token, TOKENLEN);
        }
#if 0
        operands[operand_count].attribute=get_condition(token);
        if (operands[operand_count].attribute!=BRA_UNCOND)
        {
          token_type=get_token(asm_context, token, TOKENLEN);
          if (token_type<=0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
#endif
      }
        else
      if (IS_TOKEN(token,'.'))
      {
        token_type=get_token(asm_context, token, TOKENLEN);
        char s[TOKENLEN];
        lower_copy(s, token);

        if (IS_TOKEN(s,'b')) { flag=FLAG_B; }
        else if (IS_TOKEN(s,'w')) { flag=FLAG_W; }
        else if (IS_TOKEN(s,'c')) { flag=FLAG_C; }
        else if (IS_TOKEN(s,'z')) { flag=FLAG_Z; }
        else if (IS_TOKEN(s,'n')) { flag=FLAG_N; }
        else if (IS_TOKEN(s,'d')) { flag=FLAG_D; }
        else if (IS_TOKEN(s,'u')) { flag=FLAG_U; }
        else if (IS_TOKEN(s,'s')) { flag=FLAG_S; }
        else if (IS_TOKEN(s,'r')) { flag=FLAG_R; }
        else if (strcmp(s,"sw")==0) { flag=FLAG_SW; }
        else if (strcmp(s,"sd")==0) { flag=FLAG_SD; }
        else if (strcmp(s,"uw")==0) { flag=FLAG_UW; }
        else if (strcmp(s,"ud")==0) { flag=FLAG_UD; }
        else if (strcmp(s,"ss")==0) { flag=FLAG_SS; }
        else if (strcmp(s,"su")==0) { flag=FLAG_SU; }
        else if (strcmp(s,"us")==0) { flag=FLAG_US; }
        else if (strcmp(s,"uu")==0) { flag=FLAG_UU; }
        else
        {
          return -1;
          print_error_unexp(token, asm_context);
        }

        token_type=get_token(asm_context, token, TOKENLEN);
        if (token_type<0) return -1;
      }
    }

    if (IS_TOKEN(token,'a') || IS_TOKEN(token,'A'))
    {
      operands[operand_count].type=OPTYPE_ACCUM;
      num=0;
    }
      else
    if (IS_TOKEN(token,'b') || IS_TOKEN(token,'B'))
    {
      operands[operand_count].type=OPTYPE_ACCUM;
      num=1;
    }
      else
    if (IS_TOKEN(token,'#'))
    {
      operands[operand_count].type=OPTYPE_LIT;

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          eat_operand(asm_context);
        }
          else
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
      }
    }
      else
    if (IS_TOKEN(token,'['))
    {
      operands[operand_count].type=OPTYPE_REGISTER;

      if (IS_TOKEN(token,'+'))
      {
        token_type=get_token(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token,'+'))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].attribute=REG_INDIRECT_PRE_INC;
      }
        else
      if (IS_TOKEN(token,'-'))
      {
        token_type=get_token(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token,'-'))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
        operands[operand_count].attribute=REG_INDIRECT_PRE_DEC;
      }

      token_type=get_token(asm_context, token, TOKENLEN);
      if (token_type<0) { print_error_unexp(token, asm_context); return -1; }

      num=get_register_dspic(token);
      if (num<0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      token_type=get_token(asm_context, token, TOKENLEN);
      if (operands[operand_count].type==OPTYPE_REGISTER)
      {
        if (IS_TOKEN(token,'+'))
        {
          token_type=get_token(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token,'+'))
          {
            // Check for: [W + #] and [W + W]
            if (IS_TOKEN(token,'#'))
            {
              int a;
              operands[operand_count].type=OPTYPE_W_PLUS_LIT;
              if (eval_expression(asm_context, &a)!=0)
              {
                print_error_unexp(token, asm_context);
                return -1;
              }
              operands[operand_count].attribute=a;
            }
              else
            {
              operands[operand_count].reg2=get_register_dspic(token);
              if (operands[operand_count].reg2==-1)
              {
                print_error_unexp(token, asm_context);
                return -1;
              }
              operands[operand_count].attribute=REG_INDIRECT_W_PLUS_W;
            }
          }
            else
          {
            operands[operand_count].attribute=REG_INDIRECT_POST_INC;
          }
        }
          else
        if (IS_TOKEN(token,'-'))
        {
          token_type=get_token(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token,'-')) { return -1; }
          operands[operand_count].attribute=REG_INDIRECT_POST_DEC;
        }
      }

      if (IS_NOT_TOKEN(token,']'))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,'-') || IS_TOKEN(token,'+'))
      {
        operands[operand_count].type=OPTYPE_W_OP_EQ_NUM;
        int a=0;
        if (IS_TOKEN(token,'+')) { a=1; }
        token_type=get_token(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token,'='))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }
        if (eval_expression(asm_context, &a)!=0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].attribute=a;
      }
        else
      {
        pushback(asm_context, token, token_type);
      }
    }
      else
    if (strcasecmp(token, "wreg")==0)
    {
      operands[operand_count].type=OPTYPE_WREG;
    }
      else
    {
      num=get_register_dspic(token);
      if (num>=0)
      {
        operands[operand_count].type=OPTYPE_REGISTER;
        token_type=get_token(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token,'*'))
        {
          token_type=get_token(asm_context, token, TOKENLEN);
          operands[operand_count].reg2=get_register_dspic(token);
          if (operands[operand_count].reg2==-1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
          operands[operand_count].type=OPTYPE_W_MUL_W;
        }
          else
        {
          pushback(asm_context, token, token_type);
          //print_error_unexp(token, asm_context);
          //return -1;
        }
      }
        else
      {
        operands[operand_count].type=OPTYPE_NUM;
        pushback(asm_context, token, token_type);

        if (eval_expression(asm_context, &num)!=0)
        {
          if (asm_context->pass==1)
          {
            eat_operand(asm_context);
          }
            else
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
      }
    }

    operands[operand_count++].value=num;

    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token,',')) { print_error_unexp(token, asm_context); return -1; }
  }

#ifdef DEBUG
  printf("-------- instr=%s  flag=%d\n", instr, flag);
  for (n=0; n<operand_count; n++)
  {
    printf("operand %d: value=%d type=%d attribute=%d\n", n, operands[n].value, operands[n].type, operands[n].attribute);
  }
#endif

  // On pass 1 we only calculate address.
  if (asm_context->pass==1)
  {
    add_bin_dspic(asm_context, 0x0000000, IS_OPCODE);

    if (strcmp("do", instr_case)==0 ||
        (strcmp("goto", instr_case)==0 && operand_count==1 &&
         operands[0].type==OPTYPE_REGISTER))
    {
      add_bin_dspic(asm_context, 0x0000000, IS_OPCODE);
    }

    return 0;
  }

  n=0;
  struct _dspic_table *dspic_entry=NULL;
  while(dspic_table[n].name!=NULL && dspic_entry==NULL)
  {
    if (strcmp(dspic_table[n].name, instr_case)==0)
    {
//printf("Matched %s\n", dspic_table[n].name);
      //int local_flag;
      matched=1;
      range_error=-1;

      if (flag==FLAG_NONE)
      {
        // If default flag is none, then a flag is required
        if (dspic_table[n].default_flag==FLAG_NONE &&
            dspic_table[n].flags!=0)
        {
          n++;
          continue;
        }

        flag=dspic_table[n].default_flag;
//printf("Setting default flag=%d\n", flag);
      }

      if ((flag&dspic_table[n].flags)==0 && dspic_table[n].flags!=0)
      {
//printf("n=%d\n", 0);
        n++;
        continue;
      }

//printf("flag=%d\n", flag);
      opcode=dspic_table[n].opcode;

      int r=0,curr_operand=0;
      while(dspic_table[n].operands[r].type!=OP_NONE && r<dspic_table[n].args)
      {
        wrong_op=0;

        if (curr_operand>=operand_count)
        {
          // Who the fuck makes an optional argument equal to value 2
          // forcing the assembler to have to have some stupid shit like
          // like this in it?
          if (dspic_table[n].operands[r].type==OP_ACC_WB)
          {
            opcode|=2<<dspic_table[n].operands[r].bitpos;
            r++;
            continue;
          }

          if (dspic_table[n].operands[r].optional==1) { r++; continue; }
          break;
        }

//printf("NEXT: n=%d r=%d curr_operand=%d type=%d  operand_type=%d %s\n", n, r, curr_operand, dspic_table[n].operands[r].type, operands[curr_operand].type, instr_case);
        switch(dspic_table[n].operands[r].type)
        {
          case OP_BRA:
          {
            if (operands[curr_operand].type==OPTYPE_NUM)
            {
              int value=(operands[curr_operand].value-(asm_context->address+2))/2;
              if (value<-32768 || value>32767)
              {
                range_error=curr_operand; 
              }
              opcode|=(unsigned short int)value<<dspic_table[n].operands[r].bitpos;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_F:
          case OP_EXPR:
          {
            if (operands[curr_operand].type==OPTYPE_NUM)
            {
              if (check_range(dspic_table[n].operands[r].bitlen, operands[curr_operand].value)!=0)
              {
                range_error=curr_operand; 
              }
              opcode|=operands[curr_operand].value<<dspic_table[n].operands[r].bitpos;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_LIT:
          {
            if (operands[curr_operand].type==OPTYPE_LIT)
            {
              if (check_range(dspic_table[n].operands[r].bitlen, operands[curr_operand].value)!=0)
              {
                range_error=curr_operand; 
              }
              opcode|=operands[curr_operand].value<<dspic_table[n].operands[r].bitpos;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_ACC:
          {
            if (operands[curr_operand].type==OPTYPE_ACCUM)
            {
              opcode|=operands[curr_operand].value<<dspic_table[n].operands[r].bitpos;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_WREG:
          {
            if (operands[curr_operand].type==OPTYPE_WREG)
            {
              opcode|=1<<dspic_table[n].operands[r].bitpos;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_WN:
          case OP_WM:
          case OP_WB:
          case OP_WNS:
          case OP_WND:
          {
            if (operands[curr_operand].type==OPTYPE_REGISTER &&
                operands[curr_operand].attribute==REG_NORMAL)
            {
              opcode|=operands[curr_operand].value<<dspic_table[n].operands[r].bitpos;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_WD:
          case OP_WS:
          case OP_W_INDEX:
          {
            if (operands[curr_operand].type==OPTYPE_REGISTER &&
                operands[curr_operand].attribute<REG_INDIRECT_W_PLUS_W)
            {
              if (operands[curr_operand].attribute==REG_NORMAL &&
                  dspic_table[n].operands[r].type==OP_W_INDEX) { wrong_op=1; }
              opcode|=operands[curr_operand].value<<dspic_table[n].operands[r].bitpos;
              opcode|=operands[curr_operand].attribute<<dspic_table[n].operands[r].attrpos;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_WS_LIT:
          case OP_WD_LIT:
          {
            if (operands[curr_operand].type==OPTYPE_W_PLUS_LIT)
            {
              opcode|=operands[curr_operand].value<<dspic_table[n].operands[r].bitpos;
              opcode|=operands[curr_operand].attribute<<dspic_table[n].operands[r].attrpos;
              if (check_range(dspic_table[n].operands[r].bitlen, operands[curr_operand].attribute)!=0)
              {
                range_error=curr_operand; 
              }
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_WXD:
          case OP_WYD:
          {
            if (operands[curr_operand].type==OPTYPE_REGISTER)
            {
              if (operands[curr_operand].value<4 ||
                  operands[curr_operand].value>7)
              {
                //print_error("Register out of range (w4-w7)", asm_context);
                wrong_op=1;
              }
                else
              {
                opcode|=(operands[curr_operand].value-4)<<dspic_table[n].operands[r].bitpos;
              }
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_MULT2:
          {
            // Wm*Wm
            if (operands[curr_operand].type==OPTYPE_W_MUL_W)
            {
              if (operands[curr_operand].value!=operands[curr_operand].attribute ||
                  operands[curr_operand].value<4 ||
                  operands[curr_operand].value>7)
              {
                wrong_op=1;
              }
                else
              {
                opcode|=(operands[curr_operand].value-4)<<dspic_table[n].operands[r].bitpos;
              }
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_MULT3:
          {
            // Wm*Wn
            if (operands[curr_operand].type==OPTYPE_W_MUL_W)
            {
              int value=0;
              int reg1=operands[curr_operand].value;
              int reg2=operands[curr_operand].attribute;
              if (reg1==4 && reg2==5) { value=0; }
              else if (reg1==4 && reg2==6) { value=1; }
              else if (reg1==4 && reg2==7) { value=2; }
              else if (reg1==5 && reg2==6) { value=4; }
              else if (reg1==5 && reg2==7) { value=5; }
              else if (reg1==6 && reg2==7) { value=6; }
              else { wrong_op=1; break; }

              opcode|=(value)<<dspic_table[n].operands[r].bitpos;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_PREFETCH_ED_X:
          case OP_PREFETCH_ED_Y:
          case OP_PREFETCH_X:
          case OP_PREFETCH_Y:
          {
            int value=0;
            if (dspic_table[n].operands[r].type==OP_PREFETCH_ED_X)
            {
              if (operands[curr_operand].value==8) { value=0; }
              else if (operands[curr_operand].value==9) { value=8; }
            }
              else
            if (dspic_table[n].operands[r].type==OP_PREFETCH_ED_Y)
            {
              if (operands[curr_operand].value==8) { value=0; }
              else if (operands[curr_operand].value==9) { value=8; }
            }
              else
            { wrong_op=1; break; }

            if (operands[curr_operand].type==OPTYPE_REGISTER &&
                operands[curr_operand].attribute==REG_INDIRECT_W_PLUS_W)
            {
              opcode|=(value|4)<<dspic_table[n].operands[r].bitpos;
            }
              else
            if (operands[curr_operand].type==OPTYPE_W_PLUS_LIT)
            {
              int attr=operands[curr_operand].attribute;
              if ((attr&0x1)==1 || attr>6 || attr<-6) { wrong_op=1; break; }
              attr=attr/2;
              attr=attr&0x7;
              opcode|=(value|attr)<<dspic_table[n].operands[r].bitpos;
            }
              else
            if (operands[curr_operand].type==OPTYPE_REGISTER &&
                operands[curr_operand].attribute==REG_INDIRECT)
            {
              opcode|=(value)<<dspic_table[n].operands[r].bitpos;
            }
              else
            {
              wrong_op=1;
              break;
            }

            if (dspic_table[n].operands[r].type==OP_PREFETCH_X ||
                dspic_table[n].operands[r].type==OP_PREFETCH_Y)
            {
              curr_operand++;
              if (curr_operand>=operand_count ||
                  operands[curr_operand].type!=OPTYPE_REGISTER ||
                  operands[curr_operand].attribute!=REG_NORMAL ||
                  operands[curr_operand].value<4 ||
                  operands[curr_operand].value>7)
              {
                printf("Error: Expecting register w4-w7 at %s:%d.\n", asm_context->filename, asm_context->line);
                return -1;
              }

              opcode|=(operands[curr_operand].value-4)<<dspic_table[n].operands[r].attrpos;

            }
            break;
          }
          case OP_ACC_WB:
          {
            if (operands[curr_operand].type==OPTYPE_REGISTER &&
                operands[curr_operand].attribute==REG_NORMAL &&
                operands[curr_operand].value==13)
            {
              // aa = 0, do nothing (arf arf)
            }
              else
            if (operands[curr_operand].type==OPTYPE_W_PLUS_LIT &&
                operands[curr_operand].value==13 &&
                operands[curr_operand].attribute==2)
            {
              opcode|=1<<dspic_table[n].operands[r].bitpos;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_EXPR_GOTO:
          {
            // FIXME - add 24 bits
            if (operands[curr_operand].type==OPTYPE_NUM)
            {
              if (check_range(dspic_table[n].operands[r].bitlen, operands[curr_operand].value)!=0)
              {
                range_error=curr_operand; 
              }
              if ((operands[curr_operand].value&1)==1)
              {
                printf("Error: Address not on boundary at %s:%d.\n", asm_context->filename, asm_context->line);
              }
              //opcode|=operands[curr_operand].value<<dspic_table[n].operands[r].bitpos;
              opcode|=operands[curr_operand].value&0xfffe;
              extra24=operands[curr_operand].value>>16;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          case OP_EXPR_DO:
          {
            // FIXME - is this okay?
            if (operands[curr_operand].type==OPTYPE_NUM)
            {
              if (check_range(dspic_table[n].operands[r].bitlen, operands[curr_operand].value)!=0)
              {
                range_error=curr_operand; 
              }
              extra24=operands[curr_operand].value;
            }
              else
            {
              wrong_op=1;
            }
            break;
          }
          default:
            printf("Internal Error: %s:%d\n", __FILE__, __LINE__);
            break;
        }

        if (wrong_op==1)
        {
          if (dspic_table[n].operands[r].optional!=1)
          {
            r=dspic_table[n].args+1;
            break;
          }
        }
          else
        {
          curr_operand++;
        }

        r++;
      }

      if (r==dspic_table[n].args)
      {
        dspic_entry=&dspic_table[n];
      }
    }

    n++;
  }

  if (dspic_entry==NULL)
  {
    if (matched==1)
    {
      printf("Error: Unknown flag/operands combo for '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
    }
      else
    {
      printf("Error: Unknown instruction '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
    }

    return -1;
  }

  if (range_error!=-1)
  {
    printf("Error: Operand %d is out of range (%d bits) at %s:%d\n", range_error+1, dspic_entry->operands[range_error].bitlen, asm_context->filename, asm_context->line);
    return -1;
  }

  if (dspic_entry->flag_pos!=-1)
  {
//printf("flag=%d dspic_entry->flags=%d\n", flag, dspic_entry->flags);
    flag=convert_dspic_flag_combo(flag, dspic_entry->flags);
    flag=flag==-1?0:flag;
    opcode|=flag<<dspic_entry->flag_pos;
  }

  add_bin_dspic(asm_context, opcode, IS_OPCODE);
  if (extra24!=-1)
  {
    add_bin_dspic(asm_context, extra24, IS_OPCODE);
  }

  asm_context->line++;

  return 0;
}


