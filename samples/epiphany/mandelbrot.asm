.epiphany

.include "epiphany.inc"

.org 0x00
  b _sync_interrupt
.org 0x04
  b _sw_exception_interrupt
.org 0x08
  b _memory_fault_interrupt
.org 0x0c
  b _timer0_interrupt
.org 0x10
  b _timer1_interrupt
.org 0x14
  b _message_interrupt
.org 0x18
  b _dma0_interrupt
.org 0x1c
  b _dma1_interrupt
.org 0x20
  b _wand_interrupt
.org 0x24
  b _user_interrupt

.org 0x2000
_sync_interrupt:

;; Wait here until receiving a user interrupt signal from the Parallella
main:
  ;; r16 = base register mandel_params
  ;; r17 = base register image
  ;mov r16, #mandel_params
  mov r16, #0x0000
  movt r16, #0x8e00
  ;mov r17, #image

  ;; r18 signals that the next row should start
  mov r18, #0

  mov r0, #0x00
  movts IMASK, r0
  movts IPEND, r0
  mov r0, #0x3ff
  movts ILATCL, r0

  ;; DEBUG
  mov r0, #3
  str r0, [r16,#5]

  gie

wait_for_host:
  ;movfs r0, IRET
  ;mov r0, #111
  ;; DEBUG
  str r2, [r16, #5]
  ;add r19, r19, #1

  sub r0, r18, #1
  bne wait_for_host

start_mandel:
  ;; Clear anything interrupt related
  ;mov r0, #0x200
  ;mov r1, #0
  ;movts ILATCL, r0
  ;movts IPEND, r1
  mov r18, #0

next_row:
  ;; Wait till CPU signals continue

  ;; r0 = real_start
  ;; r1 = imaginary_start
  ;; r2 = real_inc
  ;; r3 = data_count
  ldr r0, [r16,#0]
  ldr r1, [r16,#1]
  ldr r2, [r16,#2]
  ldr r3, [r16,#3]

  ;; r17 = picture
  mov r17, #0x0400
  movt r17, #0x8e00

  ;; r8 = 4.0
  mov r8, #4
  float r8, r8

next_pixel:
  mov r4, #127

  ;; z = r5 + r6i
  ;; c = r0 + r1i
  mov r5, #0
  mov r6, #0
  float r5, r5
  float r6, r6

next_iteration:
  ;; z = z^2 + c
  ;; z^2 = (x + yi) * (x + yi)
  ;;     = x^2 + 2xyi - y^2
  ;;     = (x^2 - y^2) + 2xyi

  ;; r7 = 2*x*y
  fmul r7, r5, r6
  fadd r7, r7, r7

  ;; r5 = x^2 - y^2
  fmul r5, r5, r5
  fmsub r5, r6, r6

  ;; z = (r5 + r6i) + (r0 + r1i)
  fadd r5, r5, r0 
  fadd r6, r7, r1 

  ;; iteration--
  ;; if interation == 0 { break }
  sub r4, r4, #1
  beq pixel_done

  ;; if r5^2 + r6^2 > 4 { break; }
  fmul r7, r5, r5
  fmadd r7, r6, r6
  fsub r7, r7, r8
  bblt next_iteration

pixel_done:
  ;; *image = count >> 3
  ;; image += 1
  lsr r4, r4, #3
  strb r4, [r17]
  add r17, r17, #1

  ;; DEBUG
  ;str r4, [r16,#5]

  ;; real_start += real_inc
  fadd r0, r0, r2

  ;; data_count--
  ;; if (data_count != 0) { goto next_pixel; }
  sub r3, r3, #1
  bne next_pixel

  ;; Set done = 1
  mov r0, #1
  str r0, [r16,#4]

  ;; Pause the core
  ;idle

  b wait_for_host

_sw_exception_interrupt:
_memory_fault_interrupt:
_timer0_interrupt:
_timer1_interrupt:
_message_interrupt:
_dma0_interrupt:
_dma1_interrupt:
_wand_interrupt:
  ;; DEBUG
  mov r0, #14
  str r0, [r16,#5]
;crash:
;  b crash
  rti
_user_interrupt:
  mov r18, #1
  rti

.org 0x4000
mandel_params:
real_start:
  dc32 0
imaginary_start:
  dc32 0
real_inc:
  dc32 0
count:
  dc32 0
done_flag:
  dc32 1
debug:
  dc32 0

.org 0x6000
image:


