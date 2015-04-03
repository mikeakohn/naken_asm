#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "macros.h"
#include "tokens.h"

int errors = 0;

const char *answer_1[] = { "one", "two", "three", NULL };

void test(const char *macro)
{
  struct _asm_context asm_context = { 0 };
  char token[TOKENLEN];
  int token_type; 
  //int num;

  printf("Testing: %s ... ", macro);

  tokens_open_buffer(&asm_context, macro);
  tokens_reset(&asm_context);

  while(1)
  {
    token_type = tokens_get(&asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOF) { break; }
    if (token_type == TOKEN_EOL) { continue; }

    if (strcasecmp(token, "macro") == 0)
    {
      if (macros_parse(&asm_context, IS_MACRO) != 0)
      {
        errors++;
        return;
      }
    }
      else
    {
      printf("%s\n", token);
    }

  {
    //printf("FAILED. %d should be %d\n", num, answer);
    //errors++;
  }
  }

  tokens_close(&asm_context);
}

int main(int argc, char *argv[])
{
  printf("macros.o test\n");

  test(".macro blah\none\ntwo\nthree\n.endm\nblah()\n");

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


