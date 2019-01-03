/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "imports_get_int.h"
#include "imports_obj.h"

#define SHT_SYMTAB 0x2
#define SHT_STRTAB 0x3
#define SHT_REL 0x9

//#define DEBUG 1

int imports_obj_verify(const uint8_t *buffer, int file_size)
{
  int i;

  const uint8_t e_ident[] = { 0x7f, 0x45, 0x4c, 0x46, };

  for (i = 0; i < 4; i++)
  {
    if (buffer[i] != e_ident[i]) { return -1; }
  }

  return 0;
}

#ifdef DEBUG
static void imports_obj_elf_print_header32(struct _elf_header32 *elf_header)
{
  printf(" -- elf32 --\n");
  printf("        e_ident=%.4s\n", elf_header->e_ident + 1);
  printf("  e_ident_class=%d\n", elf_header->e_ident_class);
  printf("   e_ident_data=%d\n", elf_header->e_ident_data);
  printf("e_ident_version=%d\n", elf_header->e_ident_version);
  printf("  e_ident_osabi=%d\n", elf_header->e_ident_osabi);
  printf("         e_type=%d\n", get_int16_le(elf_header->e_type));
  printf("      e_machine=%d\n", get_int16_le(elf_header->e_machine));
  printf("      e_version=%d\n", get_int32_le(elf_header->e_machine));
  printf("        e_entry=%d\n", get_int32_le(elf_header->e_entry));
  printf("        e_phoff=%d\n", get_int32_le(elf_header->e_phoff));
  printf("        e_shoff=%d\n", get_int32_le(elf_header->e_shoff));
  printf("       e_ehsize=%d\n", get_int16_le(elf_header->e_ehsize));
  printf("    e_phentsize=%d\n", get_int16_le(elf_header->e_phentsize));
  printf("        e_phnum=%d\n", get_int16_le(elf_header->e_phnum));
  printf("    e_shentsize=%d\n", get_int16_le(elf_header->e_shentsize));
  printf("        e_shnum=%d\n", get_int16_le(elf_header->e_shnum));
  printf("     e_shstrndx=%d\n", get_int16_le(elf_header->e_shstrndx));
}
#endif

#ifdef DEBUG
static void imports_obj_elf_print_section32(
  struct _elf_section32 *section,
  int i,
  const char *name)
{
  printf("------ section %d ------\n", i);
  printf("     sh_name=%d (%s)\n", get_int32_le(section->sh_name), name);
  printf("     sh_type=%d\n", get_int32_le(section->sh_type));
  printf("    sh_flags=%d\n", get_int32_le(section->sh_flags));
  printf("     sh_addr=%d\n", get_int32_le(section->sh_addr));
  printf("   sh_offset=%d\n", get_int32_le(section->sh_offset));
  printf("     sh_size=%d\n", get_int32_le(section->sh_size));
  printf("     sh_link=%d\n", get_int32_le(section->sh_link));
  printf("     sh_info=%d\n", get_int32_le(section->sh_info));
  printf("sh_addralign=%d\n", get_int32_le(section->sh_addralign));
  printf("  sh_entsize=%d\n", get_int32_le(section->sh_entsize));
}
#endif

#if DEBUG
static void imports_obj_elf_print_symbol32(
  struct _elf_symbol32 *symbol,
  const char *name)
{
  printf(" -- symbol --\n");
  printf("st_name=%d %s\n", get_int32_le(symbol->st_name), name);
  printf("st_value=%d\n", get_int32_le(symbol->st_value));
  printf("st_size=%d\n", get_int32_le(symbol->st_size));
  printf("st_info=%d\n", symbol->st_info);
  printf("st_other=%d\n", symbol->st_other);
  printf("st_shndx=%d\n", get_int32_le(symbol->st_shndx));
}
#endif

static int imports_obj_symbol_table_lookup_by_name(
  const uint8_t *symbol_table,
  int symbol_table_size,
  const uint8_t *symbol_string_table,
  int symbol_string_table_size,
  const char *symbol,
  uint32_t *offset,
  uint32_t *function_size)
{
  int ptr = 0;
  struct _elf_symbol32 *elf_symbol32;

  while(ptr < symbol_table_size)
  {
    elf_symbol32 = (struct _elf_symbol32 *)(symbol_table + ptr);

    int st_name = get_int32_le(elf_symbol32->st_name);
    int st_size = get_int32_le(elf_symbol32->st_size);
    const char *name = (const char *)(symbol_string_table + st_name);

    if (st_name > symbol_string_table_size) { st_name = 0; }

#ifdef DEBUG
    imports_obj_elf_print_symbol32(elf_symbol32, name);
#endif

    if (st_size != 0 && strcmp(name, symbol) == 0)
    {
      *function_size = st_size;
      *offset = get_int32_le(elf_symbol32->st_value);
      return 0;
    }

    ptr += 16;
  }

  return -1;
}

