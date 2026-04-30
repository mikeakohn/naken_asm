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
#include <stdint.h>
#include <string.h>

#include "imports_ar.h"
#include "imports_obj.h"
#include "Linker.h"

Linker::Linker() :
  imports                 (NULL),
  symbol_list_buffer      (NULL),
  symbol_list_buffer_size (0),
  symbol_list_buffer_end  (0)
{
}

Linker::~Linker()
{
  Imports *imports = this->imports;

  while (imports != NULL)
  {
    Imports *curr = imports;
    imports = imports->next;
    free(curr);
  }

  if (symbol_list_buffer != NULL)
  {
    free(symbol_list_buffer);
  }
}

int Linker::add_file(const char *filename)
{
  FILE *fp;
  int type = -1, n;

  n = strlen(filename);

  while (n >= 0)
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

  Imports *imports = (Imports *)malloc(sizeof(Imports) + n);

  imports->next = this->imports;
  imports->size = n;
  imports->type = type;

  if (fread(imports->code, n, 1, fp) != 1)
  {
    printf("Error: Couldn't read file %s\n", filename);
    free(imports);
    fclose(fp);
    return -2;
  }

  if (verify_import(imports) != 0)
  {
    printf("Error: Not a supported file %s\n", filename);
    free(imports);
    fclose(fp);
    return -2;
  }

  this->imports = imports;

  fclose(fp);

  return 0;
}

int Linker::search_code_from_symbol(const char *symbol)
{
  //if (this == NULL) { return -1; }

  Imports *imports = this->imports;
  uint32_t function_offset;
  uint32_t function_size;
  uint32_t file_offset;
  int ret;

  // If this symbol is already in the list, then don't search it again.
  if (get_from_symbol_list(symbol) != NULL) { return 1; }

  // FIXME: Deal with duplicate symbols in .a / .o files. Currently
  // this will find the first match and not check to see if it's a dup.

  while (imports != NULL)
  {
    if (imports->type == IMPORT_TYPE_AR)
    {
      uint8_t *obj_file;
      uint32_t obj_size;

      ret = imports_ar_find_code_from_symbol(
        imports->code,
        imports->size,
        symbol,
        &function_offset,
        &function_size,
        &file_offset,
        &obj_file,
        &obj_size);
    }
      else
    if (imports->type == IMPORT_TYPE_OBJ)
    {
      ret = imports_obj_find_code_from_symbol(
        imports->code,
        imports->size,
        symbol,
        &function_offset,
        &function_size,
        &file_offset);
    }

    if (ret != -1)
    {
      add_to_symbol_list(imports, symbol);

      return 1;
    }

    imports = imports->next;
  }

  return 0;
}

uint8_t *Linker::get_code_from_symbol(
  Imports **ret_imports,
  const char *symbol,
  uint32_t *function_size,
  uint32_t *function_offset,
  uint8_t **obj_file,
  uint32_t *obj_size)
{
  //if (this == NULL) { return NULL; }

  *ret_imports = NULL;

  Imports *imports;
  uint32_t file_offset;
  int ret;

  // If this symbol is already in the list, then point directly to it.
  imports = get_from_symbol_list(symbol);

  if (get_from_symbol_list(symbol) != NULL)
  {
    imports = this->imports;
  }

  while (imports != NULL)
  {
    if (imports->type == IMPORT_TYPE_AR)
    {
      ret = imports_ar_find_code_from_symbol(
        imports->code,
        imports->size,
        symbol,
        function_size,
        function_offset,
        &file_offset,
        obj_file,
        obj_size);
    }
      else
    if (imports->type == IMPORT_TYPE_OBJ)
    {
      ret = imports_obj_find_code_from_symbol(
        imports->code,
        imports->size,
        symbol,
        function_offset,
        function_size,
        &file_offset);

      *obj_file = imports->code;
      *obj_size = imports->size;
    }

    if (ret != -1)
    {
      *ret_imports = imports;
      return imports->code + file_offset;
    }

    imports = imports->next;
  }

  return NULL;
}

const char *Linker::find_name_from_offset(uint32_t offset)
{
  //if (this == NULL) { return NULL; }

  Imports *imports = this->imports;
  const char *name = NULL;

  while (imports != NULL)
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
      const uint32_t local_offset = -1;

      name = imports_obj_find_name_from_offset(
        imports->code,
        imports->size,
        offset,
        local_offset);
    }

    if (name != NULL) { return name; }

    imports = imports->next;
  }

  return NULL;
}

int Linker::get_symbol_count()
{
  SymbolList *symbol_list;
  uint8_t *buffer = symbol_list_buffer;
  int ptr = 0, end = symbol_list_buffer_end, count = 0;

  while (ptr < end)
  {
    symbol_list = (SymbolList *)(buffer + ptr);
    ptr += sizeof(SymbolList) + strlen(symbol_list->name) + 1;
    count++;
  }

  return count;
}

const char *Linker::get_symbol_at_index(int index)
{
  SymbolList *symbol_list;
  uint8_t *buffer = symbol_list_buffer;
  int ptr = 0, end = symbol_list_buffer_end, count = 0;

  while (ptr < end)
  {
    symbol_list = (SymbolList *)(buffer + ptr);

    if (count == index)
    {
      return symbol_list->name;
    }

    ptr += sizeof(SymbolList) + strlen(symbol_list->name) + 1;
    count++;
  }

  return NULL;
}

void Linker::print_symbol_list()
{
  SymbolList *symbol_list;
  uint8_t *buffer = symbol_list_buffer;
  int ptr = 0, end = symbol_list_buffer_end, count = 0;

  printf(" -- linker symbol list --\n");

  while (ptr < end)
  {
    symbol_list = (SymbolList *)(buffer + ptr);

    printf(" %d) %p %s\n", count, symbol_list->imports, symbol_list->name);

    ptr += sizeof(SymbolList) + strlen(symbol_list->name) + 1;
    count++;
  }
}

int Linker::verify_import(Imports *imports)
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

Imports *Linker::get_from_symbol_list(const char *name)
{
  SymbolList *symbol_list;
  uint8_t *buffer = symbol_list_buffer;
  uint32_t ptr = 0;

  while (ptr < symbol_list_buffer_end)
  {
    symbol_list = (SymbolList *)(buffer + ptr);

    if (strcmp(name, symbol_list->name) == 0)
    {
      return symbol_list->imports;
    }

    ptr += sizeof(SymbolList) + strlen(symbol_list->name) + 1;
  }

  return NULL;
}

void Linker::add_to_symbol_list(Imports *imports, const char *name)
{
  // Allocate buffer if needed.
  if (symbol_list_buffer == NULL)
  {
    symbol_list_buffer_size = 0x10000;
    symbol_list_buffer = (uint8_t *)malloc(symbol_list_buffer_size);
    symbol_list_buffer[0] = 0;
  }

  // If this symbol is already in the list, then don't add it again.
  //if (get_from_symbol_list(name) != NULL) { return; }

  const int len = sizeof(SymbolList) + strlen(name) + 1;

  if (symbol_list_buffer_end + len >= symbol_list_buffer_size)
  {
    symbol_list_buffer_size += 0x10000;
    symbol_list_buffer =
       (uint8_t *)realloc(symbol_list_buffer, symbol_list_buffer_size);
  }

  SymbolList *symbol_list =
    (SymbolList *)(symbol_list_buffer + symbol_list_buffer_end);

  symbol_list->imports = imports;
  strcpy(symbol_list->name, name);

  symbol_list_buffer_end += len;
}

