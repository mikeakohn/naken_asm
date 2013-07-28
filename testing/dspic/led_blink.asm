
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
  mov w4, 1000
repeat_loop:
  dec w4, w4
  bra nz, repeat_loop
  return


