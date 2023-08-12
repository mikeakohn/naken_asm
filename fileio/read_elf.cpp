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
#include <stdint.h>
#include <inttypes.h>

#include "common/assembler.h"
#include "common/Symbols.h"
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

typedef uint32_t (*get_int16_t)(FILE *);
typedef uint32_t (*get_int32_t)(FILE *);
typedef uint64_t (*get_int64_t)(FILE *);

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

static uint32_t get_int16_le(FILE *in)
{
  uint32_t i;

  i  = getc(in);
  i |= (getc(in) << 8);

  return i;
}

static uint32_t get_int32_le(FILE *in)
{
  uint32_t i;

  i  =  getc(in);
  i |= (getc(in) << 8);
  i |= (getc(in) << 16);
  i |= (getc(in) << 24);

  return i;
}

static uint64_t get_int64_le(FILE *in)
{
  uint64_t i;

  i  =  (uint64_t)getc(in);
  i |= ((uint64_t)getc(in) << 8);
  i |= ((uint64_t)getc(in) << 16);
  i |= ((uint64_t)getc(in) << 24);
  i |= ((uint64_t)getc(in) << 32);
  i |= ((uint64_t)getc(in) << 40);
  i |= ((uint64_t)getc(in) << 48);
  i |= ((uint64_t)getc(in) << 56);

  return i;
}

static uint32_t get_int16_be(FILE *in)
{
  uint32_t i;

  i = (getc(in) << 8);
  i |= getc(in);

  return i;
}

static uint32_t get_int32_be(FILE *in)
{
  uint32_t i;

  i =  (getc(in) << 24);
  i |= (getc(in) << 16);
  i |= (getc(in) << 8);
  i |=  getc(in);

  return i;
}

static uint64_t get_int64_be(FILE *in)
{
  uint32_t i;

  i =  ((uint64_t)getc(in) << 56);
  i |= ((uint64_t)getc(in) << 48);
  i |= ((uint64_t)getc(in) << 40);
  i |= ((uint64_t)getc(in) << 32);
  i |= ((uint64_t)getc(in) << 24);
  i |= ((uint64_t)getc(in) << 16);
  i |= ((uint64_t)getc(in) << 8);
  i |=  (uint64_t)getc(in);

  return i;
}

static int read_shdr_32(
  FILE *in,
  struct _elf_shdr *elf_shdr,
  get_int32_t get_int32)
{
  elf_shdr->sh_name   = get_int32(in);
  elf_shdr->sh_type   = get_int32(in);
  elf_shdr->sh_flags  = get_int32(in);
  elf_shdr->sh_addr   = get_int32(in);
  elf_shdr->sh_offset = get_int32(in);
  elf_shdr->sh_size   = get_int32(in);

  return 0;
}

static int read_shdr_64(
  FILE *in,
  struct _elf_shdr *elf_shdr,
  get_int32_t get_int32,
  get_int64_t get_int64)
{
  elf_shdr->sh_name   = get_int32(in);
  elf_shdr->sh_type   = get_int32(in);
  elf_shdr->sh_flags  = get_int64(in);
  elf_shdr->sh_addr   = get_int64(in);
  elf_shdr->sh_offset = get_int64(in);
  elf_shdr->sh_size   = get_int64(in);

  return 0;
}

static int read_name(FILE *in, char *name, int len, long offset)
{
  long marker = ftell(in);
  int ptr = 0;

  fseek(in, offset, SEEK_SET);

  while (1)
  {
    int ch = getc(in);
    if (ch == 0) { break; }
    name[ptr++] = ch;
    if (ptr == len - 1) { break; }
  }
  name[ptr] = 0;

  fseek(in, marker, SEEK_SET);

  return 0;
}

