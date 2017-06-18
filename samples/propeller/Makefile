
default:
	../../naken_asm -I ../../include/propeller -l -b \
	   -o flip_blink_code.bin flip_blink_code.asm

program:
	openspin flip_blink.spin
	propeller-load -r -p /dev/ttyUSB0 flip_blink.binary

clean:
	@rm -f flip_blink_code.bin flip_blink_code.lst flip_blink.binary
	@echo "Clean!"

