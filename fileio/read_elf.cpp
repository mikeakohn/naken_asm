/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/assembler.h"
#include "common/Symbols.h"
#include "fileio/FileIo.h"
#include "fileio/read_elf.h"

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xffffffff

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKPROC 0xf0000000

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_LOPROC 13
#define STT_HIPROC 15

struct _elf_shdr
{
  uint32_t sh_name;
  uint32_t sh_type;
  uint64_t sh_flags;
  uint64_t sh_addr;
  uint64_t sh_offset;
  uint64_t sh_size;
};

struct _elf_sym
{
  uint32_t st_name;
  uint64_t st_value;
  uint64_t st_size;
  uint8_t  st_info;
  uint8_t  st_other;
  uint16_t st_shndx;
};

static int read_shdr_32(FileIo &file, _elf_shdr *elf_shdr)
{
  elf_shdr->sh_name   = file.get_int32();
  elf_shdr->sh_type   = file.get_int32();
  elf_shdr->sh_flags  = file.get_int32();
  elf_shdr->sh_addr   = file.get_int32();
  elf_shdr->sh_offset = file.get_int32();
  elf_shdr->sh_size   = file.get_int32();

  return 0;
}

static int read_shdr_64(FileIo &file, _elf_shdr *elf_shdr)
{
  elf_shdr->sh_name   = file.get_int32();
  elf_shdr->sh_type   = file.get_int32();
  elf_shdr->sh_flags  = file.get_int64();
  elf_shdr->sh_addr   = file.get_int64();
  elf_shdr->sh_offset = file.get_int64();
  elf_shdr->sh_size   = file.get_int64();

  return 0;
}

