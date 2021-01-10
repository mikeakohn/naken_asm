File Formats
============

naken_asm supports the following file formats:

Writing
-------

* Intel Hex
* SREC
* binary
* WDC Binary
* Amiga

Reading
-------

* Intel Hex
* SREC
* binary
* TI txt
* WDC Binary
* Amiga

The default output format is hex. To change the output format the
-type option can be used:

    ./naken_asm -type bin -o sample.bin sample.asm
    ./naken_asm -type elf -o sample.elf sample.asm

