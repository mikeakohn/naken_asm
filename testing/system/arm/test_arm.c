#include <stdio.h>
#include <stdlib.h>

//int add_numbers_c(int a, int b);
int add_numbers_asm(int a, int b);
int counter(int a, int b);

int main(int argc, char *argv[])
{
  //printf("Hello from this program\n");

  //printf("%d\n", add_numbers_c(5,6));
  printf("%d\n", add_numbers_asm(7,8));
  printf("%d\n", counter(9,8));

  return 0;
}

