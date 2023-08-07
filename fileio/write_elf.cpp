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
#include "common/symbols.h"
#include "fileio/write_elf.h"

typedef void(*write_int64_t)(FILE *, uint64_t);
typedef void(*write_int32_t)(FILE *, uint32_t);
typedef void(*write_int16_t)(FILE *, uint32_t);

typedef struct _elf
{
  struct _sections_offset sections_offset;
  struct _sections_size sections_size;
  uint8_t e_ident[16];
  uint32_t e_entry;
  uint32_t e_phoff;
  uint32_t e_flags;
  uint32_t e_phentsize;
  uint32_t e_phnum;
  uint16_t e_type;
  uint16_t e_machine;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
  //int text_count;
  //int data_count;
  int cpu_type;
  int text_addr;
  int data_addr;
  char string_table[32768];
  write_int64_t write_int64;
  write_int32_t write_int32;
  write_int16_t write_int16;
  long shnum_offset;
  long shoff_offset;
} Elf;

static const char magic_number[16] =
{
  0x7f, 'E','L','F',  1, 1, 1, 0, //0xff,
  0, 0, 0, 0,  0, 0, 0, 0
};

// For now there will only be .text, .shstrtab, .symtab, strtab
// I can't see a use for .data and .bss unless the bootloaders know elf?
// I'm also not supporting relocations. I can do it later if requested.

// Write string table
const char string_table_default[] =
  "\0"
  //".text\0"
  //".rela.text\0"
  //".data\0"
  //".bss\0"
  ".shstrtab\0"
  ".symtab\0"
  ".strtab\0"
  ".comment\0"
  //".debug_line\0"
  //".rela.debug_line\0"
  //".debug_info\0"
  //".rela.debug_info\0"
  //".debug_abbrev\0"
  //".debug_aranges\0"
  //".rela.debug_aranges\0"
;

static void write_int64_le(FILE *out, uint64_t n)
{
  putc(n & 0xff, out);
  putc((n >> 8) & 0xff, out);
  putc((n >> 16) & 0xff, out);
  putc((n >> 24) & 0xff, out);
  putc((n >> 32) & 0xff, out);
  putc((n >> 40) & 0xff, out);
  putc((n >> 48) & 0xff, out);
  putc((n >> 56) & 0xff, out);
}

static void write_int32_le(FILE *out, uint32_t n)
{
  putc(n & 0xff, out);
  putc((n >> 8) & 0xff, out);
  putc((n >> 16) & 0xff, out);
  putc((n >> 24) & 0xff, out);
}

static void write_int16_le(FILE *out, uint32_t n)
{
  putc(n & 0xff, out);
  putc((n >> 8) & 0xff, out);
}

static void write_int64_be(FILE *out, uint64_t n)
{
  putc((n >> 56) & 0xff, out);
  putc((n >> 48) & 0xff, out);
  putc((n >> 40) & 0xff, out);
  putc((n >> 32) & 0xff, out);
  putc((n >> 24) & 0xff, out);
  putc((n >> 16) & 0xff, out);
  putc((n >> 8) & 0xff, out);
  putc(n & 0xff, out);
}

static void write_int32_be(FILE *out, uint32_t n)
{
  putc((n >> 24) & 0xff, out);
  putc((n >> 16) & 0xff, out);
  putc((n >> 8) & 0xff, out);
  putc(n & 0xff, out);
}

static void write_int16_be(FILE *out, uint32_t n)
{
  putc((n >> 8) & 0xff, out);
  putc(n & 0xff, out);
}

