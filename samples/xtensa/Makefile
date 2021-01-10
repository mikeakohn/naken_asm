
default:
	../../naken_asm -l -o xtensa.hex xtensa.asm
	../../naken_asm -l -type bin -o esp32.bin -I ../../include/xtensa esp32.asm

program:
	esptool -c esp32  write_flash 0x40000 esp32.bin

clean:
	@rm -f *.hex *.lst *.bin
	@echo "Clean!"

