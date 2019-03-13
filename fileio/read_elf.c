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
#include <string.h>

#include "common/assembler.h"
#include "common/symbols.h"
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

struct _elf32_shdr
{
  uint32_t sh_name;
  uint32_t sh_type;
  uint32_t sh_flags;
  uint32_t sh_addr;
  uint32_t sh_offset;
  uint32_t sh_size;
};

struct _elf32_sym
{
  uint32_t st_name;
  uint32_t st_value;
  uint32_t st_size;
  uint8_t st_info;
  uint8_t st_other;
  uint16_t st_shndx;
};

static uint32_t get_int16_le(FILE *in)
{
uint32_t i;

  i = getc(in);
  i |= (getc(in)<<8);

  return i;
}

static uint32_t get_int32_le(FILE *in)
{
uint32_t i;

  i = getc(in);
  i |= (getc(in)<<8);
  i |= (getc(in)<<16);
  i |= (getc(in)<<24);

  return i;
}

static uint32_t get_int16_be(FILE *in)
{
uint32_t i;

  i = (getc(in)<<8);
  i |= getc(in);

  return i;
}

static uint32_t get_int32_be(FILE *in)
{
uint32_t i;

  i = (getc(in)<<24);
  i |= (getc(in)<<16);
  i |= (getc(in)<<8);
  i |= getc(in);

  return i;
}

static int read_shdr(FILE *in, struct _elf32_shdr *elf32_shdr, get_int32_t get_int32)
{
  elf32_shdr->sh_name = get_int32(in);
  elf32_shdr->sh_type = get_int32(in);
  elf32_shdr->sh_flags = get_int32(in);
  elf32_shdr->sh_addr = get_int32(in);
  elf32_shdr->sh_offset = get_int32(in);
  elf32_shdr->sh_size = get_int32(in);

  return 0;
}