int read_elf(
  const char *filename,
  Memory *memory,
  uint8_t *cpu_type,
  Symbols *symbols)
{
  FILE *in;
  uint8_t e_ident[16];
  uint64_t e_shoff;
  int e_shentsize;
  int e_shnum;
  int e_shstrndx;
  int n;
  uint32_t start, end;
  struct _elf_shdr elf_shdr;
  long strtab_offset = 0;
  get_int16_t get_int16;
  get_int32_t get_int32;
  get_int64_t get_int64;
  uint8_t is_32_bit = 1;

  memory_clear(memory);
  //memset(dirty, 0, memory->size);

  start = 0xffffffff;
  end = 0xffffffff; 

  in = fopen(filename, "rb");

  if (in == NULL)
  {
    return -1;
  }

  memset(e_ident, 0, 16);
  n = fread(e_ident, 1, 16, in);

  if (e_ident[0] != 0x7f || e_ident[1] != 'E' ||
      e_ident[2] != 'L'  || e_ident[3] != 'F')
  {
    //printf("Not an ELF file.\n");
    fclose(in);
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
    get_int16 = get_int16_le;
    get_int32 = get_int32_le;
    get_int64 = get_int64_le;
  }
    else
  if (e_ident[EI_DATA] == 2)
  {
    memory->endian = ENDIAN_BIG;
    get_int16 = get_int16_be;
    get_int32 = get_int32_be;
    get_int64 = get_int64_be;
  }
    else
  {
    printf("ELF Error: EI_DATA incorrect data encoding\n");
    fclose(in);
    return -1;
  }

  // e_type.
  get_int16(in);

  int e_machine = get_int16(in);

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
        fclose(in);
        return -1;
      }

      break;
  }

  // e_version.
  get_int32(in);

  if (is_32_bit == 1)
  {
    // e_entry.
    // e_phoff.
    get_int32(in);
    get_int32(in);

    e_shoff = get_int32(in);
  }
    else
  {
    // e_entry.
    // e_phoff.
    get_int64(in);
    get_int64(in);

    e_shoff = get_int64(in);
  }

  // e_flags.
  // e_ehsize.
  // e_phentsize.
  // e_phnum.
  get_int32(in);
  get_int16(in);
  get_int16(in);
  get_int16(in);

  e_shentsize = get_int16(in);
  e_shnum = get_int16(in);
  e_shstrndx = get_int16(in);

  //printf("e_shoff=%d\n", e_shoff);
  //printf("e_shentsize=%d\n", e_shentsize);
  //printf("e_shnum=%d\n", e_shnum);
  //printf("e_shstrndx=%d\n", e_shstrndx);

  uint64_t stroffset;

  if (is_32_bit == 1)
  {
    fseek(in, e_shoff + (e_shstrndx * e_shentsize) + 16, SEEK_SET);
    stroffset = get_int32(in);
  }
    else
  {
    fseek(in, e_shoff + (e_shstrndx * e_shentsize) + 24, SEEK_SET);
    stroffset = get_int64(in);
  }

  char name[128];

  // Need to find .strtab so symbol names can be filled in.
  for (n = 0; n < e_shnum; n++)
  {
    fseek(in, e_shoff + (n * e_shentsize), SEEK_SET);

    if (is_32_bit == 1)
    {
      read_shdr_32(in, &elf_shdr, get_int32);
    }
      else
    {
      read_shdr_64(in, &elf_shdr, get_int32, get_int64);
    }

    if (elf_shdr.sh_type == SHT_STRTAB)
    {
      read_name(in, name, sizeof(name), stroffset + elf_shdr.sh_name);

      if (strcmp(name, ".strtab") == 0)
      {
        strtab_offset = elf_shdr.sh_offset;
        break;
      }
    }
  }

  for (n = 0; n < e_shnum; n++)
  {
    fseek(in, e_shoff + (n * e_shentsize), SEEK_SET);

    if (is_32_bit == 1)
    {
      read_shdr_32(in, &elf_shdr, get_int32);
    }
      else
    {
      read_shdr_64(in, &elf_shdr, get_int32, get_int64);
    }

    read_name(in, name, sizeof(name), stroffset + elf_shdr.sh_name);

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

      long marker = ftell(in);
      fseek(in, elf_shdr.sh_offset, SEEK_SET);

      uint32_t i;
      for (i = 0; i < elf_shdr.sh_size; i++)
      {
        //if (elf_shdr.sh_addr + i >= memory->size) { break; }
        memory_write_m(memory, elf_shdr.sh_addr + i, getc(in));
      }

      fseek(in, marker, SEEK_SET);

      printf("Loaded %d %s bytes from 0x%04" PRIx64 "\n",
        i, name, elf_shdr.sh_addr);
    }
      else
    if (elf_shdr.sh_type == SHT_SYMTAB && symbols != NULL)
    {
      long marker = ftell(in);
      fseek(in, elf_shdr.sh_offset, SEEK_SET);

      int sym_size = is_32_bit ? 16 : 24;

      uint32_t i;
      for (i = 0; i < elf_shdr.sh_size; i += sym_size)
      {
        char name[128];

        struct _elf_sym elf_sym;

        if (is_32_bit == 1)
        {
          elf_sym.st_name  = get_int32(in);
          elf_sym.st_value = get_int32(in);
          elf_sym.st_size  = get_int32(in);
          elf_sym.st_info  = getc(in);
          elf_sym.st_other = getc(in);
          elf_sym.st_shndx = get_int16(in);
        }
          else
        {
          elf_sym.st_name  = get_int32(in);
          elf_sym.st_info  = getc(in);
          elf_sym.st_other = getc(in);
          elf_sym.st_shndx = get_int16(in);
          elf_sym.st_value = get_int64(in);
          elf_sym.st_size  = get_int64(in);
        }

        read_name(in, name, sizeof(name), strtab_offset + elf_sym.st_name);

        printf("symbol %12s 0x%04" PRIx64 "\n", name, elf_sym.st_value);
        if (elf_sym.st_info != STT_NOTYPE &&
            elf_sym.st_info != STT_SECTION &&
            elf_sym.st_info != STT_FILE)
        {
          symbols_append(symbols, name, elf_sym.st_value);
        }
      }

      fseek(in, marker, SEEK_SET);
    }
  }

  memory->low_address = start;
  memory->high_address = end;

  fclose(in);

  return start;
}

