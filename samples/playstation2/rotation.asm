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

  ;; This an be done with DMA, but trying it with the main CPU
  ;; for now.  Copy GIF packet to VU1's data memory segment.
  ;li $v0, VU1_MICRO_MEM
  ;li $a1, (rotation_vu1_end - rotation_vu1_start) / 16
  ;li $v1, rotation_vu1_start
repeat_vu1_prog_copy:
  ;lq $a0, ($v1)
  ;sq $a0, ($v0)
  ;addi $v1, $v1, 16
  ;addi $v0, $v0, 16
  ;addi $a1, $a1, -1
  ;bnez $a1, repeat_vu1_prog_copy
  ;nop

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
  ;; Draw picture
  jal draw_screen
  nop

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
  li $v1, draw_triangle
  sw $t0, 20($v1)
  ;sw $t0, 4($v1)

  ;; Update sine
  li $v1, _sin_table_512
  add $v1, $v1, $v0
  lw $t0, 0($v1)
  li $v1, draw_triangle
  sw $t0, 16($v1)
  ;sw $t0, 0($v1)

  ;; Flush cache
  lui $a0, 0
  li $v1, FlushCache
  syscall

  ;; Should be able to flush the cache with instructions...
  ;li $v1, draw_triangle
  ;cache dhwoin, 0($v1)
  ;cache dhwbin, 0($v1)
  ;sync.l

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

  li $v1, VIF1_STAT
wait_on_vu1:
  lw $v0, ($v1)
  andi $v0, $s0, 0x04
  bne $v0, $0, wait_on_vu1
  nop

  ;; This can be done with DMA, but trying it with the main CPU
  ;; for now.  Copy GIF packet to VU1's data memory segment.
  ;li $v0, VU1_VU_MEM
  ;li $a1, (draw_triangle_end - draw_triangle) / 16
  ;li $v1, draw_triangle
repeat_vu1_data_copy:
  ;lq $a0, ($v1)
  ;sq $a0, ($v0)
  ;addi $v1, $v1, 16
  ;addi $v0, $v0, 16
  ;addi $a1, $a1, -1
  ;bnez $a1, repeat_vu1_data_copy
  ;nop

  ;; Send data (VIF_UNPACK) and start VU1 (VIF_MSCAL)
  li $v0, D1_CHCR
  li $v1, vif_packet_start
  sw $v1, 0x10($v0)                   ; DMA01 ADDRESS
  li $v1, ((vif_packet_end - vif_packet_start) / 16)
  sw $v1, 0x20($v0)                   ; DMA01 SIZE
  li $v1, 0x101
  sw $v1, ($v0)                       ; start

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
vif_packet_start:
  dc32 (VIF_FLUSHE << 24)
  dc32 (VIF_STMOD << 24)
  dc32 (VIF_STCYCL << 24)|(1 << 8)|1
  dc32 (VIF_UNPACK_V4_32 << 24)|(((draw_triangle_end - draw_triangle) / 16) << 16)
draw_triangle:
  dc32   0.0, 0.0, 0.0, 0.0       ; sin(rx), cos(rx), sin(ry), cos(ry)
  dc32   0.0, 0.0, 0.0, 0.0       ; sin(rz), cos(rz)
  dc32 1900.0, 2100.0, 2048.0, 0.0 ; (x,y,z)    position
  dc32 3, 0, 0, 1                 ; vertex count, do_rot_x, do_rot_y, do_rot_z
  dc64 GIF_TAG(1, 0, 0, 0, FLG_PACKED, 1), REG_A_D
  dc64 SETREG_PRIM(PRIM_TRIANGLE, 1, 0, 0, 0, 0, 0, 0, 0), REG_PRIM
  dc64 GIF_TAG(3, 1, 0, 0, FLG_PACKED, 2), (REG_A_D|(REG_XYZ2<<4))
  dc64 SETREG_RGBAQ(255,0,0,0,0x3f80_0000), REG_RGBAQ
  dc32 -100.0, -100.0, 0.0, 0
  dc64 SETREG_RGBAQ(0,255,0,0,0x3f80_0000), REG_RGBAQ
  dc32 -100.0, 110.0, 0.0, 0
  dc64 SETREG_RGBAQ(0,0,255,0,0x3f80_0000), REG_RGBAQ
  dc32 0.0, 110.0, 0.0, 0
