.msp430
.export start

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

blah:
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

