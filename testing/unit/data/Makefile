include ../../../config.mak

INCLUDES=-I../../../common -I../../../simulate
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	gcc -o data_test data_test.c \
	  ../../../build/*.o \
	  $(CFLAGS)

clean:
	@rm -f data_test
	@echo "Clean!"

