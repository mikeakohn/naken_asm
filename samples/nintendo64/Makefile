
INCLUDES=-I ../../include

default:
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o sample.N64 sample.asm
	./n64crc sample.N64

clean:
	@rm -f *.elf *.lst *.bin *.N64
	@echo "Clean!"

