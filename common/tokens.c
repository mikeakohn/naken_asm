/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "common/assembler.h"
#include "common/macros.h"
#include "common/symbols.h"
#include "common/tokens.h"

//#define assert(a) if (! a) { printf("assert failed on line %s:%d\n", __FILE__, __LINE__); raise(SIGABRT); }

int tokens_open_file(struct _asm_context *asm_context, char *filename)
{
  asm_context->in = fopen(filename, "rb");

  if (asm_context->in == NULL)
  {
    return -1;
  }

  asm_context->filename = filename;

  return 0;
}

void tokens_open_buffer(struct _asm_context *asm_context, const char *buffer)
{
  asm_context->token_buffer.code = buffer;
  asm_context->token_buffer.ptr = 0;
}

void tokens_close(struct _asm_context *asm_context)
{
  if (asm_context->in != NULL)
  {
    fclose(asm_context->in);
  }
}

void tokens_reset(struct _asm_context *asm_context)
{
  if (asm_context->in != NULL)
  {
    fseek(asm_context->in, 0, SEEK_SET);
  }

  asm_context->token_buffer.ptr = 0;

  asm_context->line = 1;
  asm_context->pushback[0] = 0;
  asm_context->pushback2[0] = 0;
  asm_context->unget[0] = 0;
  asm_context->unget_ptr = 0;
  asm_context->unget_stack_ptr = 0;
  asm_context->unget_stack[0] = 0;
}

static int tokens_hex_string_to_int(char *s, uint64_t *num, int prefixed)
{
  uint64_t n = 0;

  while(*s != 0 && (*s != 'h' && *s != 'H'))
  {
    if (*s >= '0' && *s <= '9')
    { n = (n << 4) | ((*s) - '0'); }
      else
    if (*s >= 'a' && *s <= 'f')
    { n = (n << 4) | ((*s) - 'a' + 10); }
      else
    if (*s >= 'A' && *s <= 'F')
    { n = (n << 4) | ((*s) - 'A' + 10); }
      else
    if (*s != '_')
    { return -1; }

    s++;
  }

  if ((*s == 'h' && *s =='H') && prefixed == 1) { return -1; }

  *num = n;

  return 0;
}

static int tokens_octal_string_to_int(char *s, uint64_t *num)
{
  uint64_t n = 0;

  while(*s != 0 && (*s != 'q' && *s != 'Q'))
  {
    if (*s >= '0' && *s <= '7')
    { n = (n << 3) | ((*s) - '0'); }
      else
    if (*s != '_')
    { return -1; }

    s++;
  }

  *num = n;

  return 0;
}

static int tokens_binary_string_to_int(char *s, uint64_t *num, int prefixed)
{
  int n = 0;

  while(*s!=0 && (*s != 'b' && *s != 'B'))
  {
    if (*s == '0')
    { n = n << 1; }
      else
    if (*s == '1')
    { n = (n << 1) | 1; }
      else
    if (*s != '_')
    { return -1; }

    s++;
  }

  if ((*s == 'b' && *s =='B') && prefixed == 1) { return -1; }

  *num = n;

  return 0;
}

int tokens_get_char(struct _asm_context *asm_context)
{
  int ch;

#ifdef DEBUG
//printf("debug> tokens_get_char()\n");
#endif

  // Check if something need to be ungetted
  if (asm_context->unget_ptr > asm_context->unget_stack[asm_context->unget_stack_ptr])
  {
#ifdef DEBUG
//printf("debug> tokens_get_char(?) ungetc %d %d '%c'\n", asm_context->unget_stack_ptr, asm_context->unget_stack[asm_context->unget_stack_ptr], asm_context->unget[asm_context->unget_ptr-1]);
#endif
    return asm_context->unget[--asm_context->unget_ptr];
  }

  ch = macros_get_char(asm_context);

  // Check if defines is empty
  if (ch == CHAR_EOF)
  {
    if (asm_context->unget_ptr > asm_context->unget_stack[asm_context->unget_stack_ptr])
    {
#ifdef DEBUG
//printf("debug> tokens_get_char(FILE ungetc %d %d '%c'\n", asm_context->unget_stack_ptr, asm_context->unget_stack[asm_context->unget_stack_ptr], asm_context->unget[asm_context->unget_ptr-1]);
#endif
      return asm_context->unget[--asm_context->unget_ptr];
    }
#ifdef DEBUG
//printf("debug> tokens_get_char(FILE)='%c'\n", ch);
#endif

    // Why do people still use DOS :(
    do
    {
      if (asm_context->token_buffer.code != NULL)
      {
        ch = asm_context->token_buffer.code[asm_context->token_buffer.ptr];
        if (ch == 0) { ch = EOF; }
        else { asm_context->token_buffer.ptr++; }
      }
        else
      {
        ch = getc(asm_context->in);
      }
    } while(ch == '\r');

    if (asm_context->list != NULL && asm_context->write_list_file == 1)
    {
      if (ch != EOF) { putc(ch, asm_context->list); }
    }
  }
    else
  {
#ifdef DEBUG
//printf("debug> tokens_get_char(DEFINE)='%c'\n", ch);
#endif
  }

  return ch;
}

