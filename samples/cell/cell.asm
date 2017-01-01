.cell

main:
  mpya r3, r4, r5, r6
  clz r5, r6
  ori r5, r6, 100
  shlhi r5, r6, 3
  orx r5, r6
  heq r5, r6
  heqi r5, 61
  heqi r5, -61

  clgthi r5, r6, 100

branches:
  br main
  bra main
  brsl r15, main
  brasl r15, main
  bi r15

blah:
  hbr branches, r10
  hbrp
  hbra branches, blah
  hbrr branches, blah

  csflt r10, r11, -2 
  cflts r10, r11, 2

  fscrrd r12

  stop 0x1234
  stopd r11, r12, r13
  lnop
  nop r0

  mfspr r13, 81
  mtspr 77, r13

  rdch r13, 100
  rchcnt r13, 100
  wrch 100, r13


