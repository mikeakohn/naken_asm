#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"

int test_instruction(const char *instruction)
{
  struct _asm_context asm_context;
  char code[128];

  sprintf(code, ".n64_rsp\n%s\n", instruction);

  memset(&asm_context, 0, sizeof(asm_context));

  printf("N64 RSP: %s ... ", instruction);

  symbols_init(&asm_context.symbols);
  macros_init(&asm_context.macros);

  asm_context.pass = 2;
  assembler_init(&asm_context);
  tokens_open_buffer(&asm_context, code);
  tokens_reset(&asm_context);

  int error_flag = assemble(&asm_context);

  if (error_flag != 0)
  {
    printf(" PASS!\n");
    return 0;
  }
    else
  {
    printf(" found (error)\n");
    return 1;
  }
}

int main(int argc, char *argv[])
{
  int errors = 0;

  const char *instructions[] =
  {
    "ldl $t1, 10($a1)",
    "ldr $t1, 10($a1)",
    "lwl $t3, 0x1234($t4)",
    "lwr $t3, 0x1234($t4)",
    "lwu $t3, 0x1234($t4)",
    "swl $t3, 0x1234($t4)",
    "sdl $t3, 0x1234($t4)",
    "sdr $t3, 0x1234($t4)",
    "swr $t3, 0x1234($t4)",
    "ll $t3, 0x1234($t4)",
    "lld $t3, 0x1234($t4)",
    "ldc1 $t3, 0x1234($t4)",
    "ldc2 $t3, 0x1234($t4)",
    "ld $t3, 0x1234($t4)",
    "sdc1 $t3, 0x1234($t4)",
    "sdc2 $t3, 0x1234($t4)",
    "sd $t3, 0x1234($t4)",
    "sc $t3, 0x1234($t4)",
    "scd $t3, 0x1234($t4)",
    "main: beql $t6, $t7, main",
    "main: bnel $t6, $t7, main",
    "main: bnezl $t6, $t7, main",
    "main: bgtzl $t6, $t7, main",
    "main: bltzl $t6, $t7, main",
    "main: bgezl $t6, $t7, main",
    "main: bltzall $t6, $t7, main",
    "main: bgtzall $t6, $t7, main",
    "main: bgezall $t6, $t7, main",
    "mfhi $t3",
    "mthi $t3",
    "mflo $t3",
    "mtlo $t3",
    "daddi $k0, $s0, 0x1234",
    "daddiu $k0, $s0, 0x1234",
    "dsllv $t5, $t6, $t7",
    "dsrlv $t5, $t6, $t7",
    "dsrav $t5, $t6, $t7",
    "dmult $t5, $t6, $t7",
    "dmultu $t5, $t6, $t7",
    "ddiv $t5, $t6, $t7",
    "ddivu $t5, $t6, $t7",
    "dadd $t5, $t6, $t7",
    "daddu $t5, $t6, $t7",
    "dsub $t5, $t6, $t7",
    "dsubu $t5, $t6, $t7",
    "dsll $t5, $t6, $t7",
    "dsrl $t5, $t6, $t7",
    "dsra $t5, $t6, $t7",
    "dsll32 $t5, $t6, $t7",
    "dsrl32 $t5, $t6, $t7",
    "dsra32 $t5, $t6, $t7",
    "mult $t5, $t6, $t7",
    "multu $t5, $t6, $t7",
    "div $t5, $t6, $t7",
    "divu $t5, $t6, $t7",
    "syscall",
    "sync",
    "main: bc0f main",
    "main: bc1f main",
    "main: bc2f main",
    "main: bc0fl main",
    "main: bc1fl main",
    "main: bc2fl main",
    "main: bc0t main",
    "main: bc1t main",
    "main: bc2t main",
    "main: bc0tl main",
    "main: bc1tl main",
    "main: bc2tl main",
    "tge $t3, $t4",
    "tgeu $t3, $t4",
    "tlt $t3, $t4",
    "tltu $t3, $t4",
    "teq $t3, $t4",
    "tne $t3, $t4",
    "tgei $t3, 4",
    "tgeiu $t3, 4",
    "tlti $t3, 4",
    "tltiu $t3, 4",
    "teqi $t3, 4",
    "tnei $t3, 4",
  };

  int n;

  for (n = 0; n < sizeof(instructions) / sizeof(const char *); n++)
  {
    errors += test_instruction(instructions[n]);
  }

  printf("Errors: %d\n", errors);

  if (errors != 0)
  {
    printf("Failed\n");
    exit(-1);
  }

  return 0;
}

