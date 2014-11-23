include ../../../config.mak

INCLUDES=-I../../../common -I../../../simulate
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	gcc -o unit_test unit_test.c \
          ../../../build/var.o \
	  $(CFLAGS)

clean:
	@rm -f unit_test
	@echo "Clean!"

