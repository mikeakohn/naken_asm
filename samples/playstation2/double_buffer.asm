.ps2_ee

.include "playstation2/registers_ee.inc"
.include "playstation2/registers_gs_gp.inc"
.include "playstation2/registers_gs_priv.inc"
.include "playstation2/system_calls.inc"
.include "playstation2/vif.inc"
.include "playstation2/macros.inc"

.entry_point main
.export start
.export install_vsync_handler
.export interrupt_vsync
.export vsync_count
.export vsync_id

.org 0x100000
start:
main:
  li $sp, 0x02000000

  ;ei
  jal dma_reset
  nop

  ;; Reset GS
  li $v1, GS_CSR
  li $v0, 0x200
  sd $v0, ($v1)

  ;; Interrupt mask register
  li $v1, _GsPutIMR
  li $a0, 0xff00
  syscall
  nop

  ;; interlace      { PS2_NONINTERLACED = 0, PS2_INTERLACED = 1 };
  ;; videotype      { PS2_NTSC = 2, PS2_PAL = 3 };
  ;; frame          { PS2_FRAME = 1, PS2_FIELD = 0 };

  ;; SetGsCrt(s16 interlace, s16 pal_ntsc, s16 field);
  li $v1, _SetGsCrt
  li $a0, 1
  li $a1, 2
  li $a2, 0
  syscall
  nop

  ;; Use framebuffer read circuit (1 or 2?)
  li $v1, GS_PMODE
  li $v0, 0xff62
  sd $v0, ($v1)

  ;; interlaced on (bit 0), frame mode (bit 1), DPMS (bit 3:2) = 0
  ;; seems like this should already be set from the SetGsCrt call
  ;; UPDATE: this seems to get set to 3 no matter what.. remove later.
  ;li $v1, GS_SMODE2
  ;li $v0, 0x03
  ;sd $v0, ($v1)

  ;; GS_DISPFB2 with 0x1400
  ;;         base pointer (fbp): 0x0 (0x0)
  ;;   frame buffer width (fbw): 10 (640)
  ;; pixel storage format (psm): 0 (PSMCT32)
  ;;           position x (dbx): 0 (0x0)
  ;;           position y (dby): 0 (0x0)
  ;li $v1, GS_DISPFB2
  ;li $v0, 0x1400
  ;sd $v0, ($v1)

  jal set_context_1
  nop

  ;li $v1, GS_DISPFB1
  ;li $v0, 0x1200
  ;sd $v0, ($v1)

  ;; GS_DISPLAY2 with 0x000d_f9ff_0182_4290
  ;;         x position vck units (dx): 656
  ;;      y position raster units (dy): 36
  ;;        horiz magnification (magh): 3 (x8)
  ;;         vert magnification (magv): 0 (x1)
  ;;     display width - 1 in vck (dw): 2559
  ;; display height - 1 in pixels (dh): 223
  li $v1, GS_DISPLAY2
  li $at, 0x1bf_9ff
  dsll32 $at, $at, 0
  li $v0, 0x0182_4290
  or $at, $at, $v0
  sd $at, ($v1)

  ;li $v1, GS_DISPLAY1
  ;li $at, 0xdf9ff
  ;dsll32 $at, $at, 0
  ;li $v0, 0x0182_4290
  ;or $at, $at, $v0
  ;sd $at, ($v1)

  ;jal install_vsync_handler
  ;nop

  ;li $v1, GS_BGCOLOR
  ;li $v0, 0x004400ff
  ;sd $v0, ($v1)

  ;; Reset VIF1
  li $v0, VIF1_STAT
  li $v1, 0xf
  sw $v1, 0x10($v0)     ; FBRST
  li $v1, 0x0
  sw $v1, 0x00($v0)     ; STAT
  sw $v1, 0x50($v0)     ; MODE
  sw $v1, 0x80($v0)     ; CODE
  sw $v1, 0x90($v0)     ; ITOPS
  sw $v1, 0xa0($v0)     ; BASE
  sw $v1, 0xb0($v0)     ; OFST
  sw $v1, 0xc0($v0)     ; TOPS
  sw $v1, 0xd0($v0)     ; ITOP
  sw $v1, 0xe0($v0)     ; TOP

  li $v0, D2_CHCR
  li $v1, init_video
  sw $v1, 0x10($v0)         ; DMA02 ADDRESS
  li $v1, (init_video_end - init_video) / 16
  sw $v1, 0x20($v0)         ; DMA02 SIZE
  li $v1, 0x101
  sw $v1, ($v0)             ; start

  jal dma02_wait
  nop

  ;; Send VU1 code (VIF_MPG)
  li $v0, D1_CHCR
  li $v1, vif_packet_mpg_start
  sw $v1, 0x10($v0)                   ; DMA01 ADDRESS
  li $v1, ((vif_packet_mpg_end - vif_packet_mpg_start) / 16)
  sw $v1, 0x20($v0)                   ; DMA01 SIZE
  li $v1, 0x101
  sw $v1, ($v0)                       ; start

  jal dma01_wait
  nop

