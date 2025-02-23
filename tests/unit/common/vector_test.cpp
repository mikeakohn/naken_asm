#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/Vector.h"
#include "test_checks.h"

int test_constructor()
{
  int errors = 0;

  Vector<int> v;

  TEST_INT(v.count(), 0);
  TEST_INT(v.allocated(), (int)sizeof(int) * 16);

  return errors;
}

int test_constructor_with_size()
{
  int errors = 0;

  Vector<int> v(4);
  TEST_INT(v.count(), 0);
  TEST_INT(v.allocated(), (int)sizeof(int) * 4);

  return errors;
}

int test_basic()
{
  int errors = 0;

  Vector<int> v(4);

  TEST_BOOL(v.empty(), true);

  v.append(9);
  TEST_BOOL(v.empty(), false);
  TEST_BOOL(v.count(), 1);
  TEST_INT(v.allocated(), (int)sizeof(int) * 4);

  v.append(10);
  v.append(7);
  v.append(20);
  TEST_INT(v.allocated(), (int)sizeof(int) * 4);
  TEST_INT(v[0], 9);
  TEST_INT(v[3], 20);

  // Forces a resize.
  v.append(6);
  TEST_INT(v.allocated(), (int)sizeof(int) * 8);

  TEST_INT(v[0], 9);
  TEST_INT(v[3], 20);
  TEST_INT(v[4], 6);
  TEST_INT(v.last(), 6);
  TEST_BOOL(v.count(), 5);

  TEST_INT(v.pop(), 6);
  TEST_INT(v.count(), 4);

  int values[] = { 9, 10, 7, 20 };
  int c = 0;

  for (auto i : v)
  {
    TEST_INT(i, values[c++]);
  }

  TEST_INT(c, 4);

  c = 0;
  for (auto it = v.begin(); it != v.end(); it++)
  {
    TEST_INT(*it, values[c++]);
  }

  TEST_INT(c, 4);

  return errors;
}

int test_clear()
{
  int errors = 0;

  Vector<int> v(4);

  v.append(1);
  v.append(2);
  v.append(3);

  TEST_INT(v.count(), 3);

  v.clear();
  TEST_BOOL(v.empty(), true);
  TEST_INT(v.count(),  0);

  v.append(7);
  TEST_BOOL(v.empty(), false);
  TEST_INT(v.count(),  1);
  TEST_INT(v[0],       7);
  TEST_INT(v.last(),   7);

  return errors;
}

int test_pop()
{
  int errors = 0;

  Vector<int> v(4);

  v.append(11);
  v.append(17);
  v.append(20);

  TEST_INT(v.count(), 3);
  TEST_INT(v.pop(),   20);
  TEST_INT(v.count(), 2);
  TEST_INT(v.pop(),   17);
  TEST_INT(v.count(), 1);
  TEST_INT(v.pop(),   11);
  TEST_INT(v.count(), 0);

  TEST_BOOL(v.empty(), true);

  return errors;
}

int test_empty_iterator()
{
  int errors = 0;

  Vector<int> v(4);

  int i = 0;
  for (auto value : v)
  {
    printf("%d\n", value);
    i += 1;
  }

  TEST_INT(i, 0);

  return errors;
}

int main(int argc, char *argv[])
{
  int errors = 0;

  printf("Testing Vector.h\n");

  errors += test_constructor();
  errors += test_constructor_with_size();
  errors += test_basic();
  errors += test_clear();
  errors += test_pop();
  errors += test_empty_iterator();

  if (errors != 0) { printf("Vector.h ... FAILED.\n"); return -1; }

  printf("Vector.h ... PASSED.\n");

  return 0;
}

