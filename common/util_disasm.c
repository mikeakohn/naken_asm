/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "common/util_disasm.h"

void util_disasm(UtilContext *util_context, char *token)
{
  uint32_t start, end;

  if (util_get_range(util_context, token, &start, &end) == -1) { return; }

  util_context->disasm_range(
    &util_context->memory,
    util_context->flags,
    start,
    end);
}

void util_disasm_range(UtilContext *util_context, int start, int end)
{
  uint32_t page_size,page_mask;
  int curr_start = start;
  int valid_page_start = 1;
  int address_min,address_max;
  int curr_end;
  int n;

  start = start * util_context->bytes_per_address;
  end = end * util_context->bytes_per_address;

  page_size = memory_page_size(&util_context->memory);
  page_mask = page_size - 1;
  curr_end = start | page_mask;

  int data_size = 0;

  n = start;

  while (n <= end)
  {
    data_size = page_size - (n & page_mask);

    if (memory_in_use(&util_context->memory, n))
    {
      if (valid_page_start == 0)
      {
        curr_start = n & (~page_mask);
        valid_page_start = 1;
      }
      curr_end = n | page_mask;
    }
      else
    {
      if (valid_page_start == 1)
      {
        address_min = memory_get_page_address_min(
          &util_context->memory,
          curr_start);

        address_max = memory_get_page_address_max(
          &util_context->memory,
          curr_end);

        util_context->disasm_range(
          &util_context->memory,
          util_context->flags,
          address_min,
          address_max);

        valid_page_start = 0;
      }
    }

    //n += page_size;
    n += data_size;
  }

  if (valid_page_start == 1)
  {
    address_min = memory_get_page_address_min(
      &util_context->memory,
      curr_start);

    address_max = memory_get_page_address_max(&util_context->memory, curr_end);

    util_context->disasm_range(
      &util_context->memory,
      util_context->flags,
      address_min,
      address_max);
  }
}

