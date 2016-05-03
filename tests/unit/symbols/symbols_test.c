#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/symbols.h"

int errors = 0;

void append(struct _symbols *symbols, char *name, int address)
{
  if (symbols_append(symbols, name, address) != 0)
  {
    printf("Error: %s %d  %s:%d\n", name, address, __FILE__, __LINE__);
    errors++;
  }
}

void check_symbols_count(struct _symbols *symbols, int count)
{
  if (symbols_count(symbols) != count)
  {
    printf("Error: symbols_count != %d  %s:%d\n", count, __FILE__, __LINE__);
    errors++;
  }
}

int main(int argc, char *argv[])
{
  struct _symbols symbols;

  symbols_init(&symbols);

  append(&symbols, "test1", 100);
  append(&symbols, "test2", 200);
  append(&symbols, "test3", 300);

  check_symbols_count(&symbols, 3);

  symbols_lock(&symbols);
  append(&symbols, "test4", 50);
  check_symbols_count(&symbols, 3);

  symbols_free(&symbols);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


