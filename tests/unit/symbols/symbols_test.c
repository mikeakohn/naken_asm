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

void check_value(struct _symbols *symbols, char *name, int expected)
{
  int value = symbols_lookup(symbols, name);

  if (value != expected)
  {
    printf("Error: %s != %d (%d) %s:%d\n", name, expected, value, __FILE__, __LINE__);
    errors++;
  }
}

void check_export(struct _symbols *symbols, char *name, int expected)
{
  int value = symbols_export(symbols, name);

  if (value != expected)
  {
    printf("Error: %s export %d (%d) %s:%d\n", name, expected, value, __FILE__, __LINE__);
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

  check_value(&symbols, "test1", 100);
  check_value(&symbols, "test2", 200);
  check_value(&symbols, "test3", 300);
  check_value(&symbols, "nothing", -1);

  // If the symbol was never created with symbols_set() it shouldn't
  // change here.
  symbols_set(&symbols, "test1", 150);
  check_value(&symbols, "test1", 100);

  // This symbol should change.
  symbols_set(&symbols, "test4", 150);
  check_value(&symbols, "test4", 150);
  symbols_set(&symbols, "test4", 100);
  check_value(&symbols, "test4", 100);

  symbols_scope_start(&symbols);
  append(&symbols, "test5", 333);
  check_value(&symbols, "test5", 333);
  append(&symbols, "test4", 444);
  check_value(&symbols, "test4", 444);
  symbols_scope_end(&symbols);

  // Check test5 out of scope
  check_value(&symbols, "test5", -1);

  // Test to make sure global test4 didn't change
  check_value(&symbols, "test4", 100);

  // Enter another scope
  symbols_scope_start(&symbols);
  check_value(&symbols, "test5", -1);
  append(&symbols, "test5", 1000);
  check_value(&symbols, "test5", 1000);
  append(&symbols, "test4", 2000);
  check_value(&symbols, "test4", 2000);
  check_export(&symbols, "test4", -1);
  symbols_scope_end(&symbols);

  // Check test5 out of scope
  check_value(&symbols, "test5", -1);

  // Test to make sure global test4 didn't change
  check_value(&symbols, "test4", 100);

  // From a global context, test4 should export (and test1)
  check_export(&symbols, "test4", 0);
  check_export(&symbols, "test1", 0);

  symbols_lock(&symbols);
  append(&symbols, "test4", 50);
  check_symbols_count(&symbols, 8);

  if (errors != 0)
  {
    symbols_print(&symbols);
  }

  symbols_free(&symbols);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}


