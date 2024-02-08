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
#include "fileio/read_macho.h"

struct MachoHeader
{
  uint32_t magic_number;
  uint32_t cpu_type;
  uint32_t cpu_subtype;
  uint32_t file_type;
  uint32_t load_command_count;
  uint32_t load_command_size;
  uint32_t flags;
  uint32_t reserved;
};

struct MachoLoadCommand
{
  uint32_t type;
  uint32_t size;
};

struct MachoSegmentLoad
{
  char name[16];
  uint64_t address;
  uint64_t address_size;
  uint64_t file_offset;
  uint64_t file_size;
  uint32_t protection_max;
  uint32_t protection_initial;
  uint32_t section_count;
  uint32_t flag;
};

struct MachoSection
{
  char section_name[16];
  char segment_name[16];
  uint64_t address;
  uint64_t size;
  uint32_t offset;
  uint32_t align;
  uint32_t relocation_offset;
  uint32_t relocation_count;
  uint32_t flags;
  uint32_t reserved1;
  uint32_t reserved2;
  uint32_t reserved3;
};

struct MachoSymtab
{
  uint32_t symbol_table_offset;
  uint32_t symbol_count;
  uint32_t string_table_offset;
  uint32_t string_table_size;
};

struct MachoSymbol
{
  uint32_t string_index;
  uint8_t type;
  uint8_t section;
  uint16_t desc;
  uint64_t value;
};

int macho_read_segment_load(
  MachoSegmentLoad &macho_segment_load,
  FileIo &file,
  int bits)
{
  if (file.get_chars(macho_segment_load.name, 16) != 16) { return -1; }

  if (bits == 32)
  {
    macho_segment_load.address      = file.get_int32();
    macho_segment_load.address_size = file.get_int32();
    macho_segment_load.file_offset  = file.get_int32();
    macho_segment_load.file_size    = file.get_int32();
  }
    else
  {
    macho_segment_load.address      = file.get_int64();
    macho_segment_load.address_size = file.get_int64();
    macho_segment_load.file_offset  = file.get_int64();
    macho_segment_load.file_size    = file.get_int64();
  }

  macho_segment_load.protection_max     = file.get_int32();
  macho_segment_load.protection_initial = file.get_int32();
  macho_segment_load.section_count      = file.get_int32();
  macho_segment_load.flag               = file.get_int32();

  return 0;
}

static int macho_read_section(
  MachoSection &macho_section,
  FileIo &file,
  int bits)
{
  if (file.get_chars(macho_section.section_name, 16) != 16) { return -1; }
  if (file.get_chars(macho_section.segment_name, 16) != 16) { return -1; }

  if (bits == 32)
  {
    macho_section.address = file.get_int32();
    macho_section.size    = file.get_int32();
  }
    else
  {
    macho_section.address = file.get_int64();
    macho_section.size    = file.get_int64();
  }

  macho_section.offset            = file.get_int32();
  macho_section.align             = file.get_int32();
  macho_section.relocation_offset = file.get_int32();
  macho_section.relocation_count  = file.get_int32();
  macho_section.flags             = file.get_int32();
  macho_section.reserved1         = file.get_int32();
  macho_section.reserved2         = file.get_int32();
  macho_section.reserved3         = file.get_int32();

  return 0;
}

static int macho_read_symtab(MachoSymtab &macho_symtab, FileIo &file)
{
  macho_symtab.symbol_table_offset = file.get_int32();
  macho_symtab.symbol_count        = file.get_int32();
  macho_symtab.string_table_offset = file.get_int32();
  macho_symtab.string_table_size   = file.get_int32();

  return 0;
}

static int macho_read_symbol(MachoSymbol &macho_symbol, FileIo &file, int bits)
{
  macho_symbol.string_index = file.get_int32();
  macho_symbol.type         = file.get_int8();
  macho_symbol.section      = file.get_int8();
  macho_symbol.desc         = file.get_int16();

  if (bits == 32)
  {
    macho_symbol.value = file.get_int32();
  }
    else
  {
    macho_symbol.value = file.get_int64();
  }

  return 0;
}

