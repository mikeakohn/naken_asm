include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CXX) -o memory_test memory_test.cpp ../../../build/naken_asm.a \
	  $(CFLAGS) $(CXXFLAGS)

clean:
	@rm -f memory_test
	@echo "Clean!"

