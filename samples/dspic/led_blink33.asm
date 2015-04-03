
.dspic
.include "p33fj06gs101a.inc"

.org 0
  goto main

.org 0x100
main:
  clr w0
  mov wreg, TRISA
repeat:
  mov #2, w0
  mov wreg, PORTA

  call delay

  mov #1, w0
  mov wreg, PORTA

  call delay

  bra repeat

delay:
  mov #5, w5
repeat_loop_outer:
  mov #0xffff, w4
repeat_loop_inner:
  dec w4, w4
  bra nz, repeat_loop_inner
  dec w5, w5
  bra nz, repeat_loop_outer
  return