static const char *imports_obj_symbol_table_lookup_by_local_offset(
  const uint8_t *symbol_table,
  int symbol_table_size,
  const uint8_t *symbol_string_table,
  int symbol_string_table_size,
  uint32_t offset)
{
  int ptr = 0;
  struct _elf_symbol32 *elf_symbol32;

  while(ptr < symbol_table_size)
  {
    elf_symbol32 = (struct _elf_symbol32 *)(symbol_table + ptr);

    int st_name = get_int32_le(elf_symbol32->st_name);
    int st_value = get_int32_le(elf_symbol32->st_value);
    //int st_size = get_int32_le(elf_symbol32->st_size);
    int st_info = elf_symbol32->st_info;

    if (st_name > symbol_string_table_size) { st_name = 0; }

#ifdef DEBUG
    imports_obj_elf_print_symbol32(elf_symbol32, (const char *)(symbol_string_table + st_name));
#endif

    //if (offset >= st_value && offset < st_value + st_size)
    if (offset == st_value && (st_info >> 4) == 1)
    {
      return (const char *)(symbol_string_table + st_name);
    }

    ptr += 16;
  }

  return NULL;
}

static const char *imports_obj_symbol_table_lookup_by_offset(
  const uint8_t *symbol_table,
  int symbol_table_size,
  const uint8_t *symbol_string_table,
  int symbol_string_table_size,
  const uint8_t *relocation_table,
  int relocation_table_size,
  uint32_t function_offset,
  uint32_t local_offset)
{
  int ptr = 0;
  struct _elf_relocation32 *elf_relocation32;

  while(ptr < relocation_table_size)
  {
    elf_relocation32 = (struct _elf_relocation32 *)(relocation_table + ptr);

    int r_offset = get_int32_le(elf_relocation32->r_offset);
    int r_info = get_int32_le(elf_relocation32->r_info);

#if DEBUG
int r_type = r_info & 0xff;
printf("r_offset=0x%04x offset=0x%04x type=%d\n", r_offset, function_offset, r_type);
#endif

    if (r_offset == function_offset)
    {
      int r_sym = r_info >> 8;

      r_sym *= 16;

      if (r_sym < symbol_table_size)
      {
        int symbol = get_int32_le(symbol_table + r_sym);

        if (symbol < symbol_string_table_size)
        {
          const char *name = (const char *)(symbol_string_table + symbol);

          if (name[0] != 0)
          {
            return name;
          }

          // Please excuse the long function name :(
          name = imports_obj_symbol_table_lookup_by_local_offset(
            symbol_table,
            symbol_table_size,
            symbol_string_table,
            symbol_string_table_size,
            local_offset);

          return name;
        }
      }
    }

    ptr += 8;
  }

  return NULL;
}

