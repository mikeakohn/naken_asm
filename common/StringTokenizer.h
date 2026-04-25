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

#ifndef NAKEN_ASM_STRING_TOKENIZER_H
#define NAKEN_ASM_STRING_TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "String.h"

class StringTokenizer
{
public:
  StringTokenizer(const char *text) : text (text)
  {
  }

  StringTokenizer(String &s) : text (s.value())
  {
  }

  ~StringTokenizer()
  {
  }

  bool is_empty() { return text[0] == 0; }

  bool is_num()
  {
    const char *s = text;
    while (*s != 0 && (*s == ' ' || *s == '\t')) { s++; }
    return s[0] >= '0' && s[0] <= '9';
  }

  char pop_char();
  bool pop(String &token);
  int pop_num();

  void ltrim();

private:
  const char *text;

};

#endif

