#include <stdio.h>
#include <stdlib.h>

#include "eval_expression_ex.h"
#include "tokens.h"
#include "var.h"

int errors = 0;

void test_int(const char *expression, int answer)
{
  struct _asm_context asm_context = { 0 };
  struct _var var;

  printf("Testing: %s ... ", expression);

  tokens_open_buffer(&asm_context, expression);
  tokens_reset(&asm_context);
  //assemble_init(&asm_context);

  int ret = eval_expression_ex(&asm_context, &var);
  int num = var_get_int32(&var);

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
  struct _asm_context asm_context = { 0 };
  struct _var var;

  printf("Testing: %s ... ", expression);

  tokens_open_buffer(&asm_context, expression);
  tokens_reset(&asm_context);
  //assemble_init(&asm_context);

  int ret = eval_expression_ex(&asm_context, &var);
  int64_t num = var_get_int64(&var);

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
    printf("FAILED. %ld should be %ld\n", num, answer);
    errors++;
  }

  tokens_close(&asm_context);
}

int main(int argc, char *argv[])
{
  printf("eval_expression_ex() test\n");

  test_int("3*5", 15);
  test_int("3 * 5", 15);
  test_int("3*(5+4)", 27);
  test_int("3*(5<<2)", 60);
  test_int("5+3*9", 32);
  test_int("1+(((2*3)+5)+3)", 15);
  test_int64("0xffffffff+1", 0x100000000);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


