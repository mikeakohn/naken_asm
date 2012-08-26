include config.mak

OBJS=asm_65xx.o asm_805x.o asm_arm.o asm_common.o asm_dspic.o asm_mips.o \
     asm_msp430.o assembler.o disasm_65xx.o disasm_805x.o disasm_arm.o \
     disasm_common.o disasm_dspic.o disasm_mips.o disasm_msp430.o \
     eval_expression.o file_output.o get_tokens.o ifdef_expression.o \
     lookup_tables.o macros.o memory.o parse_elf.o parse_hex.o table_65xx.o \
     table_805x.o table_mips.o table_dspic.o
SIM_OBJS=simulate_65xx.o simulate_msp430.o

default: $(OBJS) $(SIM_OBJS)
	$(CC) -o naken_asm$(CONFIG_EXT) naken_asm.c $(OBJS) \
	    -DINCLUDE_PATH="\"$(INCLUDE_PATH)\"" \
	   $(CFLAGS) $(LDFLAGS)
	$(CC) -o naken_util$(CONFIG_EXT) naken_util.c disasm_65xx.o \
	   disasm_805x.o disasm_arm.o disasm_common.o disasm_dspic.o \
	   disasm_mips.o disasm_msp430.o parse_elf.o parse_hex.o memory.o \
	   simulate_65xx.o simulate_msp430.o table_65xx.o table_dspic.o \
	   table_mips.o $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)

%.o: %.c *.h
	$(CC) -c $*.c $(CFLAGS) $(LDFLAGS)

install:
	@mkdir -p $(INSTALL_BIN)
	@mkdir -p $(INSTALL_INCLUDES)/include
	@cp naken_asm $(INSTALL_BIN)
	@cp naken_util $(INSTALL_BIN)
	@cp -r include/*.inc $(INSTALL_INCLUDES)/include


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

clean:
	@rm -f naken_asm naken_util *.exe *.o *.hex a.out *.lst *.ndbg *.elf
	@echo "Clean!"

distclean: clean
	@rm -f config.mak *.asm


