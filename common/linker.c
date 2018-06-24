/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2018 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "imports_ar.h"
#include "imports_obj.h"
#include "linker.h"

static int linker_verify_import(struct _imports *imports)
{
  if (imports->type == IMPORT_TYPE_AR)
  {
    if (imports_ar_verify(imports->code, imports->size) != 0)
    {
      return -1;
    }
  }
    else
  if (imports->type == IMPORT_TYPE_OBJ)
  {
    if (imports_obj_verify(imports->code, imports->size) != 0)
    {
      return -1;
    }
  }

  return 0;
}

static struct _imports *linker_get_from_symbol_list(
  struct _linker *linker,
  const char *name)
{
  struct _symbol_list *symbol_list;
  uint8_t *buffer = linker->symbol_list_buffer;
  int ptr = 0;

  while(ptr < linker->symbol_list_buffer_end)
  {
    symbol_list = (struct _symbol_list *)(buffer + ptr);

    ptr += sizeof(struct _symbol_list *) + strlen(symbol_list->name) + 1;
  }

  return NULL;
}

static void linker_add_to_symbol_list(
  struct _linker *linker,
  struct _imports *imports,
  const char *name)
{
  // Allocate buffer if needed.
  if (linker->symbol_list_buffer == NULL)
  {
    linker->symbol_list_buffer_size = 0x10000;
    linker->symbol_list_buffer =
      (uint8_t *)malloc(linker->symbol_list_buffer_size);
    linker->symbol_list_buffer[0] = 0;
  }

  // If this symbol is already in the list, then don't add it again.
  if (linker_get_from_symbol_list(linker, name) != NULL) { return; }

  const int len = sizeof(struct _imports *) + strlen(name) + 1;

  if (linker->symbol_list_buffer_end + len >= linker->symbol_list_buffer_size)
  {
    linker->symbol_list_buffer_size += 0x10000;
    linker->symbol_list_buffer =
      (uint8_t *)realloc(linker->symbol_list_buffer,
                         linker->symbol_list_buffer_size);
  }

  struct _symbol_list *symbol_list =
    (struct _symbol_list *)(linker->symbol_list_buffer + linker->symbol_list_buffer_end);

  symbol_list->imports = imports;
  strcpy(symbol_list->name, name);

  linker->symbol_list_buffer_end += len;
}

int linker_add_file(struct _linker *linker, const char *filename)
{
  FILE *fp;
  int type = -1, n;

  n = strlen(filename);

  while(n >= 0)
  {
    n--;
    if (filename[n] == '.') { break; }
  }

  if (strcmp(filename + n, ".a") == 0)
  {
    type = IMPORT_TYPE_AR;
  }
    else
  if (strcmp(filename + n, ".o") == 0)
  {
    type = IMPORT_TYPE_OBJ;
  }
    else
  {
    return -1;
  }

  fp = fopen(filename, "rb");

  if (fp == NULL)
  {
    printf("Error: File not found %s\n", filename);
    return -2;
  }

  fseek(fp, 0, SEEK_END);
  n = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  struct _imports *imports = (struct _imports *)malloc(sizeof(struct _imports) + n);

  imports->next = linker->imports;
  imports->size = n;
  imports->type = type;

  if (fread(imports->code, n, 1, fp) != 1)
  {
    printf("Error: Couldn't read file %s\n", filename);
    free(imports);
    fclose(fp);
    return -2;
  }

  if (linker_verify_import(imports) != 0)
  {
    printf("Error: Not a supported file %s\n", filename);
    free(imports);
    fclose(fp);
    return -2;
  }

  linker->imports = imports;

  fclose(fp);

  return 0;
}

uint32_t linker_find_code_from_symbol(
  struct _linker *linker,
  const char *symbol,
  uint32_t *function_size,
  uint32_t *file_offset)
{
  if (linker == NULL) { return -1; }

  struct _imports *imports = linker->imports;
  uint32_t offset;

  // FIXME: Deal with duplicate symbols.

  while(imports != NULL)
  {
    if (imports->type == IMPORT_TYPE_AR)
    {
      offset = imports_ar_find_code_from_symbol(
        imports->code,
        imports->size,
        symbol,
        function_size,
        file_offset);
    }
      else
    if (imports->type == IMPORT_TYPE_OBJ)
    {
      offset = imports_obj_find_code_from_symbol(
        imports->code,
        imports->size,
        symbol,
        function_size,
        file_offset);
    }

    if (offset != -1) { return offset; }

    imports = imports->next;
  }

  return -1;
}

const char *linker_find_name_from_offset(
  struct _linker *linker,
  uint32_t offset)
{
  if (linker == NULL) { return NULL; }

  struct _imports *imports = linker->imports;
  const char *name;

  while(imports != NULL)
  {
    if (imports->type == IMPORT_TYPE_AR)
    {
      name = imports_ar_find_name_from_offset(
        imports->code,
        imports->size,
        offset);
    }
      else
    if (imports->type == IMPORT_TYPE_OBJ)
    {
      name = imports_obj_find_name_from_offset(
        imports->code,
        imports->size,
        offset);
    }

    if (name != NULL) { return name; }

    imports = imports->next;
  }

  return NULL;
}

void linker_free(struct _linker *linker)
{
  if (linker == NULL) { return; }

  struct _imports *imports = linker->imports;

  while(imports != NULL)
  {
    struct _imports *curr = imports;
    imports = imports->next;
    free(curr);
  }

  if (linker->symbol_list_buffer != NULL)
  {
    free(linker->symbol_list_buffer);
  }

  free(linker);
}

