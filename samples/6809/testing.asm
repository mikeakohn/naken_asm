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

  lda a,x
  lda b,y
  lda d,s

  lda -1,x
  lda 100,y
  lda 300,s
  lda -300,u

  lda -300,pc
  lda -5,pc

  lda ,y+
  lda ,s++
  lda ,-y
  lda ,--s

  lda [,s++]
  lda [,--s]

  lda [-300,pc]
  lda [-5,pc]

  lda [-300,x]
  lda [-5,s]

  lda [,s]

  cmpd -2,s

