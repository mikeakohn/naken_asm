#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/var.h"

#define PRINT_VAR(var) \
  printf("int32=%d/%x int64=%" PRId64 "/%" PRIx64 " float=%f double=%f   (%d)\n", \
    var_get_int32(&var), \
    var_get_int32(&var), \
    var_get_int64(&var), \
    var_get_int64(&var), \
    var_get_float(&var), \
    var_get_double(&var), \
    var_get_type(&var));
#define ERROR(var) printf("FAIL: %s:%d\n", __FILE__, __LINE__); errors++; PRINT_VAR(var)

#define PASS() { printf("PASS: %s:%d\n", __FILE__, __LINE__); }

#define CHECK(var,a,b) \
  if (var_get_int32(&var) != a) { ERROR(var); } \
  if (var_get_int64(&var) != a) { ERROR(var); } \
  if (var_get_float(&var) != (float)b) { ERROR(var); } \
  if (var_get_double(&var) != (double)b) { ERROR(var); }

#define CHECK_INT(var,a,b) \
  if (var_get_int32(&var) != a) { ERROR(var); } \
  if (var_get_int64(&var) != b) { ERROR(var); }

#define TEST_OP(op,type,a,b,c,d) \
  var_set_##type(&var1, a); \
  var_set_##type(&var2, b); \
  var_##op(&var1, &var2); \
  CHECK(var1, c, d);

#define TEST_OP_INT(op,type,a,b,c,d) \
  var_set_##type(&var1, a); \
  var_set_##type(&var2, b); \
  var_##op(&var1, &var2); \
  CHECK_INT(var1, c, d);
 
int errors = 0;

int main(int argc, char *argv[])
{
  struct _var var1,var2;

  printf("Testing var.h\n");

  var_set_int(&var1, 1000);
  CHECK(var1, 1000, 1000);

  var_set_float(&var1, 99.32);
  CHECK(var1, 99, 99.32);

  var_set_int(&var1, 0x0122223333);
  CHECK_INT(var1, 0x22223333, 0x122223333);

  var_set_int(&var1, 0xffffffffffffffff);
  CHECK_INT(var1, 0xffffffff, 0xffffffffffffffff);

  var_set_int(&var1, 0x0fffffffffffffff);
  CHECK_INT(var1, 0xffffffff, 0x0fffffffffffffff);

  TEST_OP(add, int, -10, 20, 10, 10);
  TEST_OP(add, int, 10, 20, 30, 30);
  TEST_OP(sub, int, -10, 20, -30, -30);
  TEST_OP(sub, int, 10, 20, -10, -10);
  TEST_OP(mul, int, 10, 20, 200, 200);
  TEST_OP(div, int, 10, 20, 0, 0);
  TEST_OP(div, int, 20, -10, -2, -2);
  TEST_OP(add, float, -10.1, 20, 9, 9.9);
  TEST_OP(add, float, -10, 20.2, 10, 10.2);
  TEST_OP(sub, float, -10, 20.2, -30, -30.2);
  TEST_OP(mul, float, -10, 20.2, -202, -202);
  TEST_OP(mul, float, 8, 20.2, 161, 161.60);
  TEST_OP(div, float, 2.56, 8, 0, 0.32);
  TEST_OP(div, float, 32.8, 4, 8, 8.2);

  TEST_OP_INT(add, int, 0xffffffff, 1, 0, 0x100000000);

  if (errors != 0) { printf("var.h ... FAILED.\n"); return -1; }

  printf("var.h ... PASSED.\n");

  return 0;
}

