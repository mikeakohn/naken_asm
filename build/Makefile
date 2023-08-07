include ../config.mak

VPATH=../asm:../disasm:../common:../simulate:../table:../fileio:..

OBJECTS= \
  $(ASM_OBJS) \
  $(DISASM_OBJS) \
  $(COMMON_OBJS) \
  $(SIM_OBJS) \
  $(TABLE_OBJS) \
  $(UTIL_OBJS) \
  $(FILEIO_OBJS) \
  $(PROG_OBJS)

default: $(OBJECTS)
	@ar cr naken_asm.a $(ASM_OBJS) $(DISASM_OBJS) $(COMMON_OBJS) \
	  $(SIM_OBJS) $(TABLE_OBJS) $(UTIL_OBJS) $(FILEIO_OBJS) $(PROG_OBJS)
	$(CXX) -o ../naken_asm$(CONFIG_EXT) ../common/naken_asm.cpp \
	   naken_asm.a \
	    -DINCLUDE_PATH="\"$(INCLUDE_PATH)\"" \
	   $(CFLAGS) $(LDFLAGS) -I..
	$(CXX) -o ../naken_util$(CONFIG_EXT) ../common/naken_util.cpp \
	   naken_asm.a \
	   $(DFLAGS) -I.. \
	   $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)

library: default
	$(CC) -o ../libnaken_asm.so ../library/naken_asm.c \
	  naken_asm.a -shared -I.. -fPIC \
	  $(CFLAGS)

common/assembler.o: common/assembler.cpp common/assembler.h
	$(CXX) -c $< -o $*.o $(CFLAGS) $(DFLAGS) -I..

common/cpu_list.o: common/cpu_list.cpp common/cpu_list.h
	$(CXX) -c $< -o $*.o $(CFLAGS) $(DFLAGS) -I..

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS) -I..

%.o: %.c
	$(CC) -c $< -o $*.o $(CFLAGS) -I..

%.o: %.cpp %.h
	$(CXX) -c $< -o $*.o $(CFLAGS) -I..

%.o: %.cpp
	$(CXX) -c $< -o $*.o $(CFLAGS) -I..

