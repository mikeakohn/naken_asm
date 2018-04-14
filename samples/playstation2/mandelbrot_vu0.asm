.ps2_ee_vu0

  ; struct _mandel_data
  ; {
  ;   float r_step, r_step, r_step, r_step;
  ;   float i_step, i_step, i_step, i_step;
  ;   float r0, r1, r2, r3;
  ;   float i0, i1, i2, i3;
  ; };

start:
  ; vf05 = [ r_step, r_step, r_step, r_step ]
  ; vf06 = [ i_step, i_step, i_step, i_step ]
  ; vf07 = [ r0, r1, r2, r3 ]
  ; vf08 = [ i0, i1, i2, i3 ]
  ; vf04 = [ 4.0, 4.0, 4.0, 4.0 ]
  ; vf02 = [ 2.0, 2.0, 2.0, 2.0 ]
  ; vf01 = [ 0.0, 0.0, 0.0, 0.0 ]
  sub.xyzw vf01, vf01, vf01   lq.xyzw vf05, 0(vi00)
  addw.xyzw vf02, vf01, vf00w lq.xyzw vf06, 1(vi00)
  add.xyzw vf02, vf02, vf02   lq.xyzw vf07, 2(vi00)
  add.xyzw vf04, vf02, vf02   lq.xyzw vf08, 3(vi00)

  ; vi03 = 8
  nop                         iaddiu vi03, vi00, 8
for_y:

  ; vi02 = 64
  nop                         iaddiu vi02, vi00, 64
for_x:

  ; vf03 = [ 1.0, 1.0, 1.0, 1.0 ] = count_dec
  ; vf11 = [ 127.0, 127.0, 127.0, 127.0 ] = count
  addw.xyzw vf03, vf01, vf00w iaddiu vi04, vi00, 127
  nop                         mfir.xyzw vf11, vi04
  itof0.xyzw vf11, vf11, vf11 nop

  ; vf09 = zr = [ 0.0, 0.0, 0.0, 0.0 ]
  ; vf10 = zi = [ 0.0, 0.0, 0.0, 0.0 ]
  sub.xyzw vf09, vf09, vf09   nop
  sub.xyzw vf10, vf10, vf10   nop

next_iteration:
  ; z = z^2 + c
  ; z^2 = (x + yi) * (x + yi)
  ;     = x^2 + 2xyi - y^2
  ;     = (x^2 - y^2) + 2xyi

  ; vf12 = ti = (2 * zr * zi);
  mul.xyzw vf12, vf09, vf10    nop
  mul.xyzw vf12, vf12, vf02    nop

  ; vf13 = tr = ((zr * zr) - (zi * zi));
  mul.xyzw vf13, vf09, vf09   nop
  mul.xyzw vf14, vf10, vf10   nop
  sub.xyzw vf13, vf09, vf10   nop

  ; vf09 = zr = tr + r;
  ; vf10 = zi = ti + i;
  add.xyzw vf09, vf13, vf07   nop
  add.xyzw vf10, vf12, vf08   nop

  ; if ((zr * zr) + (zi * zi) > 4) break;
  mul.xyzw vf13, vf09, vf09   nop
  mul.xyzw vf14, vf10, vf10   nop
  add.xyzw vf13, vf13, vf14   nop

  ; if [ l0, l1, l2, l3 ] > 4, count = 0
  ; vf03 = [ 1.0/0.0 1.0/0.0 1.0/0.0 1.0/0.0 ]
  ftoi.xyzw vf13, vf13, vf13
  ;sub.xyzw vf13, vf04, vf13   nop
  ;max.xyzw vf13, vf01, vf13   nop
  ;nop                         esum P, vf13
  ;(if above 0, make 1)        waitp
  ;add.xyzw vf02, vf13, vf01   mfp.x vf14
  ;ftoi0.xyzw vf14, vf14       nop
  ;sub.xyzw vf11, vf11, vf03   mtir.x vi05, vf14
  

  ; count = count - 1
  nop                         isubiu vi04, vi04, 1
  nop                         ibeq vi05, vi00, break_iteration
  nop                         ibne vi04, vi00, next_iteration

break_iteration:
  ; [ r0, r1, r2, r3 ] += rstep4
  add.xyzw vf20, vf20, vf11   nop

  nop                         ibne vi0, vi02, for_x

  ; [ i0, i1, i2, i3 ] += istep
  add.xyzw vf01, vf01, vf12   nop

  nop                         ibne vi0, vi03, for_y

  nop                         nop
  nop[E]                      nop
  nop                         nop

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

