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

  bool is_uint32()
  {
    const char *s = text;
    while (*s != 0 && (*s == ' ' || *s == '\t')) { s++; }
    return s[0] >= '0' && s[0] <= '9';
  }

  bool is_int()
  {
    const char *s = text;
    while (*s != 0 && (*s == ' ' || *s == '\t')) { s++; }
    if (*s == '-') { s++; }
    return s[0] >= '0' && s[0] <= '9';
  }

  bool pop(String &token)
  {
    bool value = pop_no_ltrim(token);
    ltrim();
    return value;
  }

  char pop_char();
  bool pop_uint32(uint32_t &num);
  bool pop_int(int &num);

  void ltrim();

private:
  bool is_digit()  { return *text >= '0' && *text <= '9'; }

  bool is_letter()
  {
    return (*text >= 'a' && *text <= 'z') ||
           (*text >= 'A' && *text <= 'Z') ||
            *text == '_';
  }

  bool is_hex_digit()
  {
    return (*text >= '0' && *text <= '9') ||
           (*text >= 'a' && *text <= 'f') ||
           (*text >= 'A' && *text <= 'F');
  }

  bool is_whitespace()
  {
    return *text == '\n' || *text == '\r' || *text == '\t' || *text == ' ';
  }

  bool is_space() { return *text == ' ' || *text == '\t'; }

  bool pop_no_ltrim(String &token);

  const char *text;
};

#endif

