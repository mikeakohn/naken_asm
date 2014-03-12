/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "symbols.h"
#include "write_elf.h"

typedef void(*write_int32_t)(FILE *, unsigned int);
typedef void(*write_int16_t)(FILE *, unsigned int);

struct _elf
{
  struct _sections_offset sections_offset;
  struct _sections_size sections_size;
  uint8_t e_ident[16];
  uint32_t e_flags;
  uint16_t e_machine;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
  int text_count;
  int data_count;
  int cpu_type;
  int text_addr[ELF_TEXT_MAX];
  int data_addr[ELF_TEXT_MAX];
  char string_table[1024];
  write_int32_t write_int32;
  write_int16_t write_int16;
};

static const char magic_number[16] =
{
  0x7f, 'E','L','F',  1, 1, 1, 0, //0xff,
  0, 0, 0, 0,  0, 0, 0, 0
};

// For now we will only have .text, .shstrtab, .symtab, strtab
// I can't see a use for .data and .bss unless the bootloaders know elf?
// I'm also not supporting relocations.  I can do it later if requested.

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

static void write_int32_le(FILE *out, uint32_t n)
{
  putc(n&0xff, out);
  putc((n>>8)&0xff, out);
  putc((n>>16)&0xff, out);
  putc((n>>24)&0xff, out);
}

static void write_int16_le(FILE *out, uint32_t n)
{
  putc(n&0xff, out);
  putc((n>>8)&0xff, out);
}

static void write_int32_be(FILE *out, uint32_t n)
{
  putc((n>>24)&0xff, out);
  putc((n>>16)&0xff, out);
  putc((n>>8)&0xff, out);
  putc(n&0xff, out);
}

static void write_int16_be(FILE *out, uint32_t n)
{
  putc((n>>8)&0xff, out);
  putc(n&0xff, out);
}

static void write_elf_header(FILE *out, struct _elf *elf, struct _memory *memory)
{
  #define EI_DATA 5    // 1=little endian, 2=big endian
  #define EI_OSABI 7   // 0=SysV, 255=Embedded

  memcpy(elf->e_ident, magic_number, 16);

  if (memory->endian == ENDIAN_LITTLE)
  {
    elf->e_ident[EI_DATA] = 1;
    elf->write_int32=write_int32_le;
    elf->write_int16=write_int16_le;
  }
    else
  {
    elf->e_ident[EI_DATA] = 2;
    elf->write_int32=write_int32_be;
    elf->write_int16=write_int16_be;
  }

  // This probably should be 0 for Raspberry Pi, etc.
  elf->e_ident[EI_OSABI] = 255;

  // We may need this later
  //elf.e_ident[7]=0xff; // SYSV=0, HPUX=1, STANDALONE=255

  // mspgcc4/build/insight-6.8-1/include/elf/msp430.h (11)
  elf->e_flags = 0;
  elf->e_shnum = 4;

  // This could be a lookup table, but let's play it safe
  switch (elf->cpu_type)
  {
    case CPU_TYPE_MSP430:
    case CPU_TYPE_MSP430X:
      elf->e_machine = 0x69;
      elf->e_flags = 11;
      break;
    case CPU_TYPE_ARM:
      elf->e_machine = 40;
      elf->e_flags = 0x05000000;
      elf->e_shnum++;
      break;
    case CPU_TYPE_DSPIC:
      elf->e_machine = 118;
      elf->e_flags = 1;
      break;
    case CPU_TYPE_MIPS:
      elf->e_machine = 8;
      break;
    default:
printf("WTF? %d %d\n", elf->cpu_type, CPU_TYPE_MSP430);
      elf->e_machine = 0;
      break;
  }

  //if (asm_context->debug_file==1) { elf->e_shnum+=4; }
  elf->e_shnum++; // Null section to start...

  // Write Ehdr;
  fwrite(elf->e_ident, 1, 16, out);
  elf->write_int16(out, 1);       // e_type 0=not relocatable 1=msp_32
  elf->write_int16(out, elf->e_machine); // e_machine EM_MSP430=0x69
  elf->write_int32(out, 1);       // e_version
  elf->write_int32(out, 0);       // e_entry (this could be 16 bit at 0xfffe)
  elf->write_int32(out, 0);       // e_phoff (program header offset)
  elf->write_int32(out, 0);       // e_shoff (section header offset)
  elf->write_int32(out, elf->e_flags); // e_flags (should be set to CPU model)
  elf->write_int16(out, 0x34);    // e_ehsize (size of this struct)
  elf->write_int16(out, 0);       // e_phentsize (program header size)
  elf->write_int16(out, 0);       // e_phnum (number of program headers)
  elf->write_int16(out, 40);      // e_shentsize (section header size)
  elf->write_int16(out, elf->e_shnum); // e_shnum (number of section headers)
  elf->write_int16(out, 2);       // e_shstrndx (section header string table index)
}

