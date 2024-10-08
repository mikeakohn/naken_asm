include config.mak

vpath=asm:disasm:common:simulate

default:
	@mkdir -p build/asm
	@mkdir -p build/disasm
	@mkdir -p build/table
	@mkdir -p build/common
	@mkdir -p build/simulate
	@mkdir -p build/fileio
	$(MAKE) -C build

all: default

.PHONY: library
library:
	@$(MAKE) -C build library

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

testing_for_mikes_use: launchpad_blink.asm
	msp430-as launchpad_blink.asm
	msp430-objcopy -F ihex a.out testing.hex

msp430x:
	msp430-as testing/msp430x.asm -mmsp430x2619
	msp430-objcopy -F ihex a.out msp430x.hex

archive:
	ar rvs build/naken_asm.a build/*.o

clean:
	@rm -f naken_asm naken_util *.exe *.o *.hex a.out *.lst *.ndbg *.elf *.srec
	@rm -rf build/*.o build/*.a
	@rm -rf build/asm build/disasm build/table build/common
	@rm -rf build/simulate build/fileio
	@cd tests/unit/common && $(MAKE) clean
	@rm -f tests/unit/eval_expression/unit_test
	@rm -f tests/unit/eval_expression_ex/unit_test
	@rm -f tests/unit/data/data_test
	@rm -f tests/unit/tokens/tokens_test
	@rm -f tests/unit/macros/macros_test
	@rm -f tests/unit/memory/memory_test
	@rm -f tests/unit/symbols/symbols_test
	@rm -f tests/unit/util/util_test
	@rm -f tests/symbol_address/symbol_address
	@echo "Clean!"

.PHONY: tests
tests:
	@cd tests/regression && sh regression.sh
	@cd tests/unit/eval_expression && $(MAKE) && ./unit_test && make clean
	@cd tests/unit/eval_expression_ex && $(MAKE) && ./unit_test && make clean
	@cd tests/unit/common && $(MAKE) && $(MAKE) run && $(MAKE) clean
	@cd tests/unit/data && $(MAKE) && ./data_test && $(MAKE) clean
	@cd tests/unit/tokens && $(MAKE) && ./tokens_test && $(MAKE) clean
	@cd tests/unit/macros && $(MAKE) && ./macro_test && $(MAKE) clean
	@cd tests/unit/memory && $(MAKE) && ./memory_test && $(MAKE) clean
	@cd tests/unit/symbols && $(MAKE) && ./symbols_test && $(MAKE) clean
	@cd tests/unit/util && $(MAKE) && ./util_test && $(MAKE) clean
	@cd tests/symbol_address && $(MAKE) && ./symbol_address && $(MAKE) clean
	@cd tests/other && $(MAKE) && $(MAKE) run && $(MAKE) clean
	@cd tests/disasm && $(MAKE)
	@cd tests/comparison && $(MAKE)
	@cd tests/directives && python3 test.py

distclean: clean
	@rm -f config.mak *.asm
