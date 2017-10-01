.ps2_ee_vu0

start:
  nop iaddiu vi1, vi0, variables / 16

  ; vf3 = [ 4.0, 4.0, 4.0, 4.0 ]
  ; vf8 = [ 2.0, 2.0, 2.0, 2.0 ]
  ;nop lq.xyzw vf3, 8(vi1)
  ;nop lq.xyzw vf3, (mul_by_2 - variables)(vi1)
  nop lq.xyzw vf3, ((mandel_max - variables) / 16)(vi1)
  nop lq.xyzw vf8, ((mul_by_2 - variables) / 16)(vi1)

  nop iaddiu vi3, vi0, 240
for_y:

  nop iaddiu vi2, vi0, 320
for_x:
  ; vf2 = [ 1, 1, 1, 1 ]
  nop lq vf2, ((add_count-variables)/16)(vi1)

  ; vf4 = zr = [ 0.0, 0.0, 0.0, 0.0 ]
  ; vf5 = zi = [ 0.0, 0.0, 0.0, 0.0 ]
  sub.xyzw vf4, vf4, vf4 nop
  sub.xyzw vf5, vf5, vf5 nop

  ; count = 100
  nop iaddiu vi4, vi0, 127

next_iteration:
  ; z = z^2 + c
  ; z^2 = (x + yi) * (x + yi)
  ;     = x^2 + 2xyi - y^2
  ;     = (x^2 - y^2) + 2xyi

  ; vf7 = ti = (2 * zr * zi);
  mul.xyzw vf7, vf4, vf5 nop
  mul.xyzw vf7, vf7, vf8 nop

  ; vf4 = tr = ((zr * zr) - (zi * zi));
  mul.xyzw vf4, vf4, vf4 nop
  mul.xyzw vf5, vf5, vf5 nop
  mul.xyzw vf4, vf4, vf5 nop

  ; vf4 = zr = tr + r;
  ; vf5 = zi = ti + i;
  add.xyzw vf4, vf4, vf20 nop
  add.xyzw vf5, vf7, vf1 nop

  ; if ((zr * zr) + (zi * zi) > 4) break;
  mul.xyzw vf6, vf4, vf4 nop
  mul.xyzw vf7, vf5, vf5 nop
  add.xyzw vf6, vf6, vf7 nop
  ;cmpleps xmm6, xmm3
  sub.xyzw vf21, vf6, vf3 nop

  ; count = count - 1
  nop isubiu vi4, vi4, 1

  nop ibne vi0, vi4, next_iteration

  ; [ r0, r1, r2, r3 ] += rstep4
  add.xyzw vf20, vf20, vf11 nop

  nop ibne vi0, vi2, for_x

  ; [ i0, i1, i2, i3 ] += istep
  add.xyzw vf1, vf1, vf12 nop

  nop ibne vi0, vi3, for_y

  nop nop

variables:
mandel_max:
  dc32 4.0, 4.0, 4.0, 4.0

add_count:
  dc32 1, 1, 1, 1

mul_by_2:
  dc32 2.0, 2.0, 2.0, 2.0

vect_3:
  dc32 3, 3, 3, 3

colors:
  dd 0xff0000  ; f
  dd 0xee3300  ; e
  dd 0xcc5500  ; d
  dd 0xaa5500  ; c
  dd 0xaa3300  ; b
  dd 0x666600  ; a
  dd 0x999900  ; 9
  dd 0x669900  ; 8
  dd 0x339900  ; 7
  dd 0x0099aa  ; 6
  dd 0x0066aa  ; 5
  dd 0x0033aa  ; 4
  dd 0x0000aa  ; 3
  dd 0x000099  ; 2
  dd 0x000066  ; 1
  dd 0x000000  ; 0


