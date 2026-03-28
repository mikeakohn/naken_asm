;; Here's a sample of an LED for the Microchip dsPIC33CK Curiosity Nano board.

.dspic
.include "p33ck64mc105.inc"

.org 0
  goto main

.org 0x200
main:
  bclr TRISD, #10
repeat:
  bset LATD, #10
  call delay

  bclr LATD, #10
  call delay

  bra repeat

delay:
  mov #0xffff, w4
repeat_loop:
  dec w4, w4
  bra nz, repeat_loop
  return


