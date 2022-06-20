
INCLUDES=-I ../../include
PROGRAMS= \
  sample_no_rsp.z64 \
  sample_rsp_hardcoded.z64 \
  sample_rsp_rectangle.z64

default: rsp.bin $(PROGRAMS)

rsp:
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o rsp.bin rsp.asm

%.z64: %.asm
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o $*.z64 $<
	./n64crc $*.z64

%.bin: %.asm
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o $*.bin $<

clean:
	@rm -f *.lst *.z64 rsp.bin
	@echo "Clean!"

