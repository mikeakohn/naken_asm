.powerpc

main:
  add r3, r4, r5
  add. r3, r4, r5
  addo r3, r4, r5
  addo. r3, r4, r5
  addi r3, r4, 50
  addi r3, r4, -100
  addic r3, r4, -100
  addic. r3, r4, -100

  addme r3, r4
  addme. r3, r4
  addmeo r3, r4
  addmeo. r3, r4

  and r3, r4, r5
  and. r3, r4, r5

  andis. r3, r4, 100
  andis. r3, r4, 0xfff

  blr

  b main
  ba main
  bl main
  bla main
  bc 12, 4, main
  blt main

  li r8, 100

  bcctr 10, 11
  bcctrl 10, 11
  bclr 10, 11
  bclrl 10, 11

  cmpw cr7, r8, r9
  cmpd cr7, r8, r9
  cmpwi cr7, r8, 100
  cmpdi cr7, r8, -100

  cntlzw r8, r9
  cntlzw. r8, r9

  crand 6, 7, 8

  lbz r8, -100(r9)
  lbz r8, 123(r9)

  lswi r8, r9, 10

  mcrf cr7, cr3
  mcrxr cr7
  mfcr r31

  mfspr r9, 1
  mfspr r9, 100
  mfspr r9, ctr
  mfspr r9, sprg3

  mtspr 1, r9
  mtspr 100, r9
  mtspr ctr, r9
  mtspr sprg3, r9

  mftb r9, tbl
  mftb r9, tbu
  mftb r9, 7

  mtcrf 0xff, r9
  mtcrf 0x10, r9

  rlwimi r9, r7, 1, 2, 3
  rlwimi. r9, r31, 1, 2, 3

  srawi r9, r10, 6
  srawi. r9, r10, 6

  stbux r11, r12, r13


