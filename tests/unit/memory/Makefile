include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CC) -o memory_test memory_test.c ../../../build/naken_asm.a \
	  $(CFLAGS)

clean:
	@rm -f memory_test
	@echo "Clean!"

