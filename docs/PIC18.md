
PIC18
=====

Microchip PIC18.

NOTES
=====

This was kind of an awkward chip to implement. From 2 different PDF
files, a webpage that described the opcodes, and the output of
Microchip's mpasmx assembler there appeared to be some inconsistencies
with a few things. For example, some instructions that have an "a" flag
to indicate if the BSR should be used or it should be assumed to be the
special purpose registers. In the documentation, it will say a = 1 or
a = 0 is default. With the mpasmx, even though the documentation says
a = 1 is default, it will set it to 0 if it's not set as an opcode.
Not sure if those are bugs in the mpasmx assembler, but naken_asm will
follow what the mpasmx assembler does.

The PIC18 assembler code was tested using a sample program in the
samples/pic18 directory of this repo. The code will toggle the
green and yellow LEDs of Pololu's P-Star 45K50 Mini SV board
using Timer0. The board uses a PIC18F45K50.

For include files, there is a script in include/pic18/ that can
convert Microchip's include files into a file that naken_asm can
read.

