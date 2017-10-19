
#DUMP_MACROS=-dump_macros
INCLUDES=-I../../include
FLAGS=-l -e $(INCLUDES) $(DUMP_MACROS)

default:
	../../naken_asm $(FLAGS) -o mandelbrot_altivec.o mandelbrot_altivec.asm
	$(CC) -o mandelbrot mandelbrot.c mandelbrot_altivec.o -lspe2 \
	   -O3 -Wall -g

mandel:
	../../naken_asm $(FLAGS) -elf -o mandelbrot_spe.elf mandelbrot_spe.asm
	chmod 755 mandelbrot_spe.elf

cell:
	../../naken_asm $(FLAGS) -elf -o spe_test.elf spe_test.asm
	ppu-gcc -o host_to_cell host_to_cell.c -lspe2
	chmod 755 spe_test.elf

clean:
	@rm -f *.o *.lst mandelbrot host_to_cell
	@echo "Clean!"

