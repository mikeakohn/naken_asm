include ../../../config.mak

INCLUDES=-I../../../common -I../../../simulate
BUILDDIR=../../../build
CFLAGS=-Wall -g -DUNIT_TEST $(INCLUDES)
OBJS= \
  $(foreach var,$(COMMON_OBJS),$(BUILDDIR)/$(var)) \
  $(foreach var,$(DISASM_OBJS),$(BUILDDIR)/$(var)) \
  $(foreach var,$(TABLE_OBJS),$(BUILDDIR)/$(var)) \
  $(foreach var,$(ASM_OBJS),$(BUILDDIR)/$(var))

default:
	gcc -o unit_test unit_test.c $(OBJS) $(CFLAGS)

clean:
	@rm -f unit_test
	@echo "Clean!"

