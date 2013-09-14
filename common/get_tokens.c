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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "get_tokens.h"
#include "lookup_tables.h"
#include "macros.h"

#define assert(a) if (! a) { printf("assert failed on line %s:%d\n", __FILE__, __LINE__); raise(SIGABRT); }

int get_next_char(struct _asm_context *asm_context)
{
int ch;

#ifdef DEBUG
//printf("debug> get_next_char()\n");
#endif

  // Check if something need to be ungetted
  if (asm_context->unget_ptr>asm_context->unget_stack[asm_context->unget_stack_ptr])
  {
#ifdef DEBUG
//printf("debug> get_next_char(?) ungetc %d %d '%c'\n", asm_context->unget_stack_ptr, asm_context->unget_stack[asm_context->unget_stack_ptr], asm_context->unget[asm_context->unget_ptr-1]);
#endif
    return asm_context->unget[--asm_context->unget_ptr];
  }

  ch=defines_heap_get_char(asm_context);

  // Check if defines is empty
  if (ch==CHAR_EOF)
  {
    if (asm_context->unget_ptr>asm_context->unget_stack[asm_context->unget_stack_ptr])
    {
#ifdef DEBUG
//printf("debug> get_next_char(FILE ungetc %d %d '%c'\n", asm_context->unget_stack_ptr, asm_context->unget_stack[asm_context->unget_stack_ptr], asm_context->unget[asm_context->unget_ptr-1]);
#endif
      return asm_context->unget[--asm_context->unget_ptr];
    }
#ifdef DEBUG
//printf("debug> get_next_char(FILE)='%c'\n", ch);
#endif

    ch=getc(asm_context->in);
    if (asm_context->pass==2 && asm_context->list!=NULL && asm_context->include_count==0)
    {
      if (ch!=EOF) putc(ch, asm_context->list);
    }
  }
    else
  {
#ifdef DEBUG
//printf("debug> get_next_char(DEFINE)='%c'\n", ch);
#endif
  }

  return ch;
}

int unget_next_char(struct _asm_context *asm_context, int ch)
{
  asm_context->unget[asm_context->unget_ptr++]=ch;
  return 0;
}

