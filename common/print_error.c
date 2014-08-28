/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "assembler.h"
#include "print_error.h"

void print_error(const char *s, struct _asm_context *asm_context)
{
  printf("Error: %s at %s:%d\n", s, asm_context->filename, asm_context->line);
}

void print_error_unexp(const char *s, struct _asm_context *asm_context)
{
  printf("Error: Unexpected token '%s' at %s:%d\n", s,
    asm_context->filename,
    asm_context->line);
}

void print_error_unknown_instr(const char *instr, struct _asm_context *asm_context)
{
  printf("Error: Unknown instruction '%s' at %s:%d\n", instr,
    asm_context->filename,
    asm_context->line);
}

void print_error_opcount(const char *instr, struct _asm_context *asm_context)
{
  printf("Error: Wrong number of operands for '%s' at %s:%d\n", instr,
    asm_context->filename,
    asm_context->line);
}

void print_error_illegal_operands(const char *instr, struct _asm_context *asm_context)
{
  printf("Error: Illegal operands for '%s' at %s:%d\n", instr,
    asm_context->filename,
    asm_context->line);
}

void print_error_illegal_expression(const char *instr, struct _asm_context *asm_context)
{
  printf("Error: Illegal expression for '%s' at %s:%d\n", instr,
    asm_context->filename,
    asm_context->line);
}

void print_error_range(const char *s, int r1, int r2, struct _asm_context *asm_context)
{
  printf("Error: %s out of range (%d,%d) at %s:%d\n", s, r1, r2,
    asm_context->filename,
    asm_context->line);
}

void print_error_unknown_operand_combo(const char *instr, struct _asm_context *asm_context)
{
  printf("Error: Unknown operands combo for '%s' at %s:%d.\n", instr,
    asm_context->filename,
    asm_context->line);
}

void print_error_internal(struct _asm_context *asm_context, const char *filename, int line)
{
  if (asm_context == NULL)
  {
    printf("Internal Error: At %s:%d.\n", filename, line);
  }
    else
  {
    printf("Internal Error: At %s:%d from line %s:%d.\n", filename, line,
      asm_context->filename,
      asm_context->line);
  }
}

void print_already_defined(struct _asm_context *asm_context, char *name)
{
  printf("Error: '%s' already defined at %s:%d.\n", name,
    asm_context->filename,
    asm_context->line);
}

void print_not_defined(struct _asm_context *asm_context, char *name)
{
  printf("Error: '%s' not defined at %s:%d.\n", name,
    asm_context->filename,
    asm_context->line);
}



