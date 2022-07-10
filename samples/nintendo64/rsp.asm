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
;; byte   0: 0:Command/Signal | 1:Signal | 4,5:Offset | 6,7:Length
;; byte   8: X0, Y0, Z0
;; byte  16: X1, Y1, Z1
;; byte  24: X2, Y2, Z2
;; byte  32: rx, ry, rz
;; byte  40: dx, dy, dz
;; byte  48: fill color
;; -- Initialize screen.
;; byte  56: DP_OP_SET_COLOR_IMAGE
;; byte  64: DP_OP_SET_Z_IMAGE
;; byte  72: DP_OP_SET_SCISSOR
;; -- Polygons start.
;; byte  80: DP_OP_SYNC_PIPE
;; byte  88: DP_OP_SET_OTHER_MODES
;; byte  96: Polygon Color
;; byte 104: Polygon Data

.n64_rsp

.include "nintendo64/rsp.inc"
.include "nintendo64/rdp.inc"

.org 0
start:
  ;; Set $k0 to point to scratchpad area of DMEM.
  li $k0, 0xff0

  ;; Set $v0 to a vector of 0's.
  vxor $v0, $v0, $v0

  ;; Set $v2 to a vector of 2's.
  li $k1, 0x0202
  sh $k1,  8($k0)
  llv $v2[0], 8($k0)
  llv $v2[4], 8($k0)
  llv $v2[8], 8($k0)
  llv $v2[12], 8($k0)

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
  ;; Command 5: Draw rectangle.
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

  ;; Screen setup, run RDP commands from offset 56 to 72.
command_1:
  li $t1, 56
  li $t2, 24
  jal start_rdp
  nop
  jal wait_for_rdp
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
  jal wait_for_rdp
  nop
  sb $0, 0($0)
  b main
  nop

  ;; Calculate triangle and call start_rdp.
command_3:
  ;; Set DP_OP_SET_OTHER_MODES for retangle fill.
  li $t8, (DP_OP_SET_OTHER_MODES << 24) | (1 << 23) | (1 << 20)
  sw $t8, 88($0)
  ;; Color: $t6
  li $t8, DP_OP_SET_FILL_COLOR << 24
  lw $t9,  48($0)
  sw $t8,  96($0)
  sw $t9, 100($0)
  ;; Vertix 0: ($t0, $t1)
  ;; Vertix 1: ($t2, $t3)
  ;; Vertix 2: ($t4, $t5)
  lh $t0, 8($0)
  lh $t1, 10($0)
  lh $t2, 16($0)
  lh $t3, 18($0)
  lh $t4, 24($0)
  lh $t5, 26($0)
  ;; Sort vertex so y values go from top to bottom.
  ;; if (y1 > y2) { swap; }
  subu $t8, $t3, $t5
  blez $t8, command_3_skip_swap_0
  nop
  move $t8, $t2
  move $t9, $t3
  move $t2, $t4
  move $t3, $t5
  move $t4, $t8
  move $t5, $t9
command_3_skip_swap_0:
  ;; if (y0 > y1) { swap; }
  subu $t8, $t1, $t3
  blez $t8, command_3_skip_swap_1
  nop
  move $t8, $t0
  move $t9, $t1
  move $t0, $t2
  move $t1, $t3
  move $t2, $t8
  move $t3, $t9
command_3_skip_swap_1:
  ;; if (y1 > y2) { swap; }
  subu $t8, $t3, $t5
  blez $t8, command_3_skip_swap_2
  nop
  move $t8, $t2
  move $t9, $t3
  move $t2, $t4
  move $t3, $t5
  move $t4, $t8
  move $t5, $t9
command_3_skip_swap_2:
  ;; When y0 == y1, it can create a division by 0.
  bne $t1, $t3, command_3_not_div_0
  nop
  addiu $t3, $t3, 1
