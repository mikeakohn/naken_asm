.f100_l

main:
  nop
blah:
  add .0x1000
  add long 0x1000
  add ,0x400
  add 0x50
  add /0x60
  add /0x62+
  add /0x63-

  add [0x63]
  add [0x63]+
  add [0x63]-
  add [test+5]
  add [test]+
  add [test*2]-

  clr 4, A
  clr 14, CR
  set 3, 0x1000

  halt
  halt 100

  icz /0x60, test

  jbs 2, cr, 0x1000
  jcs 13, 0x50, 0x1000

  sra 2, a
  sla 13, 0x1000
  sre 7, 0x1020

  sra.d 7, a
  sll.d 29, cr
  sla.d 20, 0x1000

test:

