
default:
	../../naken_asm -b -o IP.BIN IP.asm
	../../naken_asm -I ../../include/dreamcast -b -l -o test_dreamcast.bin test_dreamcast.asm
	/tmp/scramble test_dreamcast.bin 1ST_READ.BIN

iso:
	dd if=/dev/zero bs=2352 count=300 of=audio.raw
	mkisofs -l -C 0,11702 -o test_dreamcast.iso 1ST_READ.BIN
	#mkisofs -l -o test_dreamcast.iso 1ST_READ.BIN
	( cat IP.BIN ; dd if=test_dreamcast.iso bs=2048 skip=16 ) > data.raw
	cat audio.raw data.raw > test_dreamcast.cdi

record: iso
	cdrecord dev=4,0,0 -multi -audio audio.raw
	cdrecord dev=4,0,0 -msinfo
	cdrecord dev=4,0,0 -multi -xa1 data.raw

clean:
	@rm -f audio.raw test_dreamcast.iso test_dreamcast.bin IP.BIN
	@rm -f test_dreamcast.cdi 1ST_READ.BIN
	@echo "Clean!"

