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
  li $v1, GsPutIMR
  li $a0, 0xff00
  syscall
  nop

  ;; interlace      { PS2_NONINTERLACED = 0, PS2_INTERLACED = 1 };
  ;; videotype      { PS2_NTSC = 2, PS2_PAL = 3 };
  ;; frame          { PS2_FRAME = 1, PS2_FIELD = 0 };

  ;; SetGsCrt(s16 interlace, s16 pal_ntsc, s16 field);
  li $v1, SetGsCrt
  li $a0, 1
  li $a1, 2
  li $a2, 1
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
  li $v1, GS_DISPFB2
  li $v0, 0x1400
  sd $v0, ($v1)

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
  li $at, 0xdf9ff
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

  ;; This an be done with DMA, but trying it with the main CPU
  ;; for now.  Copy GIF packet to VU1's data memory segment.
  li $v0, VU1_MICRO_MEM
  li $a1, (rotation_vu1_end - rotation_vu1) / 16
  li $v1, rotation_vu1
repeat_vu1_prog_copy:
  lq $a0, ($v1)
  sq $a0, ($v0)
  addi $v1, $v1, 16
  addi $v0, $v0, 16
  addi $a1, $a1, -1
  bnez $a1, repeat_vu1_prog_copy
  nop

while_1:
  ;; Draw picture
  jal draw_screen
  nop

  ;; Wait for vsync
  li $v1, GS_CSR
  li $v0, 8
  sw $v0, ($v1)
vsync_wait:
  lw $v0, ($v1)
  andi $v0, $v0, 8
  beqz $v0, vsync_wait
  nop
  b while_1
  nop

