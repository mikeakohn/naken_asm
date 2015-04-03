.epiphany

main:
  add r5,r6,r7
  bne blah 
  beq main
blah:

  ldr r5,[r6,#3]
  str r5,[r6]
  ldrb r5,[r7,#-3]
  ldrd r5,[r7,#8]

  ldr r5,[r6,r6]
  ldr r5,[r6,r1]
  ldr r5,[r6,-r6]
  ldr r5,[r6,r8]

  ldr r5,[r6],r2
  ldr r5,[r6],r3
  ldr r5,[r6],r8
  ldr r5,[r6],-r2

  ldrb r5, [r7], #100
  ldrb r5, [r7], #-200

  mov r5, #100
  mov r33, #100
  mov r33, #0xff
  mov r5, #-1

  add r5,r6,#2
  add r5,r6,#-4
  add r5,r6,#-1024
  add r5,r6,#1000

  lsr r5,r6,#10
  lsr r30,r40,#10

  jr r5
  jr r30
  jalr r5
  jalr r30

  gie
  gid
  nop
  idle
  bkpt
  mbkpt
  sync
  rti
  wand
  trap 60

  moveq r5, r7