draw_triangle_end:
vu1_start:
  dc32 (VIF_MSCAL << 24), 0, 0, 0
vif_packet_end:

.align 128
black_screen:
  dc64 GIF_TAG(14, 1, 0, 0, FLG_PACKED, 1), REG_A_D
  dc64 0x00a0000, REG_FRAME_1            ; framebuffer width = 640/64
  dc64 0x8c, REG_ZBUF_1              ; 0-8 Zbuffer base, 24-27 Z format (32bit)
  dc64 SETREG_XYOFFSET(1728 << 4, 1936 << 4), REG_XYOFFSET_1
  dc64 SETREG_SCISSOR(0,639,0,447), REG_SCISSOR_1
  dc64 1, REG_PRMODECONT                 ; refer to prim attributes
  dc64 1, REG_COLCLAMP
  dc64 0, REG_DTHE                       ; Dither off
  dc64 0x70000, REG_TEST_1
  dc64 0x30000, REG_TEST_1
  dc64 PRIM_SPRITE, REG_PRIM
  dc64 0x3f80_0000_0000_0000, REG_RGBAQ  ; Background RGBA (A, blue, green, red)
  dc64 SETREG_XYZ2(1728 << 4, 1936 << 4, 0), REG_XYZ2
  dc64 SETREG_XYZ2(2368 << 4, 2384 << 4, 0), REG_XYZ2
  dc64 0x70000, REG_TEST_1
black_screen_end:

