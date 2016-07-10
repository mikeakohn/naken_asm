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

#ifndef _WRITE_ELF_H
#define _WRITE_ELF_H

#include "common/symbols.h"

#define ELF_TEXT_MAX 64

struct _sections_offset
{
  long text[ELF_TEXT_MAX];
  long rela_text;
  long data[ELF_TEXT_MAX];
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
};

struct _sections_size
{
  int text[ELF_TEXT_MAX];
  int rela_text;
  int data[ELF_TEXT_MAX];
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
};

struct _shdr
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
};

struct _symtab
{
  uint32_t st_name;
  uint32_t st_value;
  uint32_t st_size;
  char st_info;
  char st_other;
  uint16_t st_shndx;
};

int write_elf(struct _memory *memory, FILE *out, struct _symbols *symbols, const char *filename, int cpu_type);

#endif