static void optimize_memory(struct _memory *memory)
{
  int i = 0;

  while (i <= memory->high_address)
  {
    if (memory_debug_line_m(memory, i) == -2)
    {
      // Fill gaps in data sections
      while(memory_debug_line_m(memory, i) == -2)
      {
        //printf("Found data %02x\n", i);
        while(i <= memory->high_address && memory_debug_line_m(memory, i) == -2)
        { i += 2; }
        //printf("End data %02x\n", i);
        if (i == memory->high_address) break;
        int marker = i;
        while(i <= memory->high_address && memory_debug_line_m(memory, i) == -1)
        { i += 2; }
        if (i == memory->high_address) break;

        if (i <= memory->high_address && memory_debug_line_m(memory, i)==-2)
        {
          while(marker != i) { memory_debug_line_set_m(memory, marker++, -2); }
        }
        if (i == memory->high_address) break;
      }
    }
      else
    if (memory_debug_line_m(memory, i) >= 0)
    {
      // Fill gaps in code sections
      while(i<=memory->high_address && memory_debug_line_m(memory, i)>=0)
      {
        while(i <= memory->high_address && (memory_debug_line_m(memory, i)>=0 || memory_debug_line_m(memory, i) == -3))
        { i += 2; }
        if (i == memory->high_address) { break; }

        int marker=i;
        while(i <= memory->high_address && memory_debug_line_m(memory, i) == -1)
        { i += 2; }

        if (i == memory->high_address) { break; }

        if (memory_debug_line_m(memory, i) >= 0)
        {
          while(marker != i) { memory_debug_line_set_m(memory, marker++, -3); }
        }
        if (i == memory->high_address) break;
      }
    }

    i += 2;
  }
}

static int get_string_table_len(char *string_table)
{
int n=0;

  while(string_table[n]!=0 || string_table[n+1]!=0) { n++; }

  return n+1;
}

static void string_table_append(struct _elf *elf, char *name)
{
int len;

  char *string_table = elf->string_table;

  len = get_string_table_len(string_table);
  string_table = string_table+len;
  strcpy(string_table, name);
  string_table = string_table+strlen(name)+1;
  *string_table = 0;
}

