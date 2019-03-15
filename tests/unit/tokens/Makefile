include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CC) -o tokens_test tokens_test.c ../../../build/naken_asm.a \
	  $(CFLAGS)

clean:
	@rm -f tokens_test
	@echo "Clean!"

