.msp430


.org 0xf800
main:
  mov.w #0, r15
  mov.w #10, r4
loop:
  add.w #1, r15
  dec.w r4
  jnz loop
  ret

.org 0xfffe
  .dc16 main

