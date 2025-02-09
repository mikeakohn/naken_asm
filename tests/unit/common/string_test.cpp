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

#define TEST_BOOL(a, b) \
  if (a != b) \
  { \
    errors += 1; \
    printf("Error: %s != %s  %s:%d\n", \
      a != 0 ? "true" : "false", \
      b != 0 ? "true" : "false", \
      __FILE__, __LINE__); \
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

int test_constructor_char_string()
{
  int errors = 0;

  String s("asdf");
  TEST_INT(s.len(), 4);
  TEST_TEXT(s.value(), "asdf");

  return errors;
}

int test_constructor_string()
{
  int errors = 0;

  String k("asdf");

  String s(k);
  TEST_INT(s.len(), 4);
  TEST_TEXT(s.value(), "asdf");

  return errors;
}

int test_basic()
{
  int errors = 0;

  String s;
  TEST_INT(s.len(), 0);

  s.set("yellow");
  TEST_INT(s.len(), 6);
  TEST_TEXT(s.value(), "yellow");
  TEST_BOOL(s.equals("yellow"), true);
  TEST_BOOL(s.equals("yello"),  false);
  TEST_BOOL(s.equals("yelloww"),  false);
  TEST_INT(s.char_at(-1), 'w');
  TEST_INT(s.char_at(-5), 'e');
  TEST_INT(s.char_at(-100), 0);
  TEST_INT(s.char_at(1),  'e');
  TEST_INT(s.char_at(100), 0);

  s.set("diff");
  TEST_INT(s.len(), 4);
  TEST_BOOL(s.equals("yellow"), false);
  TEST_BOOL(s.equals("diff"), true);

  s.clear();
  TEST_INT(s.len(), 0);
  TEST_TEXT(s.value(), "");
  TEST_BOOL(s.equals(""), true);

  return errors;
}

int test_append_char_string()
{
  int errors = 0;

  String s;
  s += "asdf";
  TEST_INT(s.len(), 4);
  TEST_TEXT(s.value(), "asdf");

  s.append("roar");
  TEST_INT(s.len(), 8);
  TEST_TEXT(s.value(), "asdfroar");

  s += "1234";
  TEST_INT(s.len(), 12);
  TEST_TEXT(s.value(), "asdfroar1234");

  return errors;
}

int test_append_char()
{
  int errors = 0;

  String s;
  s += 'a';
  s += 's';
  s += 'd';
  s += 'f';
  TEST_INT(s.len(), 4);
  TEST_TEXT(s.value(), "asdf");

  return errors;
}

int test_big_string()
{
  int errors = 0;

  String s;
  int size = s.allocated();

  for (int n = 0; n < size - 1; n++)
  {
    s += 'A';
  }

  TEST_INT(s.len(), size - 1);
  TEST_BOOL(s.is_heap(), false);

  s += 'B';
  TEST_BOOL(s.is_heap(), true);
  TEST_INT(s.allocated(), size * 2);

  return errors;
}

int test_big_string_append_0()
{
  int errors = 0;

  String s;
  int size = s.allocated();

  for (int n = 0; n < size - 2; n++)
  {
    s += 'A';
  }

  s += "B";
  TEST_BOOL(s.is_heap(), false);

  return errors;
}

int test_big_string_append_1()
{
  int errors = 0;

  String s;
  int size = s.allocated();

  for (int n = 0; n < size - 2; n++)
  {
    s += 'A';
  }

  s += "BB";
  TEST_BOOL(s.is_heap(), true);
  TEST_INT(s.allocated(), size * 2);

  return errors;
}

int main(int argc, char *argv[])
{
  int errors = 0;

  printf("Testing String.h\n");

  errors += test_constructor_char_string();
  errors += test_constructor_string();
  errors += test_basic();
  errors += test_append_char_string();
  errors += test_append_char();
  errors += test_big_string();
  errors += test_big_string_append_0();
  errors += test_big_string_append_1();

  if (errors != 0) { printf("String.h ... FAILED.\n"); return -1; }

  printf("String.h ... PASSED.\n");

  return 0;
}

