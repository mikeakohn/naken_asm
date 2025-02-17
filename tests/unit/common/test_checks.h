
#define TEST_INT(a, b) \
  if (a != b) \
  { \
    errors += 1; \
    printf("Error: %d != %d  %s:%d\n", a, b, __FILE__, __LINE__); \
  }

#define TEST_BOOL(a, b) \
  if (a != b) \
  { \
    errors += 1; \
    printf("Error: %s != %s  %s:%d\n", \
      a != 0 ? "true" : "false", \
      b != 0 ? "true" : "false", \
      __FILE__, __LINE__); \
  }

#define TEST_TEXT(a, b) \
  if (strcmp(a,b) != 0) \
  { \
    errors += 1; \
    printf("Error: %s != %s  %s:%d\n", a, b, __FILE__, __LINE__); \
  }

#define TEST_PTR(a, b) \
  if (a != b) \
  { \
    errors += 1; \
    printf("Error: %p != %p  %s:%d\n", a, b, __FILE__, __LINE__); \
  }

