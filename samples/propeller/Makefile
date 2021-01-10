
default:
	../../naken_asm -I ../../include/propeller -l -type bin \
	   -o flip_blink_code.bin flip_blink_code.asm

program:
	openspin flip_blink.spin
	propeller-load -r -p /dev/ttyUSB0 flip_blink.binary

mandel:
	../../naken_asm -I ../../include/propeller -l -type bin \
	   -o mandelbrot_code.bin mandelbrot_code.asm
	openspin mandelbrot.spin
	propeller-load -r -p /dev/ttyUSB0 mandelbrot.binary

multi:
	../../naken_asm -I ../../include/propeller -l -type bin \
	   -o lcd_code.bin lcd_code.asm
	../../naken_asm -I ../../include/propeller -l -type bin \
	   -o mandelbrot_code6.bin mandelbrot_code6.asm
	openspin mandelbrot_multi.spin
	propeller-load -r -p /dev/ttyUSB0 mandelbrot_multi.binary

clean:
	@rm -f flip_blink_code.bin flip_blink_code.lst flip_blink.binary
	@rm -f mandelbrot_code.bin mandelbrot_code.lst mandelbrot.binary
	@echo "Clean!"

