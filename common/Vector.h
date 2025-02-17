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

#ifndef NAKEN_ASM_VECTOR_H
#define NAKEN_ASM_VECTOR_H

#include <stdlib.h>

template<typename TYPE>
class Vector
{
public:
  Vector() : length(16), index(0)
  {
    data = (TYPE *)malloc(length * sizeof(TYPE));
  }

  Vector(int start_length) : length(start_length), index(0)
  {
    data = (TYPE *)malloc(length * sizeof(TYPE));
  }

  ~Vector()
  {
    free(data);
  }

  int count()     { return index; }
  int allocated() { return length * sizeof(TYPE); }
  void clear()    { index = 0; }
  TYPE &last()    { return data[index - 1]; }
  TYPE pop()      { return data[--index]; }
  bool empty()    { return index == 0; }
  //void reserve(int size) { }
  TYPE &operator[] (int i) { return data[i]; }

  void append(TYPE value)
  {
    if (index == length)
    {
      length *= 2;
      data = (TYPE *)realloc(data, length * sizeof(TYPE));
    }

    data[index++] = value;
  }

  class iterator
  {
  public:
    iterator() : next(nullptr) { }
    iterator(TYPE *data) : next(data) { }

    TYPE operator *() { return *next; }
    //TYPE &operator*() { return *next; }
    iterator operator ++() { iterator i = *this; next++; return i; }
    iterator operator ++(int inc) { next += 1; return *this; }
    bool operator ==(const iterator &rhs) { return next == rhs.next; }
    bool operator !=(const iterator &rhs) { return next != rhs.next; }
  private:
    TYPE *next;
  };

  iterator begin()
  {
    return iterator(data);
  }

  iterator end()
  {
    return iterator(data + index);
  }

private:
  int length;
  int index;
  TYPE *data;
};

#endif

