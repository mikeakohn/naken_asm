
default:
	../../naken_asm -l -e -I../../include -o texture.elf texture.asm

triangle:
	../../naken_asm -l -e -I../../include -o triangle.elf triangle.asm

draw_image:
	../../naken_asm -l -e -I../../include -o draw_image.elf draw_image.asm

show_gif:
	python show_gif.py test.lst `grep 'red_screen ' test.lst | sed 's/^.* 00//' | sed 's/ .*$$//'`
	python show_gif.py test.lst `grep 'draw_triangle ' test.lst | sed 's/^.* 00//' | sed 's/ .*$$//'`

iso:
	mkisofs -l -o cdr.iso draw_image.elf SYSTEM.CNF

old:
	mkisofs -xa2 -o cdr.iso 0.0 draw_image.elf SYSTEM.CNF
	mkisofs -l -o cdr.iso test.elf SYSTEM.CNF

clean:
	@rm -f *.elf *.lst cdr.iso
	@echo "Clean!"

