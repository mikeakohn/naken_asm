include ../../config.mak
  
INCLUDES=-I../../
BUILDDIR=../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
LD_FLAGS=-L../../build

default:
	$(CC) -o n64_rsp_illegal_instr n64_rsp_illegal_instr.c \
	../../build/naken_asm.a \
  	$(CFLAGS)
clean:
	@rm -f n64_rsp_illegal_instr
	@echo "Clean!"

