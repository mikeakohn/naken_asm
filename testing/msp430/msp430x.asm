.msp430x

start:
  rrc.b r8
  rrc r8
  rrc.w r8
  rrcx.w r8

  rrum.w #4, r8
  rrum.a #4, r8

  mov r8, r9

  jlo start

  mov #3, r10
  movx #0x1234, r10
  movx.w #0x1234, r10
  movx.a #0x1234, r10
  movx.b #0x34, r10

  mova @r8, r9
  mova @r8+, r9
  mova &0x10000, r9
  mova 0x1234(r8), r9

  rrcm.a #1, r8
  rram.w #2, r8
  rlam #3, r8
  rrum #4, r8

  mova r8, &0xa1234
  mova r8, 0x40(r7)

  mova #0x12345, r6
  cmpa #0x12345, r6
  adda #0x12345, r6
  suba #0x12345, r6

  mova r8, r9
  cmpa r8, r9
  adda r8, r9
  suba r8, r9

  call #blah

  call r9
  call 0x100(r9)
  call @r9
  call @r9+

blah:
  calla r9
  calla 0x100(r9)
  calla @r9
  calla @r9+
  reti

  calla blah
  calla &0x12345
  calla something
  calla #0x12345
something:
  nop
  mov.w #something, r9

  pushm.w #4, r8
  popm.a #4, r8

  rpt #5
  addx.w r8, r6

  rptc r7
  addx.w #1, r6

  rptz #5
  rrcx.w r8

  rpt #13
  addx.a r8, r8