.align 32
_sin_table_512:
  dc32  0.0000,  0.0123,  0.0245,  0.0368,
  dc32  0.0490,  0.0613,  0.0735,  0.0858,
  dc32  0.0980,  0.1102,  0.1223,  0.1345,
  dc32  0.1467,  0.1588,  0.1709,  0.1829,
  dc32  0.1950,  0.2070,  0.2190,  0.2309,
  dc32  0.2429,  0.2547,  0.2666,  0.2784,
  dc32  0.2901,  0.3019,  0.3135,  0.3252,
  dc32  0.3367,  0.3482,  0.3597,  0.3711,
  dc32  0.3825,  0.3938,  0.4050,  0.4162,
  dc32  0.4274,  0.4384,  0.4494,  0.4603,
  dc32  0.4712,  0.4820,  0.4927,  0.5033,
  dc32  0.5139,  0.5244,  0.5348,  0.5451,
  dc32  0.5553,  0.5655,  0.5756,  0.5855,
  dc32  0.5954,  0.6052,  0.6150,  0.6246,
  dc32  0.6341,  0.6436,  0.6529,  0.6621,
  dc32  0.6713,  0.6803,  0.6893,  0.6981,
  dc32  0.7068,  0.7154,  0.7240,  0.7324,
  dc32  0.7407,  0.7489,  0.7569,  0.7649,
  dc32  0.7727,  0.7805,  0.7881,  0.7956,
  dc32  0.8029,  0.8102,  0.8173,  0.8243,
  dc32  0.8312,  0.8379,  0.8446,  0.8511,
  dc32  0.8575,  0.8637,  0.8698,  0.8758,
  dc32  0.8817,  0.8874,  0.8930,  0.8984,
  dc32  0.9037,  0.9089,  0.9140,  0.9189,
  dc32  0.9237,  0.9283,  0.9328,  0.9371,
  dc32  0.9413,  0.9454,  0.9493,  0.9531,
  dc32  0.9568,  0.9602,  0.9636,  0.9668,
  dc32  0.9699,  0.9728,  0.9756,  0.9782,
  dc32  0.9806,  0.9830,  0.9852,  0.9872,
  dc32  0.9891,  0.9908,  0.9924,  0.9938,
  dc32  0.9951,  0.9962,  0.9972,  0.9981,
  dc32  0.9988,  0.9993,  0.9997,  0.9999,
  dc32  1.0000,  0.9999,  0.9997,  0.9994,
  dc32  0.9988,  0.9982,  0.9974,  0.9964,
  dc32  0.9953,  0.9940,  0.9926,  0.9910,
  dc32  0.9893,  0.9874,  0.9854,  0.9833,
  dc32  0.9810,  0.9785,  0.9759,  0.9732,
  dc32  0.9703,  0.9672,  0.9640,  0.9607,
  dc32  0.9572,  0.9536,  0.9498,  0.9459,
  dc32  0.9419,  0.9377,  0.9333,  0.9289,
  dc32  0.9243,  0.9195,  0.9146,  0.9096,
  dc32  0.9044,  0.8991,  0.8937,  0.8881,
  dc32  0.8824,  0.8766,  0.8706,  0.8645,
  dc32  0.8583,  0.8519,  0.8454,  0.8388,
  dc32  0.8321,  0.8252,  0.8182,  0.8111,
  dc32  0.8039,  0.7965,  0.7890,  0.7814,
  dc32  0.7737,  0.7659,  0.7580,  0.7499,
  dc32  0.7417,  0.7335,  0.7251,  0.7166,
  dc32  0.7080,  0.6992,  0.6904,  0.6815,
  dc32  0.6725,  0.6633,  0.6541,  0.6448,
  dc32  0.6354,  0.6258,  0.6162,  0.6065,
  dc32  0.5967,  0.5868,  0.5769,  0.5668,
  dc32  0.5566,  0.5464,  0.5361,  0.5257,
  dc32  0.5152,  0.5047,  0.4941,  0.4834,
  dc32  0.4726,  0.4617,  0.4508,  0.4398,
  dc32  0.4288,  0.4177,  0.4065,  0.3953,
  dc32  0.3840,  0.3726,  0.3612,  0.3497,
  dc32  0.3382,  0.3267,  0.3150,  0.3034,
  dc32  0.2917,  0.2799,  0.2681,  0.2563,
  dc32  0.2444,  0.2325,  0.2205,  0.2086,
  dc32  0.1966,  0.1845,  0.1724,  0.1604,
  dc32  0.1482,  0.1361,  0.1239,  0.1117,
  dc32  0.0996,  0.0873,  0.0751,  0.0629,
  dc32  0.0506,  0.0384,  0.0261,  0.0139,
  dc32  0.0016, -0.0107, -0.0229, -0.0352,
  dc32 -0.0475, -0.0597, -0.0719, -0.0842,
  dc32 -0.0964, -0.1086, -0.1208, -0.1329,
  dc32 -0.1451, -0.1572, -0.1693, -0.1814,
  dc32 -0.1934, -0.2054, -0.2174, -0.2294,
  dc32 -0.2413, -0.2532, -0.2650, -0.2769,
  dc32 -0.2886, -0.3003, -0.3120, -0.3236,
  dc32 -0.3352, -0.3468, -0.3582, -0.3697,
  dc32 -0.3810, -0.3923, -0.4036, -0.4148,
  dc32 -0.4259, -0.4370, -0.4480, -0.4589,
  dc32 -0.4698, -0.4806, -0.4913, -0.5019,
  dc32 -0.5125, -0.5230, -0.5334, -0.5437,
  dc32 -0.5540, -0.5642, -0.5743, -0.5842,
  dc32 -0.5942, -0.6040, -0.6137, -0.6233,
  dc32 -0.6329, -0.6423, -0.6517, -0.6609,
  dc32 -0.6701, -0.6792, -0.6881, -0.6970,
  dc32 -0.7057, -0.7143, -0.7229, -0.7313,
  dc32 -0.7396, -0.7478, -0.7559, -0.7639,
  dc32 -0.7717, -0.7795, -0.7871, -0.7946,
  dc32 -0.8020, -0.8092, -0.8164, -0.8234,
  dc32 -0.8303, -0.8371, -0.8437, -0.8502,
  dc32 -0.8566, -0.8629, -0.8690, -0.8750,
  dc32 -0.8809, -0.8866, -0.8923, -0.8977,
  dc32 -0.9031, -0.9083, -0.9133, -0.9183,
  dc32 -0.9230, -0.9277, -0.9322, -0.9366,
  dc32 -0.9408, -0.9449, -0.9488, -0.9526,
  dc32 -0.9563, -0.9598, -0.9632, -0.9664,
  dc32 -0.9695, -0.9724, -0.9752, -0.9778,
  dc32 -0.9803, -0.9827, -0.9849, -0.9869,
  dc32 -0.9888, -0.9906, -0.9922, -0.9936,
  dc32 -0.9950, -0.9961, -0.9971, -0.9980,
  dc32 -0.9987, -0.9992, -0.9996, -0.9999,
  dc32 -1.0000, -1.0000, -0.9998, -0.9994,
  dc32 -0.9989, -0.9983, -0.9975, -0.9965,
  dc32 -0.9954, -0.9942, -0.9928, -0.9912,
  dc32 -0.9895, -0.9877, -0.9857, -0.9836,
  dc32 -0.9813, -0.9788, -0.9762, -0.9735,
  dc32 -0.9706, -0.9676, -0.9644, -0.9611,
  dc32 -0.9577, -0.9541, -0.9503, -0.9464,
  dc32 -0.9424, -0.9382, -0.9339, -0.9295,
  dc32 -0.9249, -0.9201, -0.9153, -0.9103,
  dc32 -0.9051, -0.8998, -0.8944, -0.8888,
  dc32 -0.8832, -0.8773, -0.8714, -0.8653,
  dc32 -0.8591, -0.8528, -0.8463, -0.8397,
  dc32 -0.8330, -0.8261, -0.8191, -0.8120,
  dc32 -0.8048, -0.7975, -0.7900, -0.7824,
  dc32 -0.7747, -0.7669, -0.7590, -0.7510,
  dc32 -0.7428, -0.7345, -0.7262, -0.7177,
  dc32 -0.7091, -0.7004, -0.6916, -0.6827,
  dc32 -0.6736, -0.6645, -0.6553, -0.6460,
  dc32 -0.6366, -0.6271, -0.6175, -0.6078,
  dc32 -0.5980, -0.5881, -0.5782, -0.5681,
  dc32 -0.5580, -0.5477, -0.5374, -0.5271,
  dc32 -0.5166, -0.5061, -0.4954, -0.4847,
  dc32 -0.4740, -0.4631, -0.4522, -0.4413,
  dc32 -0.4302, -0.4191, -0.4080, -0.3967,
  dc32 -0.3854, -0.3741, -0.3627, -0.3512,
  dc32 -0.3397, -0.3282, -0.3166, -0.3049,
  dc32 -0.2932, -0.2814, -0.2696, -0.2578,
  dc32 -0.2459, -0.2340, -0.2221, -0.2101,
  dc32 -0.1981, -0.1861, -0.1740, -0.1619,
  dc32 -0.1498, -0.1377, -0.1255, -0.1133,
  dc32 -0.1011, -0.0889, -0.0767, -0.0645,
  dc32 -0.0522, -0.0400, -0.0277, -0.0155,

