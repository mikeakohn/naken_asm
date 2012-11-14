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

