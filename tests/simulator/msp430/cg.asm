.msp430


.org 0xf800
main:
  mov.w #0, r15
  add.w #1, r15
  add.w #-1, r15
  add.w #2, r15
  add.w #10, r15
  add.w #4, r15
  add.w #8, r15
  ret

.org 0xfffe
  .dc16 main

