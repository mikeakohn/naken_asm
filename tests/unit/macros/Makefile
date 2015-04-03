include ../../../config.mak

INCLUDES=-I../../../common -I../../../simulate
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	gcc -o macro_test macro_test.c \
          ../../../build/macros.o \
          ../../../build/memory_pool.o \
          ../../../build/print_error.o \
          ../../../build/symbols.o \
          ../../../build/tokens.o \
	  $(CFLAGS)

clean:
	@rm -f macro_test
	@echo "Clean!"

