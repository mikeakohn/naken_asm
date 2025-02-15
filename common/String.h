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

#ifndef NAKEN_ASM_STRING_H
#define NAKEN_ASM_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// The reason for using this instead of std::string is:
// 1) It avoids requiring libstc++.
// 2) It allocates to the stack unless more is needed.
// 3) It can be easily extended.
class String
{
public:
  String() : text (data), size (sizeof(data)), length (0)
  {
    data[0] = 0;
  }

  String(const char *text) : text (data), size (sizeof(data)), length (0)
  {
    set(text);
  }

  String(String &s) : text (data), size (sizeof(data)), length (0)
  {
    this->set(s.value());
  }

  ~String()
  {
    if (is_heap()) { free(text); }
  }

  bool is_heap()      const { return text != data; }
  int len()           const { return length; }
  int allocated()     const { return size; }
  const char *value() const { return text; }

  void clear()
  {
    text[0] = 0;
    length = 0;
  }

  bool equals(const char *value)
  {
    return strcmp(text, value) == 0;
  }

  char char_at(int index) const
  {
    if (index < 0) { index = length + index; }
    if (index < 0 || index >= length) { return 0; }

    return text[index];
  }

  void set(const char *value)
  {
    int l = strlen(value) + 1;
    if (l > size) { resize(l); }

    memcpy(text, value, l);
    length = l - 1;
  }

  void append(const char *value)
  {
    int text_len = strlen(value);
    int l = text_len + 1;
    if (length + l > size) { resize(length + l); }

    memcpy(text + length, value, l);
    length += text_len;
  }

  void append(char c)
  {
    if (length + 2 > size) { resize(length + 2); }

    text[length++] = c;
    text[length] = 0;
  }

  bool startswith(const char *value)
  {
    for (int i = 0; i < length + 1; i++)
    {
      if (value[i] == 0) { return true; }
      if (text[i] != value[i]) { return false; }
    }

    return false;
  }

  int find(char s)
  {
    for (int i = 0; i < length; i++)
    {
      if (text[i] == s) { return i; }
    }

    return -1;
  }

  void replace_at(int index, char value)
  {
    if (index < 0 || index > length) { return; }
    text[index] = value;
  }

  int as_int()
  {
    return strtol(text, NULL, 0);
  }

  int is_number()
  {
    int s = 0;

    if (text[0] == '0' && text[1] == 'x') { s = 2; }

    for (int i = s; i < length; i++)
    {
      if (text[i] <= '0' || text[i] >= '9') { return false; }
    }

    return true;
  }

  void operator=(const char *text)
  {
    set(text);
  }

  void operator=(String s)
  {
    set(s.value());
  }

  static bool is_whitespace(const char c)
  {
    return c == '\r' || c == '\n' || c == '\t' || c == ' ';
  }

  void ltrim()
  {
    int s;

    for (s = 0; s < length; s++)
    {
      if (! is_whitespace(text[s])) { break; }
    }

    if (s == 0) { return; }

    length -= s;
    memmove(text, text + s, length + 1);
  }

  void rtrim()
  {
    while (length > 0)
    {
      if (! is_whitespace(text[length - 1])) { break; }
      length -= 1;
    }

    text[length] = 0;
  }

  void trim()
  {
    ltrim();
    rtrim();
  }

#if 0
  String operator+(const char *text)
  {
    String s;
    s.set(this->value());
    s.append(text);
    return s;
  }
#endif

  void operator+=(const char *text)
  {
    append(text);
  }

  void operator+=(const char s)
  {
    append(s);
  }

  bool operator==(const char *s)
  {
    return strcmp(text, s) == 0;
  }

  bool operator==(const String &s)
  {
    return strcmp(text, s.value()) == 0;
  }

  void dump()
  {
    printf(" --- String ---\n");
    printf("       text: %s\n", text);
    printf("       size: %d\n", size);
    printf("     length: %d\n", length);
    printf("  is_heap(): %d\n", is_heap());
  }

protected:
  // String is always 0 terminated.
  // length is the actual langth of the data without the 0 padding.
  // size is how much data is currently allocated.
  char *text;
  char data[256];
  int size;
  int length;

private:
  void resize(int new_size)
  {
    while (size < new_size)
    {
      size = size * 2;
    }

    if (text == data)
    {
      text = (char *)malloc(size);
    }
      else
    {
      text = (char *)realloc(text, size);
    }
  }
};

#endif

