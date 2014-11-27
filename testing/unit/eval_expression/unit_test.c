#include <stdio.h>
#include <stdlib.h>

#include "eval_expression.h"
#include "tokens.h"

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

int main(int argc, char *argv[])
{
  printf("eval_expression() test\n");

  test("3*5", 15);
  test("3 * 5", 15);
  test("3*(5+4)", 27);
  test("3*(5<<2)", 60);
  test("5+3*9", 32);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


