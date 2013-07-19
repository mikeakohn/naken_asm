.dspic

main:
  nop
  nopr

  push 0x1234
  pop 0xfff0

  sub 0x556
  sub.b 0x555
  sub.b 0x555, wreg

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



