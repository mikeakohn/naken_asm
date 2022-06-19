;; Simple Nintendo 64 sample.
;;
;; Copyright 2022 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; RSP code for drawing / rotating triangles.
;;
;; DMEM Format Is:
;; 
;; byte  0: 0:Signal | 1:Signal | 4,5:Offset | 6,7:Length
;; byte  8: X0, Y0, Z0
;; byte 16: X1, Y1, Z1
;; byte 24: X2, Y2, Z2
;; byte 32: rx, ry, rz
;; byte 40: dx, dy, dz
;; byte 48: fill color
;; byte 56: DP_OP_SET_COLOR_IMAGE
;; byte 64: DP_OP_SET_Z_IMAGE
;; byte 72: DP_OP_SET_SCISSOR
;; byte 80: DP_OP_SET_OTHER_MODES
;; byte 88: Polygons start

.n64_rsp

.include "nintendo64/rsp.inc"

.org 0
start:
  ;;sw $0, 0($0)

  ;; When DP_END_REG is written to, if it doesn't equal to DP_START_REG
  ;; it will start the RDP executing commands.
  ;; 0x000a = Set Freeze, Set DMEM DMA.
  li $t0, 0x000a
  mtc0 $t0, RSP_CP0_CMD_STATUS
  mtc0 $0, RSP_CP0_CMD_START
  mtc0 $0, RSP_CP0_CMD_END

main:
  ;; Wait until byte 0 is a 1.
  lb $t0, 0($0)
  beq $t0, $0, main
  nop

  ;; Wait until End/Start Valid are cleared.
wait_end_start_valid:
  mfc0 $t0, RSP_CP0_CMD_STATUS
  andi $t0, $t0, 0x600
  bne $t0, $0, wait_end_start_valid
  nop

  ;; 0x0004 = Clear Freeze.
  li $t0, 0x0004
  lw $t1, 4($0)
  lw $t2, 6($0)
  addu $t3, $t1, $t2
  mtc0 $t0, RSP_CP0_CMD_STATUS
  mtc0 $t1, RSP_CP0_CMD_START
  mtc0 $t3, RSP_CP0_CMD_END

while_1:
  b while_1
  nop

;semaphore:
;  mfc0 $t0, SP_SEMAPHORE
;  bne $t0, $0, dma_wait
;  nop
;  jr $ra
;  nop

