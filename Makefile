include config.mak

#OBJS=asm_65xx.o asm_680x0.o asm_805x.o asm_arm.o asm_common.o asm_dspic.o \
     asm_mips.o asm_msp430.o asm_stm8.o assembler.o disasm_65xx.o \
     disasm_680x0.o disasm_805x.o disasm_arm.o disasm_common.o disasm_dspic.o \
     disasm_mips.o disasm_msp430.o disasm_stm8.o eval_expression.o write_elf.o \
     write_hex.o get_tokens.o ifdef_expression.o lookup_tables.o macros.o \
     memory.o parse_elf.o parse_hex.o table_65xx.o table_680x0.o table_805x.o \
     table_mips.o table_dspic.o table_stm8.o
#SIM_OBJS=simulate_65xx.o simulate_msp430.o

vpath=asm:disasm:common:simulate

default:
	@+make -C build

%.o: %.c *.h
	$(CC) -c $*.c $(CFLAGS) $(LDFLAGS)

install:
	@mkdir -p $(INSTALL_BIN)
	@cp naken_asm $(INSTALL_BIN)
	@cp naken_util $(INSTALL_BIN)

install_old:
	#@mkdir -p $(INSTALL_INCLUDES)/include
	#@cp -r include/*.inc $(INSTALL_INCLUDES)/include

objs:
	@mkdir -f objs

uninstall:
	@echo "Scary stuff.  Just erase these things by hand."
	#@rm -f $(INSTALL_BIN)/naken_asm
	#@rm -f $(INSTALL_BIN)/naken_util
	#@rm -rf $(INSTALL_INCLUDES)

testing: launchpad_blink.asm
	msp430-as launchpad_blink.asm
	msp430-objcopy -F ihex a.out testing.hex

msp430x:
	msp430-as testing/msp430x.asm -mmsp430x2619
	msp430-objcopy -F ihex a.out msp430x.hex

archive:
	ar rvs build/naken_asm.a build/*.o

clean:
	@rm -f naken_asm naken_util naken_prog *.exe *.o *.hex a.out *.lst *.ndbg *.elf
	@rm -rf build/*.o build/*.a
	@echo "Clean!"

tests:
	@cd testing/regression && sh regression.sh
	@cd testing/unit/eval_expression && make && ./unit_test && make clean

distclean: clean
	@rm -f config.mak *.asm


