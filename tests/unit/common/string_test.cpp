#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/String.h"

#define TEST_INT(a, b) \
  if (a != b) \
  { \
    errors += 1; \
    printf("Error: %d != %d  %s:%d\n", a, b, __FILE__, __LINE__); \
  }

#define TEST_TEXT(a, b) \
  if (strcmp(a,b) != 0) \
  { \
    errors += 1; \
    printf("Error: %s != %s  %s:%d\n", a, b, __FILE__, __LINE__); \
  }

void what_is_this()
{
  String s("hello");
  //s.set("hello");
  //s.append("roar");

  //s = "hello";
  s += "roar";

  printf("%s\n", s.value());
}

int main(int argc, char *argv[])
{
  int errors = 0;

  printf("Testing String.h\n");

  String s;
  TEST_INT(s.len(), 0);

  s.set("hello");
  TEST_INT(s.len(), 5);
  TEST_TEXT(s.value(), "hello");

  s = "asdf";
  TEST_INT(s.len(), 4);
  TEST_TEXT(s.value(), "asdf");

  s.append("roar");
  TEST_INT(s.len(), 8);
  TEST_TEXT(s.value(), "asdfroar");

  s += "1234";
  TEST_INT(s.len(), 12);
  TEST_TEXT(s.value(), "asdfroar1234");

  String x("qwerty");
  TEST_INT(x.len(), 6);
  TEST_TEXT(x.value(), "qwerty");

  String k(x);
  TEST_INT(x.len(), 6);
  TEST_TEXT(x.value(), "qwerty");

  k += "*";
  x = k;

  TEST_INT(x.len(), 7);
  TEST_TEXT(x.value(), "qwerty*");

#if 0
  // This is slow.
  x = x + "?";
  TEST_INT(x.len(), 8);
  TEST_TEXT(x.value(), "qwerty*?");
#endif

  if (errors != 0) { printf("String.h ... FAILED.\n"); return -1; }

  printf("String.h ... PASSED.\n");

  return 0;
}

