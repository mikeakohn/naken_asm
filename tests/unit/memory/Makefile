include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	gcc -o memory_test memory_test.c \
	  ../../../build/asm/*.o \
	  ../../../build/common/assembler.o \
	  ../../../build/common/cpu_list.o \
	  ../../../build/common/directives_data.o \
	  ../../../build/common/directives_if.o \
	  ../../../build/common/directives_include.o \
	  ../../../build/common/eval_expression.o \
	  ../../../build/common/eval_expression_ex.o \
	  ../../../build/common/ifdef_expression.o \
	  ../../../build/common/macros.o \
	  ../../../build/common/memory.o \
	  ../../../build/common/memory_pool.o \
	  ../../../build/common/print_error.o \
	  ../../../build/common/symbols.o \
	  ../../../build/common/tokens.o \
	  ../../../build/common/var.o \
	  ../../../build/disasm/*.o \
	  ../../../build/simulate/*.o \
	  ../../../build/table/*.o \
	  $(CFLAGS)

clean:
	@rm -f memory_test
	@echo "Clean!"

