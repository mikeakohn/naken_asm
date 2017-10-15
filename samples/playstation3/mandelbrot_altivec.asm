.powerpc

.export render_mandelbrot_altivec

; mandel_altivec(r3=picture, r4=struct _mandel_info)
render_mandelbrot_altivec:

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
  vxor v16, v16, v16

  ; v14 = [ 1, 1, 1, 1 ]
  vspltisw v14, 1

  ; v17 = [ 2, 2, 2, 2 ]
  vspltisw v17, 2

  ; v18 = [ 3, 3, 3, 3 ]
  vspltisw v18, 3

  ; r5 = int colors[]
  lwz r5, 96(r4)

  ; v11 = [ r_step4, r_step4, r_step4, r_step4 ]
  addi r6, r0, 0
  lvx v1, r6, r4
  vspltw v11, v1, 0

  ; v12 = [ i_step, i_step, i_step, i_step ]
  vspltw v12, v1, 1

  ; v1 = [ i0, i1, i2, i3 ]  imaginary_start
  vspltw v1, v1, 2

  ; v13 = [ r0, r1, r2, r3 ]
  addi r6, r0, 32
  lvx v13, r6, r4

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
  vxor v4, v4, v4
  vxor v5, v5, v5

  ; counts = [ 0, 0, 0, 0 ]
  vxor v10, v10, v10

  ori r10, r0, 127
mandel_for_loop:
  ; v7 = ti = (2 * zr * zi);
  ;vor v7, v4, v4
  ;vmaddfp v7, v7, v5, v16
  vaddfp v7, v4, v4
  vmaddfp v7, v7, v5, v16

  ; v4 = tr = ((zr * zr) - (zi * zi));
  vnmsubfp v5, v5, v5, v16
  vmaddfp v4, v4, v4, v5

  ; v4 = zr = tr + r;
  ; v5 = zi = ti + i;
  vaddfp v4, v4, v0
  vaddfp v5, v7, v1

  ; if ((zr * zr) + (zi * zi) > 4) break;
  vmaddfp v6, v4, v4, v16
  vmaddfp v7, v5, v5, v6
  vcmpgtfpx v6, v3, v7

  ; count const = 0 if less than
  vand v2, v2, v6
  vaddsws v10, v10, v2

  ;vsumsws v6, v2, v2
  ;ori r6, r0, 80
  ;stvx v6, r6, r4
  ;lwz r9, 92(r4)
  ;or. r9, r9, r9
  beq exit_mandel

  addic. r10, r10, -1
  bne mandel_for_loop

exit_mandel:
  vsraw v10, v10, v18
  vslw v10, v10, v17

  addi r6, r0, 80
  stvx v10, r6, r4

  ; map colors into picture
  lwz r9, 80(r4)
  add r9, r5, r9
  lwz r9, 0(r9)
  stw r9, 0(r3)

  lwz r9, 84(r4)
  add r9, r5, r9
  lwz r9, 0(r9)
  stw r9, 4(r3)

  lwz r9, 88(r4)
  add r9, r5, r9
  lwz r9, 0(r9)
  stw r9, 8(r3)

  lwz r9, 92(r4)
  add r9, r5, r9
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
  bne for_y

;; DEBUG DEBUG
  ;addi r6, r0, 96
  ;stvx v11, r6, r4
;; DEBUG

  ; return
  blr

