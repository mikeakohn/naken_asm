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

static int parse_number(struct _asm_context *asm_context, int *num)
{
char token[TOKENLEN];
int token_type;
int size=1;

  if (eval_expression(asm_context, num)!=0)
  {
    if (asm_context->pass==1)
    {
      memory_write_m(&asm_context->memory, asm_context->address, 1);
      return -2;
    }

    print_error_illegal_operands(instr, asm_context);
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

      unsigned char a=(unsigned char)*num;
      *num=a;
      return 1;
#if 0
      if (asm_context->pass==1)
      {
        memory_write_m(&asm_context->memory, asm_context->address, 1);
      }
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

      unsigned short int a=(unsigned short)*num;
      *num=a;
      return 2;
#if 0
      if (asm_context->pass==1)
      {
        memory_write_m(&asm_context->memory, asm_context->address, 2);
      }
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

      unsigned int a=(unsigned int)*num;
      *num=a&0xffffff;
      return 3;
#if 0
      if (asm_context->pass==1)
      {
        memory_write_m(&asm_context->memory, asm_context->address, 3);
      }
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

  if (num>=-128 && num<=255)
  {
    unsigned char a=(unsigned char)*num;
    *num=a;
    return 1;
  }
    else
  if (num>=-32768 && num<=65535)
  {
    unsigned int a=(unsigned int)*num;
    *num=a&0xffff;
    return 2;
  }
    else
  if (num>=0 && num<=((1<<24)-1))
  {
    unsigned int a=(unsigned int)*num;
    *num=(a&0xffffff);
    return 3;
  }
    else
  {
    printf("Error: Constant out of range at %s:%d\n", asm_context->filename, asm_context->line);
    return -1;
  }
}

static int parse_stm8_type1(struct _asm_context *asm_context, char *instr, int opcode_nibble)
{
char token[TOKENLEN];
int token_type;
int num;

  token_type=get_token(asm_context, token, TOKENLEN);

  if (token_type==TOKEN_POUND)
  {
    if (asm_context->pass==1)
    {
      eat_operand(asm_context);
    }

    if (eval_expression(asm_context, &num)!=0)
    {
      print_error_illegal_operands(instr, asm_context);
    }

    if (num>255 || num<-128)
    {
      print_error_range(instr, -128, 255, asm_context);
    }

    add_bin8(asm_context, 0xa0|opcode_nibble, IS_OPCODE);
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
      }

      add_bin8(asm_context, 0xf0|opcode_nibble, IS_OPCODE);
      return size;
    }

    if (IS_TOKEN(token,'['))   // XOR A, ([$1000.w],X)
    {
    }

    pushback(asm_context, token, token_type);
  }
    else
  if (IS_TOKEN(token,'['))
  {
  }
    else
  {
    pushback(asm_context, token, token_type);

    if (eval_expression(asm_context, &num)!=0)
    {
      if (asm_context->pass==1)
      {
        add_bin16(asm_context, 1, IS_OPCODE);
        add_bin8(asm_context, 1, IS_OPCODE);
        return 3;
      }

      print_error_illegal_operands(instr, asm_context);
      return -1;
    }

    if (num>=-128 && num<=255 &&
        memory_read_m(&asm_context->memory, asm_context->address)!=1)
    {
      add_bin8(asm_context, 0xb0|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, (unsigned char)num, IS_OPCODE);
      return 2;
    }

    if (num>=-32768 && num<=32767)
    {
      unsigned int n=num;
      n=(n&0xffff);
      add_bin8(asm_context, 0xc0|opcode_nibble, IS_OPCODE);
      add_bin8(asm_context, (n>>8), IS_OPCODE);
      add_bin8(asm_context, (n&0xff), IS_OPCODE);
      return 3;
    }
    print_error_range(instr, -32768, 32767, asm_context);
  }

  return -1;
}

int parse_instruction_stm8(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
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

      if (strcasecmp(token, "a")==0)
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
        int size=parse_stm8_type1(asm_context, instr, n);
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
        return size;
      }
    }
  }

  for (n=0; n<16; n++)
  {
    if (stm8_type2[n]==NULL) { continue; }
    if (strcmp(stm8_type2[n], instr_case)==0)
    {
    }
  }

#if 0
  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL) { break; }

    if (operand_count==0 && IS_TOKEN(token,'.'))
    {
      strcat(instr_case, ".");
      strcat(instr, ".");
      token_type=get_token(asm_context, token, TOKENLEN);
      strcat(instr, token);
      n=0;
      while(token[n]!=0) { token[n]=tolower(token[n]); n++; }
      strcat(instr_case, token);
      continue;
    }

  }

  if (asm_context->pass==1)
  {
    add_bin8(asm_context, 0, IS_OPCODE);
    return 4;
  }
#endif


  printf("Error: Unknown instruction '%s'  at %s:%d\n", instr, asm_context->filename, asm_context->line);

  return -1;
}


