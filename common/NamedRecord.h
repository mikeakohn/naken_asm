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
//

#ifndef NAKEN_ASM_NAMED_RECORD_H
#define NAKEN_ASM_NAMED_RECORD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "StringHeap.h"
#include "MemoryPoolFixed.h"

template<typename TYPE> class NamedRecord
{
public:
  NamedRecord() : entry_count (0)
  {
    memset(buckets, 0, sizeof(buckets));
  }

  ~NamedRecord()
  {
  }

  void clear()
  {
    for (int i = 0; i < 256; i++)
    {
      Record *record = buckets[i];

      while (record != nullptr)
      {
        Record *current = record;
        record = record->next;

        pool.release(current);
        entry_count -= 1;
      }

      buckets[i] = nullptr;
    }
  }

  TYPE &append(const char *name)
  {
    Record *record = pool.alloc();

    insert(name, record);

    return record->data;
  }

  void append(const char *name, const TYPE &value)
  {
    TYPE &data = append(name);
    memcpy(&data, &value, sizeof(TYPE));
  }

  TYPE *find(const char *name)
  {
    int hash = compute_hash(name);

    Record *r = buckets[hash];

    while (r != nullptr)
    {
      if (strcmp(name, r->name) == 0)
      {
        return &r->data;
      }

      r = r->next;
    }

    return nullptr;
  }

  TYPE *find_or_append(const char *name)
  {
    TYPE *data = find(name);

    if (data != nullptr) { return data; }

    return &append(name);
  }

  bool remove(const char *name)
  {
    for (int i = 0; i < 256; i++)
    {
      Record *record = buckets[i];

      if (record != nullptr)
      {
        Record *last = nullptr;

        while (record != nullptr)
        {
          if (strcmp(name, record->name) == 0)
          {
            if (last == nullptr)
            {
              buckets[i] = record->next;
            }
              else
            {
              last->next = record->next;
            }

            pool.release(record);
            entry_count -= 1;

            return true;
          }

          last = record;
          record = record->next;
        }
      }
    }

    return false;
  }

  int count()     const { return entry_count; }

  void dump()
  {
    for (int i = 0; i < 256; i++)
    {
      Record *record = buckets[i];

      if (record != nullptr)
      {
         printf("%d:", i);

         while (record != nullptr)
         {
           printf(" %s", record->name);
           record = record->next;
         }

         printf("\n");
      }
    }
  }

  struct Record
  {
    Record *next;
    const char *name;
    TYPE data;
  };

  class iterator
  {
  public:
    iterator(int index, NamedRecord *base) :
      record (nullptr),
      base (base),
      index(index)
    {
      if (index == 0)
      {
        next();
      }
    }

    void next()
    {
      while (index < 256)
      {
        if (base->buckets[index] != nullptr)
        {
          record = base->buckets[index++];
          break;
        }

        index += 1;
      }
    }

    Record * operator *() { return record; }

    iterator operator ++()
    {
      iterator i = *this;
      record = record->next;

      if (record == nullptr)
      {
        next();
      }

      return i;
    }

    iterator operator ++(int inc)
    {
      iterator i = *this;
      record = record->next;

      if (record == nullptr)
      {
        next();
      }

      return i;
    }

    bool operator ==(const iterator &rhs) { return index == rhs.index && record == rhs.record; }
    bool operator !=(const iterator &rhs) { return index != rhs.index || record != rhs.record; }

    Record *record;
  private:
    NamedRecord *base;
    int index;
  };

  iterator begin()
  {
    return iterator(0, this);
  }

  iterator end()
  {
    return iterator(256, this);
  }

protected:
  StringHeap strings;
  MemoryPoolFixed<Record> pool;

  Record *buckets[256];

  int entry_count;

private:

  uint8_t compute_hash(const char *name)
  {
    uint8_t hash = 0;
    const uint8_t *s = (const uint8_t *)name;

    while (*s != 0)
    {
      hash += *s;
      s++;
    }

    return hash;
  }

  void insert(const char *name, Record *r)
  {
    int hash = compute_hash(name);

    r->next = buckets[hash];
    r->name = strings.append(name);
    buckets[hash] = r;

    entry_count += 1;
  }
};

#endif

