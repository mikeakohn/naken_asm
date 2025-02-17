#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/MemoryPoolFixed.h"
#include "test_checks.h"

struct Data
{
  int a;
  int b;
};

int test_constructor()
{
  int errors = 0;

  MemoryPoolFixed<Data> m;

  TEST_PTR(m.get_current_pool(), nullptr);
  TEST_INT(m.get_index(),  1024);
  TEST_INT(m.get_length(), 1024);

  return errors;
}

int test_constructor_with_size()
{
  int errors = 0;

  MemoryPoolFixed<Data> m(4);

  TEST_PTR(m.get_current_pool(), nullptr);
  TEST_INT(m.get_index(),  4);
  TEST_INT(m.get_length(), 4);

  return errors;
}

int test_basic()
{
  int errors = 0;

  MemoryPoolFixed<Data> m(4);

  Data *values[7];
  memset(values, 0, sizeof(values));

  values[0] = m.alloc();

  TEST_BOOL((m.get_current_pool() == nullptr), false);
  TEST_INT(m.get_index(), 1);

  values[1] = m.alloc();
  values[2] = m.alloc();
  values[3] = m.alloc();
  TEST_INT(m.get_index(), 4);

  // Forces a new pool to be allocated.
  values[4] = m.alloc();
  TEST_INT(m.get_index(), 1);

  // This should put values[4] on the free_entries list so the next alloc
  // should grab the same memory and not use array of entries in the current
  // pool.
  Data *last = values[0];
  m.release(values[0]);

  TEST_INT(m.get_index(), 1);
  values[5] = m.alloc();

  TEST_INT(m.get_index(), 1);
  TEST_PTR(values[5], last);

  // Free list should now be empty so the next alloc() will grab from the
  // array again.
  values[6] = m.alloc();
  TEST_INT(m.get_index(), 2);

  return errors;
}

int main(int argc, char *argv[])
{
  int errors = 0;

  printf("Testing MemoryPoolFixed.h\n");

  errors += test_constructor();
  errors += test_constructor_with_size();
  errors += test_basic();

  if (errors != 0) { printf("MemoryPoolFixed.h ... FAILED.\n"); return -1; }

  printf("MemoryPoolFixed.h ... PASSED.\n");

  return 0;
}

