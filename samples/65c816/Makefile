FLAGS=-l -I ../../include/65c816

default:
	naken_asm $(FLAGS) -o bender_head.hex bender_head.asm
	naken_asm $(FLAGS) -o mensch_blink.hex mensch_blink.asm

clean:
	@rm -f *.hex *.lst *.o
	@echo "Clean!"

