#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/tokens.h"

int errors = 0;

void test_constants()
{
  struct _asm_context asm_context;
  char token[TOKENLEN];
  int token_type;
  int i;
  char *test = { "1234 0x12 0b11001001 0100b 20h \"\\n\\t\\r\"" };
  char *answer[] = { "1234", "18", "201", "4", "32", "\n\t\r", NULL };

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_constants - \n");

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

void test_1()
{
  struct _asm_context asm_context;
  char token[TOKENLEN];
  int token_type;
  int i;
  char *test = { "1234 1234.w [(1234.5)] asdf55[33] 1&2|3^3\n f*e #234" };
  char *answer[] = { "1234", "1234", ".", "w", "[", "(", "1234.5", ")", "]",
                     "asdf55", "[", "33", "]", "1", "&", "2", "|", "3",
                     "^", "3", "\n", "f", "*", "e", "#", "234", NULL };

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_1 - \n");

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

void test_pushback()
{
  struct _asm_context asm_context;
  char *test = { "1234 5 20" };
  char token[TOKENLEN];
  int token_type;

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_pushback - \n");

  tokens_open_buffer(&asm_context, test);
  tokens_reset(&asm_context);

  token_type = tokens_get(&asm_context, token, TOKENLEN);

  if (strcmp(token, "1234") != 0)
  {
    printf("FAIL: expected 1234\n");
    errors++;
  }

  tokens_push(&asm_context, "bbbb", 10);
  token_type = tokens_get(&asm_context, token, TOKENLEN);

  if (strcmp(token, "bbbb") != 0 || token_type != 10)
  {
    printf("FAIL: expected bbbb with token_type=10 but got %s %d\n", token, token_type);
    errors++;
  }

  tokens_push(&asm_context, "iii", 11);
  tokens_push(&asm_context, "mmm", 12);

  token_type = tokens_get(&asm_context, token, TOKENLEN);

  if (strcmp(token, "mmm") != 0 || token_type != 12)
  {
    printf("FAIL: expected mmm with token_type=12 but got %s %d\n", token, token_type);
    errors++;
  }

  token_type = tokens_get(&asm_context, token, TOKENLEN);

  if (strcmp(token, "iii") != 0 || token_type != 11)
  {
    printf("FAIL: expected iii with token_type=11 but got %s %d\n", token, token_type);
    errors++;
  }

  token_type = tokens_get(&asm_context, token, TOKENLEN);

  if (strcmp(token, "5") != 0)
  {
    printf("FAIL: expected 5 got %s\n", token);
    errors++;
  }

  tokens_close(&asm_context);
}

void test_strings_with_dots()
{
  struct _asm_context asm_context;
  char *test = { "mov.w" };
  char token[TOKENLEN];
  int token_type;

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_strings_with_dots - \n");

  tokens_open_buffer(&asm_context, test);
  tokens_reset(&asm_context);

  asm_context.strings_have_dots = 1;

  token_type = tokens_get(&asm_context, token, TOKENLEN);

  if (strcmp(token, "mov.w") != 0)
  {
    printf("FAIL: Expected mov.w but got %s %s:%d\n",
      token, __FILE__, __LINE__);
    errors++;
  }

  if (token_type != TOKEN_STRING)
  {
    printf("FAIL: Expected TOKEN_STRING %s:%d\n", __FILE__, __LINE__);
    errors++;
  }

  tokens_close(&asm_context);
}

void test_ascii_with_null()
{
  struct _asm_context asm_context;
  const char *code = ".ascii \"test\\n\\r\\t\\0\"\n.db 5";
  const char result[] = { 't', 'e', 's', 't', '\n', '\r', '\t', 0 };
  int error_flag, i;

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_ascii_with_null - \n");

  asm_context.pass = 1;
  assembler_init(&asm_context);

  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);
  error_flag = assemble(&asm_context);

  if (error_flag != 0)
  {
    printf("error_flag=%d\n", error_flag);
    errors++;
  }

  for (i = 0; i < sizeof(result); i++)
  {
    char c = memory_read_m(&asm_context.memory, i);
    if (c != result[i])
    {
      printf("Error at position %d, got %02x but expected %02x\n",
        i, c, result[i]);
      errors++;
    }
  }
}

void test_asciiz()
{
  struct _asm_context asm_context;
  const char *code = ".asciiz \"test\"\n.db 5\n";
  const char result[] = { 't', 'e', 's', 't', 0 };
  int error_flag, i;

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_asciiz - \n");

  asm_context.pass = 1;
  assembler_init(&asm_context);

  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);
  error_flag = assemble(&asm_context);

  if (error_flag != 0)
  {
    printf("error_flag=%d\n", error_flag);
    errors++;
  }

  for (i = 0; i < 5; i++)
  {
    char c = memory_read_m(&asm_context.memory, i);
    if (c != result[i])
    {
      printf("Error at position %d, got %02x but expected %02x\n",
        i, c, result[i]);
      errors++;
    }
  }
}

