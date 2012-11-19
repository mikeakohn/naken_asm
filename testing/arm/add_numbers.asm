
.arm

add_numbers_asm:
  ;mov r0, #69
  ldr r0, [sp], #-4
  ldr r0, [sp, #-4]
  orr r0, r0, r0
  bx lr