draw_screen:
  ;; Save return address register
  move $s3, $ra

  ;; Setup draw environment
  jal dma02_wait
  nop
  li $v0, D2_CHCR
  li $v1, black_screen
  sw $v1, 0x10($v0)         ; DMA02 ADDRESS
  li $v1, (black_screen_end - black_screen) / 16
  sw $v1, 0x20($v0)         ; DMA02 SIZE
  li $v1, 0x101
  sw $v1, ($v0)             ; start

  ;; Copy triangle to VU1 so it can go through transformations.
  ;; Start by waiting for DMA to finish (shouldn't need this)
  ;; Point to VU1 Mem (the data memory segment in VU1)
  ;; Copy the DMA packet to VU1 Mem
  jal dma02_wait
  nop

  ;; This an be done with DMA, but trying it with the main CPU
  ;; for now.  Copy GIF packet to VU1's data memory segment.
  li $v0, VU1_VU_MEM
  li $a1, (draw_triangle_end - draw_triangle) / 16
  li $v1, draw_triangle
repeat_vu1_data_copy:
  lq $a0, ($v1)
  sq $a0, ($v0)
  addi $v1, $v1, 16
  addi $v0, $v0, 16
  addi $a1, $a1, -1
  bnez $a1, repeat_vu1_data_copy
  nop

  ;; Start the VU1 with a VIF packet
  li $v0, D1_CHCR
  li $v1, vu1_start
  sw $v1, 0x10($v0)         ; DMA01 ADDRESS
  li $v1, 1                 ; Length is only 1 qword
  sw $v1, 0x20($v0)         ; DMA01 SIZE
  li $v1, 0x101
  sw $v1, ($v0)             ; start

  ;li $v0, D2_CHCR
  ;li $v1, draw_triangle
  ;sw $v1, 0x10($v0)         ; DMA02 ADDRESS
  ;li $v1, (draw_triangle_end - draw_triangle) / 16
  ;sw $v1, 0x20($v0)         ; DMA02 SIZE
  ;li $v1, 0x101
  ;sw $v1, ($v0)             ; start
  ;jal dma02_wait
  ;nop

  ;; Restore return address register
  move $ra, $s3
  jr $ra
  nop

install_vsync_handler:
  di

  ;; Add Vertical Blank End interrupt handler
  li $v1, AddIntcHandler
  li $a0, INTC_VBLANK_E
  li $a1, interrupt_vsync
  li $a2, 0
  syscall
  nop

  ;; Save interrupt handler ID
  li $v1, vsync_id
  sw $v0, ($v1)

  ;; Enable Vercial Blank interrupt
  li $v1, _EnableIntc
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
  li $s0, D2_CHCR
  ;sw $zero, 0x80($s0)    ; um, why?
  sw $zero, 0x00($s0)    ; D2_CHCR
  sw $zero, 0x30($s0)    ; D2_TADR
  sw $zero, 0x10($s0)    ; D2_MADR
  sw $zero, 0x50($s0)    ; D2_ASR1
  sw $zero, 0x40($s0)    ; D2_ASR0

  li $s0, D_CTRL
  li $s1, 0xff1f
  sw $s1, 0x10($s0)      ; DMA_STAT
  ;lw $s1, 0x10($s0)      ; DMA_STAT
  ;li $s2, 0xff1f
  ;and $s1, $s1, $s2
  ;sw $s1, 0x10($s0)      ; DMA_STAT

  ;sw $zero, 0x10($s0)    ; DMA_STAT
  sw $zero, 0x00($s0)    ; DMA_CTRL
  sw $zero, 0x20($s0)    ; DMA_PCR
  sw $zero, 0x30($s0)    ; DMA_SQWC
  sw $zero, 0x50($s0)    ; DMA_RBOR
  sw $zero, 0x40($s0)    ; DMA_RBSR

  lw $s1, 0x00($s0)      ; DMA_CTRL
  ori $s1, $s1, 1
  nop
  sw $s1, 0x00($s0)      ; DMA_CTRL
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

.align 64
vsync_count:
  dc64 0
vsync_id:
  dc64 0

.align 128
draw_triangle:
  dc32   0.0, 0.0, 0.0, 0.0      ; (rx,ry,rz) rotation
  dc32 300.0, 0.0, 0.0, 0.0      ; (x,y,z)    position
  dc32 3, 0, 0, 0                ; vertex count
  dc64 GIF_TAG(1, 0, 0, 0, FLG_PACKED, 1), REG_A_D
  dc64 SETREG_PRIM(PRIM_TRIANGLE, 1, 0, 0, 0, 0, 0, 0, 1), REG_PRIM
  dc64 GIF_TAG(3, 1, 0, 0, FLG_PACKED, 2), (REG_A_D|(REG_XYZ2<<4))
  dc64 SETREG_RGBAQ(255,0,0,0,0x3f80_0000), REG_RGBAQ
  dc32 (1800 << 4), (2000 << 4), (128 << 4), 0
  dc64 SETREG_RGBAQ(0,255,0,0,0x3f80_0000), REG_RGBAQ
  dc32 (1900 << 4), (2010 << 4), (128 << 4), 0
  dc64 SETREG_RGBAQ(0,0,255,0,0x3f80_0000), REG_RGBAQ
  dc32 (1800 << 4), (2010 << 4), (128 << 4), 0
draw_triangle_end:

  ;dc32 0, (128 << 4), (2000 << 4), (1800 << 4)
  ;dc32 1800.0, 2000.0, 128.0, 0
  ;dc32 0, (128 << 4), (2010 << 4), (1900 << 4),
  ;dc32 1900.0, 2010.0, 128.0, 0
  ;dc32 0, (128 << 4), (2010 << 4), (1800 << 4),
  ;dc32 1800.0, 2010.0, 128.0, 0

.align 128
black_screen:
  dc64 0x100000000000800e, REG_A_D
  dc64 0x00a0000, REG_FRAME_1            ; framebuffer width = 640/64
  dc64 0x8c, REG_ZBUF_1              ; 0-8 Zbuffer base, 24-27 Z format (32bit)
  dc32 27648, 30976                      ; X,Y offset
  dc64 REG_XYOFFSET_1
  dc16 0,639, 0,223                      ; x1,y1,x2,y2 - scissor window
  dc64 REG_SCISSOR_1
  dc64 1, REG_PRMODECONT                 ; refer to prim attributes
  dc64 1, REG_COLCLAMP
  dc64 0, REG_DTHE                       ; Dither off
  dc64 0x70000, REG_TEST_1
  dc64 0x30000, REG_TEST_1
  dc64 6, REG_PRIM
  dc64 0x3f80_0000_0000_0000, REG_RGBAQ  ; Background RGBA (A, blue, green, red)
  dc64 0x79006c00, REG_XYZ2              ; (1728.0, 1936.0, 0)
  dc64 0x87009400, REG_XYZ2              ; (2368.0, 2160.0, 0)
  dc64 0x70000, REG_TEST_1
black_screen_end:

.align 128
vu1_start:
  dc64 0x0, (VIF_MSCAL << 24)

.align 128
rotation_vu1:
  .binfile "rotation_vu1.bin"
rotation_vu1_end:

