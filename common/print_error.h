/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _PRINT_ERROR_H
#define _PRINT_ERROR_H

void print_error(const char *s, struct _asm_context *asm_context);
void print_error_unexp(const char *s, struct _asm_context *asm_context);
void print_error_unknown_instr(const char *instr, struct _asm_context *asm_context);
void print_error_opcount(const char *instr, struct _asm_context *asm_context);
void print_error_illegal_operands(const char *instr, struct _asm_context *asm_context);
void print_error_illegal_expression(const char *instr, struct _asm_context *asm_context);
void print_error_range(const char *s, int r1, int r2, struct _asm_context *asm_context);
void print_error_unknown_operand_combo(const char *instr, struct _asm_context *asm_context);
void print_error_internal(struct _asm_context *asm_context, const char *filename, int line);

#endif

