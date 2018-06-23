/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2018 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "linker.h"

int linker_add_file(struct _linker *linker, const char *filename)
{
  FILE *fp;
  int type = -1, n;

  n = strlen(filename);

  while(n >= 0)
  {
    n--;
    if (filename[n] == '.') { break; }
  }

  if (strcmp(filename + n, ".a") == 0)
  {
    type = IMPORT_TYPE_AR;
  }
    else
  if (strcmp(filename + n, ".o") == 0)
  {
    type = IMPORT_TYPE_OBJ;
  }
    else
  {
    return -1;
  }

  fp = fopen(filename, "rb");

  if (fp == NULL)
  {
    printf("Error: File not found %s\n", filename);
    return -2;
  }

  fseek(fp, 0, SEEK_END);
  n = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  struct _imports *imports = (struct _imports *)malloc(sizeof(struct _imports) + n);

  imports->next = linker->imports;
  imports->size = n;
  imports->type = type;

  if (fread(imports->code, n, 1, fp) != 1)
  {
    printf("Error: Couldn't read file %s\n", filename);
    free(imports);
    fclose(fp);
    return -2;
  }

  if (imports_verify(imports) != 0)
  {
    printf("Error: Not a supported file %s\n", filename);
    return -2;
  }

  linker->imports = imports;

  fclose(fp);

  return 0;
}

void linker_free(struct _linker *linker)
{
  if (linker == NULL) { return; }

  struct _imports *imports = linker->imports;

  while(imports != NULL)
  {
    struct _imports *curr = imports;
    imports = imports->next;
    free(curr);
  }
}

