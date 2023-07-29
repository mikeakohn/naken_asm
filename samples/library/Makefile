
default: sample.hex sample.elf
	gcc -o test_lib test_lib.c -I../../library -Wall -g -lnaken_asm -L../..

sample.hex:
	../../naken_asm -o sample.hex sample.asm

sample.elf:
	../../naken_asm -o sample.elf -type elf sample.asm

clean:
	@rm -f sample.hex
	@echo "Clean!"

