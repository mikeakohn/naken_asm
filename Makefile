include config.mak

vpath=asm:disasm:common:simulate

default:
	@$(MAKE) -C build

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
	@rm -f naken_asm naken_util naken_prog *.exe *.o *.hex a.out *.lst *.ndbg *.elf *.srec
	@rm -rf build/*.o build/*.a
	@rm -rf testing/unit/eval_expression/unit_test
	@rm -rf testing/unit/eval_expression_ex/unit_test
	@rm -rf testing/unit/data/data_test
	@rm -rf testing/unit/var/var_test
	@rm -rf testing/unit/tokens/tokens_test
	@echo "Clean!"

tests:
	@cd testing/regression && sh regression.sh
	@cd testing/unit/eval_expression && make && ./unit_test && make clean
	@cd testing/unit/eval_expression_ex && make && ./unit_test && make clean
	@cd testing/unit/var && make && ./var_test && make clean
	@cd testing/unit/data && make && ./data_test && make clean
	@cd testing/unit/tokens && make && ./tokens_test && make clean
	@cd testing/comparison && make

distclean: clean
	@rm -f config.mak *.asm


