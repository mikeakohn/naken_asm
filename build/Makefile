include ../config.mak

VPATH=../asm:../disasm:../common:../simulate:../table:../prog

default: $(ASM_OBJS) $(DISASM_OBJS) $(COMMON_OBJS) $(SIM_OBJS) $(TABLE_OBJS) \
         $(PROG_OBJS)
	$(CC) -o ../naken_asm$(CONFIG_EXT) ../common/naken_asm.c $(ASM_OBJS) \
	   $(DISASM_OBJS) $(COMMON_OBJS) $(SIM_OBJS) $(TABLE_OBJS) \
	    -DINCLUDE_PATH="\"$(INCLUDE_PATH)\"" \
	   $(CFLAGS) $(LDFLAGS) -I../simulate -I../common
	$(CC) -o ../naken_util$(CONFIG_EXT) ../common/naken_util.c \
	   $(DISASM_OBJS) \
	   $(TABLE_OBJS) \
	   $(SIM_OBJS) \
	   $(DFLAGS) \
	   cpu_list.o parse_elf.o parse_hex.o memory.o print_error.o \
	   -I../disasm -I../common -I../simulate -I../table \
	   $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)
	$(CC) -o ../naken_prog$(CONFIG_EXT) ../common/naken_prog.c \
	   -I../prog lpc.o \
	   memory.o parse_elf.o parse_hex.o print_error.o serial.o \
	   $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)

assembler.o: assembler.c assembler.h
	$(CC) -c $< -o $*.o $(CFLAGS) $(DFLAGS) \
	   -I../common -I../disasm -I../table -I../asm -I../simulate

cpu_list.o: cpu_list.c cpu_list.h
	$(CC) -c $< -o $*.o $(CFLAGS) $(DFLAGS) \
	   -I../common -I../disasm -I../table -I../asm -I../simulate

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) \
	   -I../common -I../disasm -I../table -I../asm -I../simulate

%.o: %.c
	$(CC) -c $< -o $*.o $(CFLAGS) \
	   -I../common -I../disasm -I../table -I../asm -I../simulate



