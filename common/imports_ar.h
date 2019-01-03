/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_IMPORTS_AR_H
#define NAKEN_ASM_IMPORTS_AR_H

int imports_ar_verify(uint8_t *buffer, int file_size);

int imports_ar_read(uint8_t *buffer, int file_size);

int imports_ar_find_code_from_symbol(
  uint8_t *buffer,
  int file_size,
  const char *symbol,
  uint32_t *function_offset,
  uint32_t *function_size,
  uint32_t *file_offset,
  uint8_t **obj_file,
  uint32_t *obj_size);

const char *imports_ar_find_name_from_offset(
  uint8_t *buffer,
  int file_size,
  uint32_t offset);

#endif

