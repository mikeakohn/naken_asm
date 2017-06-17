.propeller

.include "propeller.inc"

main:
  mov dira, led_dir
while_1:
  mov outa, led_state0
  mov counter, counter_top
wait_1:
  sub counter, #1, wz
  if_nz jmp #wait_1
  ;tjnz counter, #wait_1

  mov outa, led_state1
  mov counter, counter_top
wait_2:
  sub counter, #1, wz
  if_nz jmp #wait_2
  ;tjnz counter, #wait_2
  jmp #while_1

led_dir:
  dc32 (1 << 26) | (1 << 27)
led_state0:
  dc32 (1 << 27)
led_state1:
  dc32 (1 << 26)
counter:
  dc32 0
counter_top:
  dc32 0xfffff

