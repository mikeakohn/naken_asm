PROGRAM=bender_head
CFLAGS=-Wall -O3

default:
	naken_asm -l -I ../../include/65c816 -o $(PROGRAM).hex $(PROGRAM).asm

clean:
	@rm -f *.hex *.lst *.o
	@echo "Clean!"

