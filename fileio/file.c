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
#include "common/util_context.h"
#include "fileio/file.h"
#include "fileio/read_amiga.h"
#include "fileio/read_bin.h"
#include "fileio/read_elf.h"
#include "fileio/read_hex.h"
#include "fileio/read_srec.h"
#include "fileio/read_ti_txt.h"
#include "fileio/read_wdc.h"
#include "fileio/write_amiga.h"
#include "fileio/write_bin.h"
#include "fileio/write_elf.h"
#include "fileio/write_hex.h"
#include "fileio/write_srec.h"
#include "fileio/write_wdc.h"

int file_write(const char *filename, AsmContext *asm_context, int file_type)
{
  FILE *out = fopen(filename, "wb");

  if (out == NULL) { return -1; }

  if (file_type == FILE_TYPE_HEX)
  {
    write_hex(&asm_context->memory, out);
  }
    else
  if (file_type == FILE_TYPE_BIN)
  {
    write_bin(&asm_context->memory, out);
  }
    else
  if (file_type == FILE_TYPE_SREC)
  {
    write_srec(
      &asm_context->memory,
      out,
      cpu_list[asm_context->cpu_list_index].srec_size);
  }
    else
  if (file_type == FILE_TYPE_ELF)
  {
    write_elf(
      &asm_context->memory,
      out,
      &asm_context->symbols,
      asm_context->tokens.filename,
      asm_context->cpu_type,
      cpu_list[asm_context->cpu_list_index].alignment);
  }
    else
  if (file_type == FILE_TYPE_WDC)
  {
    write_wdc(&asm_context->memory, out);
  }
    else
  if (file_type == FILE_TYPE_AMIGA)
  {
    write_amiga(&asm_context->memory, out);
  }

  fclose(out);

  return 0;
}

const char *file_get_file_type_name(int file_type)
{
  switch (file_type)
  {
    case FILE_TYPE_HEX:    return "hex";
    case FILE_TYPE_BIN:    return "bin";
    case FILE_TYPE_ELF:    return "elf";
    case FILE_TYPE_SREC:   return "srec";
    case FILE_TYPE_WDC:    return "wdc";
    case FILE_TYPE_AMIGA:  return "amiga";
    case FILE_TYPE_TI_TXT: return "ti_txt";
  }

  return "???";
}

static int check_magic(const char *filename, const char *magic)
{
  int value = 0;
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) { return 0; }

  char buffer[4];
  int length = fread(buffer, 1, sizeof(buffer), fp);

  if (length == sizeof(buffer))
  {
    if (memcmp(buffer, magic, sizeof(buffer)) == 0) { value = 1; }
  }

  fclose(fp);
  return value;
}

static int is_elf(const char *filename)
{
  return check_magic(filename, "\x7f" "ELF");
}

static int is_amiga(const char *filename)
{
  return check_magic(filename, "\x00" "\x00" "\x03" "\xf3");
}

static int is_hex(const char *filename)
{
  int value = 0;
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) { return 0; }

  if (getc(fp) == ':') { value = 1; }
  fclose(fp);

  return value;
}

static const char *get_extension(const char *filename)
{
  const char *extension = filename + strlen(filename) - 1;

  while (extension != filename)
  {
    if (*extension == '.')
    {
      extension++;
      break;
    }

    extension--;
  }

  return extension;
}

static int get_file_type(const char *filename)
{
  const char *extension = get_extension(filename);

  if (strcasecmp(extension, "hex")  == 0) { return FILE_TYPE_HEX; }
  if (strcasecmp(extension, "wdc")  == 0) { return FILE_TYPE_WDC; }
  if (strcasecmp(extension, "srec") == 0) { return FILE_TYPE_SREC; }
  if (strcasecmp(extension, "txt")  == 0) { return FILE_TYPE_TI_TXT; }

  if (is_elf(filename)   == 1) { return FILE_TYPE_ELF; }
  if (is_amiga(filename) == 1) { return FILE_TYPE_AMIGA; }
  if (is_hex(filename)   == 1) { return FILE_TYPE_HEX; }

  if (strcasecmp(extension, "bin") == 0)  { return FILE_TYPE_BIN; }

  return FILE_TYPE_BIN;
}

int file_read(
  const char *filename,
  UtilContext *util_context,
  int *file_type,
  const char *cpu_name,
  uint32_t start_address)
{
  uint8_t cpu_type = util_context->allow_unknown_cpu == 1 ? CPU_TYPE_IGNORE : 0;

  Memory *memory = &util_context->memory;
  Symbols *symbols = &util_context->symbols;

  if (*file_type == FILE_TYPE_AUTO)
  {
    *file_type = get_file_type(filename);
  }

  int ret = -2;

  switch (*file_type)
  {
    case FILE_TYPE_HEX:
      ret = read_hex(filename, memory);
      break;
    case FILE_TYPE_BIN:
      ret = read_bin(filename, memory, start_address);
      break;
    case FILE_TYPE_ELF:
      ret = read_elf(filename, memory, &cpu_type, symbols);
      break;
    case FILE_TYPE_SREC:
      ret = read_srec(filename, memory);
      break;
    case FILE_TYPE_WDC:
      ret = read_wdc(filename, memory);
      break;
    case FILE_TYPE_AMIGA:
      ret = read_amiga(filename, memory);
      cpu_type = CPU_TYPE_68000;
      break;
    case FILE_TYPE_TI_TXT:
      ret = read_ti_txt(filename, memory);
      break;
    default:
      break;
  }

  // FIXME: read_elf() and maybe others returns the start address or -1.
  if (ret >= 0) { ret = 0; }
  if (ret != 0) { return ret; }

  if (cpu_name != NULL)
  {
    util_set_cpu_by_name(util_context, cpu_name);
  }
    else
  {
    util_set_cpu_by_type(util_context, cpu_type);
  }

  return 0;
}

