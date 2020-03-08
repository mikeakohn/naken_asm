.sh4

main:
  add r4, r9
  add #-127, r9
loop:
  and #0x93, r0
  and.b #0x17, @(r0,gbr)
  bf loop
  bf/s loop
  bra loop
  brk
  braf r11
  fabs dr8
  fabs fr11
  fadd fr11, fr7
  fadd dr12, dr2
  fcnvds dr4, fpul
  fcnvsd fpul, dr8
  fipr fv8, fv4
  flds fr13, fpul
  float fpul, fr9
  fmac fr0, fr9, fr13
  fmov dr6, xd8
  fmov xd4, dr14
  fmov xd8, xd12
  fmov dr6, @r13
  fmov dr6, @-r13
  fmov dr6, @(r0,r14)
  fmov.s fr9, @r13
  fmov.s fr9, @-r13
  fmov.s fr9, @(r0,r11)
  fmov xd4, @r11
  fmov xd4, @-r11
  fmov xd4, @(r0,r11)
  fmov @r9, dr8
  fmov @r9+, dr8
  fmov @(r0,r9), dr8
  fmov.s @r9, fr7
  fmov.s @r9+, fr7
  fmov.s @(r0,r9), fr7
  fmov @r9, xd6
  fmov @r9+, xd6
  fmov @(r0,r9), xd6
  ftrv XMTRX, fv12
  jmp @r9
  ldc r9, SPC
  ldc.l @r9+, SPC
  ldc r9, r7_bank
  ldc.l @r9+, r3_bank
  mac.l @r10+, @r11+
  mov.b r5, @r9
  mov.b r11, @-r3
  mov.b r11, @(r0,r3)
  mov.b r0, @(32,GBR)
  mov.b r0, @(12,r9)
  mov.b @r8, r9
  mov.b @r7+, r9
  mov.b @(r0,r13), r0
  mov.b @(123,GBR), r0
  mov.b @(10,r9), r0
  mov.l @(400,PC), r9
  mov.l @(60,r4), r9
  mov.w @(60,PC), R0
  movca.l r0, @r9
  stc DBR, r13
  stc r7_bank, r9
  stc.l DBR, @-r9
  stc.l r3_bank, @-r11
  trapa #0x43
  mov.l data, r1
  mov.w data, r1
  mova data, r0

.align 32
data:
  .dc32 1234

