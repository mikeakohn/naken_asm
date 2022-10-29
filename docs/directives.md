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
|.1802            |RCA 1802
|.4004            |Intel 4004 (MCS-4)
|.6502            |6502, 6510, and any other 65xx CPUs
|.65816           |W65C816
|.6800            |6800
|.6809            |6809
|.68hc08          |68HC08
|.68000           |Motorola 68000 series
|.8008            |Intel 8008 (MCS-8)
|.8041            |8041
|.8048            |8048/MCS-48 (and similar)
|.8051            |8051/8052/MCS-51 (and similar)
|.86000           |86000
|.arm             |All 32 bit ARM CPUs
|.avr8            |Atmel AVR8
|.cell            |IBM Cell BE (Playstation 3)
|.copper          |Amiga Copper
|.cp1610          |CP1610 (General Instruments)
|.dspic           |dsPIC
|.epiphany        |Ephiphany/Parallella
|.java            |Java
|.lc3             |LC-3
|.m8c             |Cypress PSoC m8c
|.mips            |MIPS (32 bit)
|.mips32          |MIPS (32 bit)
|.msp430          |MSP430
|.mps430x         |MSP430X
|.n64_rsp         |Nintendo 64 RSP
|.pdk13           |Padauk PDK13
|.pdk14           |Padauk PDK14
|.pdk15           |Padauk PDK15
|.pic14           |PIC w/ 14 bit opcodes
|.pic24           |PIC24
|.pic32           |PIC32 MIPS
|.powerpc         |PowerPC
|.propeller       |Parallax Propeller
|.propeller2      |Parallax Propeller 2
|.ps2_ee          |Sony Emotion Engine (Playstation 2)
|.ps2_ee_vu0      |Sony Emotion Engine Vector Unit 0
|.ps2_ee_vu1      |Sony Emotion Engine Vector Unit 1
|.sh4             |Hitachi SH-4
|.stm8            |STM8
|.super_fx        |Super FX
|.sweet16         |SWEET16
|.unsp            |SunPlus unSP
|.riscv           |RISC-V
|.thumb           |THUMB
|.tms1000         |TMS1000/TMS1200/TMS1070/TMS1270
|.tms1100         |TMS1100/TMS1300
|.tms340          |TMS340
|.tms9900         |TMS9900
|.unsp            |SunPlus unSP
|.webasm          |WebAssembly
|.xtensa          |Xtensa (ESP32, ESP8266)
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
|.else                       |Else for .if or .ifdef
|.endif                      |Close an .ifdef or .if
|.macro {name} (opt. params) |Define a macro (should end with .endm)
|.endm                       |End of macro definition
|.repeat {count}             |Repeat the next set of instructions count times)
|.endr                       |End block of code for .repeat directive


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
|.low_address {address}     |Low address of memory written to output file
|.high_address {address}    |High address of memory written to output file
|.set {symbol}={value}      |Create or modify symbol's value (excluding labels)