static void write_elf_header(FILE *out, Elf *elf, Memory *memory)
{
  #define EI_CLASS 4   // 1=32 bit, 2=64 bit
  #define EI_DATA  5   // 1=little endian, 2=big endian
  #define EI_OSABI 7   // 0=SysV, 255=Embedded

  memcpy(elf->e_ident, magic_number, 16);

  if (memory->endian == ENDIAN_LITTLE)
  {
    elf->e_ident[EI_DATA] = 1;
    elf->write_int64 = write_int64_le;
    elf->write_int32 = write_int32_le;
    elf->write_int16 = write_int16_le;
  }
    else
  {
    elf->e_ident[EI_DATA] = 2;
    elf->write_int64 = write_int64_be;
    elf->write_int32 = write_int32_be;
    elf->write_int16 = write_int16_be;
  }

  if (memory->entry_point != 0xffffffff)
  {
    elf->e_entry = memory->entry_point;
    elf->e_phoff = 0x34;
    elf->e_phentsize = 32;
    elf->e_phnum = 1;
  }

  // This probably should be 0 for Raspberry Pi, etc.
  elf->e_ident[EI_OSABI] = 255;

  // May need this later
  //elf.e_ident[7]=0xff; // SYSV=0, HPUX=1, STANDALONE=255

  // mspgcc4/build/insight-6.8-1/include/elf/msp430.h (11)
  elf->e_flags = 0;
  elf->e_shnum = 4;

  // Relocatable
  elf->e_type = 1;

  // This could be a lookup table, but let's play it safe
  switch (elf->cpu_type)
  {
    case CPU_TYPE_MSP430:
    case CPU_TYPE_MSP430X:
      elf->e_machine = 105;
      elf->e_flags = 11;
      break;
    case CPU_TYPE_68000:
      elf->e_machine = 4;
      break;
    case CPU_TYPE_68HC08:
      elf->e_machine = 71;
      break;
    case CPU_TYPE_8051:
      elf->e_machine = 165;
      break;
    case CPU_TYPE_ARM:
      elf->e_machine = 40;
      elf->e_flags = 0x05000000;
      elf->e_shnum++;
      break;
    case CPU_TYPE_AVR8:
      elf->e_machine = 0x53;
      elf->e_flags = 0x85;
      elf->e_shnum++;
      break;
    case CPU_TYPE_CELL:
      elf->e_machine = 23;
      elf->e_ident[EI_OSABI] = 0;
      elf->e_type = 2;  // Executable
      break;
    case CPU_TYPE_DSPIC:
      elf->e_machine = 118;
      elf->e_flags = 1;
      break;
    case CPU_TYPE_EBPF:
      elf->e_machine = 247;
      elf->e_ident[EI_OSABI] = 0;
      elf->e_ident[EI_CLASS] = 2;
      elf->e_type = 1;
      break;
    case CPU_TYPE_EMOTION_ENGINE:
      elf->e_machine = 8;
      elf->e_ident[EI_OSABI] = 0;
      elf->e_flags = 0x20924001;
      elf->e_type = 2;  // Executable
      break;
    case CPU_TYPE_EPIPHANY:
      elf->e_machine = 0x1223;
      elf->e_ident[EI_OSABI] = 0;
      elf->e_type = 1;  // Relocatable
      break;
    case CPU_TYPE_MIPS32:
      elf->e_machine = 8;
      break;
    case CPU_TYPE_POWERPC:
      elf->e_machine = 20;
      elf->e_ident[EI_OSABI] = 0;
      break;
    case CPU_TYPE_RISCV:
      elf->e_machine = 243;
      elf->e_ident[EI_OSABI] = 0;
      elf->e_type = 2;  // Executable
      break;
    case CPU_TYPE_STM8:
      elf->e_machine = 186;
      break;
    case CPU_TYPE_XTENSA:
      elf->e_machine = 94;
      break;
    case CPU_TYPE_Z80:
      elf->e_machine = 220;
      break;
    default:
      elf->e_machine = 0;
      break;
  }

  // Null section to start...
  elf->e_shnum++;

  // Write Ehdr;
  fwrite(elf->e_ident, 1, 16, out);

  if (elf->e_ident[EI_CLASS] == 1)
  {
    elf->write_int16(out, elf->e_type);    // e_type 0=not relocatable 1=msp_32
    elf->write_int16(out, elf->e_machine); // e_machine EM_MSP430=0x69
    elf->write_int32(out, 1);              // e_version
    elf->write_int32(out, elf->e_entry);   // e_entry (start address)
    elf->write_int32(out, elf->e_phoff);   // e_phoff (program header offset)
    elf->shoff_offset = ftell(out);
    elf->write_int32(out, 0);              // e_shoff (section header offset)
    elf->write_int32(out, elf->e_flags);   // e_flags (set to CPU model)
    elf->write_int16(out, 0x34);           // e_ehsize (size of this struct)
    elf->write_int16(out, elf->e_phentsize); // e_phentsize (pheader size)
    elf->write_int16(out, elf->e_phnum);   // e_phnum (program headers count)
    elf->write_int16(out, 40);             // e_shentsize (section header size)
    elf->shnum_offset = ftell(out);
    elf->write_int16(out, elf->e_shnum);   // e_shnum (section headers count)
    elf->write_int16(out, 2);              // e_shstrndx (section header string table index)
  }
    else
  {
    elf->write_int16(out, elf->e_type);    // e_type 0=not relocatable 1=msp_32
    elf->write_int16(out, elf->e_machine); // e_machine EM_MSP430=0x69
    elf->write_int32(out, 1);              // e_version
    elf->write_int64(out, elf->e_entry);   // e_entry (start address)
    elf->write_int64(out, elf->e_phoff);   // e_phoff (program header offset)
    elf->shoff_offset = ftell(out);
    elf->write_int64(out, 0);              // e_shoff (section header offset)
    elf->write_int32(out, elf->e_flags);   // e_flags (set to CPU model)
    elf->write_int16(out, 0x34);           // e_ehsize (size of this struct)
    elf->write_int16(out, elf->e_phentsize); // e_phentsize (pheader size)
    elf->write_int16(out, elf->e_phnum);   // e_phnum (program headers count)
    elf->write_int16(out, 64);             // e_shentsize (section header size)
    elf->shnum_offset = ftell(out);
    elf->write_int16(out, elf->e_shnum);   // e_shnum (section headers count)
    elf->write_int16(out, 2);              // e_shstrndx (section header string table index)
  }
}

