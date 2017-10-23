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

  ; Create a ring buffer for the calculated images
  ; r32 = picture
  ; r33 = r32 + 128k
  ; r34 = 4096
  il r32, picture
  il r33, 256
  shli r34, r33, 4
  shli r33, r33, 9
  a r33, r33, r32

  hbra branch_for_x, for_x
  hbra branch_mandel_for_loop, mandel_for_loop

do_next_row:
  ; Wait for data on channel 29
  ; r0 = [ real0, real1, real2, real3 ]
  ; r1  = [ imaginary0, imaginary0, imaginary0, imaginary0 ]
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

  ; r100 = [ real4, real5, real6, real7 ]
  fa r100, r0, r11

  ; r20 = picture
  or r20, r32, r32

  ; x = 1024
  il r22, 1024

  ; for (x = 0; x < width; y++)
for_x:
  ; r4 = zr = [ 0.0, 0.0, 0.0, 0.0 ]
  ; r5 = zi = [ 0.0, 0.0, 0.0, 0.0 ]
  il r4, 0
  il r5, 0

  il r104, 0
  il r105, 0

  ; counts = [ 0, 0, 0, 0 ]
  ; inc = [ 1, 1, 1, 1 ]
  il r10, 0
  il r2, 1
  il r103, 0
  il r102, 1

  ; loop_counter = [ 127, ?, ?, ? ]
  il r23, 127
mandel_for_loop:
.if 0
  ;; This is the code written cleanly, but it runs slower
  ;; because of .. I believe stalls due to using a register
  ;; as a source directly after it has been changed.  The
  ;; speed increase is pretty noticable.

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
  gb r6, r6
  a r10, r10, r2
  or r6, r6, r106

  brz r6, exit_mandel

  ai r23, r23, -1
.endif

  ; r7 = ti = (2 * zr * zi);
  fm r40, r5, r5
  fm r41, r105, r105
  fa r7, r4, r4
  fa r107, r104, r104

  ; r4 = tr = ((zr * zr) - (zi * zi));
  fms r4, r4, r4, r40
  fms r104, r104, r104, r41
  ;fm r7, r5, r7
  fma r5, r5, r7, r1
  fma r105, r105, r107, r1

  ; v4 = zr = tr + r;
  ; v5 = zi = ti + i;
  fa r4, r4, r0
  fa r104, r104, r100
  ;fa r5, r7, r1

  fm r6, r4, r4
  fm r106, r104, r104
  fma r7, r5, r5, r6
  fma r107, r105, r105, r106
  ai r23, r23, -1

  ; if ((zr * zr) + (zi * zi) > 4) break;
  fcgt r6, r3, r7
  fcgt r106, r3, r107

  ; count const = 0 if less than
  and r2, r2, r6
  and r102, r102, r106
  a r10, r10, r2
  a r103, r103, r102

  gb r6, r6
  gb r106, r106
  or r6, r6, r106

  brz r6, exit_mandel

branch_mandel_for_loop:
  brnz r23, mandel_for_loop

exit_mandel:
  ; r10 = (r10 >> 3) << 2
  ;rotmai r10, r10, 3
  andi r10, r10, 0x00f8
  andi r103, r103, 0x00f8
  shli r10, r10, 1
  shli r103, r103, 1

  ; map colors into picture
  lqd r26, colors(r10)
  lqd r116, colors(r103)
  shlqbyi r10, r10, 4
  shlqbyi r103, r103, 4
  lqd r27, colors(r10)
  lqd r117, colors(r103)
  shlqbyi r10, r10, 4
  shlqbyi r103, r103, 4
  lqd r28, colors(r10)
  lqd r118, colors(r103)
  shlqbyi r10, r10, 4
  shlqbyi r103, r103, 4
  lqd r29, colors(r10)
  lqd r119, colors(r103)

  and r26, r26, r16
  and r116, r116, r16
  and r27, r27, r17
  and r117, r117, r17
  and r28, r28, r18
  and r118, r118, r18
  and r29, r29, r19
  and r119, r119, r19

  or r26, r26, r27
  or r116, r116, r117
  or r28, r28, r29
  or r118, r118, r119
  or r26, r26, r28
  or r116, r116, r118

  stqd r26, 0(r20)
  stqd r116, 16(r20)

  ; picture += 4 pixels (16 bytes)
  ai r20, r20, 32

  ; [ real0, real1, real2, real3 ] += rstep4;
  fa r0, r0, r11
  fa r0, r0, r11
  fa r100, r100, r11
  fa r100, r100, r11

  ; next x
  ; Sub: r22 = r22 - 1
  ai r22, r22, -8
branch_for_x:
  brnz r22, for_x

  dsync

  ; Write back to PPE
  ;or r20, r32, r32
  wrch 28, r32

  ; if r32 == r33 then set r32 back to picture
  a r32, r32, r34
  sf r35, r32, r33
  brnz r35, do_next_row

  il r32, picture
  bra do_next_row

  ;; Stop the cell program
  sync
  stop 0

.align_bytes 16
picture:

