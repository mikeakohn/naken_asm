#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/StringHeap.h"
#include "test_checks.h"

int test_constructor()
{
  int errors = 0;

  StringHeap s;
  TEST_INT(s.allocated(), 4096);
  TEST_INT(s.count(), 0);
  TEST_INT(s.len(), 0);

  return errors;
}

int test_constructor_with_size()
{
  int errors = 0;

  StringHeap s(8192);

  TEST_INT(s.allocated(), 8192);
  TEST_INT(s.count(), 0);
  TEST_INT(s.len(), 0);

  return errors;
}

int test_basic()
{
  int errors = 0;
  int i;

  const char *name;

  StringHeap s;

  TEST_INT(s.count(), 0);

  name = s.append("hello");
  TEST_INT(s.count(), 1);
  TEST_INT(s.len(), 8);
  TEST_BOOL((strcmp(name, "hello") == 0), true);

  name = s.append("blah");
  TEST_INT(s.count(), 2);
  TEST_INT(s.len(), 15);
  TEST_BOOL((strcmp(name, "blah") == 0), true);

  TEST_INT(s.find("hello"), 0);
  TEST_INT(s.find("blah"),  1);
  TEST_INT(s.find("asdf"), -1);

  s.dump();

  i = 0;
  for (StringHeap::iterator it = s.begin(); it != s.end(); it++)
  {
    switch (i)
    {
      case 0: TEST_BOOL((strcmp(*it, "hello") == 0), true); break;
      case 1: TEST_BOOL((strcmp(*it, "blah") == 0),  true); break;
      default: break;
    }

    i += 1;
  }

  i = 0;
  for (auto value : s)
  {
    switch (i)
    {
      case 0: TEST_BOOL((strcmp(value, "hello") == 0), true); break;
      case 1: TEST_BOOL((strcmp(value, "blah") == 0),  true); break;
      default: break;
    }

    i += 1;
  }

  return errors;
}

int test_set()
{
  int errors = 0;

  StringHeap s;

  s.set("mike\0kohn\0wrote\0this\0\0");

  TEST_INT(s.count(), 4);
  TEST_INT(s.find("mike"),  0);
  TEST_INT(s.find("kohn"),  1);
  TEST_INT(s.find("wrote"), 2);
  TEST_INT(s.find("this"),  3);

  s.set("test\0and\0this\0class\0\0", 3);

  TEST_INT(s.find("mike"),  -1);
  TEST_INT(s.find("test"),   0);
  TEST_INT(s.find("and"),    1);
  TEST_INT(s.find("this"),   2);
  TEST_INT(s.find("class"), -1);

  TEST_INT(s.count(), 3);

  return errors;
}

int test_resize()
{
  int errors = 0;

  StringHeap s(16);

  TEST_INT(s.allocated(), 16);

  s.set("mike\0kohn\0wrote\0this\0\0");

  TEST_INT(s.allocated(), 32);
  TEST_INT(s.count(), 4);
  TEST_INT(s.find("mike"),  0);
  TEST_INT(s.find("kohn"),  1);
  TEST_INT(s.find("wrote"), 2);
  TEST_INT(s.find("this"),  3);

  return errors;
}

int test_empty_iterator()
{
  int errors = 0;

  StringHeap s(16);

  int i = 0;
  for (auto value : s)
  {
    printf("%s\n", value);
    i += 1;
  }

  TEST_INT(i, 0);

  return errors;
}

int main(int argc, char *argv[])
{
  int errors = 0;

  printf("Testing StringHeap.h\n");

  errors += test_constructor();
  errors += test_constructor_with_size();
  errors += test_basic();
  errors += test_set();
  errors += test_resize();
  errors += test_empty_iterator();

  if (errors != 0) { printf("StringHeap.h ... FAILED.\n"); return -1; }

  printf("StringHeap.h ... PASSED.\n");

  return 0;
}

