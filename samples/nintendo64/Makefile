
INCLUDES=-I ../../include
PROGRAMS= \
  sample_no_rsp.z64

default: $(PROGRAMS)
	./n64crc sample_no_rsp.z64

rsp:
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o rsp.bin rsp.asm

%.z64: %.asm
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o $*.z64 $<

clean:
	@rm -f *.lst *.z64 rsp.bin
	@echo "Clean!"

