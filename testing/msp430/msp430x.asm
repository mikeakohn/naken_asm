.msp430

start:
  rrc.b r8
  rrc r8
  rrc.w r8
  rrcx.w r8

  mov r8, r9

  jlo start

  mov #3, r10
  movx #0x1234, r10

  mova @r8, r9
  mova @r8+, r9
  mova &0x10000, r9
  mova 0x1234(r8), r9

