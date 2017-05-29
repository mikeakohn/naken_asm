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

main:
  ;; r19 = base of shared memory for this core.
  ;; Core seems to be based row=32, col=8.
  ;; Substract 32 from row and 8 from column.
  mov r20, #0x3f
  movfs r1, COREID
  lsr r0, r1, #6
  and r0, r0, r20
  sub r0, r0, #32
  and r1, r1, r20
  sub r1, r1, #8

  ;; r16 = base register mandel_params
  ;; r16 = ((row * 4) + col) * 2048
  mov r20, #0x0000
  movt r20, #0x8e00
  lsl r0, r0, #2
  add r16, r0, r1
  lsl r16, r16, #11
  add r16, r16, r20

  ;; r21 = base register image
  mov r20, #1024
  add r21, r16, r20

  ;; r18 signals that the next row should start
  mov r18, #0

  mov r0, #0x00
  movts IMASK, r0
  movts IPEND, r0
  mov r0, #0x3ff
  movts ILATCL, r0

  gie

;; Wait here until receiving a user interrupt signal from the Parallella
wait_for_host:
  ;movfs r0, IRET
  ;; DEBUG
  ;str r2, [r16, #5]

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

  ;; r17 = picture (precalculated from r21)
  mov r17, r21

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


