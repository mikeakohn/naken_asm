
default:
	../../naken_asm -type bin -o IP.BIN IP.asm
	../../naken_asm -I ../../include/dreamcast -type bin -l -o dreamcast.bin dreamcast.asm
	/tmp/scramble dreamcast.bin 1ST_READ.BIN
	mkisofs -l -C 0,11702 -V dreamcast -G IP.BIN -r -J -o dreamcast.iso 1ST_READ.BIN
	cdi4dc dreamcast.iso dreamcast.cdi

iso:
	dd if=/dev/zero bs=2352 count=300 of=audio.raw
	mkisofs -l -C 0,11702 -o dreamcast.iso 1ST_READ.BIN
	( cat IP.BIN ; dd if=dreamcast.iso bs=2048 skip=16 ) > data.raw

record: iso
	cdrecord dev=4,0,0 -multi -audio audio.raw
	cdrecord dev=4,0,0 -msinfo
	cdrecord dev=4,0,0 -multi -xa1 -data data.raw

clean:
	@rm -f audio.raw *.iso *.cdi *.lst *.raw dreamcast.bin IP.BIN
	@rm -f 1ST_READ.BIN
	@echo "Clean!"

