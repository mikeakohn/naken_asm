.680x0

main:
  illegal
  rts

  clr.l d0

blah:
  addi.w #5, d1
  dbne d1, blah

  asl.w #4, d3
  asl.b d1, d3
  asl #5, d1

  ror.w #3, d3
  rol.b d1, d3

  lsl.w #3, d3
  lsr.b d1, d3

  pea d5
  pea a5
  pea (a5)
  pea -(a5)
  pea (a5)+

  ;and.w d1, a2
  and.b d3, d1
  and.l d2, d1
  and.b d2, d1

  trapv
  trap #5
  bkpt #5

  unlk a6

  rtm a3
  rtm d1

  adda.w d6, a3
  adda.w #500, a3
  adda.l #$ff, a3
  adda.l #$fffff, a3
  adda.l #$ffffff, a3

  adda.l (6,a5), a3
  adda.l (-6,a5), a3
  adda.l (-6,PC), a7

  cmp.b #5, d6
  cmp.l d7, d6

  add.b #5, d1
  add.b d2, d1
  add.b d2, (a3)
  add.b (a3), d2

  lea (a3), a2
  lea (-6,a3), a2
  lea ($ff), a2

  jsr (a6)

  jsr main

  addq.b #5, d5
  subq.b #5, d5
  moveq #100, d5

  move CCR, d6
  move d6, CCR
  move SR, d6

  movea.l #400, a6
  cmpm.w (a6)+, (a1)+

  abcd d1, d2
  abcd -(a1), -(a2)

  add.b d1, d2
  add.b a1, d2
  addx.b d1, d2
  addx.w -(a2), -(a3)

  sub.b d1, d2
  sub.b a1, d2
  subx.b d1, d2
  subx.w -(a2), -(a3)

  roxl (a1)
  roxr ($1000)

  roxl.w d1, d2
  roxr.b #8, d2

  and.w d1, d2

  exg d1, d2
  exg a1, a2
  exg d1, a2

test_addr:
  ;bclr #7, d5
  ;bclr #7, $ff
  ;bclr #7, $fffff

  bra.s blah
  bne.s crap

  chk.w #7, d3
  ori #5, CCR
crap:
  bra.s test_addr
  bne.s test_addr
  bne.s blah

  sne d5
  st (a6)

  ext.w d1
  ext.l d2

  link a6, #5
  unlk a6

  muls.w #5, d6
  divu.w #5, d2

  movep.w d6, (8,a6)
  movep.l d2, (-8,a1)
  movep.w (100,a6), d3
  movep.l (-16,a1), d4

  movem.w d3-d7/a7, ($1000)
  movem.w ($1000), d3-d7/a7

  move.w ($1234), ($5678)

  add.l #1, ($1000)


