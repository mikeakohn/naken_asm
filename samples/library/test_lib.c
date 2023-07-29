#include <stdio.h>
#include <stdlib.h>

#include "naken_asm.h"

int main(int argc, char *argv[])
{
  void *context;

  context = naken_util_create();
  naken_util_set_cpu_type(context, "msp430");

  if (naken_util_open(context, "sample.hex") == -1)
  {
    printf("Couldn't open file.\n");
    naken_util_destroy(context);
    return -1;
  }

  naken_util_disasm(context, "0-20");
  naken_util_destroy(context);

  return 0;
}

