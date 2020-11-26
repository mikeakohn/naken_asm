
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
  ;; 8: Threshold when display mode is ARGB8888.
  ;; 2: Pixel mode 3 = RGB0888 (4 bytes / pixel)
  ;; 1: Line double.
  ;; 0: Enable.
  .dc32 (0xff << 8) | (3 << 2) | 1
display_memory_1:
  .dc32 POWERVR_FB_DISPLAY_ADDR1
display_memory_1_value:
  .dc32 DISPLAY_BUFFER_1
display_memory_2:
  .dc32 POWERVR_FB_DISPLAY_ADDR2
display_memory_2_value:
  .dc32 DISPLAY_BUFFER_2
frame_buffer:
  .dc32 0xa500_0000
test_color:
  .dc32 0xff00ff
pixel_count:
  .dc32 640 * 240
display_sync_load:
  .dc32 POWERVR_SYNC_LOAD
display_sync_load_value:
  .dc32 (524 << 16) | 857
display_sync_width:
  .dc32 POWERVR_SYNC_WIDTH
display_sync_width_value:
  .dc32 (0x1f << 22) | (364 << 12) | (0x06 << 8) | 0x3f
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
  .dc32 (641 << 20) | (239 << 10) | 639
video_config:
  .dc32 POWERVR_VIDEO_CFG
video_config_value:
  .dc32 (0x16 << 16)
unknown_1:
  .dc32 0xa05f8110
unknown_1_value:
  .dc32 0x00093f39
unknown_2:
  .dc32 0xa05f8114
unknown_2_value:
  .dc32 0x00200000

