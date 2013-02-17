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
#include <ctype.h>

#include "asm_common.h"
#include "asm_stm8.h"
#include "assembler.h"
#include "disasm_stm8.h"
#include "get_tokens.h"
#include "eval_expression.h"
#include "table_stm8.h"

static int parse_num(struct _asm_context *asm_context, char *instr, int *num, int max_size)
{
char token[TOKENLEN];
int token_type;
int size=1;

  if (eval_expression(asm_context, num)!=0)
  {
    if (asm_context->pass==1)
    {
      memory_write_m(&asm_context->memory, asm_context->address, 1);
      return max_size;
    }

    print_error_illegal_expression(instr, asm_context);
    return -1;
  }

  token_type=get_token(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token,'.'))
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (strcasecmp(token,"b")==0)
    {
      if (*num<-128 || *num>255)
      {
        print_error_range("Constant", -128, 255, asm_context);
        return -1;
      }

#if 0
      unsigned char a=(unsigned char)*num;
      *num=a;
      return 1;
#endif
    }
      else
    if (strcasecmp(token,"w")==0)
    {
      if (*num<-32768 || *num>65535)
      {
        print_error_range("Constant", -32768, 65535, asm_context);
        return -1;
      }

      size=2;
#if 0
      unsigned short int a=(unsigned short)*num;
      *num=a;
      return 2;
#endif
    }
      else
    if (strcasecmp(token,"e")==0)
    {
      if (*num<-(1<<23) || *num>((1<<24)-1))
      {
        print_error_range("Constant", -(1<<23), ((1<<24)-1), asm_context);
        return -1;
      }

      size=3;
#if 0
      unsigned int a=(unsigned int)*num;
      *num=a&0xffffff;
      return 3;
#endif
    }
      else
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }
    else
  {
    pushback(asm_context, token, token_type);
  }

  if (*num>=-128 && *num<=255)
  {
    unsigned char a=(unsigned char)*num;
    *num=a;
  }
    else
  if (*num>=-32768 && *num<=65535)
  {
    if (max_size<2)
    {
      print_error_range("Constant", -128, 255, asm_context);
      return -1;
    }

    unsigned int a=(unsigned int)*num;
    *num=a&0xffff;

    size=2;
  }
    else
  if (*num>=0 && *num<=((1<<24)-1))
  {
    if (max_size<2)
    {
      print_error_range("Constant", -(1<<((max_size*8)-1)), ((1<<(max_size*8))-1), asm_context);
      return -1;
    }

    unsigned int a=(unsigned int)*num;
    *num=(a&0xffffff);

    size=3;
  }
    else
  {
    printf("Error: Constant out of range at %s:%d\n", asm_context->filename, asm_context->line);
    return -1;
  }

  if (memory_read_m(&asm_context->memory, asm_context->address)==1)
  {
    size=max_size;
  }

  return size;
}

// Populate pass 1 bytes
static void stm8_pass1(struct _asm_context *asm_context, int bytes)
{
  int n;
  int a=memory_read_m(&asm_context->memory, asm_context->address);
  for (n=0; n<bytes; n++) { add_bin8(asm_context, a, IS_OPCODE); }
}

