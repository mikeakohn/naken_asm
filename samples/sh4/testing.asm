.sh4

main:
  add r4, r9
  add #-127, r9
loop:
  and #0x93, r0
  and.b #0x17, @(r0, gbr)
  bf loop
  bf/s loop
  bra loop

