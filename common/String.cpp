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

#include "String.h"

void String::dump()
{
  printf(" --- String ---\n");
  printf("       text: %s\n", text);
  printf("       size: %d\n", size);
  printf("     length: %d\n", length);
  printf("  is_heap(): %d\n", is_heap());
}

void String::resize(int new_size)
{
  while (size < new_size)
  {
    size = size * 2;
  }

  if (text == data)
  {
    text = (char *)malloc(size);
    memcpy(text, data, length + 1);
  }
    else
  {
    text = (char *)realloc(text, size);
  }
}

