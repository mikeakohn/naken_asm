
default:
	../../naken_asm -l -b -o mandelbrot_core.bin mandelbrot_core.asm
	../../naken_asm -l -b -o lcd_code.bin lcd_code.asm

testing:
	../../naken_asm -l -o testing.bin testing.asm

clean:
	@rm -f *.lst *.bin *.hex
	@echo "Clean!"