int read_elf(
  const char *filename,
  Memory *memory,
  uint8_t *cpu_type,
  Symbols *symbols)
{
  FileIo file;
  uint8_t e_ident[16];
  uint64_t e_shoff;
  int e_shentsize;
  int e_shnum;
  int e_shstrndx;
  int n;
  uint32_t start, end;
  struct _elf_shdr elf_shdr;
  long strtab_offset = 0;
  uint8_t is_32_bit = 1;

  memory->clear();

  start = 0xffffffff;
  end = 0xffffffff; 

  if (file.open_for_reading(filename) != 0)
  {
    return -1;
  }

  memset(e_ident, 0, 16);
  n = file.get_bytes(e_ident, 16);

  if (e_ident[0] != 0x7f || e_ident[1] != 'E' ||
      e_ident[2] != 'L'  || e_ident[3] != 'F')
  {
    //printf("Not an ELF file.\n");
    file.close_file();
    return -2;
  }

  #define EI_CLASS 4   // 1=32 bit, 2=64 bit
  #define EI_DATA 5    // 1=little endian, 2=big endian
  #define EI_OSABI 7   // 0=SysV, 255=Embedded

  if (e_ident[EI_CLASS] == 2)
  {
    is_32_bit = 0;
  }

  // EI_DATA
  if (e_ident[EI_DATA] == 1)
  {
    memory->endian = ENDIAN_LITTLE;
    file.set_endian(FileIo::FILE_ENDIAN_LITTLE);
  }
    else
  if (e_ident[EI_DATA] == 2)
  {
    memory->endian = ENDIAN_BIG;
    file.set_endian(FileIo::FILE_ENDIAN_BIG);
  }
    else
  {
    printf("ELF Error: EI_DATA incorrect data encoding\n");
    file.close_file();
    return -1;
  }

  // e_type.
  file.get_int16();

  int e_machine = file.get_int16();

  switch (e_machine)
  {
    case 4:
      *cpu_type = CPU_TYPE_68000;
      break;
    case 8:
    case 10:
      *cpu_type = CPU_TYPE_MIPS32;
      break;
    case 20:
      *cpu_type = CPU_TYPE_POWERPC;
      break;
    case 23:
      *cpu_type = CPU_TYPE_CELL;
      break;
    case 40:
      *cpu_type = CPU_TYPE_ARM;
      break;
    case 71:
      *cpu_type = CPU_TYPE_68HC08;
      break;
    case 83:
      *cpu_type = CPU_TYPE_AVR8;
      break;
    case 94:
      *cpu_type = CPU_TYPE_XTENSA;
      break;
    case 105:
      *cpu_type = CPU_TYPE_MSP430;
      break;
    case 118:
      *cpu_type = CPU_TYPE_DSPIC;
      break;
    case 165:
      *cpu_type = CPU_TYPE_8051;
      break;
    case 186:
      *cpu_type = CPU_TYPE_STM8;
      break;
    case 220:
      *cpu_type = CPU_TYPE_Z80;
      break;
    case 243:
      *cpu_type = CPU_TYPE_RISCV;
      break;
    case 247:
      *cpu_type = CPU_TYPE_EBPF;
      break;
    case 0x1223:
      *cpu_type = CPU_TYPE_EPIPHANY;
      break;
    default:
      if (*cpu_type == CPU_TYPE_IGNORE)
      {
        printf("ELF Error: e_machine unknown\n");
        file.close_file();
        return -1;
      }

      break;
  }

  // e_version.
  file.get_int32();

  if (is_32_bit == 1)
  {
    // e_entry.
    // e_phoff.
    file.get_int32();
    file.get_int32();

    e_shoff = file.get_int32();
  }
    else
  {
    // e_entry.
    // e_phoff.
    file.get_int64();
    file.get_int64();

    e_shoff = file.get_int64();
  }

  // e_flags.
  // e_ehsize.
  // e_phentsize.
  // e_phnum.
  file.get_int32();
  file.get_int16();
  file.get_int16();
  file.get_int16();

  e_shentsize = file.get_int16();
  e_shnum     = file.get_int16();
  e_shstrndx  = file.get_int16();

  //printf("e_shoff=%d\n", e_shoff);
  //printf("e_shentsize=%d\n", e_shentsize);
  //printf("e_shnum=%d\n", e_shnum);
  //printf("e_shstrndx=%d\n", e_shstrndx);

  uint64_t stroffset;

  if (is_32_bit == 1)
  {
    file.set(e_shoff + (e_shstrndx * e_shentsize) + 16);
    stroffset = file.get_int32();
  }
    else
  {
    file.set(e_shoff + (e_shstrndx * e_shentsize) + 24);
    stroffset = file.get_int64();
  }

  char name[128];

  // Need to find .strtab so symbol names can be filled in.
  for (n = 0; n < e_shnum; n++)
  {
    file.set(e_shoff + (n * e_shentsize));

    if (is_32_bit == 1)
    {
      read_shdr_32(file, &elf_shdr);
    }
      else
    {
      read_shdr_64(file, &elf_shdr);
    }

    if (elf_shdr.sh_type == SHT_STRTAB)
    {
      file.get_string_at_offset(
        name,
        sizeof(name),
        stroffset + elf_shdr.sh_name);

      if (strcmp(name, ".strtab") == 0)
      {
        strtab_offset = elf_shdr.sh_offset;
        break;
      }
    }
  }

  for (n = 0; n < e_shnum; n++)
  {
    file.set(e_shoff + (n * e_shentsize));

    if (is_32_bit == 1)
    {
      read_shdr_32(file, &elf_shdr);
    }
      else
    {
      read_shdr_64(file, &elf_shdr);
    }

    file.get_string_at_offset(name, sizeof(name), stroffset + elf_shdr.sh_name);

    //printf("name=%s\n", name);

    int is_text = (elf_shdr.sh_flags & SHF_EXECINSTR) != 0 ? 1 : 0;
    if (is_text ||
        strncmp(name, ".data", 5) == 0 ||
        strcmp(name, ".vectors") == 0)
    {
      if (is_text)
      {
        if (start == 0xffffffff)
        {
          start = elf_shdr.sh_addr;
        }
          else
        if (start > elf_shdr.sh_addr)
        {
          start = elf_shdr.sh_addr;
        }

        if (end == 0xffffffff)
        {
          end = elf_shdr.sh_addr + elf_shdr.sh_size - 1;
        }
          else
        if (end < elf_shdr.sh_addr + elf_shdr.sh_size)
        {
          end = elf_shdr.sh_addr + elf_shdr.sh_size - 1;
        }
      }

      long marker = file.tell();
      file.set(elf_shdr.sh_offset);

      uint32_t i;
      for (i = 0; i < elf_shdr.sh_size; i++)
      {
        memory->write8(elf_shdr.sh_addr + i, file.get_int8());
      }

      file.set(marker);

      printf("Loaded %d %s bytes from 0x%04" PRIx64 "\n",
        i, name, elf_shdr.sh_addr);
    }
      else
    if (elf_shdr.sh_type == SHT_SYMTAB && symbols != NULL)
    {
      long marker = file.tell();
      file.set(elf_shdr.sh_offset);

      int sym_size = is_32_bit ? 16 : 24;

      uint32_t i;
      for (i = 0; i < elf_shdr.sh_size; i += sym_size)
      {
        char name[128];

        struct _elf_sym elf_sym;

        if (is_32_bit == 1)
        {
          elf_sym.st_name  = file.get_int32();
          elf_sym.st_value = file.get_int32();
          elf_sym.st_size  = file.get_int32();
          elf_sym.st_info  = file.get_int8();
          elf_sym.st_other = file.get_int8();
          elf_sym.st_shndx = file.get_int16();
        }
          else
        {
          elf_sym.st_name  = file.get_int32();
          elf_sym.st_info  = file.get_int8();
          elf_sym.st_other = file.get_int8();
          elf_sym.st_shndx = file.get_int16();
          elf_sym.st_value = file.get_int64();
          elf_sym.st_size  = file.get_int64();
        }

        file.get_string_at_offset(
          name,
          sizeof(name),
          strtab_offset + elf_sym.st_name);

        printf("symbol %12s 0x%04" PRIx64 "\n", name, elf_sym.st_value);
        if (elf_sym.st_info != STT_NOTYPE &&
            elf_sym.st_info != STT_SECTION &&
            elf_sym.st_info != STT_FILE)
        {
          symbols->append(name, elf_sym.st_value);
        }
      }

      file.set(marker);
    }
  }

  memory->low_address = start;
  memory->high_address = end;

  file.close_file();

  return start;
}

