.8051

start:
  add A, @r1
  add A, r5

  mov r3, #5
  ajmp 515

repeat:
  anl 50, #3
  anl A, #100
  anl A, 0x55

  cpl P1.1

  jb P1.2, repeat

