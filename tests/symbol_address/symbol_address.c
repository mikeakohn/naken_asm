#include <stdio.h>
#include <stdlib.h>

#include "common/assembler.h"
#include "65c816_test.h"
#include "6502_test.h"
#include "68000_test.h"
#include "epiphany_test.h"
#include "mips_test.h"
#include "msp430_test.h"
#include "stm8_test.h"

int test_symbols(const char *label, const char *code)
{
  struct _asm_context asm_context = { 0 };
  int error_flag;
  uint32_t first_address_1, first_address_2;
  uint32_t second_address_1, second_address_2;
  uint32_t third_address_1, third_address_2;
  uint32_t fourth_address_1, fourth_address_2;
  uint32_t fifth_address_1, fifth_address_2;
  int errors = 0;

  printf("---- %s ----\n", label);

  printf("%s\n", code);

  symbols_init(&asm_context.symbols);
  macros_init(&asm_context.macros);

  asm_context.pass = 1;
  assembler_init(&asm_context);
  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);
  error_flag = assemble(&asm_context);

  if (error_flag != 0)
  {
    printf("error_flag=%d\n", error_flag);
    errors++;
  }

  symbols_lookup(&asm_context.symbols, "first", &first_address_1);
  symbols_lookup(&asm_context.symbols, "second", &second_address_1);
  symbols_lookup(&asm_context.symbols, "third", &third_address_1);
  symbols_lookup(&asm_context.symbols, "fourth", &fourth_address_1);
  symbols_lookup(&asm_context.symbols, "fifth", &fifth_address_1);

  asm_context.pass = 2;
  //symbols_lock(&asm_context.symbols);
  asm_context.symbols.locked = 0;
  asm_context.symbols.debug = 1;
  symbols_scope_reset(&asm_context.symbols);
  assembler_init(&asm_context);
  error_flag = assemble(&asm_context);

  if (error_flag != 0)
  {
    printf("error_flag=%d\n", error_flag);
    errors++;
  }

  symbols_lookup(&asm_context.symbols, "first", &first_address_2);
  symbols_lookup(&asm_context.symbols, "second", &second_address_2);
  symbols_lookup(&asm_context.symbols, "third", &third_address_2);
  symbols_lookup(&asm_context.symbols, "fourth", &fourth_address_2);
  symbols_lookup(&asm_context.symbols, "fifth", &fifth_address_2);

  printf("first=%d  %d\n", first_address_1, first_address_2);
  printf("second=%d  %d\n", second_address_1, second_address_2);
  printf("third=%d  %d\n", third_address_1, third_address_2);
  printf("fourth=%d  %d\n", fourth_address_1, fourth_address_2);
  printf("fifth=%d  %d\n", fifth_address_1, fifth_address_2);

  if (first_address_1 != first_address_2)
  {
    printf("Error: first 0x%x 0x%0x doesn't match.\n",
      first_address_1, first_address_2);
    errors++;
  }

  if (second_address_1 != second_address_2)
  {
    printf("Error: second 0x%x 0x%0x doesn't match.\n",
      second_address_1, second_address_2);
    errors++;
  }

  if (third_address_1 != third_address_2)
  {
    printf("Error: third 0x%x 0x%0x doesn't match.\n",
      third_address_1, third_address_2);
    errors++;
  }

  if (fourth_address_1 != fourth_address_2)
  {
    printf("Error: fourth 0x%x 0x%0x doesn't match.\n",
      fourth_address_1, fourth_address_2);
    errors++;
  }

  if (fifth_address_1 != fifth_address_2)
  {
    printf("Error: fifth 0x%x 0x%0x doesn't match.\n",
      fifth_address_1, fifth_address_2);
    errors++;
  }

  tokens_close(&asm_context);

  if (errors != 0) { printf(">>>>> %s FAILED\n", label); }

  return errors;
}

int main(int argc, char *argv[])
{
  int errors = 0;

  errors += test_symbols("65c816", w65c816);
  errors += test_symbols("6502 1", w6502_1);
  errors += test_symbols("6502 2", w6502_2);
  errors += test_symbols("68000", mc68000);
  errors += test_symbols("Epiphany", epiphany);
  errors += test_symbols("MSP430 1", msp430_1);
  errors += test_symbols("MSP430 2", msp430_2);
  errors += test_symbols("MSP430 3", msp430_3);
  errors += test_symbols("MSP430 4", msp430_4);
  errors += test_symbols("MSP430 5", msp430_5);
  errors += test_symbols("MSP430 6", msp430_6);
  errors += test_symbols("MSP430 7", msp430_7);
  errors += test_symbols("MIPS", mips);
  errors += test_symbols("STM8", stm8);

  printf("Total errors: %d\n", errors);
  printf("%s\n", errors == 0 ? "PASSED." : "FAILED.");

  if (errors != 0) { return -1; }

  return 0;
}

