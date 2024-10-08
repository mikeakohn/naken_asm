include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -O3 -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CXX) -o var_test var_test.cpp \
          ../../../build/naken_asm.a \
	  $(CFLAGS) $(CXXFLAGS)
	$(CXX) -o string_test string_test.cpp \
	  ../../../build/naken_asm.a \
	  $(CFLAGS) $(CXXFLAGS)

run:
	./var_test
	./string_test

clean:
	@rm -f var_test string_test
	@echo "Clean!"

