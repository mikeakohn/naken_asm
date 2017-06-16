.propeller

start:
  clkset 100
  if_a ror 80, 84
  if_ne ror 80, #84

  jmp #start
  abs 80, 84
  nop
  tjz 80, #start

