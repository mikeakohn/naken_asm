
default:
	../../naken_asm -I ../../include/propeller -l -b \
	   -o flip_blink.bin flip_blink.asm

program:
	python bin2spin.py flip_blink.bin > flip_blink.spin
	openspin flip_blink.spin
	propeller-load -r -p /dev/ttyUSB0 flip_blink.binary

clean:
	@rm -f flip_blink.bin flip_blink.spin flip_blink.lst flip_blink.binary
	@echo "Clean!"