int read_macho(
  const char *filename,
  Memory *memory,
  uint8_t *cpu_type,
  Symbols *symbols)
{
  FileIo file;
  MachoHeader macho_header;
  MachoLoadCommand macho_load_command;
  MachoSegmentLoad macho_segment_load;
  MachoSection macho_section;
  MachoSymtab macho_symtab;
  MachoSymbol macho_symbol;

  memory->clear();

  uint32_t start = 0xffffffff;
  uint32_t end   = 0xffffffff;

  if (file.open_for_reading(filename) != 0)
  {
    return -1;
  }

  macho_header.magic_number = file.get_int32();

  if (macho_header.magic_number != 0xfeedface &&
      macho_header.magic_number != 0xfeedfacf)
  {
    return -1;
  }

  macho_header.cpu_type           = file.get_int32();
  macho_header.cpu_subtype        = file.get_int32();
  macho_header.file_type          = file.get_int32();
  macho_header.load_command_count = file.get_int32();
  macho_header.load_command_size  = file.get_int32();
  macho_header.flags              = file.get_int32();

  // 64 bit files have 4 extra bytes (probably for alignment).
  if (macho_header.magic_number == 0xfeedfacf)
  {
    macho_header.reserved = file.get_int32();
  }

  int bits = (macho_header.cpu_type & 0x01000000) == 0x01000000 ? 64 : 32;
  if (bits == 64) { macho_header.cpu_type ^= 0x01000000; }

  switch (macho_header.cpu_type)
  {
    case 0x00000006:
      *cpu_type = CPU_TYPE_68000;
     break;
    case 0x0000000c:
      *cpu_type = (bits == 32) ? CPU_TYPE_ARM : CPU_TYPE_ARM64;
      break;
    case 0x00000012:
      *cpu_type = CPU_TYPE_POWERPC;
      file.set_endian(FileIo::FILE_ENDIAN_BIG);
      break;
    default:
      break;
  }

  for (uint32_t i = 0; i < macho_header.load_command_count; i++)
  {
    macho_load_command.type = file.get_int32();
    macho_load_command.size = file.get_int32();

    switch (macho_load_command.type)
    {
      case 0x00000001:
      case 0x00000019:
      {
        // LC_SEGMENT_32
        // LC_SEGMENT_64
        macho_read_segment_load(macho_segment_load, file, bits);

        for (uint32_t n = 0; n < macho_segment_load.section_count; n++)
        {
          macho_read_section(macho_section, file, bits);

          if (strcmp(macho_section.section_name, "__text") == 0)
          {
            long marker = file.tell();
            file.set(macho_section.offset);

            for (uint32_t t = 0; t < macho_section.size; t++)
            {
              memory->write8(macho_section.address + t, file.get_int8());
            }

            start = macho_section.address;
            end = macho_section.address + macho_section.size - 1;

            file.set(marker);
          }
        }
        break;
      }
      case 0x00000002:
      {
        // LC_SYMTAB
        macho_read_symtab(macho_symtab, file);

        uint32_t strtab = macho_symtab.string_table_offset;
        char name[128];

        long marker = file.tell();
        file.set(macho_symtab.symbol_table_offset);

        for (uint32_t n = 0; n < macho_symtab.symbol_count; n++)
        {
          macho_read_symbol(macho_symbol, file, bits);

          // Check N_EXT (external symbol bit).
          if ((macho_symbol.type & 1) == 1)
          {
            file.get_string_at_offset(
              name,
              sizeof(name),
              strtab + macho_symbol.string_index);

            printf("symbol %12s 0x%04" PRIx64 "\n", name, macho_symbol.value);
            symbols->append(name, macho_symbol.value);
          }
        }

        file.set(marker);

        break;
      }
      default:
      {
        file.skip(macho_load_command.size - 8);
        break;
      }
    }
  }

  file.close_file();

  memory->low_address  = start;
  memory->high_address = end;

  return start;
}

