include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CXX) -o unit_test unit_test.cpp \
          ../../../build/naken_asm.a \
	  $(CFLAGS)

clean:
	@rm -f unit_test
	@echo "Clean!"

