#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "common/eval_expression.h"
#include "common/tokens.h"
#include "common/Var.h"

int errors = 0;

int get_int(const char *expression)
{
  AsmContext asm_context;
  Var var;

  printf("Testing: %s\n", expression);

  tokens_open_buffer(&asm_context, expression);
  tokens_reset(&asm_context);

  int ret = eval_expression(&asm_context, var);
  int num = var.get_int32();

  if (ret != 0)
  {
    printf("FAILED.  (error evaluating)\n");
  }

  tokens_close(&asm_context);

  return num;
}

void test_int(const char *expression, int answer)
{
  AsmContext asm_context;
  Var var;

  printf("Testing: %s ... ", expression);

  tokens_open_buffer(&asm_context, expression);
  tokens_reset(&asm_context);
  //assemble_init(&asm_context);

  int ret = eval_expression(&asm_context, var);
  int num = var.get_int32();

  if (ret != 0)
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

void test_int64(const char *expression, int64_t answer)
{
  AsmContext asm_context;
  Var var;

  printf("Testing: %s ... ", expression);

  tokens_open_buffer(&asm_context, expression);
  tokens_reset(&asm_context);
  //assemble_init(&asm_context);

  int ret = eval_expression(&asm_context, var);
  int64_t num = var.get_int64();

  if (ret != 0)
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
    printf("FAILED. %" PRId64 " should be %" PRId64 "\n", num, answer);
    errors++;
  }

  tokens_close(&asm_context);
}

void should_fail(const char *expression)
{
  AsmContext asm_context;
  Var var;

  printf("Testing: %s ... ", expression);

  tokens_open_buffer(&asm_context, expression);
  tokens_reset(&asm_context);

  int ret = eval_expression(&asm_context, var);

  if (ret == 0)
  {
    printf("FAILED.  (should have failed)\n");
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

  if (argc >= 2)
  {
    int n = get_int(argv[1]);
    printf("Answer: %d\n", n);
    exit(0);
  }


  test_int("-6", -6);
  test_int("6 + -5", 1);
  test_int("+6 + -5", 1);
  test_int("6 + -5 + -3", -2);
  test_int("~0xfe", 0xffffff01);
  test_int("3 * \"\\0\" + 1", 1);
  test_int("3 * '0' + 1", 145);
  test_int("3*5", 15);
  test_int("3+5+1*2", 10);
  test_int("3 * 5", 15);
  test_int("3*(5+4)", 27);
  test_int("3*(5<<2)", 60);
  test_int("5+3*9", 32);
  test_int("1+(((2*3)+5)+3)", 15);
  test_int("(106 - 100) * 2 - 2", 10);
  test_int("(106 - 100) / 2 - 2", 1);
  test_int("6 / 2 - 2", 1);
  test_int("6 * 2 - 2", 10);
  test_int("8 * 4 / 2", 16);
  test_int64("0xffffffff+1", 0x100000000);
  test_int64("~0", -1);
  test_int64("1 - ~0", 2);
  test_int64("1 + ~0", 0);
  test_int64("1 + ~1", -1);
  test_int64("1 + ~(0 - 1)", 1);
  test_int64("2 + ~~0", 2);
  test_int64("1 + ~~(0 - 1)", 0);
  test_int64("~~5", 5);
  test_int64("~~5 + 1", 6);
  test_int64("~1 + 1", -1);
  test_int64("~-1 + 5", 5);
  test_int64("~-~1 + 5", 2);
  test_int("6(", 6);
  test_int("-6*(3+4)", -42);
  test_int("6-4*(3+4)", -22);
  test_int("6-4(3+4)", 2);
  test_int("-6(", -6);
  test_int64("6(", 6);
  test_int64("-6*(3+4)", -42);
  test_int64("6-4*(3+4)", -22);
  test_int64("6-4(3+4)", 2);
  test_int64("-6(", -6);
  should_fail("23 23");
  should_fail("23~23");
  test_int("((1 << 6)+(2 << 5)) >> 1", 64);
  test_int64("(1 << 6)+(2 << 5) >> 1", 64);
  test_int("(1 << 6)|(2 << 5) >> 1", 96);
  test_int64("((1) << 6)|((2) << 5) >> 1", 96);
  test_int("((1) << 6)|((2) << 5) >> 32", 64);
  test_int64("((1) << 6)|((2) << 5) >> 32", 64);
  test_int64("(1 << 6)|(2 << 5) >> 1", 96);
  test_int64("((1 << 6)|(2 << 5)) >> 1", 32);
  test_int64("((1 << 6)|(2 << 5)) >> 32", 0);
  test_int64("0xffffffff", 0xffffffff);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


