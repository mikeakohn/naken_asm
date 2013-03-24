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

#include "asm_65xx.h"
#include "asm_680x.h"
#include "asm_68hc08.h"
#include "asm_680x0.h"
#include "asm_805x.h"
#include "asm_arm.h"
#include "asm_common.h"
#include "asm_dspic.h"
#include "asm_mips.h"
#include "asm_msp430.h"
#include "asm_stm8.h"
#include "asm_tms1000.h"
#include "asm_tms9900.h"
#include "asm_z80.h"
#include "assembler.h"
#include "disasm_65xx.h"
#include "disasm_680x.h"
#include "disasm_68hc08.h"
#include "disasm_680x0.h"
#include "disasm_arm.h"
#include "disasm_dspic.h"
#include "disasm_mips.h"
#include "disasm_msp430.h"
#include "disasm_805x.h"
#include "disasm_stm8.h"
#include "disasm_tms1000.h"
#include "disasm_tms9900.h"
#include "disasm_z80.h"
#include "eval_expression.h"
#include "get_tokens.h"
#include "ifdef_expression.h"
#include "lookup_tables.h"
#include "macros.h"

void print_error(const char *s, struct _asm_context *asm_context)
{
  printf("Error: %s at %s:%d\n", s, asm_context->filename, asm_context->line);
}

void print_error_unexp(const char *s, struct _asm_context *asm_context)
{
  printf("Error: Unexpected token '%s' at %s:%d\n", s, asm_context->filename, asm_context->line);
}