int tokens_unget_char(struct _asm_context *asm_context, int ch)
{
  asm_context->unget[asm_context->unget_ptr++] = ch;
  return 0;
}

int tokens_get(struct _asm_context *asm_context, char *token, int len)
{
  int token_type = TOKEN_EOF;
  int ch;
  int ptr = 0;

#ifdef DEBUG
//printf("Enter tokens_get()\n");
#endif

  token[0] = 0;

  if (asm_context->pushback2[0] != 0)
  {
    strcpy(token, asm_context->pushback2);
    asm_context->pushback2[0] = 0;
    return asm_context->pushback2_type;
  }

  if (asm_context->pushback[0] != 0)
  {
    strcpy(token, asm_context->pushback);
    asm_context->pushback[0] = 0;
    return asm_context->pushback_type;
  }

  while(1)
  {
#ifdef DEBUG
//printf("debug> tokens_get, grabbing next char ptr=%d\n", ptr);
#endif
    ch = tokens_get_char(asm_context);
#ifdef DEBUG
//printf("debug> getc()='%c'  ptr=%d  token='%s'\n", ch, ptr, token);
#endif

    if (token_type == TOKEN_DOLLAR)
    {
      if ((ch >= '0' && ch <= '9') ||
          (ch >= 'a' && ch <= 'f') ||
          (ch >= 'A' && ch <= 'F'))
      {
        token[0] = '0';
        token[1] = 'x';
        ptr = 2;
      }
        else
      if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')))
      {
        tokens_unget_char(asm_context, ch);
        break;
      }

      token_type=TOKEN_STRING;
    }

    if (ch == ';')
    {
      if (ptr != 0)
      {
        tokens_unget_char(asm_context, ch);
        break;
        //int n;
        //for(n=0; n<ptr; n++) printf("%c\n", token[n]);
        //printf("\n");
      }

      while(1)
      { ch = tokens_get_char(asm_context); if (ch == '\n' || ch == EOF) break; }

      assert(ptr == 0);
      token[0] = '\n';
      token[1] = 0;
      return TOKEN_EOL;
    }

    // Nasty, but some CPU's like Z80 need this
    if (ch == '\'')
    {
      if (asm_context->can_tick_end_string &&
          //ch=='\'' &&
          token_type == TOKEN_STRING)
      {
        token[ptr++] = ch;
        break;
      }
    }

    if (ch == '"' || ch == '\'')
    {
      char quote = ch;
      if (ch == '"' ) { token_type = TOKEN_QUOTED; }
      else { token_type = TOKEN_TICKED; }

      while(1)
      {
        ch = tokens_get_char(asm_context);
        if (ch == quote)
        {
          break;
        }

        if (ch == '\\')
        {
          ch = tokens_get_char(asm_context);
          if (ch == 'n') { ch = '\n'; }
          else if (ch == 'r') { ch = '\r'; }
          else if (ch == 't') { ch = '\t'; }
        }

        token[ptr++] = ch;
        if (ptr >= len || (token_type == TOKEN_TICKED && ptr > 1))
        {
          print_error("Unterminated quote", asm_context);
          exit(1);
        }
      }

      break;
    }

    if (ch == '\n' || ch == ' ' || ch == '\t' || ch == EOF)
    {
      if (ch == '\n')
      {
        if (ptr == 0)
        {
          token[0] = '\n';
          token[1] = 0;
          return TOKEN_EOL;
        }
          else
        { tokens_unget_char(asm_context, ch); }
      }

      if (ptr == 0)
      {
        if (ch == EOF) break;
        continue;
      }

      if (ptr >= len)
      {
        print_error_internal(asm_context, __FILE__, __LINE__);
        exit(1);
      }

      break;
    }
      else
    {
      if (ch == '#')
      {
        token_type = TOKEN_POUND;
        token[ptr++] = ch;
        break;
      }

      if (ptr == 0 && ch == '$')
      {
        if (asm_context->is_dollar_hex) { token_type = TOKEN_DOLLAR; }
        else { token_type = TOKEN_STRING; }
        token[ptr++] = ch;
        continue;
      }

      if (ch == ':')
      {
        if (token_type == TOKEN_STRING)
        {
          token_type = TOKEN_LABEL;
          break;
        }
      }

      if (token_type == TOKEN_NUMBER && ch == '_')
      {
        // Let's allow the user to type stuff like 1111_1011b
        continue;
      }

      if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
      {
        if (ptr == 0)
        {
          token_type = TOKEN_STRING;
        }
          else
        if (token_type == TOKEN_NUMBER)
        {
          // Why did I do this?
          token_type = TOKEN_STRING;
        }
          else
        if (token_type == TOKEN_FLOAT)
        {
          tokens_unget_char(asm_context, ch);
          break;
        }
      }
        else
      if (ch >= '0' && ch <= '9')
      {
        if (ptr == 0)
        {
          token_type = TOKEN_NUMBER;
        }
      }
        else
      if (ch == '.' && token_type == TOKEN_NUMBER)
      {
        //token[ptr++] = ch;
        token_type = TOKEN_FLOAT;
      }
        else
      {
        if (ptr == 0)
        {
          token_type = TOKEN_SYMBOL;
          token[ptr++] = ch;
          if (ch == '/')
          {
            ch = tokens_get_char(asm_context);
            if (ch == '*')
            {
              ptr = 0;
              token[0] = 0;

              while(1)
              {
                ch = tokens_get_char(asm_context);
                if (ch == EOF)
                {
                  print_error("Unterminated comment", asm_context);
                  return TOKEN_EOF;
                }

                if (ch == '\n')
                {
                  asm_context->line++;
                }
                  else
                if (ch== '*')
                {
                  ch = tokens_get_char(asm_context);
                  if (ch == '/') break;
                  tokens_unget_char(asm_context, ch);
                }
              }

              continue;
            }
              else
            if (ch == '/')
            {
              while(1)
              {
                ch = tokens_get_char(asm_context);
                if (ch == '\n' || ch == EOF) break;
              }

              token[0] = '\n';
              token[1] = 0;
              return TOKEN_EOL;
            }
              else
            {
              tokens_unget_char(asm_context, ch);
              break;
            }
          }
            else
          if (ch == '>' || ch == '<' || ch == '=')
          {
            int ch1;
            ch1 = tokens_get_char(asm_context);
            if (ch1 == ch)
            {
              token[ptr++] = ch;
              if (ch == '=') token_type=TOKEN_EQUALITY;
            }
              else
            if (ch1 == '=')
            {
              token[ptr++] = ch;
              token_type = TOKEN_EQUALITY;
            }
              else
            {
              if (ch!='=') token_type = TOKEN_EQUALITY;
              tokens_unget_char(asm_context, ch1);
            }
          }
            else
          if (ch == '&' || ch == '|')
          {
            int ch1;
            ch1 = tokens_get_char(asm_context);
            if (ch1 == ch)
            {
              token[ptr++] = ch;
            }
              else
            {
              tokens_unget_char(asm_context, ch1);
            }
          }
          break;
        }
          else
        {
          tokens_unget_char(asm_context, ch);
          break;
        }
      }

      token[ptr++] = ch;
    }
  }

  token[ptr] = 0;

  if (token_type == TOKEN_FLOAT)
  {
    if (token[ptr-1] == '.')
    {
      tokens_unget_char(asm_context, '.');
      token_type = TOKEN_NUMBER;
      token[--ptr] = 0;
    }
  }

  if (ptr >= len)
  {
    print_error_internal(asm_context, __FILE__, __LINE__);
    exit(1);
  }

  if (token_type == TOKEN_TICKED && ptr == 1)
  {
    ch = token[0];
    sprintf(token, "%d", ch);
    token_type = TOKEN_NUMBER;
  }

  if (IS_TOKEN(token, '$'))
  {
    sprintf(token, "%d", asm_context->address);
    token_type = TOKEN_NUMBER;
  }

  if (token_type == TOKEN_STRING)
  {
    int param_count = 0;
    char *macro = macros_lookup(&asm_context->macros, token, &param_count);
    uint32_t address;
    int ret = -1;

    if (asm_context->no_symbols == 0)
    {
      ret = symbols_lookup(&asm_context->symbols, token, &address);
    }
      else
    {
      ret = -1;
    }

    if (ret == 0 && asm_context->parsing_ifdef == 0)
    {
      sprintf(token, "%d", address);
      token_type = TOKEN_NUMBER;
    }
      else
    if (macro != NULL && asm_context->parsing_ifdef == 0)
    {
#ifdef DEBUG
printf("debug> '%s' is a macro.  param_count=%d\n", token, param_count);
#endif
      if (param_count == 0)
      {
        macros_push_define(&asm_context->macros, macro);
      }
        else
      {
        char *expanded = macros_expand_params(asm_context, macro, param_count);
        if (expanded == NULL) { return TOKEN_EOF; }
        macros_push_define(&asm_context->macros, expanded);
      }

      asm_context->unget_stack[++asm_context->unget_stack_ptr] = asm_context->unget_ptr;

#ifdef DEBUG
//printf("debug> unget_stack_ptr=%d unget_ptr=%d\n", asm_context->unget_stack_ptr, asm_context->unget_ptr);
#endif

      token_type = tokens_get(asm_context, token, len);
#ifdef DEBUG
//printf("debug> expanding.. '%s'\n", token);
#endif
    }
      else
    if (token[0] == '0' && token[1] == 'x')
    {
      // If token starts with 0x it's probably hex
      uint64_t num;
      if (tokens_hex_string_to_int(token + 2, &num, 1) != 0) { return token_type; }
      sprintf(token, "%" PRId64, num);
      token_type = TOKEN_NUMBER;
    }
      else
    if (token[0] == '0' && token[1] == 'b')
    {
      // If token starts with 0b it's probably binary
      uint64_t num;
      if (tokens_binary_string_to_int(token + 2, &num, 1) != 0) { return token_type; }
      sprintf(token, "%" PRId64, num);
      token_type = TOKEN_NUMBER;
    }
      else
    if ((token[0] >= '0' && token[0] <= '9') && tolower(token[ptr-1]) == 'h')
    {
      // If token starts with a number and ends with a h it's probably hex
      uint64_t num;
      if (tokens_hex_string_to_int(token, &num, 0) != 0) { return token_type; }
      sprintf(token, "%" PRId64, num);
      token_type = TOKEN_NUMBER;
    }
      else
    if ((token[0] >= '0' && token[0] <= '7') && tolower(token[ptr-1]) == 'q')
    {
      // If token starts with a number and ends with a q it's octal
      uint64_t num;
      if (tokens_octal_string_to_int(token, &num) != 0) { return token_type; }
      sprintf(token, "%" PRId64, num);
      token_type = TOKEN_NUMBER;
    }
      else
    if ((token[0] == '0' || token[0] == '1') && tolower(token[ptr-1]) == 'b')
    {
      // If token starts with a number and ends with a b it's probably binary
      uint64_t num;
      if (tokens_binary_string_to_int(token, &num, 0) != 0) { return token_type; }
      sprintf(token, "%" PRId64, num);
      token_type = TOKEN_NUMBER;
    }
  }

  if (token_type == TOKEN_NUMBER && token[0] == '0' && token[1] != 0)
  {
    // If token is a number and starts with a 0 it's octal
    uint64_t num;
    if (tokens_octal_string_to_int(token, &num) != 0) { return token_type; }
    sprintf(token, "%" PRId64, num);
    token_type = TOKEN_NUMBER;
  }

  //printf("next token: %s\n", token);

  return token_type;
}

void tokens_push(struct _asm_context *asm_context, char *token, int token_type)
{
  if (asm_context->pushback[0] == 0)
  {
    strcpy(asm_context->pushback, token);
    asm_context->pushback_type = token_type;
    return;
  }

  strcpy(asm_context->pushback2, token);
  asm_context->pushback2_type = token_type;
}

// Returns the number of chars eaten by this function or 0 for error
int tokens_escape_char(struct _asm_context *asm_context, unsigned char *s)
{
  int ptr = 1;

  switch(s[ptr])
  {
    case '\\':
      s[ptr++] = '\\';
      break;
    case 'n':
      s[ptr++] = '\n';
      break;
    case 'r':
      s[ptr++] = '\r';
      break;
    case 't':
      s[ptr++] = '\t';
      break;
    case '0':
      s[ptr++] = '\0';
      break;
    case 'x':
      // FIXME - probably need to add this...
    default:
      printf("Unknown escape char '\\%c' on line %s:%d.\n", s[ptr], asm_context->filename, asm_context->line);
      return 0;
  }

  return ptr - 1;
}

