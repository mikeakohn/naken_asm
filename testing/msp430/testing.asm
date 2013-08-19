.msp430x

.define number_five #5

.macro testing(a,b)
  mov a,b
.endm

start:
  mov.b #5, &100
  mov.b 100, &100
  add.w 100, &0x100
  sub.w &0x8000, &0x100
  mov.w &0x8000, r1
  rrc.w r9
  rra.w &0x100 
  jmp start
  push r9
  push.w r9
  push.b r9
  sxt r9
  swpb r9
  reti

  call &start

  jnz start
  jne start
  jz start
  jeq start
  jc start
  jhs start
  jn start
  jge start
  jl start

  testing (#2,4)
  testing (number_five,4)

  addc.w 100, &0x100
  sub.w 100, &0x100
  cmp.w 100, &0x100
  dadd.b 100, &0x100
  bit.b #5, &0x100
  bit.b #0xff, &0x100
  bis.b #0xff, &0x100
  xor.b #0xff, &0x100
  and.b #0xff, &0x100

  adc.b r9
  br #start
  clrc
  clrn
  clrz
  dadc.w &0x100
  dec.w &0x100
  decd.w &0x100
  dint
  eint
  inc.w &0x100
  inv.w &0x100
  nop
  ret
  rla.w &0x100
  rlc.w &0x100
  sbc.w &0x100
  setc
  setn
  setz
  tst.w r6

  mova @r8, r9
  mova @r8+, r9
  mova &0x12345, r9
  mova 0x100(r8), r9

  rrcm.a #3, r9
  rrcm.w #3, r9
  rrcm.a #3, r9
  rram.w #3, r9
  rlam.a #3, r9
  rrum.w #3, r9

  mova r9, &0x12345
  mova r9, 0x100(r9)

  mova #0x12345, r9
  cmpa #0x12345, r9
  adda #0x12345, r9
  suba #0x12345, r9

  mova r8, r9
  cmpa r8, r9
  adda r8, r9
  suba r8, r9

  calla start
  calla &0x12345
  calla 0x10(PC)
  calla #0x12345

  pushm.w #3, r9
  popm.w #7, r9