static void write_elf_text_and_data(FILE *out, struct _elf *elf, struct _memory *memory)
{
  int i = 0;

  while (i <= memory->high_address)
  {
    char name[32];

    if (memory_debug_line_m(memory, i) == -2)
    {
      if (elf->data_count>=ELF_TEXT_MAX)
      {
        printf("Too many elf .data sections (count=%d).  Internal error.\n", elf->data_count);
        exit(1);
      }

      if (elf->data_count == 0) { strcpy(name, ".data"); }
      else { sprintf(name, ".data%d", elf->data_count); }

      elf->data_addr[elf->data_count] = i;
      string_table_append(elf, name);
      elf->sections_offset.data[elf->data_count] = ftell(out);

      while(memory_debug_line_m(memory, i) == -2)
      {
        putc(memory_read_m(memory, i++), out);
        putc(memory_read_m(memory, i++), out);
      }

      elf->sections_size.data[elf->data_count] = ftell(out)-elf->sections_offset.data[elf->data_count];

      elf->data_count++;
      elf->e_shnum++;
    }
      else
    if (memory_debug_line_m(memory, i)!=-1)
    {
      if (elf->text_count >= ELF_TEXT_MAX)
      {
        printf("Too many elf .text sections(%d).  Internal error.\n", elf->text_count);
        exit(1);
      }

      if (elf->text_count == 0) { strcpy(name, ".text"); }
      else { sprintf(name, ".text%d", elf->text_count); }

      elf->text_addr[elf->text_count] = i;
      string_table_append(elf, name);
      elf->sections_offset.text[elf->text_count] = ftell(out);

      while(1)
      {
        int debug_line = memory_debug_line_m(memory, i);
        if (!(debug_line >= 0 || debug_line == -3))
        { break; }

        putc(memory_read_m(memory, i++), out);
        putc(memory_read_m(memory, i++), out);
      }

      elf->sections_size.text[elf->text_count] = ftell(out)-elf->sections_offset.text[elf->text_count];

      elf->text_count++;
      elf->e_shnum++;
    }

    i++;
  }

  elf->e_shstrndx = elf->data_count + elf->text_count+1;
}

