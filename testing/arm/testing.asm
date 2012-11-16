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

  orr r5, r6, r7
  orr r5, r6, r7, lsl r0
  orr r5, r6, r7, ror #5



