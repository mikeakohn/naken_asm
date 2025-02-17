include ../../../config.mak

INCLUDES=-I../../..
BUILDDIR=../../../build
CFLAGS=-Wall -g -O3 -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../../build

default:
	$(CXX) -o memory_pool_fixed_test memory_pool_fixed_test.cpp \
	  ../../../build/naken_asm.a \
	  $(CFLAGS)
	$(CXX) -o string_test string_test.cpp \
	  ../../../build/naken_asm.a \
	  $(CFLAGS)
	$(CXX) -o string_heap_test string_heap_test.cpp \
	  ../../../build/naken_asm.a \
	  $(CFLAGS)
	$(CXX) -o var_test var_test.cpp \
          ../../../build/naken_asm.a \
	  $(CFLAGS)
	$(CXX) -o vector_test vector_test.cpp \
	  ../../../build/naken_asm.a \
	  $(CFLAGS)

run:
	./memory_pool_fixed_test
	./string_test
	./string_heap_test
	./var_test
	./vector_test

clean:
	@rm -f memory_pool_fixed_test string_test string_heap_test
	@rm -f var_test vector_test
	@echo "Clean!"

