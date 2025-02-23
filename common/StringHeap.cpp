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
#include <stdint.h>
#include <string.h>

#include "StringHeap.h"

StringHeap::StringHeap() :
  data        (nullptr),
  size        (4096),
  length      (0),
  entry_count (0)
{
  data = (char *)malloc(size);
}

StringHeap::StringHeap(int start_size) :
  data        (nullptr),
  size        (start_size),
  length      (0),
  entry_count (0)
{
  data = (char *)malloc(size);
}

StringHeap::~StringHeap()
{
  free(data);
}

void StringHeap::clear()
{
  data[0]     = 0;
  length      = 0;
  entry_count = 0;
}

void StringHeap::set(const char *list, int count)
{
  int i = 0;

  clear();

  while (*list != 0 && count != i)
  {
    int l = strlen(list);
    append(list);
    list += l + 1;
    i += 1;
  }
}

const char *StringHeap::append(const char *value)
{
  const int value_len = strlen(value);
  const int data_len = value_len + 3;

  // Don't add string if it's too long.
  if (value_len > 0xffff) { return nullptr; }

  if (length + data_len > size) { resize(value_len); }

  uint8_t *s = (uint8_t *)data + length;

  s[0] = value_len & 0xff;
  s[1] = (value_len >> 8) & 0xff;

  char *name = data + length + 2;

  memcpy(name, value, value_len + 1);

  length += data_len;
  entry_count += 1;

  return name;
}

int StringHeap::find(const char *value)
{
  int i = 0;
  for (StringHeap::iterator it = this->begin(); it != this->end(); it++)
  {
    if (strcmp(*it, value) == 0) { return i; }
    i++;
  }

#if 0
  int offset = 0;

  for (int i = 0; i < entry_count; i++)
  {
    if (strcmp(data + offset + 2, value) == 0) { return i; }

    uint8_t *s = (uint8_t *)data + offset;
    const int l = s[0] | (s[1] << 8);

    offset += l + 3;
  }
#endif

  return -1;
}

void StringHeap::dump()
{
  printf(" --- StringHeap ---\n");
  printf("        size: %d\n", size);
  printf("      length: %d\n", length);
  printf(" entry_count: %d\n", entry_count);

  int i = 0;
  for (StringHeap::iterator it = this->begin(); it != this->end(); it++)
  {
    printf("  %d) %s\n", i++, *it);
  }
}

void StringHeap::resize(int string_len)
{
  while (size < length + string_len + 3)
  {
    size = size * 2;
  }

  data = (char *)realloc(data, size);
}