static int get_string_table_len(char *string_table)
{
  int n = 0;

  while (string_table[n] != 0 || string_table[n + 1] != 0) { n++; }

  return n + 1;
}

static void string_table_append(Elf *elf, const char *name)
{
  int len;

  char *string_table = elf->string_table;

  len = get_string_table_len(string_table);
  string_table = string_table + len;
  strcpy(string_table, name);
  string_table = string_table + strlen(name) + 1;
  *string_table = 0;
}

static void write_elf_text_and_data(
  FILE *out,
  Elf *elf,
  Memory *memory,
  int alignment)
{
  const char *name = ".text";
  uint32_t i;

  elf->text_addr = memory->low_address;
  string_table_append(elf, name);
  elf->sections_offset.text = ftell(out);

  for (i = memory->low_address; i <= memory->high_address; i++)
  {
    putc(memory_read_m(memory, i), out);
  }

  if (alignment > 1)
  {
    int count = memory->high_address - memory->low_address + 1;
    int mask = alignment - 1;

    while ((count & mask) != 0)
    {
      putc(0, out);
      count++;
    }
  }

  elf->sections_size.text = ftell(out) - elf->sections_offset.text;

  elf->e_shnum++;
}

static void write_arm_attribute(FILE *out, Elf *elf)
{
  const uint8_t aeabi[] = {
    0x41, 0x30, 0x00, 0x00, 0x00, 0x61, 0x65, 0x61,
    0x62, 0x69, 0x00, 0x01, 0x26, 0x00, 0x00, 0x00,
    0x05, 0x36, 0x00, 0x06, 0x06, 0x08, 0x01, 0x09,
    0x01, 0x0a, 0x02, 0x12, 0x04, 0x14, 0x01, 0x15,
    0x01, 0x17, 0x03, 0x18, 0x01, 0x19, 0x01, 0x1a,
    0x02, 0x1b, 0x03, 0x1c, 0x01, 0x1e, 0x06, 0x2c,
    0x01
  };

#if 0
  const uint8_t aeabi[] = {
    0x41, 0x2d, 0x00, 0x00, 0x00, 0x61, 0x65, 0x61,
    0x62, 0x69, 0x00, 0x01, 0x23, 0x00, 0x00, 0x00,
    0x05, 0x41, 0x52, 0x4d, 0x39, 0x54, 0x44, 0x4d,
    0x49, 0x00, 0x06, 0x02, 0x08, 0x01, 0x12, 0x04,
    0x14, 0x01, 0x15, 0x01, 0x17, 0x03, 0x18, 0x01,
    0x19, 0x01, 0x1a, 0x02, 0x1e, 0x06
  };
#endif

  string_table_append(elf, ".ARM.attributes");

  elf->sections_offset.arm_attribute = ftell(out);
  fwrite(aeabi, 1, sizeof(aeabi), out);
  elf->sections_size.arm_attribute = ftell(out) - elf->sections_offset.arm_attribute;
  putc(0x00, out); // null
  putc(0x00, out); // null
  putc(0x00, out); // null
}

