
Hitachi SH-4
============

One instruction that naken_asm is not matching sh4-linux-gnu-as is
any instruction that uses displacement on PC such as:

mov.l @(16,PC), r4
mov.w @(12,PC), r5
mova @(20,PC), r0

The instructions work, the difference is GNU's assembler will adjust
the displacement by 4 bytes since the program counter will have moved
4 bytes forward when this instruction runs.  Since this seems like something
that was done for being an assembler for a compiler, naken_asm will instead
put the displacement directly into the instruction.  So if code needs
to access data at a label it could be done like:

mov.l @(data-$-4,PC), r4

data:
  db 100

The displacement here is calculated as the address of data - $
(where $ represents the address of the current instruction) minus 4
since the program counter's value when this instruction runs will be $ + 4.

