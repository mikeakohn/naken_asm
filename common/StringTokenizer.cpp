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

#include "StringTokenizer.h"

char StringTokenizer::pop_char()
{
  ltrim();
  if (is_empty()) { return 0; }

  char ch = text[0];
  text++;

  return ch;
}

bool StringTokenizer::pop(String &token)
{
  token.clear();

  ltrim();
  if (is_empty()) { return false; }

  int type = -1;

  while (*text != 0)
  {
    if (*text >= '0' && *text <= '9')
    {
      if (type != -1 && type != 1) { break; }
      type = 1;
      token.append(*text);
    }
      else
    {
      if (type != -1) { break; }
      token.append(*text);
      text++;
      break;
    }

    text++;
  }

  ltrim();

  return true;
}

int StringTokenizer::pop_num()
{
  ltrim();
  if (is_empty()) { return -1; }

  int num = 0;
  int count = 0;
  bool is_hex = false;

  while (*text != 0)
  {
    if (count == 0 && *text == ' ') { continue; }

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
      if (*text < '0' || *text > '9') { break; }
      num = (num * 10) + *text - '0';
    }

    text++;
    count++;
  }

  ltrim();

  return count != 0 ? num : -1;
}

void StringTokenizer::ltrim()
{
  while (*text != 0)
  {
    if (*text != '\r' && *text != '\n' &&
        *text != '\t' && *text != ' ')
    {
      break;
    }

    text++;
  }
}