static void write_phdr(FILE *out, Elf *elf, uint32_t address, uint32_t filesz)
{
  if (elf->e_ident[EI_CLASS] == 1)
  {
    elf->write_int32(out, 1);          // p_type: 1 (LOAD)
    elf->write_int32(out, 0x1000);     // p_offset
    elf->write_int32(out, address);    // p_vaddr
    elf->write_int32(out, address);    // p_paddr
    elf->write_int32(out, filesz);     // p_filesz
    elf->write_int32(out, filesz);     // p_memsz
    elf->write_int32(out, 7);          // p_flags: 7 RWX
    elf->write_int32(out, 4096);       // p_align
  }
    else
  {
    elf->write_int32(out, 1);          // p_type: 1 (LOAD)
    elf->write_int32(out, 7);          // p_flags: 7 RWX
    elf->write_int64(out, 0x1000);     // p_offset
    elf->write_int64(out, address);    // p_vaddr
    elf->write_int64(out, address);    // p_paddr
    elf->write_int64(out, filesz);     // p_filesz
    elf->write_int64(out, filesz);     // p_memsz
    elf->write_int64(out, 4096);       // p_align
  }
}

static void write_shdr(FILE *out, struct _shdr *shdr, Elf *elf)
{
  if (elf->e_ident[EI_CLASS] == 1)
  {
    elf->write_int32(out, shdr->sh_name);
    elf->write_int32(out, shdr->sh_type);
    elf->write_int32(out, shdr->sh_flags);
    elf->write_int32(out, shdr->sh_addr);
    elf->write_int32(out, shdr->sh_offset);
    elf->write_int32(out, shdr->sh_size);
    elf->write_int32(out, shdr->sh_link);
    elf->write_int32(out, shdr->sh_info);
    elf->write_int32(out, shdr->sh_addralign);
    elf->write_int32(out, shdr->sh_entsize);
  }
    else
  {
    elf->write_int32(out, shdr->sh_name);
    elf->write_int32(out, shdr->sh_type);
    elf->write_int64(out, shdr->sh_flags);
    elf->write_int64(out, shdr->sh_addr);
    elf->write_int64(out, shdr->sh_offset);
    elf->write_int64(out, shdr->sh_size);
    elf->write_int32(out, shdr->sh_link);
    elf->write_int32(out, shdr->sh_info);
    elf->write_int64(out, shdr->sh_addralign);
    elf->write_int64(out, shdr->sh_entsize);
  }
}

static void write_symtab(FILE *out, struct _symtab *symtab, Elf *elf)
{
  if (elf->e_ident[EI_CLASS] == 1)
  {
    elf->write_int32(out, symtab->st_name);
    elf->write_int32(out, symtab->st_value);
    elf->write_int32(out, symtab->st_size);
    putc(symtab->st_info, out);
    putc(symtab->st_other, out);
    elf->write_int16(out, symtab->st_shndx);
  }
    else
  {
    elf->write_int32(out, symtab->st_name);
    putc(symtab->st_info, out);
    putc(symtab->st_other, out);
    elf->write_int16(out, symtab->st_shndx);
    elf->write_int64(out, symtab->st_value);
    elf->write_int64(out, symtab->st_size);
  }
}

static int find_section(const char *sections, const char *name, int len)
{
  int n = 0;

  while (n < len)
  {
    if (sections[n] == '.')
    {
      if (strcmp(sections + n, name) == 0) { return n; }
      n += strlen(sections + n);
    }

    n++;
  }

  return -1;
}

static void elf_addr_align(FILE *out)
{
  long marker = ftell(out);
  while ((marker % 4) != 0) { putc(0x00, out); marker++; }
}

