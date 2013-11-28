
.arm

add_numbers_asm:
  add r0, r0, r1
  bx lr

counter:
  mov r1, #10
repeat:
  add r0, r0, #1
  subs r1, r1, #1
  bne repeat
  bx lr



