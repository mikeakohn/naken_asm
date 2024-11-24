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

int write_uf2(Memory *memory, FILE *out)
{
  FileIo file;

  file.set_fp(out);

  // REVIEW: Does uf2 do big endian?
  file.set_endian(FileIo::FILE_ENDIAN_LITTLE);

  const int board_family = 0xe48bff56;

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
      file.write_int32(0x0a324655);
      file.write_int32(0x9e5d5157);
      file.write_int32(0x00002000);
      file.write_int32(i);
      file.write_int32(256);
      file.write_int32(block);
      file.write_int32(total_blocks);
      file.write_int32(board_family);
    }

    file.write_int8(memory->read8(i));
    ptr += 1;

    if (ptr == 256)
    {
      for (; ptr < 476; ptr++) { file.write_int8(0); }
      file.write_int32(0x0ab16f30);
      ptr = 0;
      block += 1;
    }
  }

  if (ptr != 0)
  {
    for (; ptr < 476; ptr++) { file.write_int8(0); }
    file.write_int32(0x0ab16f30);
  }

  // FIXME: naken_asm shouldn't be controlling the FILE *.
  file.set_fp(NULL); 
  file.close_file();

  return 0;
}

