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
  const char *value() const { return text; }

  void set(const char *text)
  {
    int l = strlen(text) + 1;
    if (l > size) { resize(l); }

    memcpy(this->text, text, l);
    length = l - 1;
  }

  void append(const char *text)
  {
    int l = strlen(text) + 1;
    if (length + l > size) { resize(length + l); }

    memcpy(this->text + length, text, l);
    length += l - 1;
  }

  void operator=(const char *text)
  {
    set(text);
  }

  void operator=(String s)
  {
    set(s.value());
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

  void dump()
  {
    printf(" --- String ---\n");
    printf("       text: %s\n", text);
    printf("       size: %d\n", size);
    printf("     length: %d\n", length);
    printf("  is_heap(): %d\n", is_heap());
  }

protected:
  char *text;
  char data[256];
  int size;
  int length;

private:
  void resize(int new_size)
  {
    new_size = new_size * 2;
    if (new_size < 512) { new_size = 512; }

    if (text == data)
    {
      text = (char *)malloc(new_size);
    }
      else
    {
      text = (char *)realloc(text, new_size);
    }

    size = new_size;
  }
};

#endif

