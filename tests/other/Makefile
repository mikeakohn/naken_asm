include ../../config.mak

INCLUDES=-I../../
BUILDDIR=../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../build

default:
	$(CXX) -o n64_rsp_illegal_instr n64_rsp_illegal_instr.cpp \
	  ../../build/naken_asm.a \
  	  $(CFLAGS)
	./n64_rsp_illegal_instr
	bash check_libstdcplusplus.sh

check_libstdcplusplus:
	$(CXX) -o check_libstdcplusplus check_libstdcplusplus.cpp \
	  ../../build/naken_asm.a \
  	  $(CFLAGS)
	./check_libstdcplusplus

clean:
	@rm -f n64_rsp_illegal_instr
	@rm -f check_libstdcplusplus
	@echo "Clean!"

