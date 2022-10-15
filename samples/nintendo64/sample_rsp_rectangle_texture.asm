;; Simple Nintendo 64 sample with RSP code.
;;
;; Copyright 2022 - By Michael Kohn
;; https://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Sets up the Nintendo 64 display, clear parts of the screen with two colors,
;; and draws a rectangle with a texture using the RSP to initiate the drawing.

.mips

.include "nintendo64/rdp.inc"
.include "nintendo64/rsp.inc"
.include "nintendo64/system.inc"
.include "nintendo64/video_interface.inc"

.high_address 0x8010_ffff

.define COLOR(r,g,b) (r << 24) | (g << 16) | (b << 8) | 0xff

.org 0x8000_0000
cartridge_header:
  ;; PI_BSB_DOM1_LAT_REG
  ;; PI_BSB_DOM1_PGS_REG
  ;; PI_BSB_DOM1_PWD_REG
  ;; PI_BSB_DOM1_PGS_REG
  .db 0x80
  .db 0x37
  .db 0x12
  .db 0x40

  ;; Clock rate.
  .dc32 0x000f

  ;; Vector
  .dc32 start
  .dc32 0x1444

  ;; Complement checksum and checksum.
  .db "CRC1"
  .db "CRC2"

  ;; Unused.
  .dc32 0, 0

  ;; Program title.
  .db "NAKEN_ASM SAMPLE           "

  ;; Developer ID code.
  .db 0

  ;; Cartridge ID code.
  .dc16 0x0000

  ;; Country code (D=Germany, E=USA, J=Japan, P=Europe, U=Australia)
  .db 'E'

  ;; Version (00 = 1.0).
  .db 0

bootcode:
  ;; This was downloaded from Peter Lemon's git repo. It seems like it's
  ;; needed to initialize the hardware.
.binfile "bootcode.bin"

start:
  ;; Exception vector location in 32 bit mode is 0xbfc0_0000.
  ;; 0x1fc0_0000 to 0x1fc0_07bf is PIF Boot ROM.
  li $a0, PIF_BASE
  li $t0, 8
  sw $t0, PIF_RAM+0x3c($a0)

  ;; Enable interrupts.
  mfc0 $t0, CP0_STATUS
  ori $t0, $t0, 1
  mtc0 $t0, CP0_STATUS

  ;; Color the first 100 scan lines blue.
  ;; Color Bits: rrrr_rggg_ggbb_bbba (0000_0000_0011_1110).
  li $a0, KSEG1 | 0x0010_0000
  li $t0, 0x003e
  li $t1, 320 * 100
fill_loop_top:
  sh $t0, 0($a0)
  addiu $a0, $a0, 2
  addiu $t1, $t1, -1
  bne $t1, $0, fill_loop_top
  nop

  ;; Color the last 140 scan lines brown.
  ;; Color Bits: rrrr_rggg_ggbb_bbba (0011_1000_1100_0010).
  li $t0, 0x38c2
  li $t1, 320 * 140
fill_loop_bottom:
  sh $t0, 0($a0)
  addiu $a0, $a0, 2
  addiu $t1, $t1, -1
  bne $t1, $0, fill_loop_bottom
  nop

  ;; This reads reads in a set of [ 32 bit address, 32 bit value ]
  ;; from ROM memory. For each 32 bit value, it is written to the
  ;; 32 bit address which sets up the video display.
setup_video:
  li $t0, ntsc_320x240x16
  li $t1, (ntsc_320x240x16_end - ntsc_320x240x16) / 8
setup_video_loop:
  lw $a0, 0($t0)
  lw $t2, 4($t0)
  sw $t2, 0($a0)
  addiu $t0, $t0, 8
  addiu $t1, $t1, -1
  bne $t1, $0, setup_video_loop
  nop

  ;; Wait for vertical blank before drawing.
  jal wait_for_vblank
  nop

  ;; Make sure RSP is halted.
  li $a0, KSEG1 | RSP_CPU_BASE_REG
  li $t0, 0x02
  sw $t0, RSP_CPU_STATUS($a0)

  ;; Copy texture to video memory space beyond 2 pages of 320x240x2 + zbuffer.
  li $t1, (texture_end - texture_start) / 4
  li $a1, texture_start
  li $a0, KSEG1 | (0x0010_0000 + (((320 * 240 * 2) + (320 * 240 * 2)) * 2))
copy_texture_loop:
  lw $t2, 0($a1)
  sw $t2, 0($a0)
  addiu $a1, $a1, 4
  addiu $a0, $a0, 4
  addiu $t1, $t1, -1
  bne $t1, $0, copy_texture_loop
  nop

  ;; Copy RSP code from ROM to RSP instruction memory.
  ;; Must be done 32 bits at a time, not 64 bit.
setup_rsp:
  li $t1, (rsp_code_end - rsp_code_start) / 4
  li $a1, rsp_code_start
  li $a0, KSEG1 | RSP_IMEM
setup_rsp_loop:
  lw $t2, 0($a1)
  sw $t2, 0($a0)
  addiu $a1, $a1, 4
  addiu $a0, $a0, 4
  addiu $t1, $t1, -1
  bne $t1, $0, setup_rsp_loop
  nop

  ;; Copy RDP instructions from ROM to RSP data memory.
  ;; Must be done 32 bits at a time, not 64 bit.
setup_rdp:
  li $a0, KSEG1 | RSP_DMEM
  li $t1, (dp_setup_end - dp_setup) / 4
  li $a1, dp_setup
  sw $0, 0($a0)
  addiu $a0, $a0, 56
setup_rdp_loop:
  lw $t2, 0($a1)
  sw $t2, 0($a0)
  addiu $a1, $a1, 4
  addiu $a0, $a0, 4
  addiu $t1, $t1, -1
  bne $t1, $0, setup_rdp_loop
  nop

  ;; Reset RSP PC and clear halt to start it.
  li $a0, KSEG1 | RSP_PC
  sw $0, 0($a0)
  li $a0, KSEG1 | RSP_CPU_BASE_REG
  li $t0, 0x01
  sw $t0, RSP_CPU_STATUS($a0)

  ;; Signal to RSP code to start just the RDP setup commands.
  jal send_rdp_setup
  nop

  ;; Load the texture into TMEM.
  jal load_texture
  nop

  ;; Draw textured rectangle at (100.0, 90.0) to (150.0, 120.0).
  li $t0, 100 << 2
  li $t1,  90 << 2
  li $t2, 150 << 2
  li $t3, 120 << 2
  jal draw_rectangle
  nop

  ;; Draw textured rectangle at (200.0, 50.0) to (250.0, 90.0).
  li $t0, 200 << 2
  li $t1,  50 << 2
  li $t2, 250 << 2
  li $t3,  90 << 2
  jal draw_rectangle
  nop

  ;; Infinite loop at end of program.
while_1:
  b while_1
  nop

send_rdp_setup:
  ;; Signal RSP code to start.
  li $a0, KSEG1 | RSP_DMEM
  li $t0, 1 << 24
  sw $t0, 0($a0)
send_rdp_setup_wait_for_rsp:
  lb $t0, 0($a0)
  bne $t0, $0, send_rdp_setup_wait_for_rsp
  nop
  jr $ra
  nop

;; load_texture();
load_texture:
  li $a0, KSEG1 | RSP_DMEM
  ;; $t0 = width, $t1 = height, $t2 = address
  li $t0, 32
  li $t1, 9
  li $t2, 0x0010_0000 + (((320 * 240 * 2) + (320 * 240 * 2)) * 2)
  sll $t0, $t0, 16
  or $t0, $t0, $t1
  sw $t0, 40($a0)
  sw $t2, 48($a0)
  ;; Set command to load_texture.
  li $t0, 7 << 24
  sw $t0, 0($a0)
load_texture_wait_for_rsp:
  lw $t0, 0($a0)
  bne $t0, $0, load_texture_wait_for_rsp
  nop
  jr $ra
  nop

;; draw_rectange($t0=x0, $t1=y0, $t2=x1, $t3=y1, $t4=color);
draw_rectangle:
  li $a0, KSEG1 | RSP_DMEM
  ;; (XH, YH)
  sll $t0, $t0, 16
  or $t0, $t0, $t1
  sw $t0, 8($a0)
  ;; (XL, YL)
  sll $t2, $t2, 16
  or $t2, $t2, $t3
  sw $t2, 16($a0)
  sw $t4, 48($a0)
  ;; Set command to draw_rectangle_with_texture.
  li $t0, 6 << 24
  sw $t0, 0($a0)
draw_rectangle_wait_for_rsp:
  lw $t0, 0($a0)
  bne $t0, $0, draw_rectangle_wait_for_rsp
  nop
  jr $ra
  nop

wait_for_vblank:
  li $a0, KSEG1 | VI_BASE
  li $t0, 512
wait_for_vblank_loop:
  lw $t1, VI_V_CURRENT_LINE_REG($a0)
  bne $t0, $t1, wait_for_vblank_loop
  nop
  jr $ra
  nop

;; NTSC values found in the Reality Coprocessor.pdf
;; VI_CONTROL_REG:                   0 0011 0010 0000 1110
;; VI_TIMING_REG:  0000 0011 1110 0101 0010 0010 0011 1001
;; VI_BASE is the video interface base in KSEG1 (no TLB, no cache).
.align_bits 32
ntsc_320x240x16:
  .dc32 KSEG1 | VI_BASE | VI_CONTROL_REG,     0x0000_320e
  .dc32 KSEG1 | VI_BASE | VI_DRAM_ADDR_REG,   0xa010_0000
  .dc32 KSEG1 | VI_BASE | VI_H_WIDTH_REG,     0x0000_0140
  .dc32 KSEG1 | VI_BASE | VI_V_INTR_REG,      0x0000_0200
  .dc32 KSEG1 | VI_BASE | VI_TIMING_REG,      0x03e5_2239
  .dc32 KSEG1 | VI_BASE | VI_V_SYNC_REG,      0x0000_020d
  .dc32 KSEG1 | VI_BASE | VI_H_SYNC_REG,      0x0000_0c15
  .dc32 KSEG1 | VI_BASE | VI_H_SYNC_LEAP_REG, 0x0c15_0c15
  .dc32 KSEG1 | VI_BASE | VI_H_VIDEO_REG,     0x006c_02ec
  .dc32 KSEG1 | VI_BASE | VI_V_VIDEO_REG,     0x0025_01ff
  .dc32 KSEG1 | VI_BASE | VI_V_BURST_REG,     0x000e_0204
  .dc32 KSEG1 | VI_BASE | VI_X_SCALE_REG,     0x0000_0200
  .dc32 KSEG1 | VI_BASE | VI_Y_SCALE_REG,     0x0000_0400
ntsc_320x240x16_end:

.align_bits 64
dp_setup:
  .dc64 (DP_OP_SET_COLOR_IMAGE << 56) | (2 << 51) | (319 << 32) | 0x10_0000
  .dc64 (DP_OP_SET_Z_IMAGE << 56) | (0x10_0000 + (320 * 240 * 2))
  .dc64 (DP_OP_SET_SCISSOR << 56) | ((320 << 2) << 12) | (240 << 2)
  .dc64 (DP_OP_SYNC_PIPE << 56)
  .dc64 (DP_OP_SET_OTHER_MODES << 56) | (1 << 55) | (3 << 52)
dp_setup_end:

rsp_code_start:
  .binfile "rsp.bin"
rsp_code_end:

texture_start:
  .binfile "picture.raw"
texture_end:

