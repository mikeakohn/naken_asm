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

#ifndef NAKEN_ASM_WRITE_ELF_H
#define NAKEN_ASM_WRITE_ELF_H

#include <stdio.h>
#include <stdlib.h>

#include "common/memory.h"
#include "common/symbols.h"

#define ELF_TEXT_MAX 64

typedef struct _sections_offset
{
  long text;
  long rela_text;
  long data;
  long bss;
  long shstrtab;
  long symtab;
  long strtab;
  long comment;
  long debug_line;
  long rela_debug_line;
  long debug_info;
  long rela_debug_info;
  long debug_abbrev;
  long debug_aranges;
  long rela_debug_aranges;
  long arm_attribute;
} SectionsOffset;

typedef struct _sections_size
{
  int text;
  int rela_text;
  int data;
  int bss;
  int shstrtab;
  int symtab;
  int strtab;
  int comment;
  int debug_line;
  int rela_debug_line;
  int debug_info;
  int rela_debug_info;
  int debug_abbrev;
  int debug_aranges;
  int rela_debug_aranges;
  int arm_attribute;
} SectionsSize;

typedef struct _shdr
{
  uint32_t sh_name;
  uint32_t sh_type;
  uint32_t sh_flags;
  uint32_t sh_addr;
  uint32_t sh_offset;
  uint32_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint32_t sh_addralign;
  uint32_t sh_entsize;
} Shdr;

typedef struct _symtab
{
  uint32_t st_name;
  uint32_t st_value;
  uint32_t st_size;
  char st_info;
  char st_other;
  uint16_t st_shndx;
} Symtab;

int write_elf(
  Memory *memory,
  FILE *out,
  Symbols *symbols,
  const char *filename,
  int cpu_type,
  int alignment);

#endif

