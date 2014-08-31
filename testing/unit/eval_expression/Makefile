include ../../../config.mak

INCLUDES=-I../../../common -I../../../simulate
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	gcc -o unit_test unit_test.c \
          ../../../build/eval_expression.o \
          ../../../build/macros.o \
          ../../../build/memory_pool.o \
          ../../../build/print_error.o \
          ../../../build/symbols.o \
          ../../../build/tokens.o \
	  $(CFLAGS)

clean:
	@rm -f unit_test
	@echo "Clean!"

