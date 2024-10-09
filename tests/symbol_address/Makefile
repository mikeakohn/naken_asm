include ../../config.mak

INCLUDES=-I../../
BUILDDIR=../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../build

default:
	$(CXX) -o symbol_address symbol_address.cpp \
	  ../../build/naken_asm.a \
	  $(CFLAGS) $(CXXFLAGS)

clean:
	@rm -f symbol_address
	@echo "Clean!"

