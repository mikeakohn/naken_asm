.msp430


.org 0xf800
main:
  mov.w #165, r15 
  ret

.org 0xfffe
  .dc16 main

