
default:
	../../naken_asm -l -e -I../../include/playstation2 -o test.elf test.asm

iso:
	mkisofs -xa2 -o cdr.iso 0.0 test.elf SYSTEM.CNF

old:
	mkisofs -l -o cdr.iso test.elf SYSTEM.CNF

clean:
	@rm -f test.elf cdr.iso test.lst
	@echo "Clean!"