void test_escape_chars_in_db()
{
  struct _asm_context asm_context;
  const char *code = ".db '\\n', '\\r', '\\t', '\\0', '\\\\', '\\'', 5\n";
  const char result[] = { '\n',   '\r',  '\t',  '\0',   '\\',  '\'' };
  int error_flag, i;

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_escape_chars_in_db - \n");

  //printf("code: %s\n", code);

  asm_context.pass = 1;
  assembler_init(&asm_context);

  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);
  error_flag = assemble(&asm_context);

  if (error_flag != 0)
  {
    printf("error_flag=%d\n", error_flag);
    errors++;
  }

  for (i = 0; i < sizeof(result); i++)
  {
    char c = memory_read_m(&asm_context.memory, i);
    if (c != result[i])
    {
      printf("Error at position %d, got %02x but expected %02x\n",
        i, c, result[i]);
      errors++;
    }
  }
}

void test_escape_chars_in_code_const()
{
  struct _asm_context asm_context;
  const char *code = ".6502\n"
                     "lda #'\\n'\n"
                     "lda #'\\0'\n"
                     "lda #'\\\\'\n"
                     "lda #'\\''\n"
                     "lda #'\\\"'\n"
                     "lda #'\"'\n";
  const char result[] = { '\n', '\0', '\\', '\'', '"', '"' };
  int error_flag, ptr, i;
  uint8_t c;

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_escape_chars_in_code_const - \n");

  //printf("code: %s\n", code);

  asm_context.pass = 1;
  assembler_init(&asm_context);
  asm_context.pass = 2;
  assembler_init(&asm_context);

  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);
  error_flag = assemble(&asm_context);

  if (error_flag != 0)
  {
    printf("error_flag=%d\n", error_flag);
    errors++;
  }

  ptr = 1;

  for (i = 0; i < sizeof(result); i++)
  {
    c = memory_read_m(&asm_context.memory, ptr);

    if (c != result[i])
    {
      printf("Error at position %d, got 0x%02x but expected 0x%02x\n",
        i, c, result[i]);
      errors++;
    }

    ptr += 2;
  }
}

void test_db_quote_error()
{
  struct _asm_context asm_context;
  const char *code = ".ascii \"hello\n";
  int error_flag;

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_db_quote_error - \n");

  asm_context.pass = 1;
  assembler_init(&asm_context);
  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);
  error_flag = assemble(&asm_context);

  if (error_flag == 0)
  {
    printf("Error: error_flag=%d\n", error_flag);
    errors++;
  }
}

void test_db_tick_error()
{
  struct _asm_context asm_context;
  const char *code = ".db '\\n\n";
  int error_flag;

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_db_tick_error - \n");

  asm_context.pass = 1;
  assembler_init(&asm_context);
  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);
  error_flag = assemble(&asm_context);

  if (error_flag == 0)
  {
    printf("Error: error_flag=%d\n", error_flag);
    errors++;
  }
}

void test_code_tick_error()
{
  struct _asm_context asm_context;
  const char *code = ".6502\nlda #'\\n\n";
  int error_flag;

  memset(&asm_context, 0, sizeof(asm_context));

  printf(" - test_code_tick_error - \n");

  asm_context.pass = 1;
  assembler_init(&asm_context);
  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);
  error_flag = assemble(&asm_context);

  if (error_flag == 0)
  {
    printf("Error: error_flag=%d\n", error_flag);
    errors++;
  }
}

int main(int argc, char *argv[])
{
  printf("tokens.o test\n");

  test_1();
  test_constants();
  test_pushback();
  test_strings_with_dots();

  // These might be more of data tests... although the tokenizer is involved
  // in much of this.
  test_ascii_with_null();
  test_asciiz();
  test_escape_chars_in_db();
  test_escape_chars_in_code_const();
  test_db_quote_error();
  test_db_tick_error();
  test_code_tick_error();

  printf("Testing: tokens.o ... ");

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


