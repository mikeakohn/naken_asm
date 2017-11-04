Assembling
==========

    Usage: naken_asm [options] <infile>
       -o <outfile>
       -h             [output hex file]
       -e             [output elf file]
       -s             [output srec file]
       -b             [output binary file]
       -d             [add debug info (no longer implement)]
       -l             [create .lst listing file]
       -I             [add to include path]
       -q             Quite (only output errors)
       -dump_symbols  Dump all symbols at end of assembly
       -dump_macros   Dump all macros at end of assembly
       -optimize      Optimize instructions (see docs for info)

To compile a simple program, from the naken_asm directory type:

    ./naken_asm -I include/msp430_old -o launchpad_blink.hex
        -l testing/msp430/launchpad_blink.asm

The -h option is not needed since the default output file type is hex format.

The -optimize command line argument is a hint sent to the assemblers that
they can try to optimize things.  An example is: mov.w 0(r4), r6 with MSP430
is supposed to use up 4 bytes, but this is equivalent to mov.w @r4, r6.
If the -optimize argument is set, naken_asm will automatically convert it
to mov.w @r4, r6.  As of this writing, this is the only optimization that
is supported.  See documentation for each CPU to see what -optimize will
do if set for those assemblers.

If ELF is desired the -e option can be used with -o launchpad_blink.elf.
In order to assemble launchpad_blink.asm, an include file is required.

The -I option gives a path to the include file.  The -l option is highly
recommended since it creates a file that combines the original source code
with the same code disassembled.  This can be quite useful for debugging,
plus if there happens to be a problem with the assembler it can help show
what's wrong.  If the naken_asm can figure out cycle counts, the lst file
will also display this.

MSP430 is the default CPU, although it's still recommended to the use the
.msp430 directive at the top of the file.  If another CPU is desired to
assemble against, for example if this was a dsPIC program, the directive
.dspic can be placed at the top of the program.  All assembler directives
are listed below.


