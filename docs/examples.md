Examples
========

Like other MSP430 assemblers, naken430asm accepts C style .define, .ifdef, .if defined(). An example would be:

    .define BLAH 50
    .define SOMETHING

Alternatively, to help with compatibility with include files from other
assemblers .define and .include can be written as #define and #include:

    #define BLAH 50

To test for a definition the following code works:

    .ifdef SOMETHING
      mov.w r1, r12
    .else
      mov.w r3, r12
    .endif

or

    .if defined(SOMETHING) && !defined(ANOTHER) && BLAH>50
      mov.w @r12+, r3
    .endif

Standard assembler syntax "equ" is also accepted too:

    BLAH equ 50

Also .macro / .endm works like the following:

    .macro BLAH
      mov.w #5, r10
      mov.w #6, r11
    .endm

or also:

    .macro BLAH(a,b)
      mov.w #a, r10
      mov.w #b, r11
    .endm

If there is need for a label inside of a macro, the scoping feature can be used:

    .macro PAUSE(a)
      mov.w #100, r6
    .scope
    wait:
      dec.w r6
      jnz wait
    .ends
    .endm

Scoping is also possible inside of function definitions.  In the example
below, the label wait will not be available outside this function and
can be reused:

    .func pause
    wait:
      dec.w r6
      jnz wait
    .endf

    call #pause


