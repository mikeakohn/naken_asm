;; Multicore Mandelbrots.
;;
;; Copyright 2020 - By Michael Kohn
;; http://www.mikekohn.net/
;; mike@mikekohn.net
;;
;; Example of using a Parallax Propeller 2 to compute mandelbrots.

.propeller2

main:
  ;; Signal is start of buffer + (cogid * 16).
  mov signal, ptra
  cogid temp0
  shl temp0, #4
  add signal, temp0

  ;; Image is start of buffer + (signal buffer).
  mov image, ptra
  add image, #8 * 16

wait_signal:
  ;; Wait for spin code to signal this cog to read the image.
  ;rdbyte temp0, signal, wz
  ;if_z jmp #wait_signal
  waitatn

  ;; temp0 points to the next element in the signal array.
  mov temp0, signal

  ;; curr_r = sign_ext(signal[1])
  add temp0, #2
  rdword curr_r, temp0
  test curr_r, const_8000, wz
  if_nz or curr_r, const_ffff0000

  ;; curr_i = sign_ext(signal[2])
  add temp0, #2
  rdword curr_i, temp0
  test curr_i, const_8000, wz
  if_nz or curr_i, const_ffff0000

  ;; dx = sign_ext(signal[3])
  add temp0, #2
  rdword dx, temp0
  test dx, const_8000, wz
  if_nz or dx, const_ffff0000

  ;; dy = sign_ext(signal[4])
  add temp0, #2
  rdword dy, temp0
  test dy, const_8000, wz
  if_nz or dy, const_ffff0000

  ;; Calculate row offset to image_ptr.
  ;; temp0 = row * 96
  ;; image_ptr = image + temp0
  mov image_ptr, image
  add temp0, #2
  rdword temp1, temp0
  muls temp1, #96
  add image_ptr, temp1

  ;; Do mandelbrot row.
  mov column, #96

next_x:
  mov zr, #0
  mov zi, #0

  mov count, #127
next_iteration:

  ;zr2 = (zr * zr) >> DEC_PLACE;
  ;zi2 = (zi * zi) >> DEC_PLACE;
  abs zr2, zr
  abs zi2, zi
  mul zr2, zr2
  mul zi2, zi2
  shr zi2, #10
  shr zr2, #10

  ;if (zr2 + zi2) > (4 << DEC_PLACE)) { break; }
  mov temp0, zr2
  adds temp0, zi2
  cmps temp0, four, wcz
  if_a jmp #exit_iteration

  ;tr = zr2 - zi2;
  mov tr, zr2
  subs tr, zi2

  ;ti = 2 * ((zr * zi) >> DEC_PLACE);
  mov ti, zr
  muls ti, zi
  sar ti, #(10 - 1)

  ;zr = tr + curr_r;
  ;zi = ti + curr_i;
  mov zr, tr
  adds zr, curr_r
  mov zi, ti
  adds zi, curr_i

  djnz count, #next_iteration
exit_iteration:

  ;; count = count >> 3 and then write that to shared memory.
  shr count, #3
  wrbyte count, image_ptr
  add image_ptr, #1

  adds curr_r, dx
  djnz column, #next_x

  ;; Reset the the signal so the SPIN cog knows this cog is ready.
  ;; for more data.
  wrbyte zero, signal

  jmp #wait_signal

count:
  dc32 0
bit_count:
  dc32 0
data:
  dc32 0x0
line:
  dc32 0
column:
  dc32 0
r_start_0:
  dc32 (-2 << 10)
i_start_0:
  dc32 (-1 << 10)
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
signal:
  dc32 0
image:
  dc32 0
image_ptr:
  dc32 0
zero:
  dc32 0
const_8000:
  dc32 0x8000
const_ffff0000:
  dc32 0xffff0000

