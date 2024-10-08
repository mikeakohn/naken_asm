include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES) -g
LD_FLAGS=-L../../../build

default:
	$(CXX) -o symbols_test symbols_test.cpp \
	  ../../../build/naken_asm.a \
	  $(CFLAGS) $(CXXFLAGS)

clean:
	@rm -f symbols_test
	@echo "Clean!"

