.msp430


.org 0xf800
main:
  mov.w #0x0204, r4
  mov.w #165, 4(r4)
  mov.w &0x0208, r15
  ret

.org 0xfffe
  .dc16 main

