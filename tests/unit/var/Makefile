include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CC) -o var_test var_test.c \
          ../../../build/common/var.o \
	  $(CFLAGS)

clean:
	@rm -f var_test
	@echo "Clean!"

