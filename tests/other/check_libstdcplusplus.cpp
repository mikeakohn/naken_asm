#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/util_context.h"
#include "fileio/file.h"

// FIXME: Need a hook to sh_name: 105 (.dynstr) to make this work.

int check_executable(const char *filename)
{
  printf("check_executable(%s)\n", filename);

  UtilContext util_context;
  SymbolsIter iter;
  int file_type = FILE_TYPE_AUTO;

  util_init(&util_context);
  util_context.allow_unknown_cpu = 1;

  if (file_read(filename, &util_context, &file_type, "", 0) != 0)
  {
    printf("Error: Couldn't open file %s.\n", filename);
    return -1;
  }

  memset(&iter, 0, sizeof(iter));
  while (symbols_iterate(&util_context.symbols, &iter) != -1)
  {
    //if (iter.flag_export == false) { continue; }

    printf("symbol: %s\n", iter.name);

    if (strncmp(iter.name, "libstdc++", strlen("libstdc++")) == 0)
    {
      printf("Error: Found libstdc++ as a requirement.");
      return 1;
    }
  }

  return 0;
}

int main(int argc, char *argv[])
{
  int errors = 0;

  errors += check_executable("../../naken_asm");
  errors += check_executable("../../naken_util");

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");
    
  if (errors != 0) { return -1; }

  return 0;
}

