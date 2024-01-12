#include <stdio.h>
#include <stdlib.h>

int add_nums(int a, int b);

void nothing()
{
  printf("nothing\n");
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: ./test <num> <num>\n");
    exit(0); 
  }

  int a = atoi(argv[1]);
  int b = atoi(argv[2]);
  int c = add_nums(a, b);

  printf("%d + %d = %d\n", a, b, c);

  return 0;
}

