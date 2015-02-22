include ../config.mak

VPATH=../asm:../disasm:../common:../simulate:../table:../prog:../fileio:..

default: $(ASM_OBJS) $(DISASM_OBJS) $(COMMON_OBJS) $(SIM_OBJS) $(TABLE_OBJS) \
         $(FILEIO_OBJS) $(PROG_OBJS)
	$(CC) -o ../naken_asm$(CONFIG_EXT) ../common/naken_asm.c $(ASM_OBJS) \
	   $(DISASM_OBJS) $(FILEIO_OBJS) $(COMMON_OBJS) $(SIM_OBJS) \
	   $(TABLE_OBJS) \
	    -DINCLUDE_PATH="\"$(INCLUDE_PATH)\"" \
	   $(CFLAGS) $(LDFLAGS) -I../simulate -I../common -I../fileio
	$(CC) -o ../naken_util$(CONFIG_EXT) ../common/naken_util.c \
	   $(DISASM_OBJS) \
	   $(TABLE_OBJS) \
	   $(SIM_OBJS) \
	   $(FILEIO_OBJS) \
	   $(DFLAGS) \
	   common/cpu_list.o common/memory.o common/memory_pool.o \
	   common/print_error.o common/symbols.o \
	   -I../disasm -I../common -I../simulate -I../table -I../fileio \
	   $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)
	$(CC) -o ../naken_prog$(CONFIG_EXT) ../common/naken_prog.c \
	   -I../prog -I../common -I../fileio prog/lpc.o common/memory.o \
	   common/memory_pool.o fileio/read_elf.o fileio/read_hex.o \
	   fileio/write_hex.o common/print_error.o prog/serial.o \
	   common/symbols.o \
	   $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)

assembler.o: assembler.c assembler.h
	$(CC) -c $< -o $*.o $(CFLAGS) $(DFLAGS) \
	   -I../common -I../disasm -I../table -I../asm -I../simulate

cpu_list.o: cpu_list.c cpu_list.h
	$(CC) -c $< -o $*.o $(CFLAGS) $(DFLAGS) \
	   -I../common -I../disasm -I../table -I../asm -I../simulate

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) \
	   -I../common -I../disasm -I../table -I../asm -I../simulate -I../fileio

%.o: %.c
	$(CC) -c $< -o $*.o $(CFLAGS) \
	   -I../common -I../disasm -I../table -I../asm -I../simulate -I../fileio



