
AVR8
===

Addresses
---------

An odd thing that Atmel did with the AVR8 was, since each instruction
is 16 bits the instruction address in flash is counted by instruction
and not by bytes.  To account for this, naken_asm will divide each
address by 2.  So if there is a jmp instruction to some label, the
label is not the byte offset but the 16 bit word offset.  So for
example:

    start:
      jmp test
      ldi r16, 0
    test:

In this case test is at byte offset 6, but the assembler writes it
as jmp 3.

This can cause an issue when, for example, loading the Z register
(the combination of r31:r30) with a pointer to a data section in
flash.  So to point Z to some data the address needs to be multiplied
by two to load Z with the byte address and not the instruction address:

      ldi r30, (speech_init * 2) & 0xff
      ldi r31, (speech_init * 2) >> 8
      rcall uart_sw_send_string

    .align_bytes 2
    speech_init:
      asciiz "\nP0\nV18\nSready for action\n"

When using .org, that address must also be the word address.  So for
a chip like the ATtiny402 where the memory map shows the start of
flash at 0x8000, .org should be set to 0x4000.  The vector addresses
have the same issue.

