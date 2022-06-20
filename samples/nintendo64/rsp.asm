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
;; byte  0: 0:Command/Signal | 1:Signal | 4,5:Offset | 6,7:Length
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
.include "nintendo64/rdp.inc"

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
  ;; Wait until command byte (offset 0) is no longer 0.
  lb $t0, 0($0)
  beq $t0, $0, main
  nop

  ;; Command 1: Call start_rdp only for screen setup.
  ;; Command 2: Directly call start_rdp.
  ;; Command 3: Calculate triangle and call start_rdp.
  ;; Command 4: Calculate rotation, projection, triangle, and start_dp.
  ;; Command 5: Draw square.
  li $t1, 1
  beq $t0, $t1, command_1
  nop
  li $t1, 2
  beq $t0, $t1, command_2
  nop
  li $t1, 3
  beq $t0, $t1, command_3
  nop
  li $t1, 4
  beq $t0, $t1, command_4
  nop
  li $t1, 5
  beq $t0, $t1, command_5
  nop

  ;; Unknown command.
  b main
  sb $0, 0($0)
  nop

;; Screen setup, run RDP commands from offset 56 to 88.
command_1:
  li $t1, 56
  li $t2, 32
  jal start_rdp
  nop
  sb $0, 0($0)
  b main
  nop

;; Run RDP commands specified by user: Offset 4 is offset, offset 6 is length.
command_2:
  lh $t1, 4($0)
  lh $t2, 6($0)
  ;lw $t1, 4($0)
  ;andi $t2, $t1, 0xffff
  ;srl $t1, $t1, 16
  jal start_rdp
  nop
  sb $0, 0($0)
  b main
  nop

command_3:
  sb $0, 0($0)
  b main
  nop

command_4:
  sb $0, 0($0)
  b main
  nop

command_5:
  ;; Add Sync Pipe Command.
  li $t4, DP_OP_SYNC_PIPE << 24
  sw $0, 92($0)
  sw $t4, 88($0)
  ;; Set Fill Color Command: convert R, G, B to (RGBA << 16) | RGBA.
  li $t4, DP_OP_SET_FILL_COLOR << 24
  lb $t0, 48($0)
  lb $t1, 49($0)
  lb $t2, 50($0)
  srl $t0, $t0, 3
  srl $t1, $t1, 3
  srl $t2, $t2, 3
  sll $t0, $t0, 11
  sll $t1, $t1, 6
  sll $t2, $t2, 1
  or $t0, $t0, $t1
  or $t0, $t0, $t2
  sh $t0, 100($0)
  sh $t0, 102($0)
  sw $t4, 96($0)
  ;; Set Fill Rectangle Command.
  li $t4, DP_OP_FILL_RECTANGLE
  lh $t0, 8($0)
  lh $t1, 10($0)
  lh $t2, 16($0)
  lh $t3, 18($0)
  ;; $t2 = XL
  ;; $t3 = YL
  sll $t2, $t2, 12
  or $t3, $t3, $t2
  ;; $t0 = XH
  ;; $t1 = YH
  sll $t0, $t0, 12
  or $t1, $t1, $t0
  sw $t3, 104($0)
  sw $t1, 108($0)
  sb $t4, 104($0)
  ;; Draw it.
  li $t1, 88
  li $t2, 3 * 8
  jal start_rdp
  nop
  sb $0, 0($0)
  b main
  nop

;; start_rdp($t1=offset, $t2=length)
start_rdp:
  ;; Wait until End/Start Valid are cleared.
wait_end_start_valid:
  mfc0 $t0, RSP_CP0_CMD_STATUS
  andi $t0, $t0, 0x600
  bne $t0, $0, wait_end_start_valid
  nop

  ;; 0x0004 = Clear Freeze.
  li $t0, 0x0004
  addu $t3, $t1, $t2
  mtc0 $t0, RSP_CP0_CMD_STATUS
  mtc0 $t1, RSP_CP0_CMD_START
  mtc0 $t3, RSP_CP0_CMD_END

  jr $ra
  nop

while_1:
  b while_1
  nop

