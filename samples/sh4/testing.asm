.sh4

main:
  add r4, r9
  add #-127, r9
loop:
  and #0x93, r0
  and.b #0x17, @(r0, gbr)
  bf loop
  bf/s loop
  bra loop
  brk
  braf r11
  fabs dr7
  fabs fr11
  fadd fr11, fr7
  fadd dr3, dr7
  fcnvds dr3, fpul
  fcnvsd fpul, dr7
  fipr fv3, fv1
  flds fr13, fpul
  float fpul, fr9
  fmac fr0, fr9, fr13
  fmov dr5, xd7
  fmov dr6, @r13
  fmov dr6, @-r13
  fmov dr6, @(r0, r14)
  fmov.s fr9, @r13
  fmov.s fr9, @-r13
  fmov.s fr9, @(r0, r11)

