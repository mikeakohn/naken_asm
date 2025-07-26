/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/Symbols.h"
#include "fileio/FileIo.h"
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

static void write_elf_header(
  FileIo &file,
  Elf *elf,
  Memory *memory,
  int alignment)
{
  #define EI_CLASS 4   // 1=32 bit, 2=64 bit
  #define EI_DATA  5   // 1=little endian, 2=big endian
  #define EI_OSABI 7   // 0=SysV, 255=Embedded

  memcpy(elf->e_ident, magic_number, 16);

  if (memory->endian == ENDIAN_LITTLE)
  {
    elf->e_ident[EI_DATA] = 1;
    file.set_endian(FileIo::FILE_ENDIAN_LITTLE);
  }
    else
  {
    elf->e_ident[EI_DATA] = 2;
    file.set_endian(FileIo::FILE_ENDIAN_BIG);
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
    case CPU_TYPE_ARM64:
      elf->e_machine = 183;
      elf->e_ident[EI_CLASS] = 2;
      break;
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
      elf->e_ident[EI_CLASS] = alignment == 8 ? 2 : 1;
      // FIXME: This should be passed in from the main code.
      // 0x0001 means EF_RISCV_RVC
      // 0x0004 means EF_RISCV_FLOAT_ABI_DOUBLE
      elf->e_flags = 5;
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
  file.write_bytes(elf->e_ident, 16);

  if (elf->e_ident[EI_CLASS] == 1)
  {
    file.write_int16(elf->e_type);    // e_type 0=not relocatable 1=msp_32
    file.write_int16(elf->e_machine); // e_machine EM_MSP430=0x69
    file.write_int32(1);              // e_version
    file.write_int32(elf->e_entry);   // e_entry (start address)
    file.write_int32(elf->e_phoff);   // e_phoff (program header offset)
    elf->shoff_offset = file.tell();
    file.write_int32(0);              // e_shoff (section header offset)
    file.write_int32(elf->e_flags);   // e_flags (set to CPU model)
    file.write_int16(0x34);           // e_ehsize (size of this struct)
    file.write_int16(elf->e_phentsize); // e_phentsize (pheader size)
    file.write_int16(elf->e_phnum);   // e_phnum (program headers count)
    file.write_int16(40);             // e_shentsize (section header size)
    elf->shnum_offset = file.tell();
    file.write_int16(elf->e_shnum);   // e_shnum (section headers count)
    file.write_int16(2);              // e_shstrndx (section header string table index)
  }
    else
  {
    file.write_int16(elf->e_type);    // e_type 0=not relocatable 1=msp_32
    file.write_int16(elf->e_machine); // e_machine EM_MSP430=0x69
    file.write_int32(1);              // e_version
    file.write_int64(elf->e_entry);   // e_entry (start address)
    file.write_int64(elf->e_phoff);   // e_phoff (program header offset)
    elf->shoff_offset = file.tell();
    file.write_int64(0);              // e_shoff (section header offset)
    file.write_int32(elf->e_flags);   // e_flags (set to CPU model)
    file.write_int16(0x34);           // e_ehsize (size of this struct)
    file.write_int16(elf->e_phentsize); // e_phentsize (pheader size)
    file.write_int16(elf->e_phnum);   // e_phnum (program headers count)
    file.write_int16(64);             // e_shentsize (section header size)
    elf->shnum_offset = file.tell();
    file.write_int16(elf->e_shnum);   // e_shnum (section headers count)
    file.write_int16(2);              // e_shstrndx (section header string table index)
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
  FileIo &file,
  Elf *elf,
  Memory *memory,
  int alignment)
{
  const char *name = ".text";
  uint32_t i;

  elf->text_addr = memory->low_address;
  string_table_append(elf, name);
  elf->sections_offset.text = file.tell();

  for (i = memory->low_address; i <= memory->high_address; i++)
  {
    file.write_int8(memory->read8(i));
  }

  if (alignment > 1)
  {
    int count = memory->high_address - memory->low_address + 1;
    int mask = alignment - 1;

    while ((count & mask) != 0)
    {
      file.write_int8(0);
      count++;
    }
  }

  elf->sections_size.text = file.tell() - elf->sections_offset.text;

  elf->e_shnum++;
}

static void write_arm_attribute(FileIo &file, Elf *elf)
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

  elf->sections_offset.arm_attribute = file.tell();
  file.write_bytes(aeabi, sizeof(aeabi));
  elf->sections_size.arm_attribute = file.tell() - elf->sections_offset.arm_attribute;
  file.write_int8(0x00); // null
  file.write_int8(0x00); // null
  file.write_int8(0x00); // null
}

static void write_phdr(
  FileIo &file,
  Elf *elf,
  uint32_t address,
  uint32_t filesz)
{
  if (elf->e_ident[EI_CLASS] == 1)
  {
    file.write_int32(1);          // p_type: 1 (LOAD)
    file.write_int32(0x1000);     // p_offset
    file.write_int32(address);    // p_vaddr
    file.write_int32(address);    // p_paddr
    file.write_int32(filesz);     // p_filesz
    file.write_int32(filesz);     // p_memsz
    file.write_int32(7);          // p_flags: 7 RWX
    file.write_int32(4096);       // p_align
  }
    else
  {
    file.write_int32(1);          // p_type: 1 (LOAD)
    file.write_int32(7);          // p_flags: 7 RWX
    file.write_int64(0x1000);     // p_offset
    file.write_int64(address);    // p_vaddr
    file.write_int64(address);    // p_paddr
    file.write_int64(filesz);     // p_filesz
    file.write_int64(filesz);     // p_memsz
    file.write_int64(4096);       // p_align
  }
}

static void write_shdr(FileIo &file, struct _shdr *shdr, Elf *elf)
{
  if (elf->e_ident[EI_CLASS] == 1)
  {
    file.write_int32(shdr->sh_name);
    file.write_int32(shdr->sh_type);
    file.write_int32(shdr->sh_flags);
    file.write_int32(shdr->sh_addr);
    file.write_int32(shdr->sh_offset);
    file.write_int32(shdr->sh_size);
    file.write_int32(shdr->sh_link);
    file.write_int32(shdr->sh_info);
    file.write_int32(shdr->sh_addralign);
    file.write_int32(shdr->sh_entsize);
  }
    else
  {
    file.write_int32(shdr->sh_name);
    file.write_int32(shdr->sh_type);
    file.write_int64(shdr->sh_flags);
    file.write_int64(shdr->sh_addr);
    file.write_int64(shdr->sh_offset);
    file.write_int64(shdr->sh_size);
    file.write_int32(shdr->sh_link);
    file.write_int32(shdr->sh_info);
    file.write_int64(shdr->sh_addralign);
    file.write_int64(shdr->sh_entsize);
  }
}

static void write_symtab(FileIo &file, struct _symtab *symtab, Elf *elf)
{
  if (elf->e_ident[EI_CLASS] == 1)
  {
    file.write_int32(symtab->st_name);
    file.write_int32(symtab->st_value);
    file.write_int32(symtab->st_size);
    file.write_int8(symtab->st_info);
    file.write_int8(symtab->st_other);
    file.write_int16(symtab->st_shndx);
  }
    else
  {
    file.write_int32(symtab->st_name);
    file.write_int8(symtab->st_info);
    file.write_int8(symtab->st_other);
    file.write_int16(symtab->st_shndx);
    file.write_int64(symtab->st_value);
    file.write_int64(symtab->st_size);
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

int write_elf(
  Memory *memory,
  FILE *out_,
  Symbols *symbols,
  const char *filename,
  int cpu_type,
  int alignment)
{
  struct _shdr shdr;
  struct _symtab symtab;
  Elf elf;
  FileIo file;
  //int i;

  file.set_fp(out_);

  memset(&elf, 0, sizeof(elf));
  elf.cpu_type = cpu_type;

  memset(&elf.sections_offset, 0, sizeof(elf.sections_offset));
  memset(&elf.sections_size, 0, sizeof(elf.sections_size));

  memcpy(elf.string_table, string_table_default, sizeof(string_table_default));

  write_elf_header(file, &elf, memory, alignment);

  // For Playstaiton 2 ELF to be executable, need this LOAD program header
  if (elf.e_phnum > 0)
  {
    uint32_t filesz = memory->high_address - memory->low_address + 1;

    write_phdr(file, &elf, memory->low_address, filesz);

    // Align 4096 for Playstation 2.
    long marker = file.tell();
    while (marker < 4096) { file.write_int8(0); marker++; }
  }

  // .text and .data sections
  write_elf_text_and_data(file, &elf, memory, alignment);

  // string index should be next
  //elf.e_shstrndx = elf.e_shnum;

  // .ARM.attribute
  if (elf.cpu_type == CPU_TYPE_ARM)
  {
    write_arm_attribute(file, &elf);
  }

  // .shstrtab section
  elf.sections_offset.shstrtab = file.tell();
  file.write_chars(elf.string_table, get_string_table_len(elf.string_table));
  file.write_int8(0x00); // null
  elf.sections_size.shstrtab = file.tell() - elf.sections_offset.shstrtab;

  int symbol_count = 0;
  const int strtab_extras = 2;

  {
    SymbolsIter iter;
    int sym_offset;
    int n;

    symbol_count = symbols->export_count();

    int symbol_address[symbol_count];

    // .strtab section
    file.align(4);
    elf.sections_offset.strtab = file.tell();
    file.write_int8(0x00); // none

    file.write_string(filename);
    sym_offset = strlen(filename) + 2;

    n = 0;
    iter.reset();

    while (symbols->iterate(&iter) != -1)
    {
      if (iter.flag_export == false) { continue; }

      symbol_address[n++] = sym_offset;
      file.write_string(iter.name);
      sym_offset += strlen((char *)iter.name) + 1;
    }

    elf.sections_size.strtab = file.tell() - elf.sections_offset.strtab;

    // .symtab section
    file.align(4);
    elf.sections_offset.symtab = file.tell();

    // symtab null
    memset(&symtab, 0, sizeof(symtab));
    write_symtab(file, &symtab, &elf);

    // symtab filename
    memset(&symtab, 0, sizeof(symtab));
    symtab.st_name = 1;
    symtab.st_info = 4;
    symtab.st_shndx = 65521;
    write_symtab(file, &symtab, &elf);

    // symtab text
    memset(&symtab, 0, sizeof(symtab));
    symtab.st_info = 3;
    symtab.st_shndx = 1;
    write_symtab(file, &symtab, &elf);

    // symtab ARM.attribute
    if (elf.cpu_type == CPU_TYPE_ARM)
    {
      memset(&symtab, 0, sizeof(symtab));
      symtab.st_info = 3;
      symtab.st_shndx = elf.e_shnum - 1;
      write_symtab(file, &symtab, &elf);
    }

    // symbols from lookup tables
    n = 0;
    iter.reset();

    while (symbols->iterate(&iter) != -1)
    {
      if (iter.flag_export == false) { continue; }

      memset(&symtab, 0, sizeof(symtab));
      symtab.st_name = symbol_address[n++];
      symtab.st_value = iter.address;
      symtab.st_size = 0;
      symtab.st_info = 18;
      symtab.st_shndx = 1;
      write_symtab(file, &symtab, &elf);
    }

    elf.sections_size.symtab = file.tell() - elf.sections_offset.symtab;
  }

  // .comment section
  elf.sections_offset.comment = file.tell();
  file.write_string("Created with naken_asm. https://www.mikekohn.net/", false);
  elf.sections_size.comment = file.tell() - elf.sections_offset.comment;

  // Align sections
  file.align(4);

  // A little ex-lax to dump the SHT's
  long marker = file.tell();
  file.set(elf.shoff_offset);

  // e_shoff (section header offset)
  if (elf.e_ident[EI_CLASS] == 1)
  {
    file.write_int32(marker);
  }
    else
  {
    file.write_int64(marker);
  }

  file.set(marker);

  // ------------------------ fold here -----------------------------

  // NULL section
  memset(&shdr, 0, sizeof(shdr));
  write_shdr(file, &shdr, &elf);

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
    write_shdr(file, &shdr, &elf);

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
    write_shdr(file, &shdr, &elf);

    elf.e_shstrndx++;
  }

  // SHT .shstrtab
  memset(&shdr, 0, sizeof(shdr));
  shdr.sh_name = find_section(elf.string_table, ".shstrtab", sizeof(elf.string_table));
  shdr.sh_type = 3;
  shdr.sh_offset = elf.sections_offset.shstrtab;
  shdr.sh_size = elf.sections_size.shstrtab;
  shdr.sh_addralign = 1;
  write_shdr(file, &shdr, &elf);

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
  write_shdr(file, &shdr, &elf);

  // SHT .strtab
  memset(&shdr, 0, sizeof(shdr));
  shdr.sh_name = find_section(elf.string_table, ".strtab", sizeof(elf.string_table));
  shdr.sh_type = 3;
  shdr.sh_offset = elf.sections_offset.strtab;
  shdr.sh_size = elf.sections_size.strtab;
  shdr.sh_addralign = 1;
  write_shdr(file, &shdr, &elf);

  // SHT .comment
  memset(&shdr, 0, sizeof(shdr));
  shdr.sh_name = find_section(elf.string_table, ".comment", sizeof(elf.string_table));
  shdr.sh_type = 1;
  shdr.sh_flags = 0x30;
  shdr.sh_offset = elf.sections_offset.comment;
  shdr.sh_size = elf.sections_size.comment;
  shdr.sh_addralign = 1;
  shdr.sh_entsize = 1;
  write_shdr(file, &shdr, &elf);

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
    write_shdr(file, &shdr, &elf);
  }

  marker = file.tell();
  file.set(elf.shnum_offset);
  file.write_int16(elf.e_shnum);    // e_shnum (section count)
  file.write_int16(elf.e_shstrndx); // e_shstrndx (string_table index)
  file.set(marker);

  // FIXME: naken_asm shouldn't be controlling the FILE *.
  file.set_fp(NULL);
  file.close_file();

  return 0;
}

