.msp430

start:
  mov.w #5, r4
  mov.w r4, r10
while_1:
  add.w #100, r10
  jmp while_1

