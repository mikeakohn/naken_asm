
default:
	../../naken_asm -l -type elf -o add_nums.o add_nums.asm
	gcc -o test test.c add_nums.o -Wall

clean:
	@rm -f *.o *.lst *.elf *.hex test
	@echo "Clean!"

