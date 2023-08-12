#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/Var.h"

#define PRINT_VAR(var) \
  printf("int32=%d/%x int64=%" PRId64 "/%" PRIx64 " float=%f double=%f   (%d)\n", \
    var.get_int32(), \
    var.get_int32(), \
    var.get_int64(), \
    var.get_int64(), \
    var.get_float(), \
    var.get_double(), \
    var.get_type());

#define ERROR(var) printf("FAIL: %s:%d\n", __FILE__, __LINE__); \
  errors++; \
  PRINT_VAR(var)

#define PASS() { printf("PASS: %s:%d\n", __FILE__, __LINE__); }

#define CHECK(var, a, b) \
  if (var.get_int32() != a) { ERROR(var); } \
  if (var.get_int64() != a) { ERROR(var); } \
  if (var.get_float() != (float)b) { ERROR(var); } \
  if (var.get_double() != (double)b) { ERROR(var); }

#define CHECK_INT(var, a, b) \
  if (var.get_int32() != (int32_t)a) { ERROR(var); } \
  if (var.get_int64() != (int64_t)b) { ERROR(var); } \
  if (var.get_type() != VAR_INT) { ERROR(var); }

#define TEST_OP(op, type, a, b, c, d) \
  var1.set_##type(a); \
  var2.set_##type(b); \
  var1.op(var1, var2); \
  CHECK(var1, c, d);

#define TEST_OP_INT(op, type, a, b, c, d) \
  var1.set_##type(a); \
  var2.set_##type(b); \
  var1.op(var1, var2); \
  CHECK_INT(var1, c, d);

int errors = 0;

int main(int argc, char *argv[])
{
  Var var1, var2;

  printf("Testing var.h\n");

  var1.set_int(1000);
  CHECK(var1, 1000, 1000);

  var1.set_float(99.32);
  CHECK(var1, 99, 99.32);

  var1.set_int(0x0122223333);
  CHECK_INT(var1, 0x22223333, 0x122223333);

  var1.set_int(0xffffffffffffffff);
  CHECK_INT(var1, 0xffffffff, 0xffffffffffffffff);

  var1.set_int(0x0fffffffffffffff);
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
  TEST_OP(shift_left,  int, 32, 1, 64, 64);
  TEST_OP(shift_right, int, 32, 1, 16, 16);
  TEST_OP(shift_left,  int, -4, 1, -8, -8);
  TEST_OP(shift_right, int, -4, 1, -2, -2);
  TEST_OP(shift_right_unsigned, int, 32, 1, 16, 16);
  //TEST_OP(shift_right_unsigned, int, -4, 1, 0x7ffffffffffffffe, 0x7ffffffffffffffe);

  TEST_OP_INT(add, int, 0xffffffff, 1, 0, 0x100000000);

  var1.set_int(1000);
  var2.set_int(2000);

  var2 = var1;
  CHECK_INT(var1, 1000, 1000);
  CHECK_INT(var2, 1000, 1000);

  if (errors != 0) { printf("var.h ... FAILED.\n"); return -1; }

  printf("var.h ... PASSED.\n");

  return 0;
}

