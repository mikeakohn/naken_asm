INCLUDE=../../include/dspic
NAKEN_ASM=../../naken_asm

OBJECTS= \
  curiosity_blink.hex \
  curiosity_uart.hex \
  led_blink33.hex \
  timer_interrupt.hex \
  led_blink.hex \
  testing.hex


default: $(OBJECTS)

%.hex: %.asm
	$(NAKEN_ASM) -l -I$(INCLUDE) -o $*.hex $<

clean:
	@rm -f *.hex *.lst
	@echo "Clean!"