int imports_obj_find_code_from_symbol(
  uint8_t *buffer,
  int file_size,
  const char *symbol,
  uint32_t *function_offset,
  uint32_t *function_size,
  uint32_t *file_offset)
{
  struct _elf_header32 *elf_header;
  struct _elf_section32 *section;
  int sh_offset;

  *function_size = 0;
  *file_offset = 0;

  if (imports_obj_verify(buffer, file_size) == -1)
  {
    printf("Not an ELF\n");
    return -1;
  }

  elf_header = (struct _elf_header32 *)buffer;
#if DEBUG
  imports_obj_elf_print_header32(elf_header);
#endif

  int section_count = get_int16_le(elf_header->e_shnum);
  int section_size = get_int16_le(elf_header->e_shentsize);
  int ptr = get_int32_le(elf_header->e_shoff);
  int i;

  const uint8_t *symbol_table = NULL;
  const uint8_t *symbol_string_table = NULL;
  int symbol_table_size = 0;
  int symbol_string_table_size = 0;
  int text_offset = 0;

  // Point to strtab for section names.
  int e_shstrndx = get_int16_le(elf_header->e_shstrndx);
  section = (struct _elf_section32 *)(buffer + ptr + (e_shstrndx * section_size));
  sh_offset = get_int32_le(section->sh_offset);
  const uint8_t *section_string_table = buffer + sh_offset;

  for (i = 0; i < section_count; i++)
  {
    section = (struct _elf_section32 *)(buffer + ptr);

    int sh_name = get_int32_le(section->sh_name);
    int sh_type = get_int32_le(section->sh_type);
    int sh_size = get_int32_le(section->sh_size);
    int sh_offset = get_int32_le(section->sh_offset);
    const char *name = (char *)(section_string_table + sh_name);

#ifdef DEBUG
    imports_obj_elf_print_section32(section, i, name);
#endif

    if (sh_type == SHT_SYMTAB)
    {
      symbol_table = buffer + sh_offset;
      symbol_table_size = sh_size;
    }
      else
    if (sh_type == SHT_STRTAB && strcmp(name, ".strtab") == 0)
    {
      symbol_string_table = buffer + sh_offset;
      symbol_string_table_size = sh_size;
    }
      else
    if (/* sh_type == SHT_STRTAB &&*/ strcmp(name, ".text") == 0)
    {
      text_offset = sh_offset;
    }

    ptr += section_size;
  }

  if (symbol_table != NULL && symbol_string_table != NULL)
  {
    uint32_t offset = 0;

    int ret = imports_obj_symbol_table_lookup_by_name(
      symbol_table,
      symbol_table_size,
      symbol_string_table,
      symbol_string_table_size,
      symbol,
      &offset,
      function_size);

    if (ret == 0)
    {
      *file_offset = text_offset + offset;
      *function_offset = offset;

      return 0;
    }
  }

  return -1;
}

const char *imports_obj_find_name_from_offset(
  const uint8_t *buffer,
  int file_size,
  uint32_t function_offset,
  uint32_t local_offset)
{
  struct _elf_header32 *elf_header;
  struct _elf_section32 *section;
  int sh_offset;

  if (imports_obj_verify(buffer, file_size) == -1)
  {
    printf("Not an ELF\n");
    return NULL;
  }

  elf_header = (struct _elf_header32 *)buffer;
#ifdef DEBUG
  imports_obj_elf_print_header32(elf_header);
#endif

  int section_count = get_int16_le(elf_header->e_shnum);
  int section_size = get_int16_le(elf_header->e_shentsize);
  int ptr = get_int32_le(elf_header->e_shoff);
  int i;

  const uint8_t *symbol_table = NULL;
  const uint8_t *symbol_string_table = NULL;
  const uint8_t *relocation_table = NULL;
  int symbol_table_size = 0;
  int symbol_string_table_size = 0;
  int relocation_table_size = 0;

  // Point to strtab for section names.
  int e_shstrndx = get_int16_le(elf_header->e_shstrndx);
  section = (struct _elf_section32 *)(buffer + ptr + (e_shstrndx * section_size));
  sh_offset = get_int32_le(section->sh_offset);
  const uint8_t *section_string_table = buffer + sh_offset;

  for (i = 0; i < section_count; i++)
  {
    section = (struct _elf_section32 *)(buffer + ptr);

    int sh_name = get_int32_le(section->sh_name);
    int sh_type = get_int32_le(section->sh_type);
    int sh_size = get_int32_le(section->sh_size);
    int sh_offset = get_int32_le(section->sh_offset);
    const char *name = (char *)(section_string_table + sh_name);

#ifdef DEBUG
    imports_obj_elf_print_section32(section, i, name);
#endif

    if (sh_type == SHT_SYMTAB)
    {
      symbol_table = buffer + sh_offset;
      symbol_table_size = sh_size;
    }
      else
    if (sh_type == SHT_STRTAB && strcmp(name, ".strtab") == 0)
    {
      symbol_string_table = buffer + sh_offset;
      symbol_string_table_size = sh_size;
    }
      else
    if (sh_type == SHT_REL && strcmp(name, ".rel.text") == 0)
    {
      relocation_table = buffer + sh_offset;
      relocation_table_size = sh_size;
    }

    ptr += section_size;
  }

  if (symbol_table != NULL && symbol_string_table != NULL)
  {
    const char *name = imports_obj_symbol_table_lookup_by_offset(
       symbol_table,
       symbol_table_size,
       symbol_string_table,
       symbol_string_table_size,
       relocation_table,
       relocation_table_size,
       function_offset,
       local_offset);

    if (name != NULL) { return name; }
  }

  return NULL;
}

