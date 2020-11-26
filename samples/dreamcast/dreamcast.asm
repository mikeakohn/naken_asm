;; This is an adaptation of video.s from http://mc.pp.se/dc/sw.html.

.sh4
.include "powervr.inc"

;; Page 340
;; 640 * 240 * 4 = 614,000 (less than 1MB)
TILE_WIDTH equ (640 / 32)
TILE_HEIGHT equ (480 / 32)
DISPLAY_BUFFER_1 equ 0
DISPLAY_BUFFER_2 equ (640 * 240 * 4)
; TILE_BUFFER_ADDRESS grows downward in memory, START should be higher.
; size = 64 * (640 / 32) * (480 / 32) = 64 * 20 * 15 = 19,200
TILE_BUFFER_ADDRESS_END equ 0
TILE_BUFFER_ADDRESS_START equ 64 * TILE_WIDTH * TILE_HEIGHT
; TILE_DESCRIPTOR_BUFFER
; size = 24 + 6 * (640 / 32) * (480 / 32)
VERTEX_BUFFER_ADDRESS equ TILE_BUFFER_ADDRESS_START

;.org 0x0c000000
.org 0x8c010000

start:
  ; First, make sure to run in the P2 area (privileged mode, no cache).
  mov.l setup_cache_addr, r0
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
  ;; Setup NTSC and turn on video output enable.
  mov.l sync_cfg, r1
  mov.l sync_cfg_value, r2
  mov.l r2, @r1

  ;; Setup video sync.
  mov.l display_sync_load, r1
  mov.l display_sync_load_value, r2
  mov.l r2, @r1

  mov.l display_sync_width, r1
  mov.l display_sync_width_value, r2
  mov.l r2, @r1

  ;; Setup display mode.
  mov.l display_mode, r1
  mov.l display_mode_value, r2
  mov.l r2, @r1

  ;; Setup hborder.
  mov.l hborder, r1
  mov.l hborder_value, r2
  mov.l r2, @r1

  ;; Setup vborder.
  mov.l vborder, r1
  mov.l vborder_value, r2
  mov.l r2, @r1

  ;; Setup hposition.
  mov.l hposition, r1
  mov.l hposition_value, r2
  mov.l r2, @r1

  ;; Setup vposition.
  mov.l vposition, r1
  mov.l vposition_value, r2
  mov.l r2, @r1

  ;; Setup display size.
  mov.l display_size, r1
  mov.l display_size_value, r2
  mov.l r2, @r1

  ;; Setup misc setting.
  mov.l video_config, r1
  mov.l video_config_value, r2
  mov.l r2, @r1

  ;; Unknown...
  mov.l unknown_1, r1
  mov.l unknown_1_value, r2
  mov.l r2, @r1

  mov.l unknown_2, r1
  mov.l unknown_2_value, r2
  mov.l r2, @r1

  ;; Setup display memory region.
  mov.l display_memory_1, r1
  mov.l display_memory_1_value, r2
  ;mov #0, r2
  mov.l r2, @r1

  mov.l display_memory_2, r1
  mov.l display_memory_2_value, r2
  ;mov #0, r2
  mov.l r2, @r1

  ;; Setup border color
  mov.l border_col, r1
  mov #127, r2
  mov.l r2, @r1

  ;; Put some stuff in the frame buffer.
  mov.l frame_buffer, r1
  mov.l test_color, r2
  mov.l pixel_count, r0
frame_buffer_loop:
  mov.l r2, @r1
  add #4, r1
  add #-1, r0
  cmp/eq #0, r0
  bf frame_buffer_loop
  nop

  bra tile_accelerator
  nop

.include "config_video.inc"

tile_accelerator:
  ;; Point r1 to the tile accelerator registers.
  ;mov.l ta_registers, r1

  ;; Reset tile accelerator.
  mov.l ta_reset, r1
  mov #1, r2
  mov.l r2, @r1
  mov #0, r2
  mov.l r2, @r1

  ;; Set object pointer buffer start.
  mov.l ta_object_pointer_buffer_start, r1
  mov.l ta_object_pointer_buffer_start_value, r2
  mov.l r2, @r1

  ;; Set object pointer buffer end.
  mov.l ta_object_pointer_buffer_end, r1
  mov.l ta_object_pointer_buffer_end_value, r2
  mov.l r2, @r1

  ;; Set vertex buffer start.
  mov.l ta_vertex_buffer_start, r1
  mov.l ta_vertex_buffer_start_value, r2
  mov.l r2, @r1

  ;; Set vertex buffer end.
  mov.l ta_vertex_buffer_end, r1
  mov.l ta_vertex_buffer_end_value, r2
  mov.l r2, @r1

  ;; Set how many tiles are on the screen (width / height).
  mov.l ta_tile_buffer_control, r1
  mov.l ta_tile_buffer_control_value, r2
  mov.l r2, @r1

  ;; Set object pointer buffer init.
  mov.l ta_object_pointer_buffer_init, r1
  mov.l ta_object_pointer_buffer_init_value, r2
  mov.l r2, @r1

  ;; Tile accelerator configure OPB.
  ;; Bits 1-0 set to 2 is opaque polygons size_16 (15 object pointers).
  mov.l ta_opb_cfg, r1
  mov.l ta_opb_cfg_value, r2
  mov.l r2, @r1

  ;; Tile accelerator vertex registration init.
  mov.l ta_vertex_registration_init, r1
  mov.l ta_vertex_registration_init_value, r2
  mov.l r2, @r1

  ;; Start render (write anything to this register).
  mov.l ta_start_render, r1
  ;mov.l r2, @r1

  bra while_1
  nop

.include "config_ta.inc"
.include "object_list.inc"

while_1:
  bra while_1
  nop

