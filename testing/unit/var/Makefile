include ../../../config.mak

INCLUDES=-I../../../common -I../../../simulate
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	gcc -o var_test var_test.c \
          ../../../build/var.o \
	  $(CFLAGS)

clean:
	@rm -f var_test
	@echo "Clean!"

