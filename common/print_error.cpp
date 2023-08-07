/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "common/assembler.h"
#include "common/print_error.h"

void print_error(AsmContext *asm_context, const char *s)
{
  printf("Error: %s at %s:%d\n", s,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_warning(AsmContext *asm_context, const char *s)
{
  printf("Warning: %s at %s:%d\n", s,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_unexp(AsmContext *asm_context, const char *s)
{
  printf("Error: Unexpected token '%s' at %s:%d\n", *s == '\n' ? "<EOL>" : s,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_expecting(
  AsmContext *asm_context,
  const char *wanted,
  const char *got)
{
  printf("Error: Expecting '%s' but got '%s' at %s:%d\n",
    wanted,
    *got == '\n' ? "<EOL>" : got,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_unknown_instr(AsmContext *asm_context, const char *instr)
{
  printf("Error: Unknown instruction '%s' at %s:%d\n", instr,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_opcount(AsmContext *asm_context, const char *instr)
{
  printf("Error: Wrong number of operands for '%s' at %s:%d\n", instr,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_illegal_operands(AsmContext *asm_context, const char *instr)
{
  printf("Error: Illegal operands for '%s' at %s:%d\n", instr,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_illegal_expression(AsmContext *asm_context, const char *instr)
{
  printf("Error: Illegal expression for '%s' at %s:%d\n", instr,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_illegal_register(AsmContext *asm_context, const char *instr)
{
  printf("Error: Illegal register for '%s' at %s:%d\n", instr,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_range(
  AsmContext *asm_context,
  const char *s,
  int64_t r1,
  int64_t r2)
{
  printf("Error: %s out of range (%" PRId64 ",%" PRId64 ") at %s:%d\n",
    s, r1, r2,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_unknown_operand_combo(
  AsmContext *asm_context,
  const char *instr)
{
  printf("Error: Unknown operands combo for '%s' at %s:%d.\n", instr,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_internal(AsmContext *asm_context, const char *filename, int line)
{
  if (asm_context == NULL)
  {
    printf("Internal Error: At %s:%d.\n", filename, line);
  }
    else
  {
    printf("Internal Error: At %s:%d from line %s:%d.\n", filename, line,
      asm_context->tokens.filename,
      asm_context->tokens.line);
  }
}

void print_already_defined(AsmContext *asm_context, char *name)
{
  printf("Error: '%s' already defined at %s:%d.\n", name,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_not_defined(AsmContext *asm_context, char *name)
{
  printf("Error: '%s' not defined at %s:%d.\n", name,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

void print_error_align(AsmContext *asm_context, int align)
{
  printf("Error: %d byte misalignment at %s:%d.\n",
    align,
    asm_context->tokens.filename,
    asm_context->tokens.line);
}

