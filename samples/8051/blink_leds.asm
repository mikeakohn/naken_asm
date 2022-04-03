.8051

.org 0

start:
  mov P0, #0
  mov P1, #0xf0

repeat:
  setb P0.0
  clr P0.1

  mov r1, 0
loop_outer1:
  mov r0, 0
loop_inner1:
  dec r0
  cjne r0, #0, loop_inner1
  dec r1
  cjne r1, #0, loop_outer1

  setb P0.1
  clr P0.0

  mov r1, 0
loop_outer2:
  mov r0, 0
loop_inner2:
  dec r0
  cjne r0, #0, loop_inner2
  dec r1
  cjne r1, #0, loop_outer2

  sjmp repeat

