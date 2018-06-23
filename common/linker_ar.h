
#ifndef PARSE_AR_H
#define PARSE_AR_H

int linker_ar_verify(uint8_t *buffer, int file_size);

int linker_ar_read(uint8_t *buffer, int file_size);

uint32_t linker_ar_find_code_from_symbol(
  uint8_t *buffer,
  int file_size,
  const char *symbol,
  uint32_t *function_size,
  uint32_t *file_offset);

const char *linker_ar_find_name_from_offset(
  uint8_t *buffer,
  int file_size,
  uint32_t offset);

#endif

