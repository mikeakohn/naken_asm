include ../config.mak

VPATH=../asm:../disasm:../common:../simulate:../table

default: $(ASM_OBJS) $(DISASM_OBJS) $(COMMON_OBJS) $(SIM_OBJS) $(TABLE_OBJS)
	$(CC) -o ../naken_asm$(CONFIG_EXT) ../common/naken_asm.c $(ASM_OBJS) \
	   $(DISASM_OBJS) $(COMMON_OBJS) $(SIM_OBJS) $(TABLE_OBJS) \
	    -DINCLUDE_PATH="\"$(INCLUDE_PATH)\"" \
	   $(CFLAGS) $(LDFLAGS)
	$(CC) -o ../naken_util$(CONFIG_EXT) ../common/naken_util.c \
	   disasm_65xx.o disasm_805x.o disasm_arm.o disasm_common.o \
	   disasm_dspic.o disasm_mips.o disasm_msp430.o disasm_stm8.o \
	   parse_elf.o parse_hex.o memory.o simulate_65xx.o \
	   simulate_msp430.o table_65xx.o table_805x.o table_dspic.o \
	   table_mips.o table_stm8.o \
	   $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) $(LDFLAGS) \
	   -I../common -I../disasm -I../table -I../asm -I../simulate