void print_error_unknown_instr(const char *instr, struct _asm_context *asm_context)
{
  printf("Error: Unknown instruction '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
}

void print_error_opcount(const char *instr, struct _asm_context *asm_context)
{
  printf("Error: Wrong number of operands for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
}

void print_error_illegal_operands(const char *instr, struct _asm_context *asm_context)
{
  printf("Error: Illegal operands for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
}

void print_error_illegal_expression(const char *instr, struct _asm_context *asm_context)
{
  printf("Error: Illegal expression for '%s' at %s:%d\n", instr, asm_context->filename, asm_context->line);
}

void print_error_range(const char *s, int r1, int r2, struct _asm_context *asm_context)
{
  printf("Error: %s out of range (%d,%d) at %s:%d\n", s, r1, r2, asm_context->filename, asm_context->line);
}

struct _memory_pool *add_pool(struct _naken_heap *heap, int heap_len)
{
  struct _memory_pool *curr_pool;
  struct _memory_pool *memory_pool=malloc(heap_len+sizeof(struct _memory_pool));

  memory_pool->len=heap_len;
  memory_pool->ptr=0;
  memory_pool->next=NULL;

  if (heap->memory_pool==NULL)
  {
    heap->memory_pool=memory_pool;
  }
    else
  {
    curr_pool=heap->memory_pool;
    while (curr_pool->next!=NULL) { curr_pool=curr_pool->next; }
    curr_pool->next=memory_pool;
  }

#ifdef DEBUG
printf("add_pool pool=%p\n", memory_pool);
#endif

  return memory_pool;
}

void free_pools(struct _memory_pool *memory_pool)
{
  struct _memory_pool *curr_pool;
  struct _memory_pool *last_pool;

  curr_pool=memory_pool;

  while (curr_pool!=NULL)
  {
#ifdef DEBUG
printf("free_pools pool=%p\n", curr_pool);
#endif
    last_pool=curr_pool;
    curr_pool=curr_pool->next;
    free(last_pool);
  }
}

int add_to_include_path(struct _asm_context *asm_context, char *paths)
{
int ptr=0;
int n=0;
char *s;

  s=asm_context->include_path;
  while(!(s[ptr]==0 && s[ptr+1]==0)) { ptr++; }
  if (ptr!=0) ptr++;

  while(paths[n]!=0)
  {
    if (paths[n]==':')
    {
      n++;
      s[ptr++]=0;
    }
      else
    {
      s[ptr++]=paths[n++];
    }

    if (ptr>=INCLUDE_PATH_LEN-1) return -1;
  }

  return 0;
}

static int parse_binfile(struct _asm_context *asm_context)
{
FILE *in;
char token[TOKENLEN];
unsigned char buffer[8192];
//int token_type;
int len;
int n;

  if (asm_context->segment==SEGMENT_BSS)
  {
    printf("Error: .bss segment doesn't support initialized data at %s:%d\n", asm_context->filename, asm_context->line);
    return -1;
  }

  get_token(asm_context, token, TOKENLEN);
#ifdef DEBUG
printf("binfile file %s.\n", token);
#endif

  in=fopen(token, "rb");
  if (in==NULL)
  {
    printf("Cannot open binfile file '%s' at %s:%d\n", token, asm_context->filename, asm_context->line);
    return -1;
  }

  while(1)
  {
    len=fread(buffer, 1, 8192, in);
    if (len<=0) break;

    for (n=0; n<len; n++)
    {
      memory_write_inc(asm_context, buffer[n], DL_DATA);
    }
    asm_context->data_count+=len;
  }

  fclose(in);

  return 0;
}

static int parse_include(struct _asm_context *asm_context)
{
char token[TOKENLEN];
//int token_type;
const char *oldname;
int oldline;
FILE *oldfp;
int ret;

  get_token(asm_context, token, TOKENLEN);
#ifdef DEBUG
printf("including file %s.\n", token);
#endif

  asm_context->include_count++;

  oldfp=asm_context->in;

  asm_context->in=fopen(token, "rb");

  if (asm_context->in==NULL)
  {
    int ptr=0;
    char *s=asm_context->include_path;
    char filename[1024];

    while(1)
    {
      if (s[ptr]==0) break;

      if (strlen(token)+strlen(s+ptr)<1022)
      {
        sprintf(filename, "%s/%s", s+ptr, token);
        asm_context->in=fopen(filename, "rb");
        if (asm_context->in!=NULL) break;
      }

      while (s[ptr]!=0) ptr++;
      ptr++;
    }
  }

  if (asm_context->in==NULL)
  {
    printf("Cannot open include file '%s' at %s:%d\n", token, asm_context->filename, asm_context->line);
    ret=-1;
  }
    else
  {
    oldname=asm_context->filename;
    oldline=asm_context->line;

    asm_context->filename=token;
    asm_context->line=1;

    ret=assemble(asm_context);

    asm_context->filename=oldname;
    asm_context->line=oldline;
  }

  if (asm_context->in!=NULL) { fclose(asm_context->in); }

  asm_context->in=oldfp;
  asm_context->include_count--;

  return ret;
}

static int parse_org(struct _asm_context *asm_context)
{
int num;

  if (eval_expression(asm_context, &num)==-1)
  {
    printf("Parse error on line %d. ORG expects an address.\n", asm_context->line);
    return -1;
  }

  asm_context->address=num;

  return 0;
}

static int parse_name(struct _asm_context *asm_context)
{
char token[TOKENLEN];
//int token_type;

  get_token(asm_context, token, TOKENLEN);

  printf("Program name: %s (ignored)\n", token);

  return 0;
}

static int parse_public(struct _asm_context *asm_context)
{
char token[TOKENLEN];
//int token_type;

  get_token(asm_context, token, TOKENLEN);

  printf("Public symbol: %s (ignored)\n", token);

  return 0;
}

#if 0
static void list_data(struct _asm_context *asm_context, int address, int len)
{
int n;

  fprintf(asm_context->list, "0x%04x:", address);

  for (n=address; n<address+len; n++)
  {
    fprintf(asm_context->list, " 0x%02x:", memory_read(asm_context, n));
  }

  fprintf(asm_context->list, "\n");
}
#endif

static int parse_db(struct _asm_context *asm_context, int null_term_flag)
{
char token[TOKENLEN];
int token_type;
int data32;
//int address=asm_context->address;

  if (asm_context->segment==SEGMENT_BSS)
  {
    printf("Error: .bss segment doesn't support initialized data at %s:%d\n", asm_context->filename, asm_context->line);
    return -1;
  }

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) break;

    if (token_type==TOKEN_QUOTED)
    {
      unsigned char *s=(unsigned char *)token;
      while(*s!=0)
      {
        if (*s=='\\')
        {
          int e=escape_char(asm_context, s);
          if (e==0)
          {
            return -1;
          }
          s=s+e;
        }

        memory_write_inc(asm_context, *s, DL_DATA);

        asm_context->data_count++;
        s++;
      }

      if (null_term_flag==1)
      {
        memory_write_inc(asm_context, 0, DL_DATA);
        asm_context->data_count++;
      }
    }
      else
    {
      pushback(asm_context, token, token_type);
      if (eval_expression(asm_context, &data32)==-1)
      {
        eat_operand(asm_context);
      }

      memory_write_inc(asm_context, (unsigned char)data32, DL_DATA);
      asm_context->data_count++;
    }

    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) break;

    if (IS_NOT_TOKEN(token,','))
    {
      printf("Parse error: expecting a ',' on line %d.\n", asm_context->line);
      return -1;
    }
  }

  //list_data(asm_context, address, asm_context->address-address);

  asm_context->line++;

  return 0;
}

