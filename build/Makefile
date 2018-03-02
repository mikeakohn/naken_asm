include ../config.mak

VPATH=../asm:../disasm:../common:../simulate:../table:../prog:../fileio:..

default: $(ASM_OBJS) $(DISASM_OBJS) $(COMMON_OBJS) $(SIM_OBJS) $(TABLE_OBJS) \
         $(FILEIO_OBJS) $(PROG_OBJS)
	$(CC) -o ../naken_asm$(CONFIG_EXT) ../common/naken_asm.c $(ASM_OBJS) \
	   $(DISASM_OBJS) $(FILEIO_OBJS) $(COMMON_OBJS) $(SIM_OBJS) \
	   $(TABLE_OBJS) \
	    -DINCLUDE_PATH="\"$(INCLUDE_PATH)\"" \
	   $(CFLAGS) $(LDFLAGS) -I..
	$(CC) -o ../naken_util$(CONFIG_EXT) ../common/naken_util.c \
	   $(DISASM_OBJS) \
	   $(TABLE_OBJS) \
	   $(SIM_OBJS) \
	   $(FILEIO_OBJS) \
	   $(DFLAGS) \
	   common/cpu_list.o common/memory.o common/memory_pool.o \
	   common/print_error.o common/symbols.o -I.. \
	   $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)

prog:
	$(CC) -o ../naken_prog$(CONFIG_EXT) ../common/naken_prog.c \
	   prog/lpc.o common/memory.o common/memory_pool.o fileio/read_elf.o \
	   fileio/read_hex.o fileio/write_hex.o common/print_error.o \
	   prog/serial.o common/symbols.o -I.. \
	   $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)


common/assembler.o: common/assembler.c common/assembler.h
	$(CC) -c $< -o $*.o $(CFLAGS) $(DFLAGS) -I..

common/cpu_list.o: common/cpu_list.c common/cpu_list.h
	$(CC) -c $< -o $*.o $(CFLAGS) $(DFLAGS) -I..

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) -I..

%.o: %.c
	$(CC) -c $< -o $*.o $(CFLAGS) -I..



