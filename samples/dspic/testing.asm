.dspic

main:
  nop
  nopr

  push 0x1234
  pop 0xfff0

  sub 0x556
  sub.b 0x555
  sub.b 0x555, wreg
  clr.w 0x234

  add A
  sub B

  sac B, W1
  sac.r B, W0
  sac.r A, #5, [W9]
  sac.r A, #-5, [W7+W2]
  sac B, #-2, [++W1]
  sac B, #2, [--W15]
  sac B, #1, [W15++]
  sac B, #1, [W15--]

blah:
  sftac A, #15
  sftac B, #-15
  sftac B, #-16
  bra ge, blah

  bra asdf
  sftac A, w3
asdf:

  cp0 w4
  cp0.w w4
  cp0.b [--w4]
  cp0.b [w2++]
  cp0.b [w8]

  cp.b 0x08
  cp 0x100
  cp.w 0x100

  mov.d w4, w8
  mov.d [w5], w6
  mov.d [w5--], w8

  mov.d w6, [w5]
  mov.d w8, [--w5]

  bset 0x100, #4
  bset.b 0x100, #4
  bset.b 0x101, #7
  bset 0x100, #15

  btsts.c w2, #15
  btsts.z [w9], #8
  btsts.z [--w9], #8

  ;do #5, end_do
  mov 0x122, w6
  mov 0xfffe, w6
;end_do:

  goto blah

  pwrsav #1

  retlw #100, w3
  sub.b #100, w3

  repeat #100

  mov.b #0x43, w5
  mov #0x4347, w7

  lnk #0x64
  pop.d w6
  pop.d w8

  pop.s
  pop [w1]
  pop [--w7]
  pop [w3+w4]

  mov.b [w1+w2], [w4+w2]
  mov.b [w1+w2], [w4++]

  push.s
  push.d w8

  mul.ss w3, w4, w6
  mul.ss w3, [w4++], w6
  mul.su w3, #31, w6
  mul.uu w3, #15, w8
  mul.su w3, w4, w6
  mul.us w3, [--w4], w6
  mul.uu w3, [--w1], w6

  div.u w3, w3
  div.ud w2, w3

  lsr w1, w3
  lsr.b [w1++], [--w3]

  se w1, w2
  se [--w1], w2

  bsw.z [w3++], w3
  bsw.c [w8++], w15

  lsr w1, #15, w3
  cp w3, #17
  cp.b w9, #31

  ior w1, #31, [--w3]
  ior.b w7, #31, [w3--]

  cpseq w1, w2
  cpsgt.b w1, w2

  lsr w1, w2, w3
  cp w1, [w2++]

  divf w1, w2

  ior w3, w5, [--w7]
  ior.b w1, [++w15], [--w7]

  setm [++w1]
  setm.b w9

  ;do w4, label3
  swap w4
  swap.b w4
  call w9
  bra w9
  daw.b w10
;label3:

  mov w3, 0xfffe
  mov w3, 0x1234

  mov w5, [w1+100]
  mov w5, [w1-100]

  exch w3, w4

  bset [w0], #15
  bset.b [w0], #7

  btsc [--w2], #15

  mov [w1-100], w5
  mov [w1+100], w5
  mov.b [w9+200], w5

  add [w1], #-5, B
  lac [w1+w2], #5, A

  push [w3]
  push [w3+w4]
  push [w3++]

  mov.b wreg, 163
  mov.b 163
  mov.b 163, wreg

  movsac A, [w9], w6, [w11]+=4, w7, w13
  movsac A, [w11]+=4, w7, w13
  clr A, [w11+w12], w7, w13

  mpy.n w4*w5, A, [w8]+=2, w4, [w10]+=2, w5
  mac w4*w4, A, [w8]+=2, w4, [w10]+=2, w5

  ed w4*w4, B, [w8]+=2, [w10]+=2, w5

  ;ed w4*w5, B, [w8]+=2, w4, [w10]+=2, w5, w13
  mac w4*w5, B, [w8]+=2, w4, [w10]+=2, w5, w13





