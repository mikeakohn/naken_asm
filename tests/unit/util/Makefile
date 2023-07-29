include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CC) -o util_test util_test.c ../../../build/naken_asm.a \
	  $(CFLAGS)

clean:
	@rm -f util_test
	@echo "Clean!"

