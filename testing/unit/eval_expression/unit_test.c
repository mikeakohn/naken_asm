#include <stdio.h>
#include <stdlib.h>

#include "assembler.h"
#include "eval_expression.h"
#include "tokens.h"

int errors = 0;

void test(const char *expression, int answer)
{
  struct _asm_context asm_context;
  int num;

  printf("Testing: %s ... ", expression);

  tokens_open_buffer(&asm_context, expression);

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
  test("3*5", 15);
  test("3 * 5", 15);
  test("3*(5+4)", 27);
  test("3*(5<<2)", 60);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  return 0;
}