static int parse_dw(struct _asm_context *asm_context)
{
char token[TOKENLEN];
int token_type;
//int sign=1;
int data32;
unsigned short int data16;
//int address=asm_context->address;

  if (asm_context->segment==SEGMENT_BSS)
  {
    printf("Error: .bss segment doesn't support initialized data at %s:%d\n", asm_context->filename, asm_context->line);
    return -1;
  }

#if 0
  if ((asm_context->address&0x01)!=0)
  {
    if (asm_context->pass==2)
    {
      printf("Warning: dw doesn't start on 16 bit boundary at %s:%d.  Padding with a 0.\n", asm_context->filename, asm_context->line);
    }

    memory_write_inc(asm_context, 0, DL_DATA);
    asm_context->data_count++;
  }
#endif

  while(1)
  {
    // if the user has a comma at the end, but no data, this is okay
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) break;
    pushback(asm_context, token, token_type);

    if (eval_expression(asm_context, &data32)==-1)
    {
      eat_operand(asm_context);
    }
    data16=(unsigned short)data32;

    memory_write_inc(asm_context, data16&255, DL_DATA);
    memory_write_inc(asm_context, data16>>8, DL_DATA);

    asm_context->data_count+=2;
    token_type=get_token(asm_context, token, TOKENLEN);
    if (token_type==TOKEN_EOL || token_type==TOKEN_EOF) break;

    if (IS_NOT_TOKEN(token,','))
    {
      printf("Parse error: expecting a ',' on line %d.\n", asm_context->line);
      return -1;
    }

    //sign=1;
  }

  //list_data(asm_context, address, asm_context->address-address);

  asm_context->line++;

  return 0;
}

static int parse_ds(struct _asm_context *asm_context, int n)
{
char token[TOKENLEN];
int token_type;
int num;
//int address=asm_context->address;

  token_type=get_token(asm_context, token, TOKENLEN);
  if (token_type!=TOKEN_NUMBER)
  {
    printf("Parse error: memory length on line %d.\n", asm_context->line);
    return -1;
  }

  num=atoi(token)*n;

  for (n=0; n<num; n++)
  {
    if (asm_context->segment!=SEGMENT_BSS)
    {
      memory_write_inc(asm_context, 0, DL_DATA);
    }
      else
    {
      asm_context->address++;
    }

    if (asm_context->address>=asm_context->memory.size)
    {
       printf("Error: ds overran 64k boundary at %s:%d", asm_context->filename, asm_context->line);
       return -1;
    }
  }

  //list_data(asm_context, address, asm_context->address-address);

  asm_context->data_count+=num;

  return 0;
}

