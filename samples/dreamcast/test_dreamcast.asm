;; This is an adaptation of video.s from http://mc.pp.se/dc/sw.html.

.sh4
.include "powervr.inc"

;.org 0x0c000000
.org 0x8c010000

start:
  ; First, make sure to run in the P2 area (privileged mode, no cache).
  mov.l setup_cache_addr, r0
  nop
  mov.l p2_mask, r1
  or r1, r0
  jmp @r0
  nop

setup_cache:
  ; Now that the SH4 is in P2, it's safe to enable the cache.
  mov.l ccr_addr, r0
  mov.w ccr_data, r1
  mov.l r1, @r0
  ; After changing CCR, eight instructions must be executed before it's
  ; safe to enter a cached area such as P1.
  nop
  mov.l main_addr, r0 ; 1
  mov #0, r1          ; 2
  nop                 ; 3
  nop                 ; 4
  nop                 ; 5
  nop                 ; 6
  nop                 ; 7
  nop                 ; 8
  jmp @r0             ; go
  mov r1, r0

.align 32
p2_mask:
  .dc32 0xa0000000
setup_cache_addr:
  .dc32 setup_cache
main_addr:
  .dc32 main
ccr_addr:
  .dc32 0xff00001c
ccr_data:
  .dc16 0x090d

main:
  ;; Setup border color
  nop
  mov.l border_col, r1
  mov #127, r2
  mov.l r2, @r1

  ;; Setup NTSC and turn on video output enable.
  nop
  mov.l sync_cfg, r1
  nop
  mov.l sync_cfg_value, r2
  mov.l r2, @r1

  ;; Setup video size.
  mov.l full_video_size, r1
  nop
  mov.l full_video_size_value, r2
  mov.l r2, @r1

  ;; Setup display mode.
  mov.l display_mode, r1
  nop
  mov.l display_mode_value, r2
  mov.l r2, @r1

  ;; Setup hborder.
  mov.l hborder, r1
  nop
  mov.l hborder_value, r2
  mov.l r2, @r1

  ;; Setup vborder.
  mov.l vborder, r1
  nop
  mov.l vborder_value, r2
  mov.l r2, @r1

  ;; Setup hposition.
  mov.l vposition, r1
  nop
  mov.l hposition_value, r2
  mov.l r2, @r1

  ;; Setup vposition.
  mov.l vposition, r1
  nop
  mov.l vposition_value, r2
  mov.l r2, @r1

  ;; Setup display size.
  mov.l display_size, r1
  nop
  mov.l display_size_value, r2
  mov.l r2, @r1

  ;; Setup display memory region.
  mov.l display_memory_1, r1
  mov #0, r2
  mov.l r2, @r1

  nop
  mov.l display_memory_2, r1
  mov #0, r2
  mov.l r2, @r1

  ;; Put some stuff in the frame buffer.
  nop
  mov.l frame_buffer, r1
  mov #0, r2
  mov #10, r0
frame_buffer_loop:
  mov.l r2, @r1
  add #4, r1
  add #-1, r0
  cmp/eq #0, r0
  bt frame_buffer_loop

while_1:
  bra while_1
  nop

.align 32
border_col:
  .dc32 POWERVR_BORDER_COL
sync_cfg:
  .dc32 POWERVR_SYNC_CFG
sync_cfg_value:
  .dc32 POWERVR_SYNC_ENABLE | POWERVR_VIDEO_NTSC | POWERVR_VIDEO_INTERLACE
display_mode:
  .dc32 POWERVR_FB_DISPLAY_CFG
display_mode_value:
  .dc32 (3 << 2) | 1
display_memory_1:
  .dc32 POWERVR_FB_DISPLAY_ADDR1
display_memory_value_1:
  .dc32 0
display_memory_2:
  .dc32 POWERVR_FB_DISPLAY_ADDR1
display_memory_value_2:
  .dc32 0
frame_buffer:
  .dc32 0xa500_0000
full_video_size:
  .dc32 POWERVR_SYNC_LOAD
full_video_size_value:
  .dc32 (524 << 16) | 857
hborder:
  .dc32 POWERVR_HBORDER
hborder_value:
  .dc32 (126 << 16) | 837
vborder:
  .dc32 POWERVR_VBORDER
vborder_value:
  .dc32 (36 << 16) | 516
hposition:
  .dc32 POWERVR_HPOS
hposition_value:
  .dc32 164
vposition:
  .dc32 POWERVR_VPOS
vposition_value:
  .dc32 (18 << 16) | 18
display_size:
  .dc32 POWERVR_FB_DISPLAY_SIZE
display_size_value:
  .dc32 (1 << 20) | (239 << 10) | 639

