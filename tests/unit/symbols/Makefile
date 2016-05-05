include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES) -g
LD_FLAGS=-L../../../build

default:
	$(CC) -o symbols_test symbols_test.c \
	  ../../../common/symbols.c \
	  ../../../common/memory_pool.c \
	  $(CFLAGS)

clean:
	@rm -f symbols_test
	@echo "Clean!"

