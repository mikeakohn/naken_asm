#include "table_powerpc.h"

struct _table_powerpc table_powerpc[] =
{
  { "blcr", 0x4c000000, 0xfc00f801, OP_BRANCH, 0, 0 },
  { "blcrl", 0x4c000001, 0xfc00f801, OP_BRANCH, 0, 0 },
  { NULL, 0, 0, 0, 0 }
};

