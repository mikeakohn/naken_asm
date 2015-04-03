.tms9900

main:
  mov r0, r1
  a r0, @data
  a *r5, @data(r3)
  a @data(r3), *r1+
  a @data(r3), @data(r4)
  a @data, @data

  czc @data, r6

loop:
  clr r8
  b *r8
  blwp *r8+
  inc @data+2
  inc @data(r5)

  ldcr *r5, 6
  stcr @data, 7

  jle loop
  jh skip

  sbo -5
  sbz 5
skip:
  sla r5, 6

  li r6, 65535
  ai r7, -1

  lwpi 1000
  stst r6

  idle
  rtwp

data:
  dw 100, 200, 300

