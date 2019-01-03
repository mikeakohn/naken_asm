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

#ifndef NAKEN_ASM_IMPORTS_OBJ_H
#define NAKEN_ASM_IMPORTS_OBJ_H

struct _elf_header32
{
  uint8_t e_ident[4];
  uint8_t e_ident_class;
  uint8_t e_ident_data;
  uint8_t e_ident_version;
  uint8_t e_ident_osabi;
  uint8_t e_ident_abiversion;
  uint8_t e_ident_pad[7];
  uint8_t e_type[2];
  uint8_t e_machine[2];
  uint8_t e_version[4];
  uint8_t e_entry[4];
  uint8_t e_phoff[4];
  uint8_t e_shoff[4];
  uint8_t e_flags[4];
  uint8_t e_ehsize[2];
  uint8_t e_phentsize[2];
  uint8_t e_phnum[2];
  uint8_t e_shentsize[2];
  uint8_t e_shnum[2];
  uint8_t e_shstrndx[2];
};

struct _elf_header64
{
  uint8_t e_ident[4];
  uint8_t e_ident_class;
  uint8_t e_ident_data;
  uint8_t e_ident_version;
  uint8_t e_ident_osabi;
  uint8_t e_ident_abiversion;
  uint8_t e_ident_pad[7];
  uint8_t e_type[2];
  uint8_t e_machine[2];
  uint8_t e_version[4];
  uint8_t e_entry[8];
  uint8_t e_phoff[8];
  uint8_t e_shoff[8];
  uint8_t e_flags[4];
  uint8_t e_ehsize[2];
  uint8_t e_phentsize[2];
  uint8_t e_phnum[2];
  uint8_t e_shentsize[2];
  uint8_t e_shnum[2];
  uint8_t e_shstrndx[2];
};

struct _elf_program32
{
  uint8_t p_type[4];
  uint8_t p_offset[4];
  uint8_t p_vaddr[4];
  uint8_t p_paddr[4];
  uint8_t p_filesz[4];
  uint8_t p_memsz[4];
  uint8_t p_flags[4];
  uint8_t p_palign[4];
};

struct _elf_program64
{
  uint8_t p_type[4];
  uint8_t p_flags[4];
  uint8_t p_offset[8];
  uint8_t p_vaddr[8];
  uint8_t p_paddr[8];
  uint8_t p_filesz[8];
  uint8_t p_memsz[8];
  uint8_t p_palign[8];
};

struct _elf_section32
{
  uint8_t sh_name[4];
  uint8_t sh_type[4];
  uint8_t sh_flags[4];
  uint8_t sh_addr[4];
  uint8_t sh_offset[4];
  uint8_t sh_size[4];
  uint8_t sh_link[4];
  uint8_t sh_info[4];
  uint8_t sh_addralign[4];
  uint8_t sh_entsize[4];
};

struct _elf_section64
{
  uint8_t sh_name[4];
  uint8_t sh_type[4];
  uint8_t sh_flags[8];
  uint8_t sh_addr[8];
  uint8_t sh_offset[8];
  uint8_t sh_size[8];
  uint8_t sh_link[4];
  uint8_t sh_info[4];
  uint8_t sh_addralign[8];
  uint8_t sh_entsize[8];
};

struct _elf_symbol32
{
  uint8_t st_name[4];
  uint8_t st_value[4];
  uint8_t st_size[4];
  uint8_t st_info;
  uint8_t st_other;
  uint8_t st_shndx[2];
};

struct _elf_relocation32
{
  uint8_t r_offset[4];
  uint8_t r_info[4];
};

struct _elf_symbol64
{
  uint8_t st_name[4];
  uint8_t st_info;
  uint8_t st_other;
  uint8_t st_shndx[2];
  uint8_t st_value[8];
  uint8_t st_size[8];
};

int imports_obj_verify(const uint8_t *buffer, int file_size);

int imports_obj_find_code_from_symbol(
  uint8_t *buffer,
  int file_size,
  const char *symbol,
  uint32_t *function_offset,
  uint32_t *function_size,
  uint32_t *file_offset);

const char *imports_obj_find_name_from_offset(
  const uint8_t *buffer,
  int file_size,
  uint32_t function_offset,
  uint32_t local_offset);

#endif