int write_elf(
  Memory *memory,
  FILE *out,
  Symbols *symbols,
  const char *filename,
  int cpu_type,
  int alignment)
{
  struct _shdr shdr;
  struct _symtab symtab;
  Elf elf;
  //int i;

  memset(&elf, 0, sizeof(elf));
  elf.cpu_type = cpu_type;

  memset(&elf.sections_offset, 0, sizeof(elf.sections_offset));
  memset(&elf.sections_size, 0, sizeof(elf.sections_size));

  memcpy(elf.string_table, string_table_default, sizeof(string_table_default));

  write_elf_header(out, &elf, memory);

  // For Playstaiton 2 ELF to be executable, need this LOAD program header
  if (elf.e_phnum > 0)
  {
    uint32_t filesz = memory->high_address - memory->low_address + 1;

    write_phdr(out, &elf, memory->low_address, filesz);

    // Align 4096 for Playstation 2.
    long marker = ftell(out);
    while (marker < 4096) { putc(0, out); marker++; }
  }

  // .text and .data sections
  write_elf_text_and_data(out, &elf, memory, alignment);

  // string index should be next
  //elf.e_shstrndx = elf.e_shnum;

  // .ARM.attribute
  if (elf.cpu_type == CPU_TYPE_ARM)
  {
    write_arm_attribute(out, &elf);
  }

  // .shstrtab section
  elf.sections_offset.shstrtab = ftell(out);
  fwrite(elf.string_table, 1, get_string_table_len(elf.string_table), out);
  putc(0x00, out); // null
  elf.sections_size.shstrtab = ftell(out) - elf.sections_offset.shstrtab;

  int symbol_count = 0;
  const int strtab_extras = 2;

  {
    SymbolsIter iter;
    int sym_offset;
    int n;

    symbol_count = symbols_export_count(symbols);

    int symbol_address[symbol_count];

    // .strtab section
    elf_addr_align(out);
    elf.sections_offset.strtab = ftell(out);
    putc(0x00, out); // none

    fprintf(out, "%s%c", filename, 0);
    sym_offset = strlen(filename) + 2;

    n = 0;
    memset(&iter, 0, sizeof(iter));
    while (symbols_iterate(symbols, &iter) != -1)
    {
      if (iter.flag_export == 0) { continue; }

      symbol_address[n++] = sym_offset;
      fprintf(out, "%s%c", iter.name, 0);
      sym_offset += strlen((char *)iter.name) + 1;
    }

    elf.sections_size.strtab = ftell(out) - elf.sections_offset.strtab;
    //putc(0x00, out); // null

    // .symtab section
    elf_addr_align(out);
    elf.sections_offset.symtab = ftell(out);

    // symtab null
    memset(&symtab, 0, sizeof(symtab));
    write_symtab(out, &symtab, &elf);

    // symtab filename
    memset(&symtab, 0, sizeof(symtab));
    symtab.st_name = 1;
    symtab.st_info = 4;
    symtab.st_shndx = 65521;
    write_symtab(out, &symtab, &elf);

    // symtab text
    memset(&symtab, 0, sizeof(symtab));
    symtab.st_info = 3;
    symtab.st_shndx = 1;
    write_symtab(out, &symtab, &elf);

    // symtab ARM.attribute
    if (elf.cpu_type == CPU_TYPE_ARM)
    {
      memset(&symtab, 0, sizeof(symtab));
      symtab.st_info = 3;
      symtab.st_shndx = elf.e_shnum - 1;
      write_symtab(out, &symtab, &elf);
    }

    // symbols from lookup tables
    n = 0;
    memset(&iter, 0, sizeof(iter));
    while (symbols_iterate(symbols, &iter) != -1)
    {
      if (iter.flag_export == 0) { continue; }

      memset(&symtab, 0, sizeof(symtab));
      symtab.st_name = symbol_address[n++];
      symtab.st_value = iter.address;
      symtab.st_size = 0;
      symtab.st_info = 18;
      symtab.st_shndx = 1;
      write_symtab(out, &symtab, &elf);
    }

    elf.sections_size.symtab = ftell(out) - elf.sections_offset.symtab;
  }

  // .comment section
  elf.sections_offset.comment = ftell(out);
  fprintf(out, "Created with naken_asm. https://www.mikekohn.net/");
  elf.sections_size.comment = ftell(out) - elf.sections_offset.comment;

  // Align sections
  elf_addr_align(out);

  // A little ex-lax to dump the SHT's
  long marker = ftell(out);
  fseek(out, elf.shoff_offset, SEEK_SET);

  // e_shoff (section header offset)
  if (elf.e_ident[EI_CLASS] == 1)
  {
    elf.write_int32(out, marker);
  }
    else
  {
    elf.write_int64(out, marker);
  }

  fseek(out, marker, SEEK_SET);

  // ------------------------ fold here -----------------------------

  // NULL section
  memset(&shdr, 0, sizeof(shdr));
  write_shdr(out, &shdr, &elf);

  elf.e_shstrndx = 1;

  char name[32];

  // SHT .text
  if (elf.sections_offset.text != 0)
  {
    int size = elf.sections_size.text;

    if (alignment != 1)
    {
      int mask = alignment - 1;
      size += (alignment - size) & mask;
    }

    strcpy(name, ".text");

    shdr.sh_name = find_section(elf.string_table, name, sizeof(elf.string_table));
    shdr.sh_type = 1;
    shdr.sh_flags = 6;
    shdr.sh_addr = elf.text_addr; //asm_context->memory.low_address;
    shdr.sh_offset = elf.sections_offset.text;
    shdr.sh_size = elf.sections_size.text;
    shdr.sh_addralign = alignment;
    write_shdr(out, &shdr, &elf);

    elf.e_shstrndx++;
  }

  // SHT .data
  if (elf.sections_offset.data != 0)
  {
    char name[32];
    int size = elf.sections_size.data;

    if (alignment != 1)
    {
      int mask = alignment - 1;
      size += (alignment - size) & mask;
    }

    strcpy(name, ".data");

    shdr.sh_name = find_section(elf.string_table, name, sizeof(elf.string_table));
    shdr.sh_type = 1;
    shdr.sh_flags = 3;
    shdr.sh_addr = elf.data_addr; //asm_context->memory.low_address;
    shdr.sh_offset = elf.sections_offset.data;
    shdr.sh_size = size;
    shdr.sh_addralign = alignment;
    write_shdr(out, &shdr, &elf);

    elf.e_shstrndx++;
  }

  // SHT .shstrtab
  memset(&shdr, 0, sizeof(shdr));
  shdr.sh_name = find_section(elf.string_table, ".shstrtab", sizeof(elf.string_table));
  shdr.sh_type = 3;
  shdr.sh_offset = elf.sections_offset.shstrtab;
  shdr.sh_size = elf.sections_size.shstrtab;
  shdr.sh_addralign = 1;
  write_shdr(out, &shdr, &elf);

  // SHT .symtab
  memset(&shdr, 0, sizeof(shdr));
  shdr.sh_name = find_section(elf.string_table, ".symtab", sizeof(elf.string_table));
  shdr.sh_type = 2;
  shdr.sh_offset = elf.sections_offset.symtab;
  shdr.sh_size = elf.sections_size.symtab;
  shdr.sh_link = 4;
  shdr.sh_info = symbol_count + strtab_extras;
  shdr.sh_addralign = 4;
  shdr.sh_entsize = elf.e_ident[EI_CLASS] == 1 ? 16 : 24;
  write_shdr(out, &shdr, &elf);

  // SHT .strtab
  memset(&shdr, 0, sizeof(shdr));
  shdr.sh_name = find_section(elf.string_table, ".strtab", sizeof(elf.string_table));
  shdr.sh_type = 3;
  shdr.sh_offset = elf.sections_offset.strtab;
  shdr.sh_size = elf.sections_size.strtab;
  shdr.sh_addralign = 1;
  write_shdr(out, &shdr, &elf);

  // SHT .comment
  memset(&shdr, 0, sizeof(shdr));
  shdr.sh_name = find_section(elf.string_table, ".comment", sizeof(elf.string_table));
  shdr.sh_type = 1;
  shdr.sh_flags = 0x30;
  shdr.sh_offset = elf.sections_offset.comment;
  shdr.sh_size = elf.sections_size.comment;
  shdr.sh_addralign = 1;
  shdr.sh_entsize = 1;
  write_shdr(out, &shdr, &elf);

#if 0
  if (asm_context->debug_file == 1)
  {
    // insert debug SHT's
  }
#endif

  // .ARM.attribute
  if (elf.cpu_type == CPU_TYPE_ARM)
  {
    memset(&shdr, 0, sizeof(shdr));
    shdr.sh_name = find_section(elf.string_table, ".ARM.attributes", sizeof(elf.string_table));
    shdr.sh_type = 1879048195;  // wtf?
    shdr.sh_offset = elf.sections_offset.arm_attribute;
    shdr.sh_size = elf.sections_size.arm_attribute;
    shdr.sh_addralign = 1;
    write_shdr(out, &shdr, &elf);
  }

  marker = ftell(out);
  fseek(out, elf.shnum_offset, SEEK_SET);
  elf.write_int16(out, elf.e_shnum);    // e_shnum (section count)
  elf.write_int16(out, elf.e_shstrndx); // e_shstrndx (string_table index)
  fseek(out, marker, SEEK_SET);

  return 0;
}