static void write_arm_attribute(FILE *out, struct _elf *elf)
{
  const unsigned char aeabi[] = {
    0x41, 0x30, 0x00, 0x00, 0x00, 0x61, 0x65, 0x61,
    0x62, 0x69, 0x00, 0x01, 0x26, 0x00, 0x00, 0x00,
    0x05, 0x36, 0x00, 0x06, 0x06, 0x08, 0x01, 0x09,
    0x01, 0x0a, 0x02, 0x12, 0x04, 0x14, 0x01, 0x15,
    0x01, 0x17, 0x03, 0x18, 0x01, 0x19, 0x01, 0x1a,
    0x02, 0x1b, 0x03, 0x1c, 0x01, 0x1e, 0x06, 0x2c,
    0x01
  };

#if 0
  const unsigned char aeabi[] = {
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

static void write_shdr(FILE *out, struct _shdr *shdr, struct _elf *elf)
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

static void write_symtab(FILE *out, struct _symtab *symtab, struct _elf *elf)
{
  elf->write_int32(out, symtab->st_name);
  elf->write_int32(out, symtab->st_value);
  elf->write_int32(out, symtab->st_size);
  putc(symtab->st_info, out);
  putc(symtab->st_other, out);
  elf->write_int16(out, symtab->st_shndx);
}

static int find_section(char *sections, char *name, int len)
{
int n = 0;

  while(n < len)
  {
    if (sections[n] == '.')
    {
      if (strcmp(sections+n, name) == 0) return n;
      n += strlen(sections+n);
    }

    n++;
  }

  return -1;
}

static void elf_addr_align(FILE *out)
{
  long marker = ftell(out);
  while((marker%4)!=0) { putc(0x00, out); marker++; }
}

int write_elf(struct _memory *memory, FILE *out, struct _symbols *symbols, const char *filename, int cpu_type)
{
struct _shdr shdr;
struct _symtab symtab;
struct _elf elf;
int i;

  memset(&elf, 0, sizeof(elf));
  elf.cpu_type = cpu_type;

  memset(&elf.sections_offset, 0, sizeof(elf.sections_offset));
  memset(&elf.sections_size, 0, sizeof(elf.sections_size));

  memcpy(elf.string_table, string_table_default, sizeof(string_table_default));

  // Fill in blank to minimize text and data sections
  optimize_memory(memory);

  write_elf_header(out, &elf, memory);

  // .text and .data sections
  write_elf_text_and_data(out, &elf, memory);

  // .ARM.attribute
  if (elf.cpu_type == CPU_TYPE_ARM)
  {
    write_arm_attribute(out, &elf);
  }

  // .shstrtab section
  elf.sections_offset.shstrtab = ftell(out);
  i = fwrite(elf.string_table, 1, get_string_table_len(elf.string_table), out);
  putc(0x00, out); // null
  elf.sections_size.shstrtab = ftell(out)-elf.sections_offset.shstrtab;

  {
    struct _symbols_iter iter;
    int symbol_count;
    int sym_offset;
    int n;

    symbol_count = symbols_export_count(symbols);

    int symbol_address[symbol_count];

    // .strtab section
    elf_addr_align(out);
    elf.sections_offset.strtab = ftell(out);
    putc(0x00, out); // none

    fprintf(out, "%s%c", filename, 0);
    sym_offset = strlen(filename)+2;

    n = 0;
    memset(&iter, 0, sizeof(iter));
    while(symbols_iterate(symbols, &iter) != -1)
    {
      if (iter.flag_export == 0) { continue; }

      symbol_address[n++] = sym_offset;
      fprintf(out, "%s%c", iter.name, 0);
      sym_offset += strlen((char *)iter.name)+1;
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
      symtab.st_info=3;
      symtab.st_shndx=elf.e_shnum-1;
      write_symtab(out, &symtab, &elf);
    }

    // symbols from lookup tables
    n = 0;
    memset(&iter, 0, sizeof(iter));
    while(symbols_iterate(symbols, &iter)!=-1)
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
  fprintf(out, "Created with naken_asm.  http://www.mikekohn.net/");
  elf.sections_size.comment = ftell(out) - elf.sections_offset.comment;

#if 0
  if (asm_context->debug_file==1)
  {
    // insert debug sections
  }
#endif

  // A little ex-lax to dump the SHT's
  long marker = ftell(out);
  fseek(out, 32, SEEK_SET);
  elf.write_int32(out, marker);         // e_shoff (section header offset)
  fseek(out, 0x30, SEEK_SET);
  elf.write_int16(out, elf.e_shnum);    // e_shnum (section count)
  elf.write_int16(out, elf.e_shstrndx); // e_shstrndx (string_table index)
  fseek(out, marker, SEEK_SET);

  // ------------------------ fold here -----------------------------

  // Let's align this eventually
  //elf_addr_align(out);

  // NULL section
  memset(&shdr, 0, sizeof(shdr));
  write_shdr(out, &shdr, &elf);

  // SHT .text
  for (i = 0; i < elf.text_count; i++)
  {
    char name[32];
    if (i == 0) { strcpy(name, ".text"); }
    else { sprintf(name, ".text%d", i); }
    shdr.sh_name = find_section(elf.string_table, name, sizeof(elf.string_table));
    shdr.sh_type = 1;
    shdr.sh_flags = 6;
    shdr.sh_addr = elf.text_addr[i]; //asm_context->memory.low_address;
    shdr.sh_offset = elf.sections_offset.text[i];
    shdr.sh_size = elf.sections_size.text[i];
    shdr.sh_addralign = 1;
    write_shdr(out, &shdr, &elf);
  }

  // SHT .data
  for (i = 0; i < elf.data_count; i++)
  {
    char name[32];
    if (i == 0) { strcpy(name, ".data"); }
    else { sprintf(name, ".data%d", i); }
    shdr.sh_name = find_section(elf.string_table, name, sizeof(elf.string_table));
    shdr.sh_type = 1;
    shdr.sh_flags = 3;
    shdr.sh_addr = elf.data_addr[i]; //asm_context->memory.low_address;
    shdr.sh_offset = elf.sections_offset.data[i];
    shdr.sh_size = elf.sections_size.data[i];
    shdr.sh_addralign = 1;
    write_shdr(out, &shdr, &elf);
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
  shdr.sh_info = 4;
  shdr.sh_addralign = 4;
  shdr.sh_entsize = 16;
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
  if (asm_context->debug_file==1)
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
    shdr.sh_size=elf.sections_size.arm_attribute;
    shdr.sh_addralign = 1;
    write_shdr(out, &shdr, &elf);
  }
 
  return 0;
}