command_3_not_div_0:
  ;; Middle vertex leans to the right (left_major).
  ;; $t6 = is_left_major = x1 > x0 ? 1 : 0;
  slt $t6, $t0, $t2
  ;; Set command_byte=8, left_major=($t6 << 7), level=0, tile=0.
  li $t8, 0x0800
  sll $t6, $t6, 7
  or $t8, $t8, $t6
  sh $t8, 104($0)
  ;; Store YL ($t5), YM ($t3), YH ($t1) as 11.2.
  srl $t7, $t5, 2
  srl $t8, $t3, 2
  srl $t9, $t1, 2
  sh $t7, 106($0)
  sh $t8, 108($0)
  sh $t9, 110($0)
  ;; Slope: y = dy/dx * x + y0
  ;; Inverse Slope: x = dx/dy * y + x0
  ;; $s0 = dx_h = x0 - x2;
  ;; $s1 = dx_m = x0 - x1;
  ;; $s2 = dx_l = x1 - x2;
  subu $s0, $t0, $t4
  subu $s1, $t0, $t2
  subu $s2, $t2, $t4
  ;; $s3 = dy_h = y0 - y2;
  ;; $s4 = dy_m = y0 - y1;
  ;; $s5 = dy_l = y1 - y2;
  subu $s3, $t1, $t5
  subu $s4, $t1, $t3
  subu $s5, $t3, $t5
  ;; if (dy_h == 0) { dy_h = 1 << 4; }
  ;; if (dy_m == 0) { dy_m = 1 << 4; }
  ;; if (dy_l == 0) { dy_l = 1; }
  bne $s3, $0, command_3_dy_h_not_0
  nop
  li $s3, 1 << 4
command_3_dy_h_not_0:
  bne $s4, $0, command_3_dy_m_not_0
  nop
  li $s4, 1 << 4
command_3_dy_m_not_0:
  bne $s5, $0, command_3_dy_l_not_0
  nop
  li $s5, 1
command_3_dy_l_not_0:
  ;; $s0 = dxhdy = (dx_h << 4) / dy_h;
  ;; $s1 = dxmdy = (dx_m << 4) / dy_m;
  ;; $s2 = dxldy = (dx_l << 4) / dy_l;
  sll $s0, $s0, 4
  sll $s1, $s1, 4
  sll $s2, $s2, 4
  sh $s0, 0($k0)
  sh $s1, 2($k0)
  sh $s2, 4($k0)
  lsv $v10[0], 0($k0)
  lsv $v10[2], 2($k0)
  lsv $v10[4], 4($k0)


  ;; yh_fraction = y0 & 0xf;
  ;; xh = x0 - ((dxhdy * yh_fraction) >> 4);
  ;; xm = x0 - ((dxmdy * yh_fraction) >> 4);
  ;; xl = x0 + ((dxmdy * (y1 - y0)) >> 4);

  ;; Store XL, XH, XM as 16.16.
  ;sll $t7, $t0, 12
  ;sll $t8, $t2, 12
  ;sll $t9, $t4, 12
  ;sw $t7, 112($0)
  ;sw $t8, 120($0)
  ;sw $t9, 128($0)

  sb $0, 0($0)
  b main
  nop

command_4:
  sb $0, 0($0)
  b main
  nop

  ;; Draw rectangle.
command_5:
  ;; Set DP_OP_SET_OTHER_MODES for retangle fill.
  li $t8, (DP_OP_SET_OTHER_MODES << 24) | (1 << 23) | (3 << 20)
  sw $t8, 88($0)
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
  ;; Convert 12.4 fixed point to 10.2.
  srl $t0, $t0, 2
  srl $t1, $t1, 2
  srl $t2, $t2, 2
  srl $t3, $t3, 2
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
  li $t1, 80
  li $t2, 4 * 8
  jal start_rdp
  nop
  jal wait_for_rdp
  nop
  sb $0, 0($0)
  b main
  nop

;; start_rdp($t1=offset, $t2=length)
start_rdp:
  ;; Wait until End/Start Valid are cleared.
start_rdp_wait_end_start_valid:
  mfc0 $t8, RSP_CP0_CMD_STATUS
  andi $t8, $t8, 0x640
  bne $t8, $0, start_rdp_wait_end_start_valid
  nop
  ;; 0x0004 = Clear Freeze.
  li $t0, 0x0004
  addu $t3, $t1, $t2
  mtc0 $t0, RSP_CP0_CMD_STATUS
  mtc0 $t1, RSP_CP0_CMD_START
  mtc0 $t3, RSP_CP0_CMD_END
  jr $ra
  nop

wait_for_rdp:
  mfc0 $t8, RSP_CP0_CMD_END
wait_for_rdp_loop:
  mfc0 $t9, RSP_CP0_CMD_CURRENT
  bne $t8, $t9, wait_for_rdp_loop
  nop
  jr $ra
  nop

while_1:
  b while_1
  nop

