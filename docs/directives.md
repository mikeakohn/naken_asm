Directives
==========

All directives can start with a . or a #.  The # was used for compatibility
with some include files.

CPU Selection:
--------------
Using one of these directives is pretty much required at
top of every source file to let naken_asm know which
CPU this source file should be assembled for.

|                 |                                      |
|-----------------|--------------------------------------|
|.4004            |Intel 4004
|.6502            |6502, 6510, and any other 65xx CPUs
|.65816           |W65C816
|.6800            |6800
|.6809            |6809
|.68hc08          |68HC08
|.68000           |Motorola 68000 series
|.8051            |8051/8052/MCS-51
|.arm             |All ARM CPUs
|.avr8            |Atmel AVR8
|.cell            |IBM Cell BE (Playstation 3)
|.dspic           |dsPIC
|.epiphany        |Ephiphany/Parallella
|.lc3             |LC-3
|.mips            |MIPS (32 bit)
|.mips32          |MIPS (32 bit)
|.msp430          |MSP430
|.mps430x         |MSP430X
|.pic14           |PIC w/ 14 bit opcodes
|.pic24           |PIC24
|.pic32           |PIC32 MIPS
|.powerpc         |PowerPC
|.propeller       |Parallax Propeller
|.ps2_ee          |Sony Emotion Engine (Playstation 2)
|.ps2_ee_v0       |Sony Emotion Engine Vector Unit 0
|.ps2_ee_v1       |Sony Emotion Engine Vector Unit 1
|.stm8            |STM8
|.super_fx        |Super FX
|.riscv           |RISC-V
|.thumb           |THUMB
|.tms1000         |TMS1000/TMS1200/TMS1070/TMS1270
|.tms1100         |TMS1100/TMS1300
|.tms9900         |TMS9900
|.z80             |Z80

Data Directives:
----------------

Directives for inserting text or numerical data in a program.

|                          |                                       |
|--------------------------|---------------------------------------|
|.ascii {text}             |Insert ASCII chars at memory address (no NULL term)
|.asciiz {text}            |Same as .ascii but NULL terminated
|.db {data bytes}          |8 bit data bytes
|.dw {data words}          |16 bit data bytes
|.dc.w {data words}        |16 bit data bytes
|.dl {data words}          |32 bit data bytes
|.dc.l {data words}        |32 bit data bytes
|.dc16 {data words}        |16 bit data bytes
|.dc32 {data words}        |32 bit data bytes
|.dc64 {data words}        |64 bit data bytes
|.dq {data words}          |32 bit floats
|.resb {data byte count}   |Reserve {count} bytes
|.resw {data words count}  |Reserve {count} 16 bit words
|.binfile "binarydata.bin" |Read in binary file and insert at memory address

Data Formatting:
-----------------
|                           |                              |
|---------------------------|------------------------------|
|.align {16, 32, 64, 128..} |Align next instr/data to a bit boundary
|.align_bits                |Align by bits (same as .align)
|.align_bytes               |Align by bytes (align instr/data to byte boundary)
|.big_endian                |Store data / code in big endian format
|.little_endian             |Store data / code in little endian format

Macros and Define Directives:
-----------------------------
For examples on how to use macros, see some of the examples in the documentation.

|                            |                                                 |
|----------------------------|-------------------------------------------------|
|.if {exression}             |Evaluate an expression and assemble code if true
|.ifdef {exression}          |If defined, can be used with .define
|.ifndef {exression}         |If not defined, can be used with .define
|.define {macro}             |Define a C style #define macro
|.else {exression}           |Else for .if or .ifdef
|.endif                      |Close an .ifdef or .if
|.macro {name} (opt. params) |Define a macro (should end with .endm
|.endm                       |End of macro definition


Scope Directives:
-----------------
Labels defined within a scope will only be valid in that
scope and may be reused in other parts of the program.

|                            |                             |
|----------------------------|-----------------------------|
|.func {name}                |Start a function (all labels will be local)
|.endf                       |End a function
|.scope                      |Start a local scope
|.ends                       |End local scope

Other Directives:
-----------------
|                           |                              |
|---------------------------|------------------------------|
|.equ                       |Set symbol to numerical value (similar to .set)
|.export                    |Export symbol so it shows up in ELF file
|.entry_point               |ELF file / SREC entry point (address of execution)
|.include "includefile.inc" |Include a file of asm source code
|.list                      |Write included file into .lst file
|.org {address}             |Address where next assembled bytes are written to
|.set {symbol}={value}      |Create or modify symbol's value (excluding labels)

