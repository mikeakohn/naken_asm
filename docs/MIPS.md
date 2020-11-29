
MIPS
====

MIPS support in naken_asm includes standard MIPS (including 64 bit
instructions), MIPS32, PIC32, and Playstation 2's R5900.

.mips
-----

This is the generic MIPS option. It defaults to big endian, although it
can be switched to little endian with the .little_endian directive.

.mips32
-------

This is pretty much the same as the .mips option, except it defaults
to little endian. Again, it can be switched to big endian with the
.big_endian directive.

.pic32
-------

This is pretty much the same as the .mips32 option.

.ps2_ee
-------

This is the same as .mips except it defaults to little endian and
adds Playstation 2 specific instructions including SIMD vector
instructions.

.n64_rsp
--------

This is for the Nintendo 64 Reality Signal Processor (RSP). This
adds all the vector instructions for the processor inside the graphics
chip in the Nintendo 64. The syntax naken_asm uses is the same syntax
used in the official RSP programmer's guide and defaults to big endian.

