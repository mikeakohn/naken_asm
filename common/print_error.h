/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_PRINT_ERROR_H
#define NAKEN_ASM_PRINT_ERROR_H

void print_error(const char *s, struct _asm_context *asm_context);
void print_warning(const char *s, struct _asm_context *asm_context);
void print_error_unexp(const char *s, struct _asm_context *asm_context);
void print_error_expecting(const char *wanted, const char *got, struct _asm_context *asm_context);
void print_error_unknown_instr(const char *instr, struct _asm_context *asm_context);
void print_error_opcount(const char *instr, struct _asm_context *asm_context);
void print_error_illegal_operands(const char *instr, struct _asm_context *asm_context);
void print_error_illegal_expression(const char *instr, struct _asm_context *asm_context);
void print_error_illegal_register(const char *instr, struct _asm_context *asm_context);
void print_error_range(const char *s, int64_t r1, int64_t r2, struct _asm_context *asm_context);
void print_error_unknown_operand_combo(const char *instr, struct _asm_context *asm_context);
void print_error_internal(struct _asm_context *asm_context, const char *filename, int line);
void print_already_defined(struct _asm_context *asm_context, char *name);
void print_not_defined(struct _asm_context *asm_context, char *name);
void print_error_align(struct _asm_context *asm_context, int align);

#endif

