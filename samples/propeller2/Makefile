
default:
	../../naken_asm -l -type bin -o mandelbrot_core.bin mandelbrot_core.asm
	../../naken_asm -l -type bin -o lcd_code.bin lcd_code.asm

testing:
	../../naken_asm -l testing.asm

clean:
	@rm -f *.lst *.bin *.hex
	@echo "Clean!"

