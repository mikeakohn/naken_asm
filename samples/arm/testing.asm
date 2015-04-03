.arm

  mov r0, #0xffffffff
  ;add r0, r0, #2 
  add r0, r0, r1
  swi
  swilt

  swp r0, r1, [r2]
  mrs r0, cpsr
  mrs r1, spsr

  msr cpsr, r0
  msr cpsr, #68
  msr cpsr, #32768
  msr cpsr, #65536
  msr cpsr, #128, 2

  mov r5, #256
  mov r5, #128, 2
  orr r5, r6, #256
  orr r5, r6, #128, 2

main:
  orr r5, r6, r7
  orr r5, r6, r7, lsl r0
  orr r5, r6, r7, ror #5

  mul r1, r2, r3
  mla r1, r2, r3, r4

  ldm r6, { r1, r2, r6-r8 }
  stm r6, { r1, r2, r6-r8 }
  stm r6, { r0, r1, r2, r6-r8 }
  stm r6!, { r0-r2, r6-r8 }

  ldr r6, [r7]
  str r7, [r8]
  str r7, 12

  b main

  ; FIXME
  ;str r7, [r1,r2, lsl #2]