_cos_table_512:
  dc32  1.0000,  0.9999,  0.9997,  0.9993,
  dc32  0.9988,  0.9981,  0.9973,  0.9963,
  dc32  0.9952,  0.9939,  0.9925,  0.9909,
  dc32  0.9892,  0.9873,  0.9853,  0.9831,
  dc32  0.9808,  0.9783,  0.9757,  0.9730,
  dc32  0.9701,  0.9670,  0.9638,  0.9605,
  dc32  0.9570,  0.9534,  0.9496,  0.9457,
  dc32  0.9416,  0.9374,  0.9331,  0.9286,
  dc32  0.9240,  0.9192,  0.9143,  0.9093,
  dc32  0.9041,  0.8988,  0.8933,  0.8878,
  dc32  0.8820,  0.8762,  0.8702,  0.8641,
  dc32  0.8579,  0.8515,  0.8450,  0.8384,
  dc32  0.8316,  0.8248,  0.8178,  0.8106,
  dc32  0.8034,  0.7960,  0.7886,  0.7810,
  dc32  0.7732,  0.7654,  0.7574,  0.7494,
  dc32  0.7412,  0.7329,  0.7245,  0.7160,
  dc32  0.7074,  0.6987,  0.6898,  0.6809,
  dc32  0.6719,  0.6627,  0.6535,  0.6442,
  dc32  0.6347,  0.6252,  0.6156,  0.6059,
  dc32  0.5961,  0.5862,  0.5762,  0.5661,
  dc32  0.5560,  0.5457,  0.5354,  0.5250,
  dc32  0.5146,  0.5040,  0.4934,  0.4827,
  dc32  0.4719,  0.4610,  0.4501,  0.4391,
  dc32  0.4281,  0.4170,  0.4058,  0.3945,
  dc32  0.3832,  0.3719,  0.3605,  0.3490,
  dc32  0.3375,  0.3259,  0.3143,  0.3026,
  dc32  0.2909,  0.2791,  0.2673,  0.2555,
  dc32  0.2436,  0.2317,  0.2198,  0.2078,
  dc32  0.1958,  0.1837,  0.1717,  0.1596,
  dc32  0.1474,  0.1353,  0.1231,  0.1110,
  dc32  0.0988,  0.0865,  0.0743,  0.0621,
  dc32  0.0498,  0.0376,  0.0253,  0.0131,
  dc32  0.0008, -0.0115, -0.0237, -0.0360,
  dc32 -0.0482, -0.0605, -0.0727, -0.0850,
  dc32 -0.0972, -0.1094, -0.1216, -0.1337,
  dc32 -0.1459, -0.1580, -0.1701, -0.1822,
  dc32 -0.1942, -0.2062, -0.2182, -0.2302,
  dc32 -0.2421, -0.2540, -0.2658, -0.2776,
  dc32 -0.2894, -0.3011, -0.3128, -0.3244,
  dc32 -0.3360, -0.3475, -0.3590, -0.3704,
  dc32 -0.3818, -0.3931, -0.4043, -0.4155,
  dc32 -0.4266, -0.4377, -0.4487, -0.4596,
  dc32 -0.4705, -0.4813, -0.4920, -0.5026,
  dc32 -0.5132, -0.5237, -0.5341, -0.5444,
  dc32 -0.5547, -0.5648, -0.5749, -0.5849,
  dc32 -0.5948, -0.6046, -0.6143, -0.6240,
  dc32 -0.6335, -0.6430, -0.6523, -0.6615,
  dc32 -0.6707, -0.6797, -0.6887, -0.6975,
  dc32 -0.7063, -0.7149, -0.7234, -0.7318,
  dc32 -0.7401, -0.7483, -0.7564, -0.7644,
  dc32 -0.7722, -0.7800, -0.7876, -0.7951,
  dc32 -0.8025, -0.8097, -0.8168, -0.8239,
  dc32 -0.8307, -0.8375, -0.8442, -0.8507,
  dc32 -0.8570, -0.8633, -0.8694, -0.8754,
  dc32 -0.8813, -0.8870, -0.8926, -0.8981,
  dc32 -0.9034, -0.9086, -0.9136, -0.9186,
  dc32 -0.9233, -0.9280, -0.9325, -0.9368,
  dc32 -0.9411, -0.9451, -0.9491, -0.9529,
  dc32 -0.9565, -0.9600, -0.9634, -0.9666,
  dc32 -0.9697, -0.9726, -0.9754, -0.9780,
  dc32 -0.9805, -0.9828, -0.9850, -0.9871,
  dc32 -0.9890, -0.9907, -0.9923, -0.9937,
  dc32 -0.9950, -0.9962, -0.9972, -0.9980,
  dc32 -0.9987, -0.9993, -0.9997, -0.9999,
  dc32 -1.0000, -0.9999, -0.9997, -0.9994,
  dc32 -0.9989, -0.9982, -0.9974, -0.9965,
  dc32 -0.9953, -0.9941, -0.9927, -0.9911,
  dc32 -0.9894, -0.9876, -0.9856, -0.9834,
  dc32 -0.9811, -0.9787, -0.9761, -0.9733,
  dc32 -0.9704, -0.9674, -0.9642, -0.9609,
  dc32 -0.9574, -0.9538, -0.9501, -0.9462,
  dc32 -0.9421, -0.9380, -0.9336, -0.9292,
  dc32 -0.9246, -0.9198, -0.9149, -0.9099,
  dc32 -0.9048, -0.8995, -0.8940, -0.8885,
  dc32 -0.8828, -0.8770, -0.8710, -0.8649,
  dc32 -0.8587, -0.8523, -0.8459, -0.8393,
  dc32 -0.8325, -0.8257, -0.8187, -0.8116,
  dc32 -0.8043, -0.7970, -0.7895, -0.7819,
  dc32 -0.7742, -0.7664, -0.7585, -0.7504,
  dc32 -0.7423, -0.7340, -0.7256, -0.7171,
  dc32 -0.7085, -0.6998, -0.6910, -0.6821,
  dc32 -0.6731, -0.6639, -0.6547, -0.6454,
  dc32 -0.6360, -0.6265, -0.6168, -0.6071,
  dc32 -0.5974, -0.5875, -0.5775, -0.5674,
  dc32 -0.5573, -0.5471, -0.5368, -0.5264,
  dc32 -0.5159, -0.5054, -0.4947, -0.4841,
  dc32 -0.4733, -0.4624, -0.4515, -0.4406,
  dc32 -0.4295, -0.4184, -0.4072, -0.3960,
  dc32 -0.3847, -0.3734, -0.3619, -0.3505,
  dc32 -0.3390, -0.3274, -0.3158, -0.3041,
  dc32 -0.2924, -0.2807, -0.2689, -0.2570,
  dc32 -0.2452, -0.2333, -0.2213, -0.2093,
  dc32 -0.1973, -0.1853, -0.1732, -0.1611,
  dc32 -0.1490, -0.1369, -0.1247, -0.1125,
  dc32 -0.1003, -0.0881, -0.0759, -0.0637,
  dc32 -0.0514, -0.0392, -0.0269, -0.0147,
  dc32 -0.0024,  0.0099,  0.0221,  0.0344,
  dc32  0.0467,  0.0589,  0.0711,  0.0834,
  dc32  0.0956,  0.1078,  0.1200,  0.1321,
  dc32  0.1443,  0.1564,  0.1685,  0.1806,
  dc32  0.1926,  0.2047,  0.2167,  0.2286,
  dc32  0.2405,  0.2524,  0.2643,  0.2761,
  dc32  0.2879,  0.2996,  0.3113,  0.3229,
  dc32  0.3345,  0.3460,  0.3575,  0.3689,
  dc32  0.3803,  0.3916,  0.4029,  0.4141,
  dc32  0.4252,  0.4363,  0.4473,  0.4582,
  dc32  0.4691,  0.4799,  0.4906,  0.5012,
  dc32  0.5118,  0.5223,  0.5327,  0.5431,
  dc32  0.5533,  0.5635,  0.5736,  0.5836,
  dc32  0.5935,  0.6033,  0.6131,  0.6227,
  dc32  0.6323,  0.6417,  0.6511,  0.6603,
  dc32  0.6695,  0.6786,  0.6875,  0.6964,
  dc32  0.7051,  0.7138,  0.7223,  0.7307,
  dc32  0.7391,  0.7473,  0.7554,  0.7633,
  dc32  0.7712,  0.7790,  0.7866,  0.7941,
  dc32  0.8015,  0.8088,  0.8159,  0.8230,
  dc32  0.8299,  0.8366,  0.8433,  0.8498,
  dc32  0.8562,  0.8625,  0.8686,  0.8747,
  dc32  0.8805,  0.8863,  0.8919,  0.8974,
  dc32  0.9027,  0.9079,  0.9130,  0.9179,
  dc32  0.9227,  0.9274,  0.9319,  0.9363,
  dc32  0.9405,  0.9446,  0.9486,  0.9524,
  dc32  0.9561,  0.9596,  0.9630,  0.9662,
  dc32  0.9693,  0.9722,  0.9750,  0.9777,
  dc32  0.9802,  0.9825,  0.9847,  0.9868,
  dc32  0.9887,  0.9905,  0.9921,  0.9936,
  dc32  0.9949,  0.9960,  0.9971,  0.9979,
  dc32  0.9986,  0.9992,  0.9996,  0.9999,

.align 128
vif_packet_mpg_start:
  dc32 0x0, 0x0, 0x0, (VIF_MPG << 24)|(((rotation_vu1_end - rotation_vu1_start) / 8) << 16)
rotation_vu1_start:
  .binfile "rotation_vu1.bin"
rotation_vu1_end:
vif_packet_mpg_end:

