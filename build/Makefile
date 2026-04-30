include ../config.mak

VPATH=../asm:../common:../core:../disasm:../fileio:../simulate:../table:..

OBJECTS= \
  $(ASM_OBJS) \
  $(COMMON_OBJS) \
  $(CORE_OBJS) \
  $(DISASM_OBJS) \
  $(FILEIO_OBJS) \
  $(SIM_OBJS) \
  $(TABLE_OBJS)

default: $(OBJECTS)
	@ar cr naken_asm.a $(ASM_OBJS) $(COMMON_OBJS) $(CORE_OBJS) \
	  $(DISASM_OBJS) $(FILEIO_OBJS) $(SIM_OBJS) $(TABLE_OBJS)
	$(CXX) -o ../naken_asm$(CONFIG_EXT) ../main/naken_asm.cpp \
	   naken_asm.a \
	    -DINCLUDE_PATH="\"$(INCLUDE_PATH)\"" \
	   $(CFLAGS) $(LDFLAGS) -I..
	$(CXX) -o ../naken_util$(CONFIG_EXT) ../main/naken_util.cpp \
	   naken_asm.a \
	   $(DFLAGS) -I.. \
	   $(CFLAGS) $(LDFLAGS) $(LDFLAGS_UTIL)

library: default
	$(CC) -o ../libnaken_asm.so ../library/naken_asm.c \
	  naken_asm.a -shared -I.. -fPIC \
	  $(CFLAGS)

core/assembler.o: core/assembler.cpp core/assembler.h
	$(CXX) -c $< -o $*.o $(CFLAGS) $(DFLAGS) -I..

core/cpu_list.o: core/cpu_list.cpp core/cpu_list.h
	$(CXX) -c $< -o $*.o $(CFLAGS) $(DFLAGS) -I..

%.o: %.c %.h
	$(CPP) -c $< -o $*.o $(CFLAGS) -I..

%.o: %.c
	$(CPP) -c $< -o $*.o $(CFLAGS) -I..

%.o: %.cpp %.h core/cpu_list.h
	$(CXX) -c $< -o $*.o $(CFLAGS) -I..

%.o: %.cpp core/cpu_list.h
	$(CXX) -c $< -o $*.o $(CFLAGS) -I..

