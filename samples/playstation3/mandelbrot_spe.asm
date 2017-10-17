.cell
.entry_point main

mandel_max:
  dd 4.0, 4.0, 4.0, 4.0

mul_by_2:
  dd 2.0, 2.0, 2.0, 2.0

main:
  ; Load some constants

  ; r16 = [ 0.0, 0.0, 0.0, 0.0 ]
  ; r14 = [ 1, 1, 1, 1 ]
  ; r17 = [ 2, 2, 2, 2 ]
  ; r18 = [ 3, 3, 3, 3 ]
  ;il r16, 0
  ;il r14, 1
  ;il r17, 2
  ;il r18, 3

  ; r3 = [ 4.0, 4.0, 4.0, 4.0 ]
  ; r8 = [ 2.0, 2.0, 2.0, 2.0 ]
  lqa r3, mandel_max
  lqa r8, mul_by_2

  ; r20 = [ picture, ?, ?, ? ]
  il r20, picture

  ; Wait for data on channel 29 
  ; r13 = [ r0, r1, r2, r3 ]
  ; r1  = [ i0, i0, i0, i0 ]
  ; r11 = [ r_step, r_step, r_step, r_step ]
  ; r12 = [ i_step, i_step, i_step, i_step ]
  rdch r13, 29 
  rdch r1, 29 
  rdch r11, 29 
  rdch r12, 29 

  ; y = 720
  il r21, 720

  ; for (y = 0; y < height; y++)
for_y:

  ; r0 = [ r0, r1, r2, r3 ]
  or r0, r13, r13

  ; x = 1024
  il r22, 1024

  ; for (x = 0; x < width; y++)
for_x:
  ; r2 = [ 1, 1, 1, 1 ]
  il r2, 1

  ; r4 = zr = [ 0.0, 0.0, 0.0, 0.0 ]
  ; r5 = zi = [ 0.0, 0.0, 0.0, 0.0 ]
  il r4, 0
  il r5, 0

  ; counts = [ 0, 0, 0, 0 ]
  il r10, 0

  ; loop_counter = [ 127, ?, ?, ? ]
  il r23, 127
mandel_for_loop:
  ; r7 = ti = (2 * zr * zi);
  fa r7, r4, r4
  fma r7, r7, r5, r16

  ; v4 = tr = ((zr * zr) - (zi * zi));
  fm r5, r5, r5
  fms r4, r4, r4, r5

  ; v4 = zr = tr + r;
  ; v5 = zi = ti + i;
  fa r4, r4, r0
  fa r5, r7, r1

  ; if ((zr * zr) + (zi * zi) > 4) break;
  fm r6, r4, r4
  fma r7, r5, r5, r6
  fcgt r6, r3, r7

  ; count const = 0 if less than
  and r2, r2, r6
  a r10, r10, r2

  gb r6, r6
  brz r6, exit_mandel

  sfi r23, r23, 1
  ;a r23, r23, -1
  brz r23, exit_mandel

exit_mandel:
  ; r10 = (r10 >> 3) << 2
  rotmai r10, r10, 3
  shli r10, r10, 2

  ; map colors into picture
  ;; FILL IN

  ; picture += 4 pixels (16 bytes)
  ai r20, r20, 16

  ; [ r0, r1, r2, r3 ] += rstep4;
  fa r0, r0, r11

  ; next x
  ; Sub: r22 = r22 - 1 
  sfi r22, r22, 1
  ; The docs have a subtract from word immediate instruction but then
  ; claim it doesn't exist.  Nice one IBM!
  ;a r22, r22, -1
  brnz r21, for_x

  ; [ i0, i0, i0, i9 ] += istep;
  fa r1, r1, r12

  ; next y
  ; Sub: r21 = r21 - 1 
  sfi r21, r21, 1
  ; The docs have a subtract from word immediate instruction but then
  ; claim it doesn't exist.  Nice one IBM!
  ;a r21, r21, -1
  brnz r21, for_y

  ; Write back to PPE
  il r20, picture
  wrch 28, r20

  ;bra main

  ;; Stop the cell program
  sync
  stop 0

.align_bytes 16
picture:

