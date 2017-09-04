.ps2_ee

.include "playstation2/registers_ee.inc"
.include "playstation2/registers_gs_gp.inc"
.include "playstation2/registers_gs_priv.inc"
.include "playstation2/system_calls.inc"
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
  li $v0, SETREG_DISPFB(0, 10, FMT_PSMCT32, 0, 0)
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
  li $at, SETREG_DISPLAY(656, 36, 3, 0, 2559, 444 - 1) >> 32
  dsll32 $at, $at, 0
  li $v0, SETREG_DISPLAY(656, 36, 3, 0, 2559, 444 - 1) & 0xffffffff
  or $at, $at, $v0
  sd $at, ($v1)

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
  li $v1, red_screen
  sw $v1, 0x10($v0)         ; DMA02 ADDRESS
  li $v1, (red_screen_end - red_screen) / 16
  sw $v1, 0x20($v0)         ; DMA02 SIZE
  li $v1, 0x101
  sw $v1, ($v0)             ; start

  ;; Draw square
  jal dma02_wait
  nop
  li $v0, D2_CHCR
  li $v1, draw_square
  sw $v1, 0x10($v0)         ; DMA02 ADDRESS
  li $v1, (draw_square_end - draw_square) / 16
  sw $v1, 0x20($v0)         ; DMA02 SIZE
  ;lw $v1, ($v0)             ; start
  ;ori $v1, $v1, 0x105
  li $v1, 0x101
  sw $v1, ($v0)             ; start

  ;; Draw Image
  jal dma02_wait
  nop
  li $v0, D2_CHCR
  li $v1, image_packet
  sw $v1, 0x10($v0)         ; DMA02 ADDRESS
  li $v1, (image_packet_end - image_packet) / 16
  sw $v1, 0x20($v0)         ; DMA02 SIZE
  li $v1, 0x101
  sw $v1, ($v0)             ; start

  jal dma02_wait
  nop

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
draw_square:
  dc64 GIF_TAG(9, 1, 0, 0, FLG_PACKED, 1, 0x0), REG_A_D
  dc64 SETREG_PRIM(PRIM_TRIANGLE_FAN, 1, 0, 0, 0, 0, 0, 0, 1), REG_PRIM
  dc64 SETREG_RGBAQ(255,0,0,0,0x3f80_0000), REG_RGBAQ
  dc64 SETREG_XYZ2(1800 << 4, 1950 << 4, 128), REG_XYZ2
  dc64 SETREG_RGBAQ(0,255,0,0,0x3f80_0000), REG_RGBAQ
  dc64 SETREG_XYZ2(1800 << 4, 2010 << 4, 128), REG_XYZ2
  dc64 SETREG_RGBAQ(0,0,255,0,0x3f80_0000), REG_RGBAQ
  dc64 SETREG_XYZ2(2100 << 4, 2010 << 4, 128), REG_XYZ2
  dc64 SETREG_RGBAQ(0,255,255,0,0x3f80_0000), REG_RGBAQ
  dc64 SETREG_XYZ2(2100 << 4, 1950 << 4, 128), REG_XYZ2
draw_square_end:

.align 128
red_screen:
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
  dc64 0x3f80_0000_0000_0010, REG_RGBAQ  ; Background RGBA
  dc64 0x79006c00, REG_XYZ2              ; (1728.0, 1936.0, 0)
  dc64 0x87009400, REG_XYZ2              ; (2368.0, 2160.0, 0)
  dc64 0x70000, REG_TEST_1
red_screen_end:

.align 128
image_packet:
  dc64 GIF_TAG(4, 0, 0, 0, FLG_PACKED, 1, 0x0), REG_A_D
  dc64 SETREG_BITBLTBUF(0, 0, 0, 0 / 64, 64 / 64, FMT_PSMCT24), REG_BITBLTBUF
  dc64 SETREG_TRXPOS(0, 0, 64, 0, DIR_UL_LR), REG_TRXPOS
  dc64 SETREG_TRXREG(64, 64), REG_TRXREG
  dc64 SETREG_TRXDIR(XDIR_HOST_TO_LOCAL), REG_TRXDIR
  dc64 GIF_TAG(768, 1, 0, 0, FLG_IMAGE, 1, 0x0), REG_A_D
.binfile "image.raw"
image_packet_end:


