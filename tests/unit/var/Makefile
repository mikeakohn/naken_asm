include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CXX) -o var_test var_test.cpp \
          ../../../build/common/var.o \
	  $(CFLAGS)

clean:
	@rm -f var_test
	@echo "Clean!"

