#include <stdio.h>
#include <stdlib.h>

#include "common/eval_expression.h"
#include "common/tokens.h"

int errors = 0;

void test(const char *expression, int answer)
{
  struct _asm_context asm_context = { 0 };
  int num;

  printf("Testing: %s ... ", expression);

  tokens_open_buffer(&asm_context, expression);
  tokens_reset(&asm_context);
  //assemble_init(&asm_context);

  if (eval_expression(&asm_context, &num) != 0)
  {
    printf("FAILED.  (error evaluating)\n");
    errors++;
  }
    else
  if (num == answer)
  {
    printf("PASS\n");
  }
    else
  {
    printf("FAILED. %d should be %d\n", num, answer);
    errors++;
  }

  tokens_close(&asm_context);
}

void should_fail(const char *expression)
{
  struct _asm_context asm_context = { 0 };
  int num;

  printf("Testing: %s ... ", expression);

  tokens_open_buffer(&asm_context, expression);
  tokens_reset(&asm_context);

  if (eval_expression(&asm_context, &num) == 0)
  {
    printf("FAILED.  (eval_expression() should have failed)\n");
    errors++;
  }
    else
  {
    printf("PASS\n");
  }

  tokens_close(&asm_context);
}

int main(int argc, char *argv[])
{
  printf("eval_expression() test\n");

  test("-6", -6);
  test("3*5", 15);
  test("3+5+1*2", 10);
  test("3 * 5", 15);
  test("3*(5+4)", 27);
  test("3*(5<<2)", 60);
  test("5+3*9", 32);
  test("(106 - 100) * 2 - 2", 10);
  test("(106 - 100) / 2 - 2", 1);
  test("6 / 2 - 2", 1);
  test("6 * 2 - 2", 10);
  test("((106 - 100) / 2) - 2", 1);
  test("~0", 0xffffffff);
  test("1 + ~0", 0);
  test("1 + ~1", -1);
  test("1 + ~(0 - 1)", 1);
  test("2 + ~~0", 2);
  test("1 + ~~(0 - 1)", 0);
  test("~~5", 5);
  test("~~5 + 1", 6);
  test("~1 + 1", -1);
  test("~-1 + 5", 5);
  test("~-~1 + 5", 2);
  test("6(", 6);
  test("-6*(3+4)", -42);
  test("6-4*(3+4)", -22);
  test("6-4(3+4)", 2);
  test("-6(", -6);
  test("1+(((2*3)+5)+3)", 15);
  should_fail("23 23");
  should_fail("23~23");
  test("((1 << 6)+(2 << 5)) >> 1", 64);
  test("(1 << 6)+(2 << 5) >> 1", 64);
  test("(1 << 6)|(2 << 5) >> 1", 96);
  test("((1) << 6)|((2) << 5) >> 1", 96);
  test("((1 << 6)|(2 << 5)) >> 1", 32);
  test("0xffffffff", -1);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


