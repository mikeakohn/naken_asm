.msp430


.org 0xf800
main:
  mov.w #165, &0x0204
  mov.w &0x0204, &0x0206
  mov.w &0x0206, r15 
  ret

.org 0xfffe
  .dc16 main

