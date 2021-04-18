
SunPlus unSP
============

This assembler has support for the SunPlus $\mu$SP (SPG2xx) compatible chips
used in consoles such as the VTech V.Smile:

https://en.wikipedia.org/wiki/V.Smile

The syntax used here is differs from the official xasm16.exe assembler.
Most of the instructions use the format of:

    instruction dest, src

with an exception of the "st" instruction with uses:

    instruction src, dest

A list of instructions that can be used can be found in:

    tests/comparison/unsp.txt