static int ifdef_ignore(struct _asm_context *asm_context)
{
char token[TOKENLEN];
int token_type;

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);

    if (token_type==TOKEN_EOF)
    {
      print_error("Missing endif", asm_context);
      return -1;
    }

    if (token_type==TOKEN_EOL)
    {
      asm_context->line++;
    }
      else
    if (token_type==TOKEN_POUND || IS_TOKEN(token,'.'))
    {
      token_type=get_token(asm_context, token, TOKENLEN);
      if (strcasecmp(token, "endif")==0) return 0;
        else
      if (strcasecmp(token, "else")==0) return 2;
    }
  }
}

static int parse_ifdef_ignore(struct _asm_context *asm_context, int ignore_section)
{
  if (ignore_section==1)
  {
    if (ifdef_ignore(asm_context)==2)
    {
      assemble(asm_context);
    }
  }
    else
  {
    if (assemble(asm_context)==2)
    {
      ifdef_ignore(asm_context);
    }
  }

  return 0;
}

static int parse_ifdef(struct _asm_context *asm_context, int ifndef)
{
char token[TOKENLEN];
int token_type;
int ignore_section=0;
int param_count; // throw away

  asm_context->ifdef_count++;

  asm_context->parsing_ifdef=1;
  token_type=get_token(asm_context, token, TOKENLEN);
  asm_context->parsing_ifdef=0;

  if (token_type!=TOKEN_STRING)
  {
    print_error("#ifdef has no label", asm_context);
    return -1;
  }

  if (defines_heap_lookup(&asm_context->defines_heap, token, &param_count)!=NULL)
  {
    if (ifndef==1) ignore_section=1;
  }
    else
  {
    if (ifndef==0) ignore_section=1;
  }

  parse_ifdef_ignore(asm_context, ignore_section);

  asm_context->ifdef_count--;

  return 0;
}

static int parse_if(struct _asm_context *asm_context)
{
int num;

  asm_context->ifdef_count++;

  asm_context->parsing_ifdef=1;
  num=eval_ifdef_expression(asm_context);
  asm_context->parsing_ifdef=0;

  if (num==-1) return -1;

  if (num!=0)
  {
    parse_ifdef_ignore(asm_context, 0);
  }
    else
  {
    parse_ifdef_ignore(asm_context, 1);
  }

  asm_context->ifdef_count--;

  return 0;
}

void assemble_init(struct _asm_context *asm_context)
{
  fseek(asm_context->in, 0, SEEK_SET);
  asm_context->parse_instruction=parse_instruction_msp430;
  asm_context->list_output=list_output_msp430;
  asm_context->address=0;
  asm_context->line=1;
  asm_context->instruction_count=0;
  asm_context->code_count=0;
  asm_context->data_count=0;
  asm_context->ifdef_count=0;
  asm_context->parsing_ifdef=0;
  asm_context->pushback[0]=0;
  asm_context->unget[0]=0;
  asm_context->unget_ptr=0;
  asm_context->unget_stack_ptr=0;
  asm_context->unget_stack[0]=0;

  defines_heap_free(&asm_context->defines_heap);
  asm_context->def_param_stack_count=0;
  if (asm_context->pass==1)
  {
    //memset(asm_context->debug_line, 0xff, sizeof(int)*asm_context->memory_size);
    memory_init(&asm_context->memory, 1<<20, 1);
  }
}

