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

#ifndef NAKEN_ASM_PRINT_ERROR_H
#define NAKEN_ASM_PRINT_ERROR_H

void print_error(AsmContext *asm_context, const char *s);
void print_warning(AsmContext *asm_context, const char *s);
void print_error_unexp(AsmContext *asm_context, const char *s);

void print_error_expecting(
  AsmContext *asm_context,
  const char *wanted,
  const char *got);

void print_error_unknown_instr(AsmContext *asm_context, const char *instr);
void print_error_opcount(AsmContext *asm_context, const char *instr);
void print_error_illegal_operands(AsmContext *asm_context, const char *instr);
void print_error_illegal_expression(AsmContext *asm_context, const char *instr);
void print_error_illegal_register(AsmContext *asm_context, const char *instr);

void print_error_range(
  AsmContext *asm_context,
  const char *s,
  int64_t r1,
  int64_t r2);

void print_error_unknown_operand_combo(
  AsmContext *asm_context,
  const char *instr);

void print_error_internal(
  AsmContext *asm_context,
  const char *filename,
  int line);

void print_already_defined(AsmContext *asm_context, char *name);
void print_not_defined(AsmContext *asm_context, char *name);
void print_error_align(AsmContext *asm_context, int align);

#endif

