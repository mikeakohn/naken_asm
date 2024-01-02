
default:
	../naken_asm/naken_asm -type elf -o get_value.o -l get_value.asm
	gcc -o test get_value.o test.c

clean:
	@rm -f *.o *.lst test
	@echo "Clean!"