static int parse_stm8_type1(struct _asm_context *asm_context, char *instr, int opcode_nibble)
{
char token[TOKENLEN];
int token_type;
int num;
//int n;

  token_type=get_token(asm_context, token, TOKENLEN);

  if (token_type==TOKEN_POUND)
  {
    int opcode=0;

    if (asm_context->pass==1)
    {
      eat_operand(asm_context);
    }
      else
    {
      if (parse_num(asm_context, instr, &num, 1)<0) { return -1; }
      opcode=0xa0|opcode_nibble;
    }

    add_bin8(asm_context, opcode, IS_OPCODE);
    add_bin8(asm_context, num, IS_OPCODE);

    return 2;
  }
    else
  if (IS_TOKEN(token,'('))
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (strcasecmp(token,"x")==0 || strcasecmp(token,"y")==0)
    {
      int size=1;
      if (strcasecmp(token,"y")==0)
      {
        add_bin8(asm_context, 0x90, IS_OPCODE);
        size++;
      }

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_NOT_TOKEN(token,')'))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      add_bin8(asm_context, 0xf0|opcode_nibble, IS_OPCODE);
      return size;
    }

    if (IS_TOKEN(token,'['))   // XOR A, ([$1000.w],X)
    {
      int size=-1;
      int bytes=parse_num(asm_context, instr, &num, 2);
      if (bytes<0) { return -1; }

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_NOT_TOKEN(token,']'))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_NOT_TOKEN(token,','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      int a=memory_read_m(&asm_context->memory, asm_context->address);
      token_type=get_token(asm_context, token, TOKENLEN);

      if (strcasecmp(token,"x")==0)
      {
        if (bytes==2)
        {
          add_bin8(asm_context, asm_context->pass==1?a:0x72, IS_OPCODE);
          add_bin8(asm_context, 0xd0|opcode_nibble, IS_OPCODE);
          add_bin8(asm_context, (unsigned char)(num>>8), IS_OPCODE);
          add_bin8(asm_context, (unsigned char)(num&0xff), IS_OPCODE);
          size=4;
        }
          else
        {
          add_bin8(asm_context, asm_context->pass==1?a:0x92, IS_OPCODE);
          add_bin8(asm_context, 0xd0|opcode_nibble, IS_OPCODE);
          add_bin8(asm_context, (unsigned char)num, IS_OPCODE);
          size=3;
        }
      }
        else
      if (strcasecmp(token,"y")==0)
      {
        if (bytes==2)
        {
          print_error_range("Constant", -128, 255, asm_context);
          return -1;
        }

        add_bin8(asm_context, asm_context->pass==1?a:0x91, IS_OPCODE);
        add_bin8(asm_context, 0xd0|opcode_nibble, IS_OPCODE);
        add_bin8(asm_context, (unsigned char)num, IS_OPCODE);
        size=3;
      }

      token_type=get_token(asm_context, token, TOKENLEN);
      if (IS_NOT_TOKEN(token,')'))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      return size;
    }

    // (num, reg)
    pushback(asm_context, token, token_type);
    int bytes=parse_num(asm_context, instr, &num, 2);
    if (bytes<0) { return -1; }

    token_type=get_token(asm_context, token, TOKENLEN);
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    int size=0;
    int a=memory_read_m(&asm_context->memory, asm_context->address);
    token_type=get_token(asm_context, token, TOKENLEN);

    if (strcasecmp(token,"sp")==0)
    {
      if (bytes>1)
      {
        print_error_range("Constant", -128, 255, asm_context);
        return -1;
      }

      if (asm_context->pass!=1)
      {
        if (opcode_nibble==6) { a=0x7b; }
        else if (opcode_nibble==7) { a=0x6b; }
        else if (opcode_nibble==0xc) { print_error_unexp(token, asm_context); return -1; }
        else { a=0x10|opcode_nibble; }
      }

      add_bin8(asm_context, a, IS_OPCODE);
      add_bin8(asm_context, (unsigned char)num, IS_OPCODE);
    }
      else
    if (strcasecmp(token,"x")==0)
    {
      if (bytes==2)
      {
        if (asm_context->pass!=1) { a=0xd0|opcode_nibble; }
        add_bin8(asm_context, a, IS_OPCODE);
        add_bin8(asm_context, (unsigned char)(num>>8), IS_OPCODE);
        add_bin8(asm_context, (unsigned char)(num&0xff), IS_OPCODE);
        size=3;
      }
        else
      {
        if (asm_context->pass!=1) { a=0xe0|opcode_nibble; }
        add_bin8(asm_context, a, IS_OPCODE);
        add_bin8(asm_context, (unsigned char)num, IS_OPCODE);
        size=2;
      }
    }
      else
    if (strcasecmp(token,"y")==0)
    {
      if (bytes==2)
      {
        add_bin8(asm_context, asm_context->pass==1?a:0x90, IS_OPCODE);
        add_bin8(asm_context, 0xd0|opcode_nibble, IS_OPCODE);
        add_bin8(asm_context, (unsigned char)(num>>8), IS_OPCODE);
        add_bin8(asm_context, (unsigned char)(num&0xff), IS_OPCODE);
        size=4;
      }
        else
      {
        add_bin8(asm_context, asm_context->pass==1?a:0x90, IS_OPCODE);
        add_bin8(asm_context, 0xe0|opcode_nibble, IS_OPCODE);
        add_bin8(asm_context, (unsigned char)num, IS_OPCODE);
        size=3;
      }
    }
      else
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    token_type=get_token(asm_context, token, TOKENLEN);
    if (IS_NOT_TOKEN(token,')'))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    return size;
  }
    else
  if (IS_TOKEN(token,'['))
  {
    int size=-1;
    int bytes=parse_num(asm_context, instr, &num, 2);
    if (bytes<0) { return -1; }
    int a=memory_read_m(&asm_context->memory, asm_context->address);

    if (bytes==2)
    {
      add_bin8(asm_context, asm_context->pass==1?a:0x72, IS_OPCODE);
      add_bin8(asm_context, 0xc0|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, (unsigned char)(num>>8), IS_OPCODE);
      add_bin8(asm_context, (unsigned char)(num&0xff), IS_OPCODE);
      size=4;
    }
      else
    {
      add_bin8(asm_context, asm_context->pass==1?a:0x92, IS_OPCODE);
      add_bin8(asm_context, 0xc0|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, (unsigned char)num, IS_OPCODE);
      size=3;
    }

    token_type=get_token(asm_context, token, TOKENLEN);
    if (IS_NOT_TOKEN(token,']'))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    return size;
  }
    else
  {
    pushback(asm_context, token, token_type);

    int bytes=parse_num(asm_context, instr, &num, 2);
    if (bytes<0) { return -1; }

    if (asm_context->pass==1) { stm8_pass1(asm_context, bytes+1); return bytes+1; }

    if (bytes==1)
    {
      add_bin8(asm_context, 0xb0|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, (unsigned char)num, IS_OPCODE);
      return 2;
    }
      else
    {
      add_bin8(asm_context, 0xc0|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, (num>>8), IS_OPCODE);
      add_bin8(asm_context, (num&0xff), IS_OPCODE);
      return 3;
    }
  }

  return -1;
}

