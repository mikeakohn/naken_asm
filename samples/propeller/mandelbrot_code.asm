.propeller

.include "propeller.inc"

.macro umul(dest, r0, r1)
  abs temp0, r0
  abs temp1, r1
  and temp0, mask16
  and temp1, mask16
  mov dest, #0
.scope
repeat_mul:
  shr temp0, #1, wc wz
  if_c add dest, temp1
  shl temp1, #1
  if_nz jmp #repeat_mul
.ends
  shr dest, #10
.endm

.macro smul(dest, r0, r1)
  mov is_neg, #0
  cmps r0, #0, wc wz
  if_b xor is_neg, #1
  cmps r1, #0, wc wz
  if_b xor is_neg, #1

  abs temp0, r0
  abs temp1, r1
  and temp0, mask16
  and temp1, mask16
  mov dest, #0
.scope
repeat_mul:
  shr temp0, #1, wc wz
  if_c add dest, temp1
  shl temp1, #1
  if_nz jmp #repeat_mul
.ends
  cmp is_neg, #1, wz
  if_e neg dest, dest
.endm

main:
  mov dira, port_dir
  mov outa, port_start

repeat_mandel:
  mov r_start, r_start_0
  mov i_start, i_start_0
  mov dx, dx_0
  mov dy, dy_0
  call mandelbrot_ret, #mandelbrot

  mov r_start, r_start_1
  mov i_start, i_start_1
  mov dx, dx_1
  mov dy, dy_1
  call mandelbrot_ret, #mandelbrot

  mov r_start, r_start_2
  mov i_start, i_start_2
  mov dx, dx_2
  mov dy, dy_2
  call mandelbrot_ret, #mandelbrot
  jmp #repeat_mandel

while_1:
  mov count, counter_top
repeat:
  sub count, #1, wz 
  if_nz jmp #repeat
  xor outa, led_xor
  jmp #while_1

mandelbrot:
  mov line, #64
  mov curr_i, i_start
next_line:
  mov row, #96
  mov curr_r, r_start
next_row:

  mov zr, #0
  mov zi, #0

  mov count, #127
next_iteration:

  ;zr2 = (zr * zr) >> DEC_PLACE;
  ;zi2 = (zi * zi) >> DEC_PLACE;
  umul(zr2, zr, zr)
  umul(zi2, zi, zi)

  ;if (zr2 + zi2) > (4 << DEC_PLACE)) { break; }
  mov temp0, zr2
  adds temp0, zi2
  cmps temp0, four, wc wz
  if_a jmp #exit_iteration

  ;tr = zr2 - zi2;
  mov tr, zr2
  subs tr, zi2

  ;ti = 2 * ((zr * zi) >> DEC_PLACE);
  smul(ti, zr, zi)
  sar ti, #(10 - 1)

  ;zr = tr + curr_r;
  ;zi = ti + curr_i;
  mov zr, tr
  adds zr, curr_r
  mov zi, ti
  adds zi, curr_i

  sub count, #1, wz
  if_nz jmp #next_iteration
exit_iteration:

  ;; count = count >> 3 and then mul by 4 (zeroes out bottom bits)
  shr count, #3
  add count, #colors
  movs get_color, count
  nop
get_color: 
  mov temp0, colors

  mov data, temp0
  shr data, #8
  call send_data_ret, #send_data
  mov data, temp0
  and data, #0xff
  call send_data_ret, #send_data

  adds curr_r, dx
  sub row, #1, wz
  if_nz jmp #next_row

  adds curr_i, dy
  sub line, #1, wz
  if_nz jmp #next_line
mandelbrot_ret:
  ret

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

count:
  dc32 0
bit_count:
  dc32 0
data:
  dc32 0x0
port_dir:
  dc32 (0x1f) | (1 << 26) | (1 << 27) 
port_start:
  dc32 (0x1f) | (1 << 26)
led_xor:
  dc32 (1 << 26) | (1 << 27) 
counter_top:
  dc32 0xfffff
line:
  dc32 0
row:
  dc32 0
r_start_0:
  dc32 (-2 << 10)
i_start_0:
  dc32 (-1 << 10)
dx_0:
  dc32 ((1 << 10) - (-2 << 10)) / 96
dy_0:
  dc32 ((1 << 10) - (-1 << 10)) / 64
r_start_1:
  dc32 (-1 << 10)
i_start_1:
  dc32 (-1 << 9)
dx_1:
  dc32 ((1 << 9) - (-1 << 10)) / 96
dy_1:
  dc32 ((1 << 9) - (-1 << 9)) / 64
r_start_2:
  dc32 (-2 << 9)
i_start_2:
  dc32 (-1<< 9)
dx_2:
  dc32 ((0 << 10) - (-2 << 9)) / 96
dy_2:
  dc32 ((0 << 10) - (-1 << 9)) / 64
r_start:
  dc32 0
i_start:
  dc32 0
dx:
  dc32 0
dy:
  dc32 0
zr:
  dc32 0
zi:
  dc32 0
tr:
  dc32 0
ti:
  dc32 0
zr2:
  dc32 0
zi2:
  dc32 0
curr_r:
  dc32 0
curr_i:
  dc32 0
temp0:
  dc32 0
temp1:
  dc32 0
is_neg:
  dc32 0
four:
  dc32 (4 << 10)
mask16:
  dc32 0xffff
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

;delay_count:
;  dc32 0x0
;delay_count_top:
;  dc32 10


