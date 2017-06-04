

default:
	../../naken_asm -dump_symbols -l -b -o testing.bin testing.asm
	@printf '\060\000' > testing.obj
	@cat testing.bin >> testing.obj

clean:
	@rm -f *.lst *.hex *.bin *.obj
	@echo "Clean!"


