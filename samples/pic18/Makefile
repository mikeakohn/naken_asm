INCLUDE_PATH=../../include/pic18
PROGRAM=p_star_blink

default:
	../../naken_asm -l -o $(PROGRAM).hex -I $(INCLUDE_PATH) $(PROGRAM).asm

clean:
	@rm -f *.lst *.hex
	@echo "Clean!"