static int parse_stm8_type2(struct _asm_context *asm_context, char *instr, int opcode_nibble)
{
char token[TOKENLEN];
int token_type;
int num;

  token_type=get_token(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token,'A') || IS_TOKEN(token,'a'))
  {
    add_bin8(asm_context, 0x40|opcode_nibble, IS_OPCODE);
    return 1;
  }
    else
  if (IS_TOKEN(token,'('))
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (IS_TOKEN(token,'X') || IS_TOKEN(token,'x'))
    {
      add_bin8(asm_context, 0x70|opcode_nibble, IS_OPCODE);
      if (expect_token_s(asm_context,")")!=0) { return -1; }
      return 1;
    }
      else
    if (IS_TOKEN(token,'Y') || IS_TOKEN(token,'y'))
    {
      add_bin8(asm_context, 0x90, IS_OPCODE);
      add_bin8(asm_context, 0x70|opcode_nibble, IS_OPCODE);
      if (expect_token_s(asm_context,")")!=0) { return -1; }
      return 2;
    }

    if (IS_TOKEN(token,'['))
    {
      int bytes=parse_num(asm_context, instr, &num, 2);
      if (bytes<0) { return -1; }
      if (expect_token_s(asm_context,"]")!=0) { return -1; }
      if (expect_token_s(asm_context,",")!=0) { return -1; }
      token_type=get_token(asm_context, token, TOKENLEN);
      if (expect_token_s(asm_context,")")!=0) { return -1; }

      if (IS_TOKEN(token,'X') || IS_TOKEN(token,'x'))
      {
        if (bytes==1)
        {
          add_bin8(asm_context, 0x92, IS_OPCODE);
          add_bin8(asm_context, 0x60|opcode_nibble, IS_OPCODE);
          add_bin8(asm_context, num, IS_OPCODE);
          return 3;
        }
          else
        {
          add_bin8(asm_context, 0x72, IS_OPCODE);
          add_bin8(asm_context, 0x60|opcode_nibble, IS_OPCODE);
          add_bin8(asm_context, num>>8, IS_OPCODE);
          add_bin8(asm_context, num&0xff, IS_OPCODE);
          return 4;
        }
      }
        else
      if (IS_TOKEN(token,'Y') || IS_TOKEN(token,'y'))
      {
        if (bytes==1)
        {
          add_bin8(asm_context, 0x91, IS_OPCODE);
          add_bin8(asm_context, 0x60|opcode_nibble, IS_OPCODE);
          add_bin8(asm_context, num, IS_OPCODE);
          return 3;
        }
          else
        {
          print_error_range("Constant", -128, 255, asm_context);
          return -1;
        }
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }

    int size=2;
    pushback(asm_context, token, token_type);
    int bytes=parse_num(asm_context, instr, &num, 2);
    if (bytes<0) { return -1; }
    if (expect_token_s(asm_context,",")!=0) { return -1; }

    token_type=get_token(asm_context, token, TOKENLEN);
    if (strcasecmp(token,"sp")==0)
    {
      if (bytes>1)
      {
        if (asm_context->pass==1)
        {
          bytes=1;
          num=0;
        }
          else
        {
          print_error_range("Constant", -128, 255, asm_context);
          return -1;
        }
      }
      add_bin8(asm_context, 0x00|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, num, IS_OPCODE);
    }
      else
    if (IS_TOKEN(token,'X') || IS_TOKEN(token,'x'))
    {
      if (bytes==1)
      {
        add_bin8(asm_context, 0x60|opcode_nibble, IS_OPCODE);
        add_bin8(asm_context, num, IS_OPCODE);
      }
        else
      {
        add_bin8(asm_context, 0x72, IS_OPCODE);
        add_bin8(asm_context, 0x40|opcode_nibble, IS_OPCODE);
        add_bin8(asm_context, num>>8, IS_OPCODE);
        add_bin8(asm_context, num&0xff, IS_OPCODE);
        size+=2;
      }
    }
      else
    if (IS_TOKEN(token,'Y') || IS_TOKEN(token,'y'))
    {
      size++;
      add_bin8(asm_context, 0x90, IS_OPCODE);

      if (bytes==1)
      {
        add_bin8(asm_context, 0x60|opcode_nibble, IS_OPCODE);
        add_bin8(asm_context, num, IS_OPCODE);
      }
        else
      {
        add_bin8(asm_context, 0x40|opcode_nibble, IS_OPCODE);
        add_bin8(asm_context, num>>8, IS_OPCODE);
        add_bin8(asm_context, num&0xff, IS_OPCODE);
        size+=1;
      }
    }
      else
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (expect_token_s(asm_context,")")!=0) { return -1; }

    return size;
  }
    else
  if (IS_TOKEN(token,'['))
  {
    int bytes=parse_num(asm_context, instr, &num, 2);
    if (bytes<0) { return -1; }
    if (expect_token_s(asm_context,"]")!=0) { return -1; }

    if (bytes==1)
    {
      add_bin8(asm_context, 0x92, IS_OPCODE);
      add_bin8(asm_context, 0x30|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, num, IS_OPCODE);
      return 3;
    }
      else
    {
      add_bin8(asm_context, 0x72, IS_OPCODE);
      add_bin8(asm_context, 0x30|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, num>>8, IS_OPCODE);
      add_bin8(asm_context, num&0xff, IS_OPCODE);
      return 4;
    }
  }
    else
  {
    pushback(asm_context, token, token_type);

    int bytes=parse_num(asm_context, instr, &num, 2);
    if (bytes<0) { return -1; }

    if (asm_context->pass==1) { stm8_pass1(asm_context, bytes+1); return bytes+1; }

    if (bytes==1)
    {
      add_bin8(asm_context, 0x30|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, (unsigned char)num, IS_OPCODE);
      return 2;
    }
      else
    {
      add_bin8(asm_context, 0x72, IS_OPCODE);
      add_bin8(asm_context, 0x50|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, (num>>8), IS_OPCODE);
      add_bin8(asm_context, (num&0xff), IS_OPCODE);
      return 4;
    }
  }

  return -1;
}

