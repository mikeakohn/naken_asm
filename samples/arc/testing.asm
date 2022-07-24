.arc

  abs r3, r4
  abs r3, 5
  abs r3, -5
  abs r3, -100
  abs_s r3, r12
  abs 0, r4
  abs 0, 5
  abs 0, -5
  abs.f r3, r4
  abs.f r3, 5

  abs r23, 5

  adc.f r10, r12, r13
  adc.f r10, r12, 10
  adc.f r10, r12, -10
  adc.pl.f r10, r10, r13
  adc.ne.f r10, r10, 55 
  adc.f r10, 50000, r55
  adc.f r10, r33, 50000
  adc.eq.f r10, r10, 50000
  adc.f 0, r12, r40
  adc.f 0, r12, 32
  adc.f 0, r12, -60000
  adc.ne.f 0, -60000, r12

  add_s r1, r2, r12
  add_s r1, r2, 4
  add_s r3, r3, 0x12345
  add_s sp, sp, 8
  add_s r0, gp, 512
  add_s r12,r12,4

  asr.f r10, r11
  asr.f r10, 50
  asr.f r10, 5000
  asr.f 0, r11
  asr.f 0, 50
  asr.f 0, 5000

  adc.f r10, r12, r13
  add r9, r1, r7
  adc.f 0, r12, r40
  adc 0, r12, r40
  adc.f r10, r10, r11
  adc.ne.f r10, r10, r11
  adc.ne.f r10, r10, 55
  adc.f r10, r12, 10

  adc.f r10, r12, -10
  adc.pl.f r10, r10, r13
  adc.f r10, 50000, r55
  adc.f r10, r33, 50000
  adc.eq.f r10, r10, 50000

  adc.f 0, r12, 32

  adc.f 0, r12, -60000
  adc.ne.f 0, -60000, r12
  add.f r10, r12, 10
  add.f r10, r12, -10
  add.pl.f r10, r10, r13
  add.ne.f r10, r10, 55
  add.f r10, 50000, r55
  add.f r10, r33, 50000
  add.eq.f r10, r10, 50000
  add.f 0, r12, r40

  brk_s
  add_s r3, r4, r5
  add_s r0, gp, -4

