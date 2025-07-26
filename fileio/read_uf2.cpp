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

#include "fileio/FileIo.h"
#include "fileio/read_uf2.h"
#include "fileio/uf2.h"

static int read_block(Uf2Block &uf2_block, FileIo &file)
{
  uf2_block.magic_0               = file.get_int32();
  uf2_block.magic_1               = file.get_int32();
  uf2_block.flags                 = file.get_int32();
  uf2_block.address               = file.get_int32();
  uf2_block.byte_count            = file.get_int32();
  uf2_block.sequence_block_number = file.get_int32();
  uf2_block.total_blocks          = file.get_int32();
  uf2_block.board_family          = file.get_int32();
  file.get_bytes(uf2_block.data, sizeof(uf2_block.data));
  uf2_block.magic_2               = file.get_int32();

  return 0;
}

static void print_block(Uf2Block &uf2_block, int offset)
{
  printf(" --- uf2_block %04x ---\n", offset);
  printf("     magic_0: 0x%08x %c%c%c\n",
     uf2_block.magic_0,
     uf2_block.magic_0 & 0xff,
    (uf2_block.magic_0 >> 8) & 0xff,
    (uf2_block.magic_0 >> 16) & 0xff);
  printf("     magic_1: 0x%08x\n", uf2_block.magic_1);
  printf("       flags: 0x%08x\n", uf2_block.flags);
  printf("     address: 0x%08x\n", uf2_block.address);
  printf("  byte_count: %d\n",     uf2_block.byte_count);
  printf("block_number: %d\n",     uf2_block.sequence_block_number);
  printf("total_blocks: %d\n",     uf2_block.total_blocks);

#if 0
  for (uint32_t i = 0; i < sizeof(uf2_block.data); i++)
  {
    if ((i % 16) == 0) { printf("\n"); }
    printf(" %02x", uf2_block.data[i]);
  }
#endif

  printf("board_family: 0x%08x\n", uf2_block.board_family);
}

int read_uf2(const char *filename, Memory *memory)
{
  FileIo file;
  Uf2Block uf2_block;

  if (file.open_for_reading(filename) != 0)
  {       
    return -1;
  }

  file.set_endian(FileIo::FILE_ENDIAN_LITTLE);

  const int length = file.get_file_length();
  //uint32_t block = 0;

  //printf("UF2: file_length=%d\n", length);

  for (int ptr = 0; ptr < length; ptr += 512)
  {
    read_block(uf2_block, file);

    if (uf2_block.magic_0 != 0x0a324655 ||
        uf2_block.magic_1 != 0x9e5d5157 ||
        uf2_block.magic_2 != 0x0ab16f30)
    {
      printf("Error: UF2 magic numbers don't match\n");
      print_block(uf2_block, file.tell());

      break;
    }

#if 0
    // REVIEW: pico2 generated programs seem to have invalid block
    // and block numbers.
    if (uf2_block.sequence_block_number != block ||
        uf2_block.total_blocks == block)
    {
      printf("Error: UF2 wrong block number %d\n", block);
      print_block(uf2_block, file.tell());

      block = uf2_block.total_blocks;

      break;
    }
#endif

    //printf("%04x %d\n", uf2_block.address, uf2_block.byte_count);
    //printf("%08x\n", uf2_block.board_family);
    //printf("%08x\n", uf2_block.flags);

    int address = uf2_block.address;

    // 0x00000001 - not main flash.
    if ((uf2_block.flags & 0x0001) == 1)
    {
      //printf("not main flash\n");
      continue;
    }

    if ((uf2_block.flags & 0x1000) == 0x1000)
    {
      //printf("file container\n");
    }

    if ((uf2_block.flags & 0x2000) == 0x2000)
    {
      //printf("family id present\n");
    }

    if ((uf2_block.flags & 0x4000) == 0x4000)
    {
      //printf("md5 checksum present\n");
    }

    if ((uf2_block.flags & 0x8000) == 0x8000)
    {
      //printf("extension tags present\n");
    }

    for (uint32_t n = 0; n < uf2_block.byte_count; n++)
    {
      memory->write8(address++, uf2_block.data[n]);
    }

    //block += 1;
  }

#if 0
  // REVIEW: Again, the pico2 SDK seems to produce uf2 files that don't
  // follow an order.
  if (uf2_block.total_blocks != block)
  {
    printf("Error: UF2 not enough blocks: %d\n", block);
  }
#endif

  file.close_file();

  return 0;
}

