.arm

  mov r0, #0xffffffff
  ;add r0, r0, #2 
  add r0, r0, r1
  swi
  swilt

  swp r0, r1, [r2]