static int parse_stm8_bit_oper(struct _asm_context *asm_context, char *instr, int oper)
{
char token[TOKENLEN];
int token_type;
int num;
//int size=-1;
int bytes;

  bytes=parse_num(asm_context, instr, &num, 2);
  if (bytes<0) { return -1; }

  token_type=get_token(asm_context, token, TOKENLEN);
  if (IS_NOT_TOKEN(token,','))
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  token_type=get_token(asm_context, token, TOKENLEN);
  if (token_type!=TOKEN_POUND)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  token_type=get_token(asm_context, token, TOKENLEN);
  if (token_type!=TOKEN_NUMBER)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  int n=atoi(token);
  if (n<0 || n>7)
  {
    print_error_range("Constant", 0, 7, asm_context);
    return -1;
  }

  add_bin8(asm_context, (oper<=1 || oper>=4)?0x72:0x90, IS_OPCODE);
  add_bin8(asm_context, ((oper<4?1:0)<<4)|(n<<1)|(oper&1), IS_OPCODE);
  add_bin8(asm_context, num>>8, IS_OPCODE);
  add_bin8(asm_context, num&0xff, IS_OPCODE);

  if (oper>=4)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    bytes=parse_num(asm_context, instr, &num, 3);
    if (bytes<0) { return -1; }

    if (asm_context->pass==1)
    {
      add_bin8(asm_context, 0, IS_OPCODE);
    }
      else
    {
      int offset=num-(asm_context->address+1);
      if (offset<-128 || offset>255)
      {
        print_error_range("Offset", -128, 255, asm_context);
        return -1;
      }
      add_bin8(asm_context, offset&0xff, IS_OPCODE);
    }

    return 5;
  }

  return 4;
}

