include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CC) -o unit_test unit_test.c \
          ../../../build/common/eval_expression.o \
          ../../../build/common/macros.o \
          ../../../build/common/memory_pool.o \
          ../../../build/common/print_error.o \
          ../../../build/common/symbols.o \
          ../../../build/common/tokens.o \
          ../../../build/common/linker.o \
          ../../../build/common/imports_ar.o \
          ../../../build/common/imports_get_int.o \
          ../../../build/common/imports_obj.o \
	  $(CFLAGS)

clean:
	@rm -f unit_test
	@echo "Clean!"

