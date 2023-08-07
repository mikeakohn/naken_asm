include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CXX) -o data_test data_test.cpp \
	  ../../../build/naken_asm.a \
	  $(CFLAGS)

clean:
	@rm -f data_test
	@echo "Clean!"

