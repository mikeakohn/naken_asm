.msp430

start:
  rrc.b r8
  rrc r8
  rrc.w r8
  rrcx.w r8

  mov r8, r9

  jlo start

  mov #3, r10
  movx #300000, r10


