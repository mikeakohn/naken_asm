/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "StringTokenizer.h"

char StringTokenizer::pop_char()
{
  ltrim();
  if (is_empty()) { return 0; }

  char ch = text[0];
  text++;

  return ch;
}

bool StringTokenizer::pop_uint32(uint32_t &num)
{
  num = 0;

  ltrim();
  if (is_empty()) { return false; }

  int count = 0;
  bool is_hex = false;

  while (*text != 0)
  {
    if (count == 0 && is_space()) { continue; }

    if (count == 1 && *text == 'x')
    {
      is_hex = true;
    }
      else
    if (is_hex)
    {
      int value;

      if      (*text >= '0' && *text <= '9') { value = *text - '0'; }
      else if (*text >= 'a' && *text <= 'f') { value = *text - 'a' + 10; }
      else if (*text >= 'A' && *text <= 'F') { value = *text - 'A' + 10; }
      else { break; }

      num = (num * 16) + value;
    }
      else
    {
      if (!is_digit()) { break; }
      num = (num * 10) + *text - '0';
    }

    text++;
    count++;
  }

  ltrim();

  return count != 0;
}

bool StringTokenizer::pop_int(int &num)
{
  int sign = 1;

  num = 0;

  ltrim();

  if (*text == '-')
  {
    text++;
    ltrim();
    sign = -1;
  }

  uint32_t value;
  if (!pop_uint32(value)) { return false; }

  num = (int)value * sign;

  return true;
}

void StringTokenizer::ltrim()
{
  while (*text != 0)
  {
    if (!is_whitespace())
    {
      break;
    }

    text++;
  }
}

bool StringTokenizer::pop_no_ltrim(String &token)
{
  token.clear();

  ltrim();
  if (is_empty()) { return false; }

  int type = 0;
  int count = 0;

  while (*text != 0)
  {
    switch(type)
    {
      case 0:
        if (is_digit())
        {
          type = 1;
        }
          else
        if (is_letter())
        {
          type = 3;
        }
          else
        {
          token.append(*text);
          text++;
          return true;
        }

        break;
      case 1:
        if (count == 1 && *text == 'x')
        {
          type = 2;
          break;
        }

        if (!is_digit()) { return true; }
        break;
      case 2:
        if (!is_hex_digit()) { return true; }
        break;
      case 3:
        if (!is_letter()) { return true; }
        break;
    }

    if (type == 2) { token.append(tolower(*text)); }
    else           { token.append(*text); }

    count++;
    text++;
  }

  return true;
}

