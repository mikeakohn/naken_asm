
PROGRAM=hifive_blink
ELFFILE=$(PROGRAM).elf

OPENOCD_CFG=$(FREEDOM_SDK)/bsp/env/freedom-e300-hifive1/openocd.cfg
GDB=$(FREEDOM_STUDIO)/SiFive/riscv64-unknown-elf-gcc-20171231-x86_64-linux-centos6/bin/riscv64-unknown-elf-gdb
#GDB=$(FREEDOM_SDK)/work/build/riscv-gnu-toolchain/riscv64-unknown-elf/prefix/bin/riscv64-unknown-elf-gdb
OPENOCD=$(FREEDOM_STUDIO)/SiFive/riscv-openocd-20171231-x86_64-linux-centos6/bin/openocd
#OPENOCD=$(FREEDOM_SDK)/work/build/openocd/prefix/bin/openocd -f

default:
	../../naken_asm -l -type elf -o $(PROGRAM).elf $(PROGRAM).asm

gnu:
	cat $(PROGRAM).asm | sed 's/_//' | grep -v '^.*;' | grep -v '.riscv' > temp.asm
	riscv64-unknown-elf-as temp.asm
	riscv64-unknown-elf-objcopy -F ihex a.out gnu.hex
	tail -n 4 gnu.hex > blah.hex
	../../naken_util -riscv -disasm blah.hex

program:
	$(OPENOCD) -f \
	$(OPENOCD_CFG) & \
	$(GDB) $(ELFFILE) --batch \
	  -ex "set remotetimeout 240" \
	  -ex "target extended-remote localhost:3333" \
	  -ex "monitor reset halt" \
	  -ex "monitor flash protect 0 64 last off" \
	  -ex "load" \
          -ex "monitor resume" \
	  -ex "monitor shutdown" \
	  -ex "quit" && \
	echo "Successfully uploaded 'out.hex' to freedom-e300-hifive1."

clean:
	@rm -f *.lst *.hex *.elf a.out temp.asm
	@echo "Clean!"

