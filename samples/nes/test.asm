;; https://wiki.nesdev.com/w/index.php/NES_reference_guide

.6502

.include "nes.inc"

.org 0x7ff0
.ascii "NES"
.db 0x1a
; PRG-ROM size LSB.
.db 2
; CHR-ROM size LSB.
.db 0
; Flags 6-7.
.db 1
.db 0
; Mapper MSB/submapper.
.db 0
; PRG-ROM/CHR-ROM size MSB (really separate nybbles).
.db 0x00
; PRG-RAM/EEPROM size.
.db 0
; PRG-RAM size.
.ascii "NI2.1"

.org 0x8000
main:
  ;; Set interrupt flag and clear decimal mode.
  sei
  cld

  ;; Set stack pointer to 0xff.
  ldx #0xff
  txs

  ;; Disable NMI, rendering, DMC IRQs
  stx NES_PPU_CONTROL
  stx NES_PPU_MASK
  stx NES_APU_MOD_CONTROL

  ;; Wait for vertical blank.
wait_vblank_1:
  bit NES_PPU_STATUS
  bpl wait_vblank_1

  ;; Set up PPU.
  lda #0x00
  sta NES_PPU_CONTROL
  lda #0x0e
  sta NES_PPU_MASK

  ;; Set palettes
  lda NES_PPU_STATUS
  lda #0x3f
  sta NES_PPU_ADDRESS
  lda #0x00
  sta NES_PPU_ADDRESS
  lda #0x23
  sta NES_PPU_DATA
  lda #0x09
  sta NES_PPU_DATA
  lda #0x11
  sta NES_PPU_DATA
  lda #0x2c
  sta NES_PPU_DATA
  lda #0x05
  sta NES_PPU_DATA

  ;; Draw something in the first two tiles (starts at location 0x0000 in PPU).
  lda NES_PPU_STATUS
  lda #0x00
  sta NES_PPU_ADDRESS
  sta NES_PPU_ADDRESS

  ;; Tile 0 (clear, 2 bitplanes of 8x8 all cleared).
  ldx #16
  lda #0x00
clear_tile_0:
  sta NES_PPU_DATA
  dex
  bne clear_tile_0

  ;; Tile 1 (diagnol line).
  ldx #0
load_tile_0:
  lda tile_data_1, x
  sta NES_PPU_DATA
  inx
  cpx #16
  bne load_tile_0

  ;; Update screen area (PPU Nametable).
  lda NES_PPU_STATUS
  lda #0x20
  sta NES_PPU_ADDRESS
  lda #0x00
  sta NES_PPU_ADDRESS
  ldx #20
  lda #0x00
update_screen:
  sta NES_PPU_DATA
  eor #1
  dex
  bne update_screen

while_1:
  jmp while_1

vblank_interrupt:
  rti

tile_data_1:
  db 0x80, 0x40, 0x20, 0x10,  0x08, 0x04, 0x02, 0x01
  db 0x00, 0x00, 0x00, 0x00,  0x08, 0x04, 0x02, 0x01

.org 0xfffa
  dc16 vblank_interrupt
.org 0xfffc
  dc16 main

