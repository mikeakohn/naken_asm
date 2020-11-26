
.align 32
ta_registers:
  .dc32 0xa05f8000
ta_reset:
  .dc32 0xa05f8008
ta_opb_cfg:
  .dc32 0xa05f8140
ta_opb_cfg_value:
  .dc32 0x0010_0002
ta_object_pointer_buffer_start:
  .dc32 0xa05f8124
ta_object_pointer_buffer_start_value:
  .dc32 TILE_BUFFER_ADDRESS_START
ta_object_pointer_buffer_end:
  .dc32 0xa05f812c
ta_object_pointer_buffer_end_value:
  .dc32 0
ta_vertex_buffer_start:
  .dc32 0xa05f8128
ta_vertex_buffer_start_value:
  .dc32 VERTEX_BUFFER_ADDRESS
ta_vertex_buffer_end:
  .dc32 0xa05f8130
ta_vertex_buffer_end_value:
  .dc32 0
ta_tile_buffer_control:
  .dc32 0xa05f813c
ta_tile_buffer_control_value:
  .dc32 ((TILE_HEIGHT - 1) << 16) | (TILE_WIDTH - 1)
ta_object_pointer_buffer_init:
  .dc32 0xa05f8164
ta_object_pointer_buffer_init_value:
  .dc32 TILE_BUFFER_ADDRESS_START
ta_start_render:
  .dc32 0xa05f8014
ta_vertex_registration_init:
  .dc32 0xa05f8144
ta_vertex_registration_init_value:
  .dc32 0xa05f8144

