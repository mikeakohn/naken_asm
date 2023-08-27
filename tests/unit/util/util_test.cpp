#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/UtilContext.h"

#define TEST_RANGES(start_expected, end_expected, ret_expected) \
  if (start != start_expected || end != end_expected) \
  { \
    printf("Error: %s:%d test_ranges() expected %d-%d, got %d-%d\n", \
       __FILE__, __LINE__, \
       start_expected, end_expected, \
       start, end); \
    errors++; \
  } \
  if (ret != ret_expected) \
  { \
    printf("Error: %s:%d test_ranges() return value expected %d, got %d\n", \
       __FILE__, __LINE__, \
      ret_expected, ret); \
  }

static int test_ranges(UtilContext *util_context)
{
  uint32_t start = -1;
  uint32_t end = -1;
  int errors = 0;
  int ret;

  ret = util_get_range(util_context, "asdf", &start, &end);
  TEST_RANGES(0, 0, -1);

  ret = util_get_range(util_context, "1000", &start, &end);
  TEST_RANGES(1000, 1000, 0);

  ret = util_get_range(util_context, "1000-1024", &start, &end);
  TEST_RANGES(1000, 1024, 0);

  ret = util_get_range(util_context, "  1000 - 1024   ", &start, &end);
  TEST_RANGES(1000, 1024, 0);

  ret = util_get_range(util_context, "-1024", &start, &end);
  TEST_RANGES(0, 1024, 0);

  util_context->symbols.append("blah", 64);
  util_context->symbols.append("asdf", 100);

  ret = util_get_range(util_context, "blah-asdf", &start, &end);
  TEST_RANGES(64, 100, 0);

  ret = util_get_range(util_context, "blah-asdf blah", &start, &end);
  if (ret != -1)
  {
    printf("Error: %s:%d test_ranges() return value expected -1, got %d\n",
       __FILE__, __LINE__, ret);
  }

  return errors;
}

int main(int argc, char *argv[])
{
  int errors = 0;
  UtilContext util_context;

  util_init(&util_context);

  errors += test_ranges(&util_context);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}

