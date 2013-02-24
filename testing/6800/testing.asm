.680x

main:
  tap
  TPA
  clv
  sev
  cpx #blah

  bne main

blah:
  lds #main
  suba #-1
  ldx #$1000
  ldaa #$10

