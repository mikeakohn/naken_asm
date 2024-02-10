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

#include "common/assembler.h"
#include "common/Symbols.h"
#include "fileio/FileIo.h"
#include "fileio/write_macho.h"

int write_macho(
  Memory *memory,
  FILE *out,
  Symbols *symbols,
  const char *filename,
  int cpu_type,
  int alignment)
{
  FileIo file;
  SymbolsIter iter;
  int sym_offset;
  int symbol_count = symbols->export_count();
  long marker;
  int length;

  struct Markers
  {
    long load_command_size;
    long segment_start;
    long segment_file_load;
    long section_text;
    long tables;
    long string_table;
    long symbol_table;
  } markers;

  file.set_fp(out);

  // FIXME: This probably needs to be done after the header is written.
  if (memory->endian == ENDIAN_LITTLE)
  {
    file.set_endian(FileIo::FILE_ENDIAN_LITTLE);
  }
    else
  {
    file.set_endian(FileIo::FILE_ENDIAN_BIG);
  }

  // Mach-O, Mach-O file. I'm going to make a Mach-O file.
  uint32_t magic_number = 0xfeedface;
  int bits = 32;

  switch (cpu_type)
  {
    case CPU_TYPE_POWERPC:
      cpu_type = 0x12;
      break;
    case CPU_TYPE_ARM:
      cpu_type = 0x0c;
      break;
    case CPU_TYPE_ARM64:
      cpu_type = 0x01000000 | 0x0c;
      magic_number = 0xfeedfacf;
      bits = 64;
      break;
    default:
      break;
  }

  file.write_int32(magic_number);
  file.write_int32(cpu_type);
  file.write_int32(0);            // cpu_subtype
  file.write_int32(1);            // file_type
  file.write_int32(4);            // load_command_count
  markers.load_command_size = file.tell();
  file.write_int32(0);            // load_command_size
  file.write_int32(0);            // flags

  if (bits == 64)
  {
    file.write_int32(0);            // 4 extra alignment bytes for 64 bit
  }

  char name[16];

  // Segment Load.
  markers.segment_start = file.tell();

  if (bits == 32)
  {
    file.write_int32(0x00000001);
  }
    else
  {
    file.write_int32(0x00000019);
  }

  file.write_int32(0);

  memset(name, 0, sizeof(name));
  file.write_chars(name, sizeof(name));

  uint32_t filesz = memory->high_address - memory->low_address + 1;

  if (bits == 32)
  {
    file.write_int32(0);      // address
    file.write_int32(filesz); // address_size
    markers.segment_file_load = file.tell();
    file.write_int32(0);      // file_offset
    file.write_int32(filesz); // file_size
  }
    else
  {
    file.write_int64(0);      // address
    file.write_int64(filesz); // address_size
    markers.segment_file_load = file.tell();
    file.write_int64(0);      // file_offset
    file.write_int64(filesz); // file_size
  }

  file.write_int32(7); // protection_max
  file.write_int32(7); // protection_initial
  file.write_int32(1); // section_count
  file.write_int32(0); // flag

  // -- section.

  strcpy(name, "__text");
  file.write_chars(name, sizeof(name));
  strcpy(name, "__TEXT");
  file.write_chars(name, sizeof(name));

  if (bits == 32)
  {
    file.write_int32(0);      // address
    file.write_int32(filesz); // size
  }
    else
  {
    file.write_int64(0);      // address
    file.write_int64(filesz); // size
  }

  markers.section_text = file.tell();

  file.write_int32(0);           // offset
  file.write_int32(0);           // align
  file.write_int32(0);           // relocation_offset
  file.write_int32(0);           // relocation_count
  file.write_int32(-2147482624); // flags
  file.write_int32(0);           // reserved1
  file.write_int32(0);           // reserved2
  file.write_int32(0);           // reserved3

  marker = file.tell();
  length = marker - markers.segment_start;
  file.write_int32_at_offset(length, markers.segment_start + 4);

  // Segment Build Version.
  file.write_int32(0x00000032);
  file.write_int32(24);
  file.write_int32(1);
  file.write_int32(0x000d0100);
  file.write_int32(0);
  file.write_int32(0);

  // Symbol Table.
  file.write_int32(0x00000002);
  file.write_int32(24);
  markers.tables = file.tell();
  file.write_int32(0);                // symbol_table_offset
  file.write_int32(symbol_count + 1); // symbol_count
  file.write_int32(0);                // string_table_offset
  file.write_int32(0);                // string_table_size

  // Dysymtab.
  file.write_int32(0x0000000b);
  file.write_int32(0x50);
  file.write_int32(0); //local_sym_index
  file.write_int32(symbol_count);     //local_sym_count
  file.write_int32(symbol_count);     //external_sym_index
  file.write_int32(symbol_count);     //external_sym_count
  file.write_int32(symbol_count + 1); //undefined_sym_index
  file.write_int32(0); //undefined_sym_count
  file.write_int32(0); //toc_offset
  file.write_int32(0); //toc_count
  file.write_int32(0); //mod_table_offset
  file.write_int32(0); //mod_count
  file.write_int32(0); //ref_sym_offset
  file.write_int32(0); //ref_sym_count
  file.write_int32(0); //indirect_sym_index
  file.write_int32(0); //indirect_sym_count
  file.write_int32(0); //external_reloc_offset
  file.write_int32(0); //external_reloc_count
  file.write_int32(0); //local_reloc_offset
  file.write_int32(0); //local_reloc_count

  file.align(bits == 32 ? 4 : 8);

  marker = file.tell();
  file.write_int32_at_offset(marker - markers.segment_start, markers.load_command_size);

  // Add code.
  for (uint32_t i = memory->low_address; i <= memory->high_address; i++)
  {
    file.write_int8(memory->read8(i));
  }

  file.write_int32_at_offset(marker, markers.segment_file_load);
  file.write_int32_at_offset(marker, markers.section_text);

  // Add string table.
  markers.string_table = file.tell();
  file.write_int8(0);

  iter.reset();
  sym_offset = 1;

  while (symbols->iterate(&iter) != -1)
  {
    if (iter.flag_export == false) { continue; }
    file.write_string(iter.name);
    sym_offset += strlen((char *)iter.name) + 1;
  }

  int ltmp0_offset = sym_offset; 
  file.write_string("ltmp0");

  file.write_int8(0);
  file.align(4);

  int symbol_table_length = file.tell() - markers.string_table;
  file.write_int32_at_offset(markers.string_table, markers.tables + 8);
  file.write_int32_at_offset(symbol_table_length, markers.tables + 12);

  // Add symbol table.
  markers.symbol_table = file.tell();
  iter.reset();
  sym_offset = 1;

  file.write_int32(ltmp0_offset);
  file.write_int8(0x0e);
  file.write_int8(1);
  file.write_int16(0);

  if (bits == 32)
  {
    file.write_int32(0);
  }
    else
  {
    file.write_int64(0);
  }

  while (symbols->iterate(&iter) != -1)
  {
    if (iter.flag_export == false) { continue; }

    file.write_int32(sym_offset);
    file.write_int8(0x0f);
    file.write_int8(1);
    file.write_int16(0);

    if (bits == 32)
    {
      file.write_int32(iter.address);
    }
      else
    {
      file.write_int64(iter.address);
    }

    sym_offset += strlen((char *)iter.name) + 1;
  }

  //int string_table_length = file.tell() - markers.symbol_table;
  file.write_int32_at_offset(markers.symbol_table, markers.tables + 0);

  // FIXME: naken_asm shouldn't be controlling the FILE *.
  file.set_fp(NULL);
  file.close_file();

  return 0;
}

