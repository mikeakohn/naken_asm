
INCLUDE=../../include/6502

default:
	../../naken_asm -I $(INCLUDE) -l -b -o test.nes test.asm

disassemble:
	../../naken_util -6502 -disasm -bin -address 0x7ff0 test.nes > out.txt

clean:
	@rm -f test.bin test.lst test.nes