void assemble_print_info(struct _asm_context *asm_context, FILE *out)
{
  fprintf(out, "\nProgram Info:\n");
#ifdef DEBUG
  address_heap_print(&asm_context->address_heap);
  defines_heap_print(&asm_context->defines_heap);
#endif

  fprintf(out, "Include Paths: .\n");
  int ptr=0;
  if (asm_context->include_path[ptr]!=0)
  {
    fprintf(out, "               ");
    while(1)
    {
      if (asm_context->include_path[ptr]==0 &&
          asm_context->include_path[ptr+1]==0)
      { fprintf(out, "\n"); break; }

      if (asm_context->include_path[ptr]==0)
      {
        fprintf(out, "\n               ");
        ptr++;
        continue;
      }
      putc(asm_context->include_path[ptr++], out); 
    }
  }

  fprintf(out, " Instructions: %d\n", asm_context->instruction_count);
  fprintf(out, "   Code Bytes: %d\n", asm_context->code_count);
  fprintf(out, "   Data Bytes: %d\n", asm_context->data_count);
  fprintf(out, "  Low Address: %04x (%d)\n", asm_context->memory.low_address, asm_context->memory.low_address);
  fprintf(out, " High Address: %04x (%d)\n", asm_context->memory.high_address, asm_context->memory.high_address);
  fprintf(out, "\n");
}

int check_for_directive(struct _asm_context *asm_context, char *token)
{
  if (strcasecmp(token, "org")==0)
  {
    if (parse_org(asm_context)!=0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "name")==0)
  {
    if (parse_name(asm_context)!=0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "public")==0)
  {
    if (parse_public(asm_context)!=0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "db")==0 || strcasecmp(token, "dc8")==0 || strcasecmp(token, "ascii")==0)
  {
    if (parse_db(asm_context, 0)!=0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "asciiz")==0)
  {
    if (parse_db(asm_context, 1)!=0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "dw")==0 || strcasecmp(token, "dc16")==0)
  {
    if (parse_dw(asm_context)!=0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "ds")==0 || strcasecmp(token, "ds8")==0)
  {
    if (parse_ds(asm_context,1)!=0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "ds16")==0)
  {
    if (parse_ds(asm_context,2)!=0) return -1;
    return 1;
  }
    else
  if (strcasecmp(token, "end")==0)
  {
    return 2;
  }
    else
#ifdef ENABLE_ARM
  if (strcasecmp(token, "arm")==0)
  {
    asm_context->parse_instruction=parse_instruction_arm;
    asm_context->list_output=list_output_arm;
    asm_context->cpu_type=CPU_TYPE_ARM;
    return 1;
  }
    else
#endif
#ifdef ENABLE_DSPIC
  if (strcasecmp(token, "dspic")==0)
  {
    asm_context->parse_instruction=parse_instruction_dspic;
    asm_context->list_output=list_output_dspic;
    asm_context->cpu_type=CPU_TYPE_DSPIC;
    return 1;
  }
    else
#endif
#ifdef ENABLE_MIPS
  if (strcasecmp(token, "mips")==0)
  {
    asm_context->parse_instruction=parse_instruction_mips;
    asm_context->list_output=list_output_mips;
    asm_context->cpu_type=CPU_TYPE_MIPS;
    return 1;
  }
    else
#endif
#ifdef ENABLE_MSP430
  if (strcasecmp(token, "msp430")==0)
  {
    asm_context->parse_instruction=parse_instruction_msp430;
    asm_context->list_output=list_output_msp430;
    asm_context->cpu_type=CPU_TYPE_MSP430;
    return 1;
  }
    else
#endif
#ifdef ENABLE_65XX
  if (strcasecmp(token, "65xx")==0)
  {
    asm_context->parse_instruction=parse_instruction_65xx;
    asm_context->list_output=list_output_65xx;
    asm_context->cpu_type=CPU_TYPE_65XX;
    asm_context->is_dollar_hex=1;
    return 1;
  }
    else
#endif
#ifdef ENABLE_680X
  if (strcasecmp(token, "680x")==0)
  {
    asm_context->parse_instruction=parse_instruction_680x;
    asm_context->list_output=list_output_680x;
    asm_context->cpu_type=CPU_TYPE_680X;
    asm_context->memory.endian=ENDIAN_BIG;
    asm_context->is_dollar_hex=1;
    return 1;
  }
    else
#endif
#ifdef ENABLE_68HC08
  if (strcasecmp(token, "68hc08")==0)
  {
    asm_context->parse_instruction=parse_instruction_68hc08;
    asm_context->list_output=list_output_68hc08;
    asm_context->cpu_type=CPU_TYPE_68HC08;
    asm_context->memory.endian=ENDIAN_BIG;
    asm_context->is_dollar_hex=1;
    return 1;
  }
    else
#endif
#ifdef ENABLE_680X0
  if (strcasecmp(token, "680x0")==0)
  {
    asm_context->parse_instruction=parse_instruction_680x0;
    asm_context->list_output=list_output_680x0;
    asm_context->cpu_type=CPU_TYPE_680X0;
    asm_context->memory.endian=ENDIAN_BIG;
    asm_context->is_dollar_hex=1;
    return 1;
  }
    else
#endif
#ifdef ENABLE_805X
  if (strcasecmp(token, "8051")==0 || strcasecmp(token, "8052")==0)
  {
    asm_context->parse_instruction=parse_instruction_805x;
    asm_context->list_output=list_output_805x;
    asm_context->cpu_type=CPU_TYPE_805X;
    return 1;
  }
    else
#endif
#ifdef ENABLE_STM8
  if (strcasecmp(token, "stm8")==0)
  {
    asm_context->parse_instruction=parse_instruction_stm8;
    asm_context->list_output=list_output_stm8;
    asm_context->cpu_type=CPU_TYPE_STM8;
    asm_context->is_dollar_hex=1;
    return 1;
  }
    else
#endif
#ifdef ENABLE_TMS1000
  if (strcasecmp(token, "tms1000")==0)
  {
    asm_context->parse_instruction=parse_instruction_tms1000;
    asm_context->list_output=list_output_tms1000;
    asm_context->cpu_type=CPU_TYPE_TMS1000;
    return 1;
  }
    else
  if (strcasecmp(token, "tms1100")==0)
  {
    asm_context->parse_instruction=parse_instruction_tms1100;
    asm_context->list_output=list_output_tms1100;
    asm_context->cpu_type=CPU_TYPE_TMS1100;
    return 1;
  }
    else
#endif
#ifdef ENABLE_TMS9900
  if (strcasecmp(token, "tms9900")==0)
  {
    asm_context->parse_instruction=parse_instruction_tms9900;
    asm_context->list_output=list_output_tms9900;
    asm_context->cpu_type=CPU_TYPE_TMS9900;
    asm_context->memory.endian=ENDIAN_BIG;
    return 1;
  }
    else
#endif
#ifdef ENABLE_Z80
  if (strcasecmp(token, "z80")==0)
  {
    asm_context->parse_instruction=parse_instruction_z80;
    asm_context->list_output=list_output_z80;
    asm_context->cpu_type=CPU_TYPE_Z80;
    asm_context->can_tick_end_string=1;
    //asm_context->is_dollar_hex=1;
    return 1;
  }
    else
#endif
  if (strcasecmp(token, "big_endian")==0)
  {
    asm_context->memory.endian=ENDIAN_BIG;
  }
    else
  if (strcasecmp(token, "little_endian")==0)
  {
    asm_context->memory.endian=ENDIAN_LITTLE;
  }

  return 0;
}

