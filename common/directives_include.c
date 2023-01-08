/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/directives_include.h"
#include "common/tokens.h"
#include "common/print_error.h"

int include_add_path(struct _asm_context *asm_context, char *paths)
{
  int ptr = 0;
  int n = 0;
  char *s;

  s = asm_context->include_path;
  while (!(s[ptr] == 0 && s[ptr+1] == 0)) { ptr++; }
  if (ptr != 0) ptr++;

  while (paths[n] != 0)
  {
    if (paths[n] == ':')
    {
      n++;
      s[ptr++] = 0;
    }
      else
    {
      s[ptr++] = paths[n++];
    }

    if (ptr >= INCLUDE_PATH_LEN-1) return -1;
  }

  return 0;
}

int binfile_parse(struct _asm_context *asm_context)
{
  FILE *in;
  char token[TOKENLEN];
  uint8_t buffer[8192];
  //int token_type;
  int len;
  int n;

  if (asm_context->segment == SEGMENT_BSS)
  {
    printf("Error: .bss segment doesn't support initialized data at %s:%d\n",
      asm_context->tokens.filename,
      asm_context->tokens.line);

    return -1;
  }

  tokens_get(asm_context, token, TOKENLEN);
  in = fopen(token, "rb");

  if (in == NULL)
  {
    printf("Cannot open binfile file '%s' at %s:%d\n",
      token,
      asm_context->tokens.filename,
      asm_context->tokens.line);

    return -1;
  }

  while (1)
  {
    len = fread(buffer, 1, sizeof(buffer), in);
    if (len <= 0) break;

    for (n = 0; n < len; n++)
    {
      memory_write_inc(asm_context, buffer[n], DL_DATA);
    }

    asm_context->data_count += len;
  }

  fclose(in);

  return 0;
}

int include_parse(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  //int token_type;
  const char *oldname;
  int oldline;
  FILE *oldfp;
  uint8_t write_list_file;
  int ret;

  tokens_get(asm_context, token, TOKENLEN);
#ifdef DEBUG
printf("including file %s.\n", token);
#endif

  write_list_file = asm_context->write_list_file;
  asm_context->write_list_file = 0;

  oldfp = asm_context->tokens.in;
  oldname = asm_context->tokens.filename;

  if (tokens_open_file(asm_context, token) != 0)
  {
    int ptr = 0;
    char *s = asm_context->include_path;
    char filename[8192];

    while (1)
    {
      if (s[ptr] == 0) { break; }

      if (strlen(token) + strlen(s + ptr) < 1022)
      {
        snprintf(filename, sizeof(filename), "%s/%s", s + ptr, token);
#ifdef DEBUG
        printf("Trying %s\n", filename);
#endif
        if (tokens_open_file(asm_context, filename) == 0) { break; }

        if (asm_context->cpu_list_index != -1)
        {
          snprintf(filename, sizeof(filename), "%s/%s/%s",
            s + ptr, cpu_list[asm_context->cpu_list_index].name, token);
#ifdef DEBUG
          printf("Trying %s\n", filename);
#endif
          if (tokens_open_file(asm_context, filename) == 0) { break; }
        }
      }

      while (s[ptr] != 0) { ptr++; }
      ptr++;
    }
  }

  if (asm_context->tokens.in == NULL)
  {
    printf("Cannot open include file '%s' at %s:%d\n",
      token, asm_context->tokens.filename, asm_context->tokens.line);
    ret = -1;
  }
    else
  {
    oldline = asm_context->tokens.line;

    asm_context->tokens.filename = token;
    asm_context->tokens.line = 1;

    ret = assemble(asm_context);

    asm_context->tokens.line = oldline;
  }

  if (asm_context->tokens.in != NULL) { fclose(asm_context->tokens.in); }

  asm_context->tokens.filename = oldname;
  asm_context->tokens.in = oldfp;
  asm_context->write_list_file = write_list_file;

  return ret;
}

