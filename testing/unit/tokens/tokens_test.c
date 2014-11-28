#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "tokens.h"

int errors = 0;

void test_1()
{
  struct _asm_context asm_context = { 0 };
  char token[TOKENLEN];
  int token_type; 
  int i;
  char *test = { "1234 1234.w [(1234.5)] asdf55[33] 1&2|3^3\n f*e #234" };
  char *answer[] = { "1234", "1234", ".", "w", "[", "(", "1234.5", ")", "]",
                     "asdf55", "[", "33", "]", "1", "&", "2", "|", "3",
                     "^", "3", "\n", "f", "*", "e", "#", "234", NULL };

  printf("Testing: %s ... ", test);

  tokens_open_buffer(&asm_context, test);
  tokens_reset(&asm_context);

  i = 0;
  while(1)
  {
    token_type = tokens_get(&asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOF) { break; }

    if (answer[i] == NULL)
    {
      printf("FAIL: too many tokens\n");
      errors++;
      break;
    }

    if (token_type == TOKEN_EOL)
    {
      if (answer[i++][0] != '\n')
      {
        printf("FAIL: Expected EOL\n");
        errors++;
      }
      continue;
    }

    //printf("'%s' '%s'\n", token, answer[i]);

    if (strcmp(token, answer[i]) != 0)
    {
      printf("FAIL: Expected '%s' and got '%s'\n", answer[i], token);
      errors++;
    }

    i++;

  }

  tokens_close(&asm_context);
}

int main(int argc, char *argv[])
{
  printf("tokens.o test\n");

  test_1();

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


