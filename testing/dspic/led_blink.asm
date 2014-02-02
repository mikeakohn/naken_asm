
.dspic
.include "p30f3012.inc"

.org 0
  goto main

.org 0x100
main:
  clr w0
  mov.b wreg, TRISB
repeat:
  mov #2, w0
  mov.b wreg, PORTB

  call delay

  mov #1, w0
  mov.b wreg, PORTB

  call delay

  bra repeat

delay:
  ;; wow mike, wtf?  Probably w4 needs to be 0xffff to create the same
  ;; delay.  I don't have a dsPIC3012 hooked up to test this right now.
  ;mov w4, 1000
  mov #0xffff, w4
repeat_loop:
  dec w4, w4
  bra nz, repeat_loop
  return


