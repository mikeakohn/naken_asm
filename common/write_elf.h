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

#ifndef _WRITE_ELF_H
#define _WRITE_ELF_H

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
  unsigned int sh_name;
  unsigned int sh_type;
  unsigned int sh_flags;
  unsigned int sh_addr;
  unsigned int sh_offset;
  unsigned int sh_size;
  unsigned int sh_link;
  unsigned int sh_info;
  unsigned int sh_addralign;
  unsigned int sh_entsize;
};

struct _symtab
{
  unsigned int st_name;
  unsigned int st_value;
  unsigned int st_size;
  char st_info;
  char st_other;
  unsigned short int st_shndx;
};

int write_elf(struct _asm_context *asm_context, FILE *out);

#endif

