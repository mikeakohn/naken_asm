#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/String.h"
#include "common/StringTokenizer.h"
#include "test_checks.h"

int test_pop_num()
{
  int errors = 0;
  int num;
  char ch;

  StringTokenizer tokenizer("   0x1a3F   -   9999   ");
  String token;

  TEST_BOOL(tokenizer.is_empty(), false);

  TEST_BOOL(tokenizer.is_num(), true);
  num = tokenizer.pop_num();
  TEST_INT(num, 0x1a3f);

  TEST_BOOL(tokenizer.is_num(), false);
  num = tokenizer.pop_num();
  TEST_INT(num, -1);
  ch = tokenizer.pop_char();
  TEST_INT(ch, '-');

  TEST_BOOL(tokenizer.is_num(), true);
  num = tokenizer.pop_num();
  TEST_INT(num, 9999);

  TEST_BOOL(tokenizer.is_empty(), true);

  num = tokenizer.pop_num();
  TEST_INT(num, -1);
  ch = tokenizer.pop_char();
  TEST_INT(ch, 0);

  return errors;
}

int test_whitespace_removal()
{
  int errors = 0;

  StringTokenizer tokenizer("   1234   -   9999   ");
  String token;

  TEST_BOOL(tokenizer.is_empty(), false);

  TEST_BOOL(tokenizer.is_num(), true);
  tokenizer.pop(token);
  TEST_TEXT(token.value(), "1234");

  TEST_BOOL(tokenizer.is_num(), false);
  tokenizer.pop(token);
  TEST_TEXT(token.value(), "-");

  TEST_BOOL(tokenizer.is_num(), true);
  tokenizer.pop(token);
  TEST_TEXT(token.value(), "9999");

  TEST_BOOL(tokenizer.is_empty(), true);

  return errors;
}

int test_from_char_array()
{
  int errors = 0;

  StringTokenizer tokenizer("1234-9999");
  String token;

  TEST_BOOL(tokenizer.is_empty(), false);

  tokenizer.pop(token);
  TEST_TEXT(token.value(), "1234");

  tokenizer.pop(token);
  TEST_TEXT(token.value(), "-");

  tokenizer.pop(token);
  TEST_TEXT(token.value(), "9999");

  TEST_BOOL(tokenizer.is_empty(), true);

  return errors;
}

int test_from_string()
{
  int errors = 0;

  String a("1234-9999");
  StringTokenizer tokenizer(a);
  String token;

  TEST_BOOL(tokenizer.is_empty(), false);

  tokenizer.pop(token);
  TEST_TEXT(token.value(), "1234");

  tokenizer.pop(token);
  TEST_TEXT(token.value(), "-");

  tokenizer.pop(token);
  TEST_TEXT(token.value(), "9999");

  TEST_BOOL(tokenizer.is_empty(), true);

  return errors;
}

int main(int argc, char *argv[])
{
  int errors = 0;

  printf("Testing String.h\n");

  errors += test_pop_num();
  errors += test_whitespace_removal();
  errors += test_from_char_array();
  errors += test_from_string();

  if (errors != 0) { printf("String.h ... FAILED.\n"); return -1; }

  printf("String.h ... PASSED.\n");

  return 0;
}

