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


