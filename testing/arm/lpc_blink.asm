
.arm

; LED on PIO0_8
GPIO0DATA equ 0x50003ffc  ; This could be 0x50000400 i think
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
  ;; r4 = GPIO0DATA,  1) set r4 to 0  2) shift in 4 bytes of address
  ;; Note: Add =numeric constant feature
  eor r4, r4, r4
  mov r4, #(GPIO0DATA>>24), lsl #24
  mov r5, r4
  orr r4, r4, #((GPIO0DATA>>8)&0xff), lsl #8
  orr r4, r4, #(GPIO0DATA&0xff)

  ;; r5 = GPIO0DIR
  orr r5, r5, #((GPIO0DIR>>8)&0xff), lsl #8
  orr r5, r5, #(GPIO0DIR&0xff)

  ;mov [r4], #0x100
  ;mov [r5], #0x100
  mov r1, #0x100
  str r1, [r4]
  str r1, [r5]

main:
  b main   ; while(1);


