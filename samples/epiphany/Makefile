INCLUDES=-I${EPIPHANY_HOME}/tools/host/include
LIBS=-L${EPIPHANY_HOME}/tools/host/lib
CFLAGS=-Wall -O3 ${INCLUDES}
LDFLAGS=${LIBS} -le-hal -le-loader

default:
	../../naken_asm -l -s -o epiphany.srec epiphany.asm
	gcc -o test_parallella test_parallella.c \
	   ${CFLAGS} \
	   ${LDFLAGS}

mandelbrot:
	../../naken_asm -l -s -o mandelbrot.srec mandelbrot.asm

clean:
	@rm -f *.lst *.hex test_parallella *.elf *.srec
	@echo "Clean!"

