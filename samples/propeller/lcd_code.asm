.propeller

.include "propeller.inc"

main:
  mov dira, port_dir
  mov outa, port_state0

  ;; Signal is start of buffer + (cogid * 16)
  mov signal, par
  cogid temp
  shl temp, #4
  add signal, temp

  ;; Image is start of buffer + (signal buffer)
  mov image, par
  add image, #8*16

wait_signal:
  ;; Wait for spin code to signal this cog to read the image
  rdbyte temp, signal, wz
  if_z jmp #wait_signal

  mov outa, port_state0

  ;; Point to image and set a counter for 96 * 64 pixels
  mov count, const_6144
  mov image_ptr, image

next_pixel:
  ;; pixel = colors[image_ptr++]
  rdbyte temp, image_ptr
  ;shl temp, #2
  add temp, #colors
  movs pixel_array, temp
  add image_ptr, #1
pixel_array:
  mov pixel, 0

  ;mov pixel, #0x00ff

  ;; Write pixel to LCD
  mov data, pixel
  shr data, #8
  and data, #0xff
  call send_data_ret, #send_data
  mov data, pixel
  and data, #0xff
  call send_data_ret, #send_data

  ;; count--
  sub count, #1, wz
  if_nz jmp #next_pixel

  ;; Reset the the signal so the SPIN cog knows the LCD is done
  wrbyte zero, signal

  mov outa, port_state1

  jmp #wait_signal

send_data:
  or outa, #0x08     ; DC = 1
  andn outa, #0x04   ; CS = 0

  rol data, #24
  mov bit_count, #8

send_data_next_bit:
  ; data := data << 1
  rol data, #1, wc

  if_c or outa, #1
  if_nc andn outa, #1

  ;or outa, #1

  andn outa, #0x02 ; CLK = 0
  or outa, #0x02   ; CLK = 1

  sub bit_count, #1, wz
  if_nz jmp #send_data_next_bit

  or outa, #0x04 ; CS = 1

send_data_ret:
  ret

bit_count:
  dc32 0
data:
  dc32 0x0
port_dir:
  dc32 (0x1f) | (1 << 26) | (1 << 27)
port_state0:
  dc32 (0x1f) | (1 << 26)
port_state1:
  dc32 (0x1f) | (1 << 27)
temp:
  dc32 0
signal:
  dc32 0
image:
  dc32 0
const_6144:
  dc32 6144
count:
  dc32 0
image_ptr:
  dc32 0
pixel:
  dc32 0
zero:
  dc32 0
colors:
  dc32 0x0000
  dc32 0x000c
  dc32 0x0013
  dc32 0x0015
  dc32 0x0195
  dc32 0x0335
  dc32 0x04d5
  dc32 0x34c0
  dc32 0x64c0
  dc32 0x9cc0
  dc32 0x6320
  dc32 0xa980
  dc32 0xaaa0
  dc32 0xcaa0
  dc32 0xe980
  dc32 0xf800

