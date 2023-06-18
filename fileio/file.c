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

int file_read(
  const char *filename,
  Memory *memory,
  Symbols *symbols,
  int *file_type,
  uint8_t *cpu_type,
  uint32_t start_address)
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

  *cpu_type = -1;

  if (*file_type != FILE_TYPE_AUTO)
  {
    switch (*file_type)
    {
      case FILE_TYPE_HEX:
        read_hex(filename, memory);
        break;
      case FILE_TYPE_BIN:
        read_bin(filename, memory, start_address);
        break;
      case FILE_TYPE_ELF:
        read_elf(filename, memory, cpu_type, symbols);
        break;
      case FILE_TYPE_SREC:
        read_srec(filename, memory);
        break;
      case FILE_TYPE_WDC:
        read_wdc(filename, memory);
        break;
      case FILE_TYPE_AMIGA:
        read_amiga(filename, memory);
        break;
      default:
        return -1;
    }
  }

  return -1;
}

