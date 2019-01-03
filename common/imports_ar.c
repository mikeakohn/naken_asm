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
#include <stdint.h>
#include <string.h>

#include "imports_get_int.h"
#include "imports_ar.h"
#include "imports_obj.h"

struct _header
{
  char file_identifier[16];
  char timestamp[12];
  char owner_id[6];
  char group_id[6];
  char mode[8];
  char size[10];
  char end[2];
};

static int imports_ar_read_signature(uint8_t *buffer, int file_size)
{
  const char *header = "!<arch>\n";
  int i;

  if (file_size < sizeof(header))
  {
    printf("Not a library archive file.\n");
    return -1;
  }

  for (i = 0; i < sizeof(header); i++)
  {
    if (buffer[i] != header[i])
    {
      printf("Not a library archive file.\n");
      return -1;
    }
  }

  return 0;
}

static int imports_ar_read_lookup_table(uint8_t *buffer, int size)
{
  int entry_count;
  int ptr_offset;
  int ptr_name;
  int n;

  entry_count = get_int32_be(buffer);

  printf("entry_count=%d\n", entry_count);

  ptr_offset = 4;
  ptr_name = ptr_offset + (entry_count * 4);

  for (n = 0; n < entry_count; n++)
  {
    int offset = get_int32_be(buffer + ptr_offset);

    printf("%d) 0x%04x %s\n", n, offset, buffer + ptr_name);

    ptr_offset += 4;
    ptr_name += strlen((char *)(buffer + ptr_name)) + 1;
  }

  return 0;
}

int imports_ar_verify(uint8_t *buffer, int file_size)
{
  if (imports_ar_read_signature(buffer, file_size) != 0)
  {
    return -1;
  }

  return 0;
}

int imports_ar_read(uint8_t *buffer, int file_size)
{
  struct _header *header;
  int i, size;
  int ptr = 8;

  if (imports_ar_read_signature(buffer, file_size) != 0) { return -1; }

  while(ptr < file_size)
  {
    header = (struct _header *)(buffer + ptr);

    printf("file_identifier: %.16s\n", header->file_identifier);
    printf("      timestamp: %.12s\n", header->timestamp);
    printf("       owner_id: %.6s\n", header->owner_id);
    printf("       group_id: %.6s\n", header->group_id);
    printf("           mode: %.8s\n", header->mode);
    printf("           size: %.10s\n", header->size);
    printf("            end: %02x %02x\n", header->end[0], header->end[1]);

    size = 0;

    for (i = 0; i < 10; i++)
    {
      if (header->size[i] == ' ') { break; }
      size = (size * 10) + (header->size[i] - '0');
    }

    ptr += 60;

    if (strncmp(header->file_identifier, "/               ", 16) == 0)
    {
      imports_ar_read_lookup_table(buffer + ptr, size);
    }

    if ((size & 1) != 0) { size++; }

    ptr += size;
  }

  return 0;
}

static int imports_ar_find_lookup_table(uint8_t *buffer, int file_size)
{
  struct _header *header;
  int ptr = 8;
  int i;

  while(ptr < file_size)
  {
    header = (struct _header *)(buffer + ptr);

    int size = 0;

    for (i = 0; i < 10; i++)
    {
      if (header->size[i] == ' ') { break; }
      size = (size * 10) + (header->size[i] - '0');
    }

    if (strncmp(header->file_identifier, "/               ", 16) == 0)
    {
      return ptr;
    }

    if ((size & 1) != 0) { size++; }

    ptr += 60 + size;
  }

  return -1;
}

int imports_ar_find_symbol_section_offset(
  uint8_t *buffer,
  int file_size,
  const char *symbol)
{
  int entry_count;
  int ptr_offset;
  int ptr_name;
  int n;

  int lookup_table = imports_ar_find_lookup_table(buffer, file_size);

  if (lookup_table == -1)
  {
    printf("Can't find lookup table\n");
    return -1;
  }

  buffer = buffer + lookup_table + 60;

  entry_count = get_int32_be(buffer);

  //printf("entry_count=%d\n", entry_count);

  ptr_offset = 4;
  ptr_name = ptr_offset + (entry_count * 4);

  for (n = 0; n < entry_count; n++)
  {
    int offset = get_int32_be(buffer + ptr_offset);

    ptr_offset += 4;
    ptr_name += strlen((char *)(buffer + ptr_name)) + 1;

    if (strcmp((const char *)(buffer + ptr_name), symbol) == 0)
    {
      return offset;
    }
  }

  return -1;
}

int imports_ar_find_code_from_symbol(
  uint8_t *buffer,
  int file_size,
  const char *symbol,
  uint32_t *function_offset,
  uint32_t *function_size,
  uint32_t *file_offset,
  uint8_t **obj_file,
  uint32_t *obj_size)
{
  //int section_offset;
  struct _header *header;
  int ptr = 8;
  int i;
  int ret = -1;

  *function_offset = 0;
  *function_size = 0;
  *file_offset = 0;
  *obj_file = NULL;
  *obj_size = 0;

  if (imports_ar_read_signature(buffer, file_size) != 0) { return -1; }

  while(ptr < file_size)
  {
    header = (struct _header *)(buffer + ptr);

#if 0
    printf("file_identifier: %.16s\n", header->file_identifier);
    printf("      timestamp: %.12s\n", header->timestamp);
    printf("       owner_id: %.6s\n", header->owner_id);
    printf("       group_id: %.6s\n", header->group_id);
    printf("           mode: %.8s\n", header->mode);
    printf("           size: %.10s\n", header->size);
    printf("            end: %02x %02x\n", header->end[0], header->end[1]);
#endif

    int size = 0;

    for (i = 0; i < 10; i++)
    {
      if (header->size[i] == ' ') { break; }
      size = (size * 10) + (header->size[i] - '0');
    }

    if (strncmp(header->file_identifier, "/               ", 16) != 0 &&
        buffer[ptr + 60] == 0x7f &&
        buffer[ptr + 61] == 'E' &&
        buffer[ptr + 62] == 'L' &&
        buffer[ptr + 63] == 'F')
    {
      ret = imports_obj_find_code_from_symbol(
        buffer + ptr + 60,
        size,
        symbol,
        function_offset,
        function_size,
        file_offset);

      if (ret == 0)
      {
        *file_offset += ptr + 60;
        *obj_file = buffer + ptr + 60;
        *obj_size = size;

#if 0
printf("linker_get_code_from_symbol(%s) file_offset=%d ret=%d function_size=%d\n",
  symbol,
  *file_offset += ptr + 60,
  ret,
  *function_size);
#endif
        return ret;
      }
    }

    if ((size & 1) != 0) { size++; }

    ptr += 60 + size;
  }

  return -1;
}

const char *imports_ar_find_name_from_offset(
  uint8_t *buffer,
  int file_size,
  uint32_t offset)
{
  const char *name = NULL;
  struct _header *header;
  int ptr = 8;
  int i;

  if (imports_ar_read_signature(buffer, file_size) != 0) { return NULL; }

  while(ptr < file_size)
  {
    header = (struct _header *)(buffer + ptr);

    int size = 0;

    for (i = 0; i < 10; i++)
    {
      if (header->size[i] == ' ') { break; }
      size = (size * 10) + (header->size[i] - '0');
    }

    if (strncmp(header->file_identifier, "/               ", 16) != 0)
    {
      const uint32_t local_offset = -1;

      name = imports_obj_find_name_from_offset(buffer + ptr + 60, size, offset, local_offset);

      if (name != NULL) { return name; }
    }

    if ((size & 1) != 0) { size++; }

    ptr += 60 + size;
  }

  return "<not found>";
}

