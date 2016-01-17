.6809

start:
  asla
  aslb

  orcc #0x12
  lda #0xff
  cmpx #0xffff

  lda 0xffd2

  rol >0x30
  bne start
  lbne start

  puls x, y
  pulu pc, u, y, x, dp, b, a, cc
  pshs pc, x, dp, b, a, cc
  pshu cc

  tfr x, b
  tfr d, u 

  lda ,x
  lda ,y
  lda ,u
  lda ,s


