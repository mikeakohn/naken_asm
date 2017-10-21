.cell
.entry_point render_mandelbrot_cell

mask0:
  dd 0xffffffff, 0x00000000, 0x00000000, 0x00000000
mask1:
  dd 0x00000000, 0xffffffff, 0x00000000, 0x00000000
mask2:
  dd 0x00000000, 0x00000000, 0xffffffff, 0x00000000
mask3:
  dd 0x00000000, 0x00000000, 0x00000000, 0xffffffff

colors:
  dd 0xff0000, 0xff0000, 0xff0000, 0xff0000 ; f
  dd 0xee3300, 0xee3300, 0xee3300, 0xee3300 ; e
  dd 0xcc5500, 0xcc5500, 0xcc5500, 0xcc5500 ; d
  dd 0xaa5500, 0xaa5500, 0xaa5500, 0xaa5500 ; c
  dd 0xaa3300, 0xaa3300, 0xaa3300, 0xaa3300 ; b
  dd 0x666600, 0x666600, 0x666600, 0x666600 ; a
  dd 0x999900, 0x999900, 0x999900, 0x999900 ; 9
  dd 0x669900, 0x669900, 0x669900, 0x669900 ; 8
  dd 0x339900, 0x339900, 0x339900, 0x339900 ; 7
  dd 0x0099aa, 0x0099aa, 0x0099aa, 0x0099aa ; 6
  dd 0x0066aa, 0x0066aa, 0x0066aa, 0x0066aa ; 5
  dd 0x0033aa, 0x0033aa, 0x0033aa, 0x0033aa ; 4
  dd 0x0000aa, 0x0000aa, 0x0000aa, 0x0000aa ; 3
  dd 0x000099, 0x000099, 0x000099, 0x000099 ; 2
  dd 0x000066, 0x000066, 0x000066, 0x000066 ; 1
  dd 0x000000, 0x000000, 0x000000, 0x000000 ; 0

render_mandelbrot_cell:
  ; Load some constants

  lqa r16, mask0
  lqa r17, mask1
  lqa r18, mask2
  lqa r19, mask3

  ; r3 = [ 4.0, 4.0, 4.0, 4.0 ]
  il r3, 2
  csflt r3, r3, 0

  ; r11 = [ r_step4, r_step4, r_step4, r_step4 ]
  ; r12 = [ 0, r_step1, r_step2, r_step3 ]
  rdch r11, 29
  rdch r12, 29

  ; r12 = [ r_step1, 0, 0, 0 ]
  rotqbyi r20, r12, 4
  or r21, r20, r12
  fa r21, r21, r12
  ; r21 = [ r_step2, 0, 0, r_step1 ]
  and r22, r21, r16
  rotqbyi r21, r21, 4
  or r21, r22, r21
  fa r21, r21, r12
  ; r21 = [ r_step3, 0, r_step1, r_step2 ]
  rotqbyi r12, r21, 4
  ; r12 = [ 0, r_step1, r_step2, r_step3 ]

  rotqbyi r22, r11, 12
  or r11, r11, r22
  rotqbyi r22, r11, 8
  or r11, r11, r22

do_next_row:
  ; Wait for data on channel 29
  ; r0 = [ r0, r1, r2, r3 ]
  ; r1  = [ i0, i0, i0, i0 ]
  rdch r0, 29
  rdch r1, 29

  ; Holy crap, there must be a better way to do this?
  rotqbyi r20, r0, 12
  rotqbyi r21, r1, 12

  or r0, r0, r20
  or r1, r1, r21

  rotqbyi r20, r0, 8
  rotqbyi r21, r1, 8

  or r0, r0, r20
  or r1, r1, r21

  fa r0, r0, r12

  ; r20 = [ picture, ?, ?, ? ]
  il r20, picture

  ; x = 1024
  il r22, 1024

  ; for (x = 0; x < width; y++)
for_x:
  ; r4 = zr = [ 0.0, 0.0, 0.0, 0.0 ]
  ; r5 = zi = [ 0.0, 0.0, 0.0, 0.0 ]
  il r4, 0
  il r5, 0

  ; counts = [ 0, 0, 0, 0 ]
  ; inc = [ 1, 1, 1, 1 ]
  il r10, 0
  il r2, 1

  ; loop_counter = [ 127, ?, ?, ? ]
  il r23, 127
mandel_for_loop:
  ; r7 = ti = (2 * zr * zi);
  fa r7, r4, r4
  fm r7, r5, r7

  ; r4 = tr = ((zr * zr) - (zi * zi));
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

  ai r23, r23, -1
  brnz r23, mandel_for_loop

exit_mandel:
  ; r10 = (r10 >> 3) << 2
  ;rotmai r10, r10, 3
  andi r10, r10, 0x00f8
  shli r10, r10, 1

  ; map colors into picture
  lqd r26, colors(r10)
  shlqbyi r10, r10, 4
  lqd r27, colors(r10)
  shlqbyi r10, r10, 4
  lqd r28, colors(r10)
  shlqbyi r10, r10, 4
  lqd r29, colors(r10)

  and r26, r26, r16
  and r27, r27, r17
  and r28, r28, r18
  and r29, r29, r19

  or r26, r26, r27
  or r28, r28, r29
  or r26, r26, r28

  stqd r26, 0(r20)

  ; picture += 4 pixels (16 bytes)
  ai r20, r20, 16

  ; [ r0, r1, r2, r3 ] += rstep4;
  fa r0, r0, r11

  ; next x
  ; Sub: r22 = r22 - 1
  ai r22, r22, -4
  brnz r22, for_x

  dsync

  ; Write back to PPE
  il r20, picture
  wrch 28, r20

  bra do_next_row

  ;; Stop the cell program
  sync
  stop 0

.align_bytes 16
picture:

