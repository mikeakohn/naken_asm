.msp430


.org 0xf800
main:
  mov.w #165, 0x0204
  mov.w 0x0204, r15
  ret

.org 0xfffe
  .dc16 main

