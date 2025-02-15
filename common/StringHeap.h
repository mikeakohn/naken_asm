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

// Data here is stored as:
// 2 byte length, string, null terminator.

#ifndef NAKEN_ASM_STRING_HEAP_H
#define NAKEN_ASM_STRING_HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

class StringHeap
{
public:
  StringHeap();
  StringHeap(int start_size);

  ~StringHeap();

  void clear();
  void set(const char *list, int count = 0x7fffffff);
  void append(const char *value);
  int find(const char *value);

  int len()       const { return length; }
  int count()     const { return entry_count; }
  int allocated() const { return size; }

#if 0
  void operator=(const char *text)
  {
    set(text);
  }

  void operator+=(const char *text)
  {
    append(text);
  }
#endif

  void dump();

  class iterator
  {
  public:
    iterator(char *data) : next (data + 2) { }

    char * operator *() { return next; }

    iterator operator ++()
    {
      iterator i = *this;
      uint8_t *s = (uint8_t *)next - 2;
      next += (s[0] | s[1] << 8) + 3;
      return i;
    }

    iterator operator ++(int inc)
    {
      iterator i = *this;
      uint8_t *s = (uint8_t *)next - 2;
      next += (s[0] | s[1] << 8) + 3;
      return i;
    }

    bool operator ==(const iterator &rhs) { return next == rhs.next; }
    bool operator !=(const iterator &rhs) { return next != rhs.next; }

  private:
    char *next;
  };

  iterator begin()
  {
    return iterator(data);
  }

  iterator end()
  {
    return iterator(data + length);
  }

protected:
  // String is always 0 terminated.
  // length is the actual langth of the data without the 0 padding.
  // size is how much data is currently allocated.
  char *data;
  int size;
  int length;
  int entry_count;

private:
  void resize(int string_len);

};

#endif

