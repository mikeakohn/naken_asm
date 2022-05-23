
default:
	../../naken_asm -l -o blink_leds.hex -I ../../include/8051 blink_leds.asm
	../../naken_asm -l -o sleepy_bee.hex -I ../../include/8051 sleepy_bee.asm
	../../naken_asm -l -o ws2812.hex -I ../../include/8051 ws2812.asm

clean:
	@rm -f *.hex *.lst
	echo "Clean!"

