INCLUDES=../../include/msp430

default:
	../../naken_asm -l -I$(INCLUDES) -o msp430x.hex msp430x.asm
	../../naken_asm -l -I$(INCLUDES) -o waveshare_lcd.hex waveshare_lcd.asm
	../../naken_asm -l -I$(INCLUDES) -o ds1305_rtc.hex ds1305_rtc.asm
	../../naken_asm -l -I$(INCLUDES) -o ds18b20.hex ds18b20.asm
	../../naken_asm -l -I$(INCLUDES) -o ethernet.hex ethernet.asm
	../../naken_asm -l -I$(INCLUDES) -o guitar_proce.hex guitar_proc.asm
	../../naken_asm -l -I$(INCLUDES) -o launchpad_blink.hex launchpad_blink.asm
	../../naken_asm -l -I$(INCLUDES) -o spi_msp430g2231.hex spi_msp430g2231.asm
	../../naken_asm -l -I$(INCLUDES) -o spi_msp430g2553.hex spi_msp430g2553.asm
	../../naken_asm -l -I$(INCLUDES) -o testing.hex testing.asm
	../../naken_asm -l -I$(INCLUDES) -o uart.hex uart.asm

clean:
	@rm -f *.lst *.hex
	@echo "Clean!"

