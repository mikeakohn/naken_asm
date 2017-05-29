INCLUDES=-I${EPIPHANY_HOME}/tools/host/include
LIBS=-L${EPIPHANY_HOME}/tools/host/lib
CFLAGS=-Wall -O3 ${INCLUDES} -g
LDFLAGS=${LIBS} -le-hal -le-loader

mandel:
	../../naken_asm -I ../../include/epiphany -l -s \
	   -o mandelbrot.srec mandelbrot.asm
	gcc -o test_mandelbrot test_mandelbrot.c \
	   ${CFLAGS} \
	   ${LDFLAGS}
	../../naken_asm -l -s -o epiphany.srec epiphany.asm
	gcc -o test_parallella test_parallella.c \
	   ${CFLAGS} \
	   ${LDFLAGS}

clean:
	@rm -f *.lst *.hex test_parallella *.elf *.srec test_mandelbrot
	@echo "Clean!"

