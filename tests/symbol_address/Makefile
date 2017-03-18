include ../../config.mak

INCLUDES=-I../../
BUILDDIR=../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../build

default:
	$(CC) -o symbol_address symbol_address.c \
          ../../build/asm/*.o \
          ../../build/common/*.o \
          ../../build/disasm/*.o \
          ../../build/simulate/*.o \
          ../../build/table/*.o \
	  $(CFLAGS)

clean:
	@rm -f symbol_address
	@echo "Clean!"

