.mips

.org 0x10000
start:
  ;; DMA Primary Control Register (0x1f80_10f0)
  ;; Enable DMA 4 (SPU2)
  li $v1, 0x1f80_10f0
  lw $t0, 0($v1)
  li $t1, 0x0008_0000
  or $t0, $t0, $t1
  sw $t0, 0($v1)

.if 0
  ;; DMA Primary Control Register (0x1f80_10f0)
  ;; Enable DMA 7 (SPU2)
  li $v1, 0x1f80_10f0
  lw $t0, 0($v1)
  li $t1, 0x8000_0000
  or $t0, $t0, $t1
  sw $t0, 0($v1)

  ;; DMA Primary Control Register DPCR2 (0x1f80_1570)
  ;; Enable DMA 8 (SPU2)
  li $v1, 0x1f80_1570
  lw $t0, 0($v1)
  li $t1, 0x0000_0008
  or $t0, $t0, $t1
  sw $t0, 0($v1)
.endif

  ;; SPU  Base 0x1f80_0000
  ;; SPU2 Base 0xbf90_0000
  ;li $v0, 0x1f80_0000
  li $v0, 0x1f90_0000

  ;; SPU2 attributes
  ;li $t0, 0x0004
  li $t0, 0x0020
  sh $t0, 0x019a($v0)

  ;; SPUCNT  0x1f80_1daa (SPU control register)
  ;; SPUSTAT 0x1f80_1dae (SPU control register)
  li $v1, 0x1f80_1da0
.if 0
  li $t0, 0
  sw $t0, 0xa($v1)
wait_spucnt_off:
  sw $t0, 0xe($v1)
  bnez $t0, wait_spucnt_off
  nop

  ;; Enable SPU2 (bit 15), DMAWrite (bits 4-5 = 2), External Audio (bit 1)
  li $t0, 0x8022
  sw $t0, 0xa($v1)
wait_spucnt_on:
  sw $t0, 0xe($v1)
  andi $t0, $t0, 0x8000
  beqz $t0, wait_spucnt_on
  nop
.endif

  ;li $t0, 0x4
  ;sw $t0, 0xc($v1)

  ;; SPU2 KEYOFF to all 24 voices
  li $t0, 0xffff
  sh $t0, 0x01a4($v0)
  sh $t0, 0x01a6($v0)

  ;; SPU Voice 0 Transfer Address
  li $t0, 0x2800
  sh $zero, 0x1a8($v0)
  sh $t0, 0x1aa($v0)

  ;; DMA 4 D_MADR (0x1f80_10c0) source address
  ;; DMA 4 D_BCRL (0x1f80_10c4) block control
  ;; DMA 4 D_CHCR (0x1f80_10c8) channel control
  ;; DMA 8 D_MADR (0x1f80_1500) source address
  ;; DMA 8 D_BCRL (0x1f80_1504) block control
  ;; DMA 8 D_CHCR (0x1f80_1508) channel control
  ;li $v1, 0x1f80_1500
  li $v1, 0x1f80_10c0
  li $t0, 0x2_0000
  sw $t0, 0($v1)

  ;; Block Size 16, Words * Blocks = 294 (25126 bytes total)
  ;; DMA send sound data.
  ;li $t0, (394 << 16) | 4
  li $t0, (1576 << 16) | 4
  ;li $t0, (1912 << 16) | 4
  sw $t0, 4($v1)
  li $t0, 0x0100_0201
  sw $t0, 8($v1)
  li $t0, 0x0100_0000
wait_dma:
  lw $t2, 8($v1)
  and $t1, $t2, $t0 
  bnez $t1, wait_dma
  nop

  ;; SPU Voice 0 Start Address
  li $t0, 0x2800
  sh $zero, 0x1c0($v0)
  sh $t0, 0x1c2($v0)

  ;; SPU Volume Control MVOLL,MVOLR Core 0
  li $t0, 0x3fff
  sh $t0, 0x760($v0)
  sh $t0, 0x762($v0)

  ;; SPU Volume Control MVOLL,MVOLR Core 1
  sh $t0, 0x788($v0)
  sh $t0, 0x78a($v0)

  ;; SPU Volume Control BVOLL,BVOLR
  li $t0, 0x7fff
  sh $t0, 0x76c($v0)
  sh $t0, 0x76e($v0)

  ;; SPU Volume Control AVOLL,AVOLR Core 0
  sh $t0, 0x768($v0)
  sh $t0, 0x76a($v0)

  ;; SPU Volume Control AVOLL,AVOLR Core 1
  ;sh $t0, 0x790($v0)
  ;sh $t0, 0x792($v0)

  ;; SPU Volume Control EVOLL,EVOLR
  ;sh $t0, 0x764($v0)
  ;sh $t0, 0x766($v0)
  ;sh $zero, 0x764($v0)
  ;sh $zero, 0x766($v0)

.if 0
  sh $t0, 0x776($v0)
  sh $t0, 0x778($v0)
  sh $t0, 0x77a($v0)
  sh $t0, 0x77c($v0)
.endif

  ;; SPU Mixer Control VMIXL,VMIXR
  ;; SPU Mixer Control VMIXEL,VMIXER (set to 0)
  li $t0, 0x1
  sh $t0, 0x18a($v0)
  sh $t0, 0x192($v0)
  sh $zero, 0x18e($v0)
  sh $zero, 0x196($v0)

  sh $t0, 0x58a($v0)
  sh $t0, 0x592($v0)

  sh $t0, 0x188($v0)
  sh $t0, 0x190($v0)
  sh $zero, 0x18c($v0)
  sh $zero, 0x194($v0)

  ;; SPU NON, PMON, IRQA should be set to 0.
  sh $zero, 0x0180($v0)
  sh $zero, 0x0182($v0)
  sh $zero, 0x0184($v0)
  sh $zero, 0x0186($v0)
  sh $zero, 0x019c($v0)
  sh $zero, 0x019e($v0)

  ;; Work address
  li $t0, 0x003f
  sh $t0, 0x033c($v0)
  li $t0, 0xffff
  sh $t0, 0x02e0($v0)
  sh $t0, 0x02e2($v0)

  ;; SPU Mixer Control MMIX
  li $t0, 0xf00
  sh $t0, 0x198($v0)
  li $t0, 0xf0c
  sh $t0, 0x598($v0)

  ;; SPU ADSR1 06
  li $t0, 0x00ff
  sh $t0, 0x0006($v0)
  ;; SPU ADSR2 08
  li $t0, 0x1fc0
  sh $t0, 0x0008($v0)

  ;; Voice 0 Left / Right Volume Control VOLL/VOLR
  li $t0, 0x3fff
  sh $t0, 0x0000($v0)
  sh $t0, 0x0002($v0)

  ;; SPU Voice 0 Sample Rate (0x1000 = 44100Hz) PITCH
  ;li $t0, 0x1000
  li $t0, 0x0eb3
  sh $t0, 0x0004($v0)

  ;; SPU Voice 0 Key ON KON0
  li $t0, 0x0001
  sh $t0, 0x01a0($v0)

wait:
  ;lh $t8, 0x000c($v0) ; VOLX
  lh $t6, 0x0340($v0) ; ENDX
  lh $t7, 0x000a($v0) ; ENVX
  lh $t8, 0x01ca($v0) ; NAXL
  lh $t9, 0x01c8($v0) ; NAXH
  ;lh $t8, 0x01c2($v0) ; Start Address
  b wait
  nop

