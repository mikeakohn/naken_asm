.propeller

start:
  clkset #100
  ror 80, 84
  ror 80, #84

  jmp #start
  abs 80, 84
  nop
  tjz 80, #start