int parse_instruction_stm8(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
int num=0;
int n;

  lower_copy(instr_case, instr);

  n=0;
  while(stm8_single[n].instr!=NULL)
  {
    if (strcmp(stm8_single[n].instr, instr_case)==0)
    {
      add_bin8(asm_context, stm8_single[n].opcode, IS_OPCODE);
      return 1;
    }

    n++;
  }

  n=0;
  while(stm8_x_y[n].instr!=NULL)
  {
    if (strcmp(stm8_x_y[n].instr, instr_case)==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);

      if (strcasecmp("y", token)==0)
      {
        add_bin8(asm_context, 0x90, IS_OPCODE);
      }
        else
      if (strcasecmp("x", token)!=0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      add_bin8(asm_context, stm8_x_y[n].opcode, IS_OPCODE);
      return 1;
    }

    n++;
  }

  for (n=0; n<16; n++)
  {
    if (stm8_type1[n]==NULL) { continue; }
    if (strcmp(stm8_type1[n], instr_case)==0)
    {
      token_type=get_token(asm_context, token, TOKENLEN);

      if ((n!=0x0d && strcasecmp(token, "a")==0) ||
          (n==0x03 && strcasecmp(token, "x")==0))
      {
        token_type=get_token(asm_context, token, TOKENLEN);
        if (IS_NOT_TOKEN(token,','))
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        return parse_stm8_type1(asm_context, instr, n);
      }
        else
      {
        pushback(asm_context, token, token_type);
        int size=parse_stm8_type1(asm_context, instr, n);

        if (n!=0x0d)
        {
          token_type=get_token(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token,','))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
          token_type=get_token(asm_context, token, TOKENLEN);
          if (strcasecmp(token, "a")!=0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
        return size;
      }
    }
  }

  for (n=0; n<17; n++)
  {
    if (stm8_type2[n]==NULL) { continue; }
    if (strcmp(stm8_type2[n], instr_case)==0)
    {
      if (n==0x10) { n=8; }
      return parse_stm8_type2(asm_context, instr, n);
    }
  }

  for (n=0; n<6; n++)
  {
    if (strcmp(stm8_bit_oper[n], instr_case)==0)
    {
      return parse_stm8_bit_oper(asm_context, instr, n);
    }
  }

  n=0;
  while(stm8_r_r[n].instr!=NULL)
  {
    if (strcmp(stm8_r_r[n].instr, instr_case)==0)
    {
      int is_x=0;
      token_type=get_token(asm_context, token, TOKENLEN);

      if (strcasecmp(token,"x")==0) { is_x=1; }
      else if (strcasecmp(token,"y")==0 && n<2) { is_x=0; }
      else { print_error_unexp(token, asm_context); return -1; }

      if (expect_token(asm_context,',')!=0) { return -1; }

      if (n<2)
      {
        if (expect_token_s(asm_context,"a")!=0) { return -1; }
        if (is_x==0)
        {
          add_bin8(asm_context, 0x90, IS_OPCODE);
          add_bin8(asm_context, stm8_r_r[n].opcode, IS_OPCODE);
          return 2;
        }
      }
        else
      {
        if (expect_token_s(asm_context,"y")!=0) { return -1; }
      }

      add_bin8(asm_context, stm8_r_r[n].opcode, IS_OPCODE);
      return 1;
    }
    n++;
  }

  n=0;
  while(stm8_jumps[n].instr!=NULL)
  {
    if (strcmp(stm8_jumps[n].instr, instr_case)==0)
    {
      int count=2;

      if (stm8_jumps[n].prefix!=0)
      {
        add_bin8(asm_context, stm8_jumps[n].prefix, IS_OPCODE);
        count++;
      }

      add_bin8(asm_context, stm8_jumps[n].opcode, IS_OPCODE);

      if (eval_expression(asm_context, &num)!=0)
      {
        if (asm_context->pass==1)
        {
          add_bin8(asm_context, 0, IS_OPCODE);
          return count;
        }

        print_error_illegal_expression(instr, asm_context);
        return -1;
      }

      num=num-(asm_context->address+1);

      if (num<-128 || num>255)
      {
        print_error_range("Offset", -128, 127, asm_context);
        return -1;
      }

      add_bin8(asm_context, (unsigned char)num, IS_OPCODE);

      return count;
    }

    n++;
  }

  if (strcmp("wfe", instr_case)==0)
  {
    add_bin8(asm_context, 0x72, IS_OPCODE);
    add_bin8(asm_context, 0x8f, IS_OPCODE);
    return 2;
  }

  if (strcmp("pop", instr_case)==0)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (IS_TOKEN(token,'A') || IS_TOKEN(token,'a'))
    {
      add_bin8(asm_context, 0x84, IS_OPCODE);
      return 1;
    }

    if (strcasecmp(token,"cc")==0)
    {
      add_bin8(asm_context, 0x86, IS_OPCODE);
      return 1;
    }

    pushback(asm_context, token, token_type);
    if (parse_num(asm_context, instr, &num, 2)<0) { return -1; }
    add_bin8(asm_context, 0x32, IS_OPCODE);
    add_bin8(asm_context, num>>8, IS_OPCODE);
    add_bin8(asm_context, num&0xff, IS_OPCODE);
    return 3;
  }

  if (strcmp("push", instr_case)==0)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (IS_TOKEN(token,'A') || IS_TOKEN(token,'a'))
    {
      add_bin8(asm_context, 0x88, IS_OPCODE);
      return 1;
    }

    if (strcasecmp(token,"cc")==0)
    {
      add_bin8(asm_context, 0x8a, IS_OPCODE);
      return 1;
    }

    if (token_type==TOKEN_POUND)
    {
      if (parse_num(asm_context, instr, &num, 1)<0) { return -1; }
      add_bin8(asm_context, 0x4b, IS_OPCODE);
      add_bin8(asm_context, num, IS_OPCODE);
      return 2;
    }

    pushback(asm_context, token, token_type);
    if (parse_num(asm_context, instr, &num, 2)<0) { return -1; }
    add_bin8(asm_context, 0x3b, IS_OPCODE);
    add_bin8(asm_context, num>>8, IS_OPCODE);
    add_bin8(asm_context, num&0xff, IS_OPCODE);
    return 3;
  }

  if (strcmp("callr", instr_case)==0)
  {
    if (parse_num(asm_context, instr, &num, 3)<0) { return -1; }
    int offset=num-(asm_context->address+2);
    if (offset<-127 || offset>128)
    {
      print_error_range("Offset", -128, 255, asm_context);
      return -1;
    }

    add_bin8(asm_context, 0xad, IS_OPCODE);
    add_bin8(asm_context, (unsigned char)offset, IS_OPCODE);

    return 2;
  }

  if (strcmp("exg", instr_case)==0)
  {
    if (expect_token_s(asm_context,"a")!=0) { return -1; }
    if (expect_token_s(asm_context,",")!=0) { return -1; }

    token_type=get_token(asm_context, token, TOKENLEN);
    if (strcasecmp(token, "XL")==0)
    {
      add_bin8(asm_context, 0x41, IS_OPCODE);
      return 1;
    }
      else
    if (strcasecmp(token, "YL")==0)
    {
      add_bin8(asm_context, 0x61, IS_OPCODE);
      return 1;
    }

    pushback(asm_context, token, token_type);
    if (parse_num(asm_context, instr, &num, 2)<0) { return -1; }

    add_bin8(asm_context, 0x31, IS_OPCODE);
    add_bin8(asm_context, num>>8, IS_OPCODE);
    add_bin8(asm_context, num&0xff, IS_OPCODE);
    return 3;
  }

  n=0;
  if (strcmp("callf", instr_case)==0) { n=0x8d; }
  else if (strcmp("jpf", instr_case)==0) { n=0xac; }

  if (n!=0)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (IS_TOKEN(token,'['))
    {
      if (parse_num(asm_context, instr, &num, 3)<0) { return -1; }
      add_bin8(asm_context, 0x92, IS_OPCODE);
      add_bin8(asm_context, n, IS_OPCODE);
      add_bin8(asm_context, num>>8, IS_OPCODE);
      add_bin8(asm_context, num&0xff, IS_OPCODE);

      if (expect_token_s(asm_context,"]")!=0) { return -1; }
      return 4;
    }

    pushback(asm_context, token, token_type);
    if (parse_num(asm_context, instr, &num, 2)<0) { return -1; }
    add_bin8(asm_context, n, IS_OPCODE);
    add_bin8(asm_context, num>>16, IS_OPCODE);
    add_bin8(asm_context, (num>>8)&0xff, IS_OPCODE);
    add_bin8(asm_context, num&0xff, IS_OPCODE);

    return 4;
  }

  print_error_unknown_instr(instr, asm_context);
  //printf("Error: Unknown instruction '%s'  at %s:%d\n", instr, asm_context->filename, asm_context->line);

  return -1;
}


