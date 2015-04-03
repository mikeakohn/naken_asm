.thumb

  lsl r3, r5, #4
  lsr r0, r1, #31
  asr r7, r1, #31

  sub r7, r6, r5
  add r7, r6, #4

  mov r3, #0x80
  cmp r4, #1
  add r2, #100
  sub r3, #10

  and r1, r2
  eor r2, r7
  lsl r3, r3
  lsr r4, r1
  asr r7, r4
  adc r4, r3
  sbc r3, r4
  ror r1, r4
  tst r0, r1
  neg r7, r0
  cmp r4, r1
  cmn r3, r2
  orr r3, r3
  mul r5, r4
  bic r2, r5
  mvn r1, r3

  add r1, r8
  add r8, r2
  add r8, r10

  cmp r1, r10
  cmp r9, r2
  cmp r8, r12

  mov r1, r10
  mov r11, r2
  mov r10, r13

  ldr r3, [PC, #44]
  ldr r3, label1

  bx r5
  bx r11
label1:

  str r5, [r4, r6]
  strb r5, [r4, r0]
  ldr r1, [r3, r0]
  ldrb r6, [r3, r3]

  strh r5, [r4, r6]
  ldrh r5, [r4, r0]
  ldsb r1, [r3, r0]
  ldsh r6, [r3, r3]

  str r5, [r4, #124]
  ldr r5, [r4, #64]
  strb r1, [r3, #9]
  ldrb r6, [r3, #31]

  strh r1, [r3, #60]
  ldrh r6, [r3, #32]

  str r1, [SP, #1020]
  ldr r6, [SP, #32]

  add r1, PC, #1020
  add r6, SP, #32

  add SP, #268
  add SP, #-104

  push { r1-r3, r7 }
  push { r1, r2, r3, r5-r7, lr }
  pop { r1-r3, r0 }
  pop { r1-r3, r6, pc }

back:
  stmia r0!, { r3-r7 }
  ldmia r4!, { r0, r3-r7 }

  bvc back

  swi 0x64

  b back

  bl back


