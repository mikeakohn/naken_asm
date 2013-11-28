

; LED on PIO0_8
GPIO0DATA equ 0x10000000  ; This could be 0x50000400 i think
;GPIO0DATA equ 0x50003ffc  ; This could be 0x50000400 i think
GPIO0DIR equ 0x50008000

.dc32 0
.dc32 start
.dc32 0
.dc32 0
.dc32 0
.dc32 0
.dc32 0
.dc32 start

start:
.if 0
.thumb
  mov r4, #GPIO0DATA>>24
  lsl r4, r4, #24
  lsl r5, r4, #0

  mov r6, #(GPIO0DATA>>8)&0xff
  lsl r6, r6, #8
  orr r4, r6
  mov r6, #GPIO0DATA&0xff
  orr r4, r6

  mov r6, #(GPIO0DIR>>8)&0xff
  lsl r6, r6, #8
  orr r5, r6
  mov r6, #GPIO0DIR&0xff
  orr r5, r6

  mov r1, #4

  str r1, [r5, #0]
  str r1, [r4, #0]

repeat:
  b repeat
  orr r1, r1
.endif

.arm
  ;; r4 = GPIO0DATA,  1) set r4 to 0  2) shift in 4 bytes of address
  ;; Note: Add =numeric constant feature
  eor r4, r4, r4
  mov r4, #(GPIO0DATA&0xff000000)
  mov r5, r4
  orr r4, r4, #(GPIO0DATA&0xff00)
  orr r4, r4, #(GPIO0DATA&0xff)

  ;; r5 = GPIO0DIR
  orr r5, r5, #GPIO0DIR&0xff00
  orr r5, r5, #(GPIO0DIR&0xff)

  mov r1, #0x100
  str r1, [r5]
  str r1, [r4]
  orr r4, r4, r4

main:
  b main   ; while(1);


