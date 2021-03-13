
INCLUDES=-I ../../include

default:
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o sample.z64 sample.asm
	./n64crc sample.z64

clean:
	@rm -f *.elf *.lst *.bin *.N64 *.z64
	@echo "Clean!"

