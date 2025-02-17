#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/String.h"
#include "test_checks.h"

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

  s = "i'm a little teapot";
  int i = s.find(' ');
  s.replace_at(i, 0);
  TEST_TEXT(s.value(), "i'm");
  s.replace_at(1000, 0);
  s.replace_at(-1, 0);
  TEST_TEXT(s.value(), "i'm");

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

int test_equals()
{
  int errors = 0;

  String s("asdf");
  String k("asdf");
  String f("1234");

  TEST_BOOL((s == "asdf"), true);
  TEST_BOOL((s == "a"), false);
  TEST_BOOL((s == k), true);
  TEST_BOOL((s == f), false);

  return errors;
}

int test_rtrim()
{
  int errors = 0;

  String s("asdf");
  String k("\n\t  asdf");
  String f("asdf \n \t  ");

  s.rtrim();
  k.rtrim();
  f.rtrim();

  TEST_INT(s.len(), 4);
  TEST_INT(k.len(), 8);
  TEST_INT(f.len(), 4);

  TEST_BOOL((s == "asdf"), true);
  TEST_BOOL((k == "\n\t  asdf"), true);
  TEST_BOOL((f == "asdf"), true);

  return errors;
}

int test_ltrim()
{
  int errors = 0;

  String s("asdf");
  String k("\n\t  asdf");
  String f("asdf \n \t  ");

  s.ltrim();
  k.ltrim();
  f.ltrim();

  TEST_INT(s.len(), 4);
  TEST_INT(k.len(), 4);
  TEST_INT(f.len(), 10);

  TEST_BOOL((s == "asdf"), true);
  TEST_BOOL((k == "asdf"), true);
  TEST_BOOL((f == "asdf \n \t  "), true);

  return errors;
}

int test_trim()
{
  int errors = 0;

  String s("asdf");
  String k("\n\t  asdf");
  String f("asdf \n \t  ");
  String a(" \tasdf \n \t  ");
  String b("asdf ");
  String c(" asdf");

  s.trim();
  k.trim();
  f.trim();
  a.trim();
  b.trim();
  c.trim();

  TEST_INT(s.len(), 4);
  TEST_INT(k.len(), 4);
  TEST_INT(f.len(), 4);
  TEST_INT(a.len(), 4);
  TEST_INT(b.len(), 4);
  TEST_INT(c.len(), 4);

  TEST_BOOL((s == "asdf"), true);
  TEST_BOOL((k == "asdf"), true);
  TEST_BOOL((f == "asdf"), true);
  TEST_BOOL((a == "asdf"), true);
  TEST_BOOL((b == "asdf"), true);
  TEST_BOOL((c == "asdf"), true);

  return errors;
}

int test_find()
{
  int errors = 0;

  String a("mike kohn");
  String b("");

  TEST_INT(a.find(' '), 4);
  TEST_INT(a.find('m'), 0);
  TEST_INT(a.find('r'), -1);
  TEST_INT(a.find(0),   -1);

  TEST_INT(b.find(' '), -1);
  TEST_INT(b.find('m'), -1);
  TEST_INT(b.find('r'), -1);
  TEST_INT(b.find(0),   -1);

  return errors;
}

int test_startswith()
{
  int errors = 0;

  String a("mike kohn");
  String b("");
  String c("mike");

  TEST_INT(a.startswith("m"),    true);
  TEST_INT(a.startswith("mike"), true);
  TEST_INT(a.startswith("ike"),  false);
  TEST_INT(a.startswith(""),     true);

  TEST_INT(b.startswith("m"),    false);
  TEST_INT(b.startswith("mike"), false);
  TEST_INT(b.startswith("ike"),  false);
  TEST_INT(b.startswith(""),     true);

  TEST_INT(c.startswith("mike"),  true);
  TEST_INT(c.startswith("mike "), false);

  return errors;
}

int test_numbers()
{
  int errors = 0;

  String a("1234");
  String b("0x1234");
  String c("1234K");

  TEST_BOOL(a.is_number(), true);
  TEST_BOOL(b.is_number(), true);
  TEST_BOOL(c.is_number(), false);

  TEST_INT(a.as_int(), 1234);
  TEST_INT(b.as_int(), 0x1234);

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
  errors += test_equals();
  errors += test_rtrim();
  errors += test_ltrim();
  errors += test_trim();
  errors += test_find();
  errors += test_startswith();
  errors += test_numbers();

  if (errors != 0) { printf("String.h ... FAILED.\n"); return -1; }

  printf("String.h ... PASSED.\n");

  return 0;
}

