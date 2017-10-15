
#DUMP_MACROS=-dump_macros
INCLUDES=-I../../include
FLAGS=-l -e $(INCLUDES) $(DUMP_MACROS)

default:
	../../naken_asm $(FLAGS) -o mandelbrot_altivec.o mandelbrot_altivec.asm
	$(CC) -o mandelbrot mandelbrot.c mandelbrot_altivec.o \
	   -O3 -Wall -g

mandel:
	$(CC) -o mandelbrot mandelbrot.c mandelbrot_altivec.o \
	   -O3 -Wall -g

test:
	../../naken_asm $(FLAGS) -o test_function.o test_function.asm

clean:
	@rm -f *.o *.lst mandelbrot
	@echo "Clean!"