int get_token(struct _asm_context *asm_context, char *token, int len)
{
int token_type=TOKEN_EOF;
int ch;
int ptr=0;

#ifdef DEBUG
//printf("Enter get_token()\n");
#endif

  token[0]=0;

  if (asm_context->pushback[0]!=0)
  {
    strcpy(token, asm_context->pushback);
    asm_context->pushback[0]=0;
#ifdef DEBUG
//printf("debug> get_token pushback: '%s'\n",token);
#endif
    return asm_context->pushback_type;
  }

  while(1)
  {
#ifdef DEBUG
//printf("debug> get_token, grabbing next char ptr=%d\n", ptr);
#endif
    ch=get_next_char(asm_context);
#ifdef DEBUG
//printf("debug> getc()='%c'  ptr=%d  token='%s'\n", ch, ptr, token);
#endif

    if (token_type==TOKEN_DOLLAR)
    {
      if ((ch>='0' && ch<='9') || (ch>='a' && ch<='f') || (ch>='A' && ch<='F'))
      {
        token[0]='0';
        token[1]='x';
        ptr=2;
      }
        else
      if (!((ch>='a' && ch<='z') || (ch>='A' && ch<='Z')))
      {
        unget_next_char(asm_context, ch);
        break;
      }

      token_type=TOKEN_STRING;
    }

    if (ch==';')
    {
      if (ptr!=0)
      {
        unget_next_char(asm_context, ch);
        break;
        //int n;
        //for(n=0; n<ptr; n++) printf("%c\n", token[n]);
        //printf("\n");
      }

      while(1)
      { ch=get_next_char(asm_context); if (ch=='\n' || ch==EOF) break; }

      assert(ptr==0);
      token[0]='\n';
      token[1]=0;
      return TOKEN_EOL;
    }

    if (ch=='\r') continue;   /* DOS sucks. Geesh get a modern OS. */

    if (ch=='"' || ch=='\'')
    {
      char quote=ch;
      if (ch=='"' ) { token_type=TOKEN_QUOTED; }
      else { token_type=TOKEN_TICKED; }

      while(1)
      {
        ch=get_next_char(asm_context);
        if (ch==quote)
        {
          break;
        }

        if (ch=='\\')
        {
          ch=get_next_char(asm_context);
          if (ch=='n') { ch='\n'; }
          else if (ch=='r') { ch='\r'; }
          else if (ch=='t') { ch='\t'; }
        }

        token[ptr++]=ch;
        if (ptr>=len)
        {
          printf("get_token.c: line=%d - Internal error\n", __LINE__);
          exit(1);
        }
      }

      break;
    }

#if 0
    if (ch=='\'')
    {
      token_type=TOKEN_TICKED;
    }

    if (quote!=0)
    {
      if (ch=='\n')
      {
        print_error("Unterminated quote", asm_context);
        return TOKEN_EOF;
      }
        else
      if (ch==quote) break;

      token[ptr++]=ch;
      continue;
    }

    if (ch=='\"' || ch=='\'')
    {
      if (asm_context->can_tick_end_string &&
          ch=='\'' &&
          token_type==TOKEN_STRING)
      {
        token[ptr++]=ch;
        break;
      }

      quote=ch;
      token_type=TOKEN_QUOTED;
      continue;
    }

    if (ch=='\\' && (token_type==TOKEN_QUOTED || token_type==TOKEN_TICKED))
    {
      token[ptr++]=ch;
      ch=get_next_char(asm_context);
      if (ch==EOF)
      {
        print_error("Unexpected end of file.", asm_context);
        token[0]=0;
        return TOKEN_EOF;
      }
      token[ptr++]=ch;
      continue;
    }
#endif

    if (ch=='\n' || ch==' ' || ch=='\t' || ch==EOF)
    {
      if (ch=='\n')
      {
        if (ptr==0)
        {
          //asm_context->line++;
          token[0]='\n';
          token[1]=0;
          return TOKEN_EOL;
        }
          else
        { unget_next_char(asm_context, ch); }
      }

      if (ptr==0)
      {
        if (ch==EOF) break;
        continue;
      }

      if (ptr>=len)
      {
        printf("get_token.c: line=%d - Internal error\n", __LINE__);
        exit(1);
      }

      break;
    }
      else
    {
      if (ch=='#')
      {
        token_type=TOKEN_POUND;
        token[ptr++]=ch;
        break;
      }

      if (ptr==0 && ch=='$')
      {
        if (asm_context->is_dollar_hex) { token_type=TOKEN_DOLLAR; }
        else { token_type=TOKEN_STRING; }
        token[ptr++]=ch;
        continue;
      }

      if (ch==':')
      {
        if (token_type==TOKEN_STRING)
        {
          token_type=TOKEN_LABEL;
          // token[ptr++]=ch;
          break;
        }
      }

      if ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || ch=='_')
      {
        if (ptr==0)
        {
          token_type=TOKEN_STRING;
        }
          else
        if (token_type==TOKEN_NUMBER)
        {
          token_type=TOKEN_STRING;
        }
      }
        else
      if (ch>='0' && ch<='9')
      {
        if (ptr==0)
        {
          token_type=TOKEN_NUMBER;
        }
      }
        else
      {
        if (ptr==0)
        {
          token_type=TOKEN_SYMBOL;
          token[ptr++]=ch;
          if (ch=='/')
          {
            ch=get_next_char(asm_context);
            if (ch=='*')
            {
              ptr=0;
              token[0]=0;

              while(1)
              {
                ch=get_next_char(asm_context);
                if (ch==EOF)
                {
                  print_error("Unterminated comment", asm_context);
                  return TOKEN_EOF;
                }

                if (ch=='\n')
                {
                  asm_context->line++;
                }
                  else
                if (ch=='*')
                {
                  ch=get_next_char(asm_context);
                  if (ch=='/') break;
                  unget_next_char(asm_context, ch);
                }
              }

              continue;
            }
              else
            if (ch=='/')
            {
              while(1)
              { ch=get_next_char(asm_context); if (ch=='\n' || ch==EOF) break; }

              token[0]='\n';
              token[1]=0;
              return TOKEN_EOL;
            }
              else
            {
              unget_next_char(asm_context, ch);
              break;
            }
          }
            else
          if (ch=='>' || ch=='<' || ch=='=')
          {
            int ch1;
            ch1=get_next_char(asm_context);
            if (ch1==ch)
            {
              token[ptr++]=ch;
              if (ch=='=') token_type=TOKEN_EQUALITY;
            }
              else
            if (ch1=='=')
            {
              token[ptr++]=ch;
              token_type=TOKEN_EQUALITY;
            }
              else
            {
              if (ch!='=') token_type=TOKEN_EQUALITY;
              unget_next_char(asm_context, ch1);
            }
          }
            else
          if (ch=='&' || ch=='|')
          {
            int ch1;
            ch1=get_next_char(asm_context);
            if (ch1==ch)
            {
              token[ptr++]=ch;
            }
              else
            {
              unget_next_char(asm_context, ch1);
            }
          }
          break;
        }
          else
        {
          unget_next_char(asm_context, ch);
          break;
        }
      }

      token[ptr++]=ch;
    }
  }

  token[ptr]=0;

  if (ptr>=len)
  {
    printf("get_token.c: line=%d - Internal error\n", __LINE__);
    exit(1);
  }

  if (token_type==TOKEN_TICKED && ptr==1) { token_type=TOKEN_NUMBER; }

  if (IS_TOKEN(token, '$'))
  {
    sprintf(token, "%d", asm_context->address);
    token_type=TOKEN_NUMBER;
  }

  if (token_type==TOKEN_STRING)
  {
    int param_count=0;
    char *define=defines_heap_lookup(&asm_context->defines_heap, token, &param_count);
    int address=address_heap_lookup(&asm_context->address_heap, token);

    if (address!=-1 && asm_context->parsing_ifdef==0)
    {
      sprintf(token, "%d", address);
      token_type=TOKEN_NUMBER;
    }
      else
    if (define!=NULL && asm_context->parsing_ifdef==0)
    {
#ifdef DEBUG
printf("debug> '%s' is a define.  param_count=%d\n", token, param_count);
#endif
      if (param_count==0)
      {
        defines_heap_push_define(&asm_context->defines_heap, define);
      }
        else
      {
        char *expanded=expand_params(asm_context, define, param_count);
        if (expanded==NULL) return TOKEN_EOF;
        defines_heap_push_define(&asm_context->defines_heap, expanded);
      }

      asm_context->unget_stack[++asm_context->unget_stack_ptr]=asm_context->unget_ptr;
#ifdef DEBUG
//printf("debug> unget_stack_ptr=%d unget_ptr=%d\n", asm_context->unget_stack_ptr, asm_context->unget_ptr);
#endif

      token_type=get_token(asm_context, token, len);
#ifdef DEBUG
//printf("debug> expanding.. '%s'\n", token);
#endif
    }
      else
    if (token[0]=='0' && token[1]=='x')
    {
      int num=0;
      char *s=token+2;
      while(*s!=0)
      {
        if (*s>='0' && *s<='9')
        { num=(num<<4)|((*s)-'0'); }
          else
        if (*s>='a' && *s<='f')
        { num=(num<<4)|((*s)-'a'+10); }
          else
        if (*s>='A' && *s<='F')
        { num=(num<<4)|((*s)-'A'+10); }
          else
        { return token_type; }
        s++;
      }
      sprintf(token, "%d", num);
      token_type=TOKEN_NUMBER;
    }
      else
    if ((token[0]>='0' && token[0]<='9') && tolower(token[ptr-1])=='h')
    {
      int num=0;
      char *s=token;
      while(*s!=0 && s!=token+ptr-1)
      {
        if (*s>='0' && *s<='9')
        { num=(num<<4)|((*s)-'0'); }
          else
        if (*s>='a' && *s<='f')
        { num=(num<<4)|((*s)-'a'+10); }
          else
        if (*s>='A' && *s<='F')
        { num=(num<<4)|((*s)-'A'+10); }
          else
        { return token_type; }

        s++;
      }
      sprintf(token, "%d", num);
      token_type=TOKEN_NUMBER;
    }
      else
    if ((token[0]>='0' && token[0]<='7') && tolower(token[ptr-1])=='q')
    {
      int num=0;
      char *s=token;
      while(*s!=0 && s!=token+ptr-1)
      {
        if (*s>='0' && *s<='7')
        { num=(num<<3)|((*s)-'0'); }
          else
        { return token_type; }

        s++;
      }
      sprintf(token, "%d", num);
      token_type=TOKEN_NUMBER;
    }
      else
    if ((token[0]=='0' || token[0]=='1') && tolower(token[ptr-1])=='b')
    {
      int num=0;
      char *s=token;
      while(*s!=0 && s!=token+ptr-1)
      {
        if (*s=='0')
        { num=num<<1; }
          else
        if (*s=='1')
        { num=(num<<1)|1; }
          else
        { return token_type; }

        s++;
      }
      sprintf(token, "%d", num);
      token_type=TOKEN_NUMBER;
    }
  }

  if (token_type==TOKEN_NUMBER && token[0]=='0' && token[1]!=0)
  {
    // Octal
    int num=0;
    int ptr=0;
    while(token[ptr]!=0)
    {
      num=num<<3;
      if (token[ptr]>='0' && token[ptr]<='7')
      {
        num=num|(token[ptr]-'0');
      }
        else
      {
        printf("Error: invalid digit '%c' in octal constant at %s:%d.\n", token[ptr], asm_context->filename, asm_context->line);
        return TOKEN_EOF;
      }

      ptr++;
    }
    sprintf(token, "%d", num);
  }

  return token_type;
}

void pushback(struct _asm_context *asm_context, char *token, int token_type)
{
  strcpy(asm_context->pushback, token);
  asm_context->pushback_type=token_type;
}

// Returns the number of chars eaten by this function or 0 for ZOMG
int escape_char(struct _asm_context *asm_context, unsigned char *s)
{
int ptr=1;

  switch(s[ptr])
  {
    case '\\':
      s[ptr++]='\\';
      break;
    case 'n':
      s[ptr++]='\n';
      break;
    case 'r':
      s[ptr++]='\r';
      break;
    case 't':
      s[ptr++]='\t';
      break;
    case '0':
      s[ptr++]='\0';
      break;
    case 'x':
      // FIXME - probably need to add this...
    default:
      printf("Unknown escape char '\\%c' on line %s:%d.\n", s[ptr], asm_context->filename, asm_context->line);
      return 0;
  }

  return ptr-1;
}

