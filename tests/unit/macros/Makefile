include ../../../config.mak

INCLUDES=-I../../../
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CXX) -o macro_test macro_test.cpp \
          ../../../build/naken_asm.a \
	  $(CFLAGS) $(CXXFLAGS)

clean:
	@rm -f macro_test
	@echo "Clean!"