int assemble(struct _asm_context *asm_context)
{
char token[TOKENLEN];
int token_type;

  while(1)
  {
    token_type=get_token(asm_context, token, TOKENLEN);
#ifdef DEBUG
    printf("%d: <%d> %s\n", asm_context->line, token_type, token);
#endif
    if (token_type==TOKEN_EOF) break;

    if (token_type==TOKEN_EOL)
    {
      if (asm_context->defines_heap.stack_ptr==0) { asm_context->line++; }
    }
      else
    if (token_type==TOKEN_LABEL)
    {
      if (address_heap_append(asm_context, token, asm_context->address)==-1)
      { return -1; }
    }
      else
    if (token_type==TOKEN_POUND || IS_TOKEN(token,'.'))
    {
      token_type=get_token(asm_context, token, TOKENLEN);
#ifdef DEBUG
    printf("%d: <%d> %s\n", asm_context->line, token_type, token);
#endif
      if (token_type==TOKEN_EOF) break;

      if (strcasecmp(token, "define")==0)
      {
        if (parse_macro(asm_context, IS_DEFINE)!=0) return -1;
      }
        else
      if (strcasecmp(token, "ifdef")==0)
      {
        parse_ifdef(asm_context, 0);
      }
        else
      if (strcasecmp(token, "ifndef")==0)
      {
        parse_ifdef(asm_context, 1);
      }
        else
      if (strcasecmp(token, "if")==0)
      {
        parse_if(asm_context);
      }
        else
      if (strcasecmp(token, "endif")==0)
      {
        if (asm_context->ifdef_count<1)
        {
          printf("Error: unmatched #endif at %s:%d\n", asm_context->filename, asm_context->ifdef_count);
          return -1;
        }
        return 0;
      }
        else
      if (strcasecmp(token, "else")==0)
      {
        if (asm_context->ifdef_count<1)
        {
          printf("Error: unmatched #else at %s:%d\n", asm_context->filename, asm_context->ifdef_count);
          return -1;
        }
        return 2;
      }
        else
      if (strcasecmp(token, "include")==0)
      {
        if (parse_include(asm_context)!=0) return -1;
      }
        else
      if (strcasecmp(token, "binfile")==0)
      {
        if (parse_binfile(asm_context)!=0) return -1;
      }
        else
      if (strcasecmp(token, "code")==0)
      {
        asm_context->segment=SEGMENT_CODE;
      }
        else
      if (strcasecmp(token, "bss")==0)
      {
        asm_context->segment=SEGMENT_BSS;
      }
        else
      if (strcasecmp(token, "macro")==0)
      {
        if (parse_macro(asm_context, IS_MACRO)!=0) return -1;
      }
        else
      {
        int ret=check_for_directive(asm_context, token);
        if (ret==2) break;
        if (ret==-1) return -1;
        if (ret!=1)
        {
          printf("Error: Unknown directive '%s' at %s:%d.\n", token, asm_context->filename, asm_context->line);
          return -1;
        }
      }
    }
      else
    if (token_type==TOKEN_STRING)
    {
      int ret=check_for_directive(asm_context, token);
      if (ret==2) break;
      if (ret==-1) return -1;
      if (ret!=1) 
      {
        int start_address=asm_context->address;
        char token2[TOKENLEN];
        int token_type2;

        token_type2=get_token(asm_context, token2, TOKENLEN);

        if (strcasecmp(token2, "equ")==0)
        {
          //token_type2=get_token(asm_context, token2, TOKENLEN);
          int ptr=0;
          int ch='\n';
          while(1)
          {
            ch=get_next_char(asm_context);
            if (ch==EOF || ch=='\n') break;
            if (ch=='*' && ptr>0 && token2[ptr-1]=='/')
            {
              eatout_star_comment(asm_context);
              ptr--;
              continue;
            }

            token2[ptr++]=ch;
            if (ptr==TOKENLEN-1)
            {
              printf("Internal Error: token overflow at %s:%d.\n", __FILE__, __LINE__);
              return -1;
            }
          }
          token2[ptr]=0;
          unget_next_char(asm_context, ch);
          strip_macro(token2);
          defines_heap_append(asm_context, token, token2, 0);
        }
          else
        {
          pushback(asm_context, token2, token_type2);
          int address=asm_context->address;

          //ret=parse_instruction_msp430(asm_context, token);
          ret=asm_context->parse_instruction(asm_context, token);
          if (asm_context->pass==2 && asm_context->list!=NULL && asm_context->include_count==0)
          {
            asm_context->list_output(asm_context, address);
            fprintf(asm_context->list, "\n");
          }

          if (ret<0) return -1;

          if (asm_context->defines_heap.stack_ptr==0) { asm_context->line++; }
          asm_context->instruction_count++;

          if (asm_context->address>start_address)
          {
            asm_context->code_count+=(asm_context->address-start_address);
          }
        }
      }
    }
      else
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  return 0;
}

