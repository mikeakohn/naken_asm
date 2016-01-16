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

