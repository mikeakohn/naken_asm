
default:
	../../naken_asm -l -e -I../../include/playstation2 -o test.elf test.asm

show_gif:
	python show_gif.py test.lst `grep 'red_screen ' test.lst | sed 's/^.* 00//' | sed 's/ .*$$//'`
	python show_gif.py test.lst `grep 'draw_triangle ' test.lst | sed 's/^.* 00//' | sed 's/ .*$$//'`

iso:
	mkisofs -xa2 -o cdr.iso 0.0 test.elf SYSTEM.CNF

old:
	mkisofs -l -o cdr.iso test.elf SYSTEM.CNF

clean:
	@rm -f test.elf cdr.iso test.lst
	@echo "Clean!"