while_1:
  ;; First context
  jal set_context_1
  nop

  ;; Draw picture
  jal draw_screen_1
  nop

  li $a0, draw_triangle_1
  jal rotate
  nop

  jal wait_for_vsync
  nop

  ;; Do page flip
  jal set_context_2
  nop

  ;; Draw picture
  jal draw_screen_2
  nop

  li $a0, draw_triangle_2
  jal rotate
  nop

  jal wait_for_vsync
  nop

  b while_1
  nop

rotate:
  ;; Calculate next rotation
  li $v1, angle
  lw $v0, ($v1)
  addiu $v0, $v0, 1
  addiu $t0, $v0, -512
  bne $t0, $0, angle_not_zero
  nop
  xor $v0, $v0, $v0
angle_not_zero:
  sw $v0, ($v1)
  sll $v0, $v0, 2

  ;; Update cosine
  li $v1, _cos_table_512
  add $v1, $v1, $v0
  lw $t0, 0($v1)
  sw $t0, 20($a0)

  ;; Update sine
  li $v1, _sin_table_512
  add $v1, $v1, $v0
  lw $t0, 0($v1)
  sw $t0, 16($a0)

  ;; Flush cache
  ;lui $a0, 0
  ;li $v1, FlushCache
  ;syscall

  ;; Should be able to flush the cache with instructions...
  li $v1, vif_packet_1_start
  sync.l
  cache dhwoin, 0($v1)
  cache dhwoin, 64($v1)
  sync.l

  li $v1, vif_packet_2_start
  sync.l
  cache dhwoin, 0($v1)
  cache dhwoin, 64($v1)
  sync.l

  jr $ra
  nop

