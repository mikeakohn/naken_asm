.unsp

main:
  goto blah
  break
  int irq,fiq
  mul.ss r3, r4
  add r3, #23
  add r3, #1000
  add r3, r4
  add r2, [r3]
  add r2, [r3++]
  add r2, [r3--]
  add r2, [++r3]
  adc r2, d:[r3]
  and r2, d:[r3++]
  add r2, d:[r3--]
  add r2, d:[++r3]
  add r3, [0x1234]
  add r3, r2, [0x1234]
  add [0x1234], r3, r2
  add r2, r1 asr 4
  add r2, r1 lsl 2
  add r2, r1 lsr 1
  add r2, r1 rol 3
  add r2, r1 ror 4
  push r3, [sp]
  push r3-r5, [sp]
  push r5-r3, [sp]
  pop r3, [sp]
  pop r3-r5, [sp]
blah:

