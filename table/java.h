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

#ifndef NAKEN_ASM_TABLE_JAVA_H
#define NAKEN_ASM_TABLE_JAVA_H

#include <stdint.h>

enum
{
  JAVA_OP_ILLEGAL,
  JAVA_OP_NONE,
  JAVA_OP_CONSTANT_INDEX8,
  JAVA_OP_CONSTANT_INDEX,
  JAVA_OP_FIELD_INDEX,
  JAVA_OP_INTERFACE_INDEX,
  JAVA_OP_METHOD_INDEX,
  JAVA_OP_CLASS_INDEX,
  JAVA_OP_SPECIAL_INDEX,
  JAVA_OP_STATIC_INDEX,
  JAVA_OP_VIRTUAL_INDEX,
  JAVA_OP_LOCAL_INDEX,
  JAVA_OP_LOCAL_INDEX_CONST,
  JAVA_OP_ARRAY_TYPE,
  JAVA_OP_CONSTANT16,
  JAVA_OP_CONSTANT8,
  JAVA_OP_OFFSET16,
  JAVA_OP_OFFSET32,
  JAVA_OP_WARN,
};

struct _table_java
{
  const char *instr;
  uint8_t length_normal;
  uint8_t length_wide;
  uint8_t op_type;
};

extern struct _table_java table_java[];

#endif

