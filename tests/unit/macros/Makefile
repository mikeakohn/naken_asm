include ../../../config.mak

INCLUDES=-I../../../
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CC) -o macro_test macro_test.c \
          ../../../build/naken_asm.a \
	  $(CFLAGS)

clean:
	@rm -f macro_test
	@echo "Clean!"

