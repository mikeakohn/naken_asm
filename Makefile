include config.mak

vpath=asm:disasm:common:simulate

default:
	@mkdir -p build/asm
	@mkdir -p build/disasm
	@mkdir -p build/table
	@mkdir -p build/common
	@mkdir -p build/simulate
	@mkdir -p build/fileio
	@mkdir -p build/prog
	@$(MAKE) -C build

all: default

%.o: %.c *.h
	$(CC) -c $*.c $(CFLAGS) $(LDFLAGS)

install:
	install -d $(INSTALL_PREFIX)/bin
	install naken_asm $(INSTALL_PREFIX)/bin 
	install naken_util $(INSTALL_PREFIX)/bin
	install -d $(INSTALL_PREFIX)/share/naken_asm
	cp -r include $(INSTALL_PREFIX)/share/naken_asm

objs:
	@mkdir -f objs

uninstall:
	@echo "Removing:"
	rm -f $(INSTALL_PREFIX)/bin/naken_asm
	rm -f $(INSTALL_PREFIX)/bin/naken_util
	rm -rf $(INSTALL_PREFIX)/share/naken_asm

testing: launchpad_blink.asm
	msp430-as launchpad_blink.asm
	msp430-objcopy -F ihex a.out testing.hex

msp430x:
	msp430-as testing/msp430x.asm -mmsp430x2619
	msp430-objcopy -F ihex a.out msp430x.hex

archive:
	ar rvs build/naken_asm.a build/*.o

clean:
	@rm -f naken_asm naken_util naken_prog *.exe *.o *.hex a.out *.lst *.ndbg *.elf *.srec
	@rm -rf build/*.o build/*.a
	@rm -rf build/asm build/disasm build/table build/common
	@rm -rf build/simulate build/fileio build/prog
	@rm -rf tests/unit/eval_expression/unit_test
	@rm -rf tests/unit/eval_expression_ex/unit_test
	@rm -rf tests/unit/data/data_test
	@rm -rf tests/unit/var/var_test
	@rm -rf tests/symbol_address/symbol_address
	@rm -rf tests/unit/tokens/tokens_test
	@rm -rf tests/unit/memory/memory_test
	@echo "Clean!"

.PHONY: tests
tests:
	@cd tests/regression && sh regression.sh
	@cd tests/unit/eval_expression && make && ./unit_test && make clean
	@cd tests/unit/eval_expression_ex && make && ./unit_test && make clean
	@cd tests/unit/var && make && ./var_test && make clean
	@cd tests/unit/data && make && ./data_test && make clean
	@cd tests/unit/tokens && make && ./tokens_test && make clean
	@cd tests/unit/macros && make && ./macro_test && make clean
	@cd tests/unit/memory && make && ./memory_test && make clean
	@cd tests/unit/symbols && make && ./symbols_test && make clean
	@cd tests/disasm && make
	@cd tests/symbol_address && make && ./symbol_address && make clean
	@cd tests/comparison && make
	@cd tests/directives && python3 test.py

distclean: clean
	@rm -f config.mak *.asm


