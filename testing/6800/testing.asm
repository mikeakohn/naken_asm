.680x

main:
  tap
  TPA
  clv
  sev
  cpx #blah

  bne main
  cpx blah, X

blah:
  lds #main
  suba #-1
  ldx #$1000
  ldaa #$10
  jsr blah
  anda $1000
  anda $10

