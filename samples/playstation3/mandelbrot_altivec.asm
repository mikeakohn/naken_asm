.powerpc

.export render_mandelbrot_altivec

; mandel_altivec(r3=picture, r4=struct _mandel_info)
render_mandelbrot_altivec:

  ; local variables are:
  ; [ r, r, r, r ] 0
  ; [ i, i, i, i ] 16
  ; x              32
  ; y              36 
  ;
  ;
  ; [ temp, temp, temp, temp] 48

  ;; Create zero constant from r0.
  xor r0, r0, r0

  ; v3 = [ 4.0, 4.0, 4.0, 4.0 ] constant
  ; v8 = [ 2.0, 2.0, 2.0, 2.0 ] constant
  addi r6, r0, 48
  lvx v3, r6, r4
  vspltw v3, v3, 0
  addi r6, r0, 64
  lvx v8, r6, r4
  vspltw v8, v8, 0

  ; v16 = [ 0.0, 0.0, 0.0, 0.0 ] constant
  vor v16, v16, v16

  ; v14 = [ 1, 1, 1, 1 ]
  vspltisw v14, 1

  ; v17 = [ 2, 2, 2, 2 ]
  vspltisw v17, 2

  ; v18 = [ 3, 3, 3, 3 ]
  vspltisw v17, 3

  ; r5 = int colors[]
  or r5, r4, r4
  addi r5, r5, 112

  ; v11 = [ r_step4, r_step4, r_step4, r_step4 ]
  xor r6, r6, r6 
  lvx v11, r6, r4
  vspltw v11, v11, 0

  ; v1 = [ i0, i1, i2, i3 ]  imaginary_start
  addi r6, r0, 16
  lvx v1, r6, r4
  vspltw v1, v1, 0

  ; v13 = [ r0, r1, r2, r3 ]
  addi r6, r0, 32
  lvx v13, r6, r4

  ; v12 = [ i_step, i_step, i_step, i_step ]
  addi r6, r0, 80
  lvx v12, r6, r4
  vspltw v12, v12, 0

  ; y = 0
  xor r7, r7, r7

  ; for (y = 0; y < height; y++)
for_y:

  ; v0 = [ r0, r1, r2, r3 ]
  vor v0, v13, v13

  ; x = 0
  xor r8, r8, r8

  ; for (x = 0; x < width; y++)
for_x:
  ; v2 = [ 1, 1, 1, 1 ]
  vor v2, v14, v14

  ; v4 = zr = [ 0.0, 0.0, 0.0, 0.0 ]
  ; v5 = zi = [ 0.0, 0.0, 0.0, 0.0 ]
  vor v4, v4, v4
  vor v5, v5, v5

  ; counts = [ 0, 0, 0, 0 ]
  vor v10, v10, v10

  ori r8, r8, 127
mandel_sse_for_loop:
  ; v7 = ti = (2 * zr * zi);
  vor v7, v4, v4
  vmaddfp v7, v7, v5, v16
  vmaddfp v7, v7, v8, v16

  ; v4 = tr = ((zr * zr) - (zi * zi));
  vmaddfp v4, v4, v4, v16
  vmaddfp v4, v5, v5, v4

  ; v4 = zr = tr + r;
  ; v5 = zi = ti + i;
  vaddfp v4, v4, v0 
  vaddfp v5, v7, v1 

  ; if ((tr * tr) + (ti * ti) > 4) break;
  vmaddfp v6, v4, v4, v16
  vmaddfp v7, v5, v5, v6
  vcmpgefpx v6, v7, v3

  ; count const = 0 if less than
  vand v2, v2, v6
  vaddsws v10, v10, v2

  vsumsws v6, v2, v2
  addi r6, r0, 96
  stvx v6, r6, r4
  lwz r9, 108(r6)
  or. r9, r9, r9
  beq exit_mandel

  addic. r8, r8, -1
  bne mandel_sse_for_loop

exit_mandel:
  vsraw v10, v10, v17 
  vslw v10, v10, v18 

  addi r6, r0, 96
  stvx v6, r6, r4

  ; map colors into picture 
  lwz r9, 96(r6)
  add r9, r9, r5
  lwz r9, 0(r9)
  stw r9, 0(r3)

  lwz r9, 100(r6)
  add r9, r9, r5
  lwz r9, 0(r9)
  stw r9, 4(r3)

  lwz r9, 104(r6)
  add r9, r9, r5
  lwz r9, 0(r9)
  stw r9, 8(r3)

  lwz r9, 108(r6)
  add r9, r9, r5
  lwz r9, 0(r9)
  stw r9, 12(r3)

  ; picture += 4
  addi r3, r3, 16

  ; [ r0, r1, r2, r3 ] += rstep4;
  vaddfp v0, v0, v11

  ; next x
  addi r8, r8, 4
  lwz r6, 20(r4)
  subf. r6, r6, r8
  bne for_x

  ; [ i0, i1, i2, i3 ] += istep;
  vaddfp v1, v1, v12

  ; next y
  addi r7, r7, 1
  lwz r6, 24(r4)
  subf. r6, r6, r7
  bne for_x

  ; return
  blr