static int read_name(FILE *in, char *name, int len, long offset)
{
long marker = ftell(in);
int ptr = 0;

  fseek(in, offset, SEEK_SET);

  while(1)
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

int read_elf(char *filename, struct _memory *memory, uint8_t *cpu_type, struct _symbols *symbols)
{
  FILE *in;
  uint8_t e_ident[16];
  int e_shoff;
  int e_shentsize;
  int e_shnum;
  int e_shstrndx;
  int n;
  int start, end;
  struct _elf32_shdr elf32_shdr;
  long strtab_offset = 0;
  get_int16_t get_int16;
  get_int32_t get_int32;

  memory_clear(memory);
  //memset(dirty, 0, memory->size);

  start = -1;
  end = -1;

  in = fopen(filename, "rb");
  if (in == 0)
  {
    return -1;
  }

  memset(e_ident, 0, 16);
  n = fread(e_ident, 1, 16, in);

  if (e_ident[0] != 0x7f || e_ident[1] != 'E' ||
      e_ident[2] != 'L' || e_ident[3] != 'F')
  {
    //printf("Not an ELF file.\n");
    fclose(in);
    return -2;
  }

  #define EI_CLASS 4   // 1=32 bit, 2=64 bit
  #define EI_DATA 5    // 1=little endian, 2=big endian
  #define EI_OSABI 7   // 0=SysV, 255=Embedded

  if (e_ident[EI_CLASS] != 1) // let's let other stuff in || e_ident[7]!=0xff)
  {
    printf("ELF Error: e_ident shows incorrect type\n");
    fclose(in);
    return -1;
  }

  // EI_DATA
  if (e_ident[EI_DATA] == 1)
  {
    memory->endian = ENDIAN_LITTLE;
    get_int16 = get_int16_le;
    get_int32 = get_int32_le;
  }
    else
  if (e_ident[EI_DATA] == 2)
  {
    memory->endian = ENDIAN_BIG;
    get_int16 = get_int16_be;
    get_int32 = get_int32_be;
  }
    else
  {
    printf("ELF Error: EI_DATA incorrect data encoding\n");
    fclose(in);
    return -1;
  }

  get_int16(in);
  n = get_int16(in);

  switch(n)
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
    case 0x1223:
      *cpu_type = CPU_TYPE_EPIPHANY;
      break;
    default:
      printf("ELF Error: e_machine unknown\n");
      fclose(in);
      return -1;
  }

  fseek(in, 32, SEEK_SET);
  e_shoff = get_int32(in);
  fseek(in, 46, SEEK_SET);
  e_shentsize = get_int16(in);
  e_shnum = get_int16(in);
  e_shstrndx = get_int16(in);

  //printf("e_shoff=%d\n", e_shoff);
  //printf("e_shentsize=%d\n", e_shentsize);
  //printf("e_shnum=%d\n", e_shnum);
  //printf("e_shstrndx=%d\n", e_shstrndx);

  fseek(in, e_shoff + (e_shstrndx * e_shentsize) + 16, SEEK_SET);
  int stroffset = get_int32(in);
  char name[32];

  // Need to find .strtab so we can fill in symbol names
  for (n = 0; n < e_shnum; n++)
  {
    fseek(in, e_shoff + (n * e_shentsize), SEEK_SET);
    read_shdr(in, &elf32_shdr, get_int32);

    if (elf32_shdr.sh_type == SHT_STRTAB)
    {
      read_name(in, name, 32, stroffset + elf32_shdr.sh_name);
      if (strcmp(name, ".strtab") == 0)
      {
        strtab_offset = elf32_shdr.sh_offset;
        break;
      }
    }
  }

  for (n = 0; n < e_shnum; n++)
  {
    // FIXME - a little inefficient eh?
    fseek(in, e_shoff + (n * e_shentsize), SEEK_SET);
    read_shdr(in, &elf32_shdr, get_int32);

    read_name(in, name, 32, stroffset + elf32_shdr.sh_name);

    //printf("name=%s\n", name);
    //int is_text = strncmp(name, ".text", 5) == 0 ? 1 : 0;
    int is_text = (elf32_shdr.sh_flags & SHF_EXECINSTR) != 0 ? 1 : 0;
    if (is_text ||
        strncmp(name, ".data", 5) == 0 || strcmp(name, ".vectors") == 0)
    {
      if (is_text)
      {
        if (start == -1) { start = elf32_shdr.sh_addr; }
        else if (start > elf32_shdr.sh_addr) { start = elf32_shdr.sh_addr; }

        if (end == -1) { end = elf32_shdr.sh_addr + elf32_shdr.sh_size - 1; }
        else if (end < elf32_shdr.sh_addr + elf32_shdr.sh_size) { end = elf32_shdr.sh_addr+elf32_shdr.sh_size - 1; }
      }

      long marker = ftell(in);
      fseek(in, elf32_shdr.sh_offset, SEEK_SET);

      int i;
      for (i = 0; i < elf32_shdr.sh_size; i++)
      {
        //if (elf32_shdr.sh_addr + i >= memory->size) { break; }
        memory_write_m(memory, elf32_shdr.sh_addr + i, getc(in));
      }

      fseek(in, marker, SEEK_SET);

      printf("Loaded %d %s bytes from 0x%04x\n", i, name, elf32_shdr.sh_addr);
    }
      else
    if (elf32_shdr.sh_type == SHT_SYMTAB && symbols != NULL)
    {
      long marker = ftell(in);
      fseek(in, elf32_shdr.sh_offset, SEEK_SET);

      int i;
      for (i = 0; i < elf32_shdr.sh_size; i += 16)
      {
        char name[128];

        struct _elf32_sym elf32_sym;
        elf32_sym.st_name = get_int32(in);
        elf32_sym.st_value = get_int32(in);
        elf32_sym.st_size = get_int32(in);
        elf32_sym.st_info = getc(in);
        elf32_sym.st_other = getc(in);
        elf32_sym.st_shndx = get_int16(in);

        read_name(in, name, 128, strtab_offset + elf32_sym.st_name);

        printf("symbol %12s 0x%04x\n", name, elf32_sym.st_value);
        if (elf32_sym.st_info != STT_NOTYPE &&
            elf32_sym.st_info != STT_SECTION &&
            elf32_sym.st_info != STT_FILE)
        {
          symbols_append(symbols, name, elf32_sym.st_value);
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