draw_screen_1:
  ;; Save return address register
  move $s3, $ra

  ;; Copy triangle to VU1 so it can go through transformations.
  ;; Start by waiting for DMA to finish (shouldn't need this)
  ;; Point to VU1 Mem (the data memory segment in VU1)
  ;; Copy the DMA packet to VU1 Mem
  jal dma02_wait
  nop

  li $v0, D2_CHCR
  li $v1, black_screen_1
  sw $v1, 0x10($v0)         ; DMA02 ADDRESS
  li $v1, (black_screen_1_end - black_screen_1) / 16
  sw $v1, 0x20($v0)         ; DMA02 SIZE
  li $v1, 0x101
  sw $v1, ($v0)             ; start

  jal dma02_wait
  nop

  ;; If VU1 is busy, wait for it to finish.
.scope
  li $v1, VIF1_STAT
wait_on_vu1:
  lw $v0, ($v1)
  andi $v0, $s0, 0x04
  bne $v0, $0, wait_on_vu1
  nop
.ends

  ;; Send data (VIF_UNPACK) and start VU1 (VIF_MSCAL)
  li $v0, D1_CHCR
  li $v1, vif_packet_1_start
  sw $v1, 0x10($v0)                   ; DMA01 ADDRESS
  li $v1, ((vif_packet_1_end - vif_packet_1_start) / 16)
  sw $v1, 0x20($v0)                   ; DMA01 SIZE
  li $v1, 0x101
  sw $v1, ($v0)                       ; start

  ;; Restore return address register
  move $ra, $s3
  jr $ra
  nop

draw_screen_2:
  ;; Save return address register
  move $s3, $ra

  ;; Copy triangle to VU1 so it can go through transformations.
  ;; Start by waiting for DMA to finish (shouldn't need this)
  ;; Point to VU1 Mem (the data memory segment in VU1)
  ;; Copy the DMA packet to VU1 Mem
  jal dma02_wait
  nop
  li $v0, D2_CHCR
  li $v1, black_screen_2
  sw $v1, 0x10($v0)         ; DMA02 ADDRESS
  li $v1, (black_screen_2_end - black_screen_2) / 16
  sw $v1, 0x20($v0)         ; DMA02 SIZE
  li $v1, 0x101
  sw $v1, ($v0)             ; start

  jal dma02_wait
  nop

  ;; If VU1 is busy, wait for it to finish.
.scope
  li $v1, VIF1_STAT
wait_on_vu1:
  lw $v0, ($v1)
  andi $v0, $s0, 0x04
  bne $v0, $0, wait_on_vu1
  nop
.ends

  ;; Send data (VIF_UNPACK) and start VU1 (VIF_MSCAL)
  li $v0, D1_CHCR
  li $v1, vif_packet_2_start
  sw $v1, 0x10($v0)                   ; DMA01 ADDRESS
  li $v1, ((vif_packet_2_end - vif_packet_2_start) / 16)
  sw $v1, 0x20($v0)                   ; DMA01 SIZE
  li $v1, 0x101
  sw $v1, ($v0)                       ; start

  ;; Restore return address register
  move $ra, $s3
  jr $ra
  nop

set_context_1:
  ;; GS_DISPFB2 with 0x1400
  ;;         base pointer (fbp): 0x0 (0x0)
  ;;   frame buffer width (fbw): 10 (640)
  ;; pixel storage format (psm): 0 (PSMCT32)
  ;;           position x (dbx): 0 (0x0)
  ;;           position y (dby): 0 (0x0)
  li $v1, GS_DISPFB2
  li $v0, SETREG_DISPFB(0, 10, FMT_PSMCT24, 0, 0)
  sd $v0, ($v1)
  jr $ra
  nop

set_context_2:
  ;; GS_DISPFB2 with 0x1400
  ;;         base pointer (fbp): 0x118000 (0x230)
  ;;   frame buffer width (fbw): 10 (640)
  ;; pixel storage format (psm): 0 (PSMCT32)
  ;;           position x (dbx): 0 (0x0)
  ;;           position y (dby): 0 (0x0)
  li $v1, GS_DISPFB2
  li $v0, SETREG_DISPFB(210, 10, FMT_PSMCT24, 0, 0)
  sd $v0, ($v1)
  jr $ra
  nop

wait_for_vsync:
  ;; Wait for vsync
  li $v1, GS_CSR
  li $v0, 8
  sw $v0, ($v1)
vsync_wait:
  lw $v0, ($v1)
  andi $v0, $v0, 8
  beqz $v0, vsync_wait
  nop
  jr $ra
  nop

install_vsync_handler:
  di

  ;; Add Vertical Blank End interrupt handler
  li $v1, _AddIntcHandler
  li $a0, INTC_VBLANK_E
  li $a1, interrupt_vsync
  li $a2, 0
  syscall
  nop

  ;; Save interrupt handler ID
  li $v1, vsync_id
  sw $v0, ($v1)

  ;; Enable Vercial Blank interrupt
  li $v1, __EnableIntc
  li $a0, INTC_VBLANK_E
  syscall
  nop

  ;; Reset counter
  li $v1, vsync_count
  li $v0, 0xff
  sw $v0, ($v1)

  ei
  jr $ra
  nop

interrupt_vsync:
  ;; Increment interrupt counter
  li $s1, vsync_count
  lw $s0, ($s1)
  addi $s0, $s0, 1
  sw $s0, ($s1)

  li $s1, GS_CSR
  li $s0, 8
  sw $s0, ($s1)

  jr $ra
  nop

dma_reset:
  li $s0, D1_CHCR
  sw $zero, 0x00($s0)    ; D1_CHCR
  sw $zero, 0x30($s0)    ; D1_TADR
  sw $zero, 0x10($s0)    ; D1_MADR
  sw $zero, 0x50($s0)    ; D1_ASR1
  sw $zero, 0x40($s0)    ; D1_ASR0

  li $s0, D2_CHCR
  sw $zero, 0x00($s0)    ; D2_CHCR
  sw $zero, 0x30($s0)    ; D2_TADR
  sw $zero, 0x10($s0)    ; D2_MADR
  sw $zero, 0x50($s0)    ; D2_ASR1
  sw $zero, 0x40($s0)    ; D2_ASR0

  li $s0, D_CTRL
  li $s1, 0xff1f
  sw $s1, 0x10($s0)      ; DMA_STAT

  lw $s1, 0x10($s0)      ; DMA_STAT
  andi $s1, $s1, 0xff1f
  sw $s1, 0x10($s0)      ; DMA_STAT

  ;sw $zero, 0x10($s0)    ; DMA_STAT
  sw $zero, 0x00($s0)    ; DMA_CTRL
  sw $zero, 0x20($s0)    ; DMA_PCR
  sw $zero, 0x30($s0)    ; DMA_SQWC
  sw $zero, 0x50($s0)    ; DMA_RBOR
  sw $zero, 0x40($s0)    ; DMA_RBSR

  lw $s1, 0x00($s0)      ; DMA_CTRL
  ori $s1, $s1, 1
  sw $s1, 0x00($s0)      ; DMA_CTRL

  jr $ra
  nop

dma01_wait:
  li $s1, D1_CHCR
  lw $s0, ($s1)
  andi $s0, $s0, 0x100
  bnez $s0, dma01_wait
  nop
  jr $ra
  nop

dma02_wait:
  li $s1, D2_CHCR
  lw $s0, ($s1)
  andi $s0, $s0, 0x100
  bnez $s0, dma02_wait
  nop
  jr $ra
  nop

.align 128
vsync_count:
  dc64 0
vsync_id:
  dc64 0
angle:
  dc64 0

.align 128
vif_packet_1_start:
  dc32 (VIF_FLUSH << 24)
  dc32 (VIF_STMOD << 24)
  dc32 (VIF_STCYCL << 24)|(1 << 8)|1
  dc32 (VIF_UNPACK_V4_32 << 24)|(((draw_triangle_1_end - draw_triangle_1) / 16) << 16)
draw_triangle_1:
  dc32   0.0, 0.0, 0.0, 0.0       ; sin(rx), cos(rx), sin(ry), cos(ry)
  dc32   0.0, 0.0, 0.0, 0.0       ; sin(rz), cos(rz)
  dc32 1900.0, 2100.0, 2048.0, 0.0 ; (x,y,z)    position
  dc32 3, 4, 2, 0                 ; vertex count, do_rot_xyz, vertex_len, 0 
  dc64 GIF_TAG(1, 0, 0, 0, FLG_PACKED, 1), REG_A_D
  dc64 SETREG_PRIM(PRIM_TRIANGLE, 1, 0, 0, 0, 0, 0, 0, 0), REG_PRIM
  dc64 GIF_TAG(3, 1, 0, 0, FLG_PACKED, 2), (REG_A_D|(REG_XYZ2<<4))
  dc64 SETREG_RGBAQ(255, 0, 0, 0, 1.0), REG_RGBAQ
  dc32 -100.0, -100.0, 0.0, 0
  dc64 SETREG_RGBAQ(0, 255, 0, 0, 1.0), REG_RGBAQ
  dc32 -100.0, 110.0, 0.0, 0
  dc64 SETREG_RGBAQ(0, 0, 255, 0, 1.0), REG_RGBAQ
  dc32 0.0, 110.0, 0.0, 0
draw_triangle_1_end:
vu1_1_start:
  dc32 (VIF_MSCAL << 24), 0, 0, 0
vif_packet_1_end:

.align 128
vif_packet_2_start:
  dc32 (VIF_FLUSH << 24)
  dc32 (VIF_STMOD << 24)
  dc32 (VIF_STCYCL << 24)|(1 << 8)|1
  dc32 (VIF_UNPACK_V4_32 << 24)|(((draw_triangle_2_end - draw_triangle_2) / 16) << 16)
draw_triangle_2:
  dc32   0.0, 0.0, 0.0, 0.0       ; sin(rx), cos(rx), sin(ry), cos(ry)
  dc32   0.0, 0.0, 0.0, 0.0       ; sin(rz), cos(rz)
  dc32 1900.0, 2100.0, 2048.0, 0.0 ; (x,y,z)    position
  dc32 3, 4, 2, 0                 ; vertex count, do_rot_xyz, vertex_len, 0 
  dc64 GIF_TAG(1, 0, 0, 0, FLG_PACKED, 1), REG_A_D
  dc64 SETREG_PRIM(PRIM_TRIANGLE, 1, 0, 0, 0, 0, 0, 1, 0), REG_PRIM
  dc64 GIF_TAG(3, 1, 0, 0, FLG_PACKED, 2), (REG_A_D|(REG_XYZ2<<4))
  dc64 SETREG_RGBAQ(255, 0, 0, 0, 1.0), REG_RGBAQ
  dc32 -100.0, -100.0, 0.0, 0
  dc64 SETREG_RGBAQ(0, 255, 0, 0, 1.0), REG_RGBAQ
  dc32 -100.0, 110.0, 0.0, 0
  dc64 SETREG_RGBAQ(0, 0, 255, 0, 1.0), REG_RGBAQ
  dc32 0.0, 110.0, 0.0, 0
draw_triangle_2_end:
vu1_2_start:
  dc32 (VIF_MSCAL << 24), 0, 0, 0
vif_packet_2_end:

; 640*224*3 = 430,080
; FB is 24 bit and Z buffer is still 32 bit, but shared by
; both contexts.
; Context 1 FB = 0        (divided by 2048 is 0)
; Context 2 FB = 430080   (divided by 2048 is 210)
; Context 1 Z  = 860160   (divided by 2048 is 420)
; Context 2 Z  = 860160   (divided by 2048 is 420)
; Textures     = 2293760

.align 128
init_video:
  dc64 GIF_TAG(11, 1, 0, 0, FLG_PACKED, 1), REG_A_D
  dc64 SETREG_FRAME(0, 10, FMT_PSMCT24, 0), REG_FRAME_1
  dc64 SETREG_FRAME(210, 10, FMT_PSMCT24, 0), REG_FRAME_2
  dc64 SETREG_ZBUF(420, 0, 0), REG_ZBUF_1
  dc64 SETREG_ZBUF(420, 0, 0), REG_ZBUF_2
  dc64 SETREG_XYOFFSET(1728 << 4, 1936 << 4), REG_XYOFFSET_1
  dc64 SETREG_XYOFFSET(1728 << 4, 1936 << 4), REG_XYOFFSET_2
  dc64 SETREG_SCISSOR(0,639,0,447), REG_SCISSOR_1
  dc64 SETREG_SCISSOR(0,639,0,447), REG_SCISSOR_2
  dc64 1, REG_PRMODECONT                 ; refer to prim attributes
  dc64 1, REG_COLCLAMP
  dc64 0, REG_DTHE                       ; Dither off
init_video_end:

.align 128
black_screen_1:
; FB is 24 bit and Z buffer is still 32 bit, but shared by
; both contexts.
  dc64 GIF_TAG(6, 1, 0, 0, FLG_PACKED, 1), REG_A_D
  dc64 0x30000, REG_TEST_1
  dc64 SETREG_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0), REG_PRIM
  dc64 SETREG_RGBAQ(0, 0, 0, 0, 1.0), REG_RGBAQ
  dc64 SETREG_XYZ2(1728 << 4, 1936 << 4, 0), REG_XYZ2
  dc64 SETREG_XYZ2(2368 << 4, 2384 << 4, 0), REG_XYZ2
  dc64 0x70000, REG_TEST_1
black_screen_1_end:

.align 128
black_screen_2:
  dc64 GIF_TAG(6, 1, 0, 0, FLG_PACKED, 1), REG_A_D
  dc64 0x30000, REG_TEST_2
  dc64 SETREG_PRIM(PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 1, 0), REG_PRIM
  dc64 SETREG_RGBAQ(0, 0, 0, 0, 1.0), REG_RGBAQ
  dc64 SETREG_XYZ2(1728 << 4, 1936 << 4, 0), REG_XYZ2
  dc64 SETREG_XYZ2(2368 << 4, 2384 << 4, 0), REG_XYZ2
  dc64 0x70000, REG_TEST_2
black_screen_2_end:

.include "sin_cos_table.inc"

.align 128
vif_packet_mpg_start:
  dc32 0x0, 0x0, 0x0, (VIF_MPG << 24)|(((rotation_vu1_end - rotation_vu1_start) / 8) << 16)
rotation_vu1_start:
  .binfile "rotation_vu1.bin"
rotation_vu1_end:
vif_packet_mpg_end:

