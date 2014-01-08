#include <stdio.h>
#include <stdlib.h>

#include "assembler.h"
#include "eval_expression.h"
#include "print_error.h"

//const char *test1[] = { "5", "+", "2", NULL };

#if 0
void reset(struct _asm_context *asm_context)
{
  memset(&asm_context, 0, asm_context);
  asm_context->pass=2;
}
#endif

int main()
{
struct _asm_context asm_context;
int num;

  //reset(&asm_context);
  //asm_context->tokens=test1;

  asm_context.in=fopen("unit.txt", "rb");
  asm_context.pass=1;
  assemble_init(&asm_context);
  asm_context.pass=2;

  if (eval_expression(&asm_context, &num)==-1)
  {
    print_error("Error: eval_expression()==-1.  Test fail", &asm_context);
    return -1;
  }
  printf("num=%d\n", num);

  return 0;
}

