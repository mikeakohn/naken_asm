.6800

main:
  tap
  TPA
  clv
  sev
  cpx #blah

  bne main
  cpx blah+1, X

blah:
  lds #main
  suba #-1
  ldx #$1000
  ldaa #$10
  jsr blah
  anda $1000
  anda $10

