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

#include "common/Memory.h"
#include "fileio/FileIo.h"
#include "fileio/write_uf2.h"

static void uf2_write_block_header(
  FileIo &file,
  uint32_t address,
  int block_number,
  int total_blocks,
  uint32_t board_family)
{
  const int32_t magic_0 = 0x0a324655;
  const int32_t magic_1 = 0x9e5d5157;
  const int32_t flags   = 0x00002000;
  const int byte_count  = 256;

  file.write_int32(magic_0);
  file.write_int32(magic_1);
  file.write_int32(flags);
  file.write_int32(address);
  file.write_int32(byte_count);
  file.write_int32(block_number);
  file.write_int32(total_blocks);
  file.write_int32(board_family);
}

static void uf2_write_block_footer(FileIo &file)
{
  uint32_t magic_2 = 0x0ab16f30;

  file.write_int32(magic_2);
}

// Not sure why the PICO SDK does this. Probably not needed
static void uf2_add_pico_ef(FileIo &file)
{
  const uint32_t board_family = 0xe48bff57;

  uf2_write_block_header(file, 0x10ffff00, 0, 2, board_family);

  for (int n = 0; n < 256; n++) { file.write_int8(0xef); }
  for (int n = 0; n < 220; n++) { file.write_int8(0x00); }

  uf2_write_block_footer(file);
}

int write_uf2(Memory *memory, FILE *out)
{
  FileIo file;

  file.set_fp(out);

  // REVIEW: Does uf2 do big endian?
  file.set_endian(FileIo::FILE_ENDIAN_LITTLE);

  // Not sure why the PICO SDK has this.
  uf2_add_pico_ef(file);

  //const int board_family = 0xe48bff56;
  const int board_family = 0xe48bff59;

  int ptr = 0;
  int block = 0;
  int length = memory->high_address - memory->low_address + 1;
 
  length = (length + 255) & ~255;
  int total_blocks = length / 256;
  //bool need_magic_3;

  // Add code.
  for (uint32_t i = memory->low_address; i <= memory->high_address; i++)
  {
    if (ptr == 0)
    {
#if 0
      file.write_int32(0x0a324655);
      file.write_int32(0x9e5d5157);
      file.write_int32(0x00002000);
      file.write_int32(i);
      file.write_int32(256);
      file.write_int32(block);
      file.write_int32(total_blocks);
      file.write_int32(board_family);
#endif
      uf2_write_block_header(file, i, block, total_blocks, board_family);
    }

    file.write_int8(memory->read8(i));
    ptr += 1;

    if (ptr == 256)
    {
      for (; ptr < 476; ptr++) { file.write_int8(0); }
      //file.write_int32(0x0ab16f30);
      uf2_write_block_footer(file);
      ptr = 0;
      block += 1;
    }
  }

  if (ptr != 0)
  {
    for (; ptr < 476; ptr++) { file.write_int8(0); }
    //file.write_int32(0x0ab16f30);
    uf2_write_block_footer(file);
  }

  // FIXME: naken_asm shouldn't be controlling the FILE *.
  file.set_fp(NULL); 
  file.close_file();

  return 0;
}

