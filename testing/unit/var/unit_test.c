#include <stdio.h>
#include <stdlib.h>

#include "var.h"

#define PRINT_VAR(var) \
  printf("int32=%d int64=%ld float=%f double=%f   (%d)\n", \
    var_get_int32(&var), \
    var_get_int64(&var), \
    var_get_float(&var), \
    var_get_double(&var), \
    var_get_type(&var));
#define ERROR(var) printf("FAIL: %s:%d\n", __FILE__, __LINE__); errors++; PRINT_VAR(var)

#define PASS() { printf("PASS: %s:%d\n", __FILE__, __LINE__); }

#define CHECK(var,value) \
  if (var_get_int32(&var) != value) { ERROR(var); } else { printf("PASS\n"); } \
  if (var_get_int64(&var) != value) { ERROR(var); }  else { printf("PASS\n"); }\
  if (var_get_float(&var) != (float)value) { ERROR(var); }  else { printf("PASS\n"); }\
  if (var_get_double(&var) != value) { ERROR(var); }  else { printf("PASS\n"); }\
 
int errors = 0;

int main(int argc, char *argv[])
{
  struct _var var1,var2;

  printf("Testing var.h\n");

  var_set_int(&var1, 1000);
  if (var_get_int32(&var1) != 1000) { ERROR(var1); }
  if (var_get_int64(&var1) != 1000) { ERROR(var1); }
  if (var_get_float(&var1) != 1000) { ERROR(var1); }
  if (var_get_double(&var1) != 1000) { ERROR(var1); }

  var_set_float(&var1, 99.32);
  if (var_get_int32(&var1) != 99) { ERROR(var1); }
  if (var_get_int64(&var1) != 99) { ERROR(var1); }
  if (var_get_float(&var1) != (float)99.32) { ERROR(var1); }
  if (var_get_double(&var1) != 99.32) { ERROR(var1); }

  var_set_int(&var1, -10);
  var_set_int(&var2, 20);
  var_add(&var1, &var2);
  if (var_get_int32(&var1) != 10) { ERROR(var1); }
  if (var_get_double(&var1) != 10) { ERROR(var1); }

  var_set_int(&var1, -10);
  var_set_int(&var2, 20);
  var_sub(&var1, &var2);
  if (var_get_int32(&var1) != -30) { ERROR(var1); }
  if (var_get_double(&var1) != -30) { ERROR(var1); }

  if (errors != 0) { printf("var.h ... FAILED.\n"); return -1; }

  printf("var.h ... PASSED.\n");

  return 0;
}

