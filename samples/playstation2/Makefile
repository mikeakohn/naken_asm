
#DUMP_MACROS=-dump_macros
INCLUDES=-I../../include
FLAGS=-l -type elf $(INCLUDES) $(DUMP_MACROS)

default:
	../../naken_asm $(FLAGS) -o triangle.elf triangle.asm
	../../naken_asm $(FLAGS) -o reglist.elf reglist.asm
	../../naken_asm $(FLAGS) -o packed_long.elf packed_long.asm
	../../naken_asm $(FLAGS) -o texture.elf texture.asm
	../../naken_asm $(FLAGS) -o draw_image.elf draw_image.asm
	../../naken_asm $(FLAGS) -l -b -o rotation_vu1.bin rotation_vu1.asm
	../../naken_asm $(FLAGS) -l -o rotation.elf rotation.asm
	../../naken_asm $(FLAGS) -l -o double_buffer.elf double_buffer.asm
	../../naken_asm $(FLAGS) -l -o iop_example.elf iop_example.asm

sound:
	../../naken_asm $(INCLUDES) -l -b -o play_sound.bin play_sound.asm
	../../naken_asm $(FLAGS) -l -o iop_example2.elf iop_example2.asm

show_gif:
	python show_gif.py test.lst `grep 'red_screen ' test.lst | sed 's/^.* 00//' | sed 's/ .*$$//'`
	python show_gif.py test.lst `grep 'draw_triangle ' test.lst | sed 's/^.* 00//' | sed 's/ .*$$//'`

iso:
	mkisofs -l -o cdr.iso draw_image.elf SYSTEM.CNF

old:
	mkisofs -xa2 -o cdr.iso 0.0 draw_image.elf SYSTEM.CNF
	mkisofs -l -o cdr.iso test.elf SYSTEM.CNF

clean:
	@rm -f *.elf *.lst *.bin cdr.iso
	@echo "Clean!"

