.propeller

start:
  clkset 100
  if_a ror 80, 84
  if_ne ror 80, #84

  sub 80, #1, wc wz

  jmp #start
  abs 80, 84
  nop
  tjz 80, #start

