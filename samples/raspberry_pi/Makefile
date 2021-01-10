

default:
	../../naken_asm -l -type elf -o asm_module.o asm_module.asm
	gcc -c c_module.c -Wall -g
	#gcc -o test_arm test_arm.c c_module.o asm_module.o -Wall
	gcc -o test_arm test_arm.c asm_module.o -Wall -g

clean:
	@rm -f *.o test_arm *.lst
	@echo "Clean!"

