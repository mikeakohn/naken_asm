;; VU1 has 16k of program program memory and 16k data memory.
;; The structure sent from the core unit to VU1 is:
;;
;; struct _object
;; {
;;   float sin_rx, cos_rx, sin_ry, cos_ry;
;;   float sin_rz, cos_rz, unused_0, unused_1;
;;   float 0, dz, dy, dx;
;;   int count, should_rotate, unused_2, unused_3;
;;   struct _data
;;   {
;;     float x, y, x, 0;
;;     struct _color
;;     {
;;       uint8_t r;
;;       uint8_t g;
;;       uint8_t b;
;;       uint8_t a;
;;       uint32_t q = 0x3f80_0000;
;;       uint64_t reg_rgbaq = REG_RGBAQ;
;;     };
;;   } data[count];
;; }
;;
;; This structure will start at location 0 in data.
;;
;; In a register loaded from memory is:
;;                          { w,   z, y, x }
;;                   vf00 = { 1.0, 0, 0, 0 }
;;                   vi00 = 0

.ps2_ee_vu1
.include "playstation2/registers_gs_gp.inc"

.org 0
start:
  ;; vi01 points to the source GIF packet.
  ;; vi02 points to the rotation, position information.
  ;; vi03 holds the count.
  ;; vi04 points to the next point to be processed.
  ;; vi05 used to calculate pi / 2.
  ;; vi10 is the next point being processed>
  ;; vf02 position { 0, dz, dy, dx }
  ;; --vf03 rotation { 0, rz, ry, rx }
  ;; --vf04 has { 0, 0, 0, 3.14 / 2 }
  nop                         iaddiu vi01, vi00, 4
  nop                         iaddiu vi04, vi00, 8

  ;; Setup count/vi03,
  nop                         ilw.x vi03, 3(vi00)

  ;; vf02 = { 0, dz, dy, dx }
  nop                         lq.xyzw vf02, 2(vi00)

next_point:
  ;; Load next point from data RAM.
  nop                         lq.xyzw vf10, 0(vi04)
  nop                         nop
  nop                         nop
  nop                         nop

  ;; Transpose point by { 0, dz, dy, dx } vector.
  add.xyz vf10, vf10, vf02    nop
  nop                         nop
  nop                         nop
  nop                         nop

  ;; Convert to X and Y to fixed point 12:4 and Z to just an integer.
  ftoi4.xy vf10, vf10         nop
  nop                         nop
  nop                         nop
  nop                         nop
  ftoi0.z  vf10, vf10         nop
  nop                         nop
  nop                         nop
  nop                         nop

  ;; Save back into GIF packet.
  nop                         sq.xyzw vf10, 0(vi04)

  ;; Increment registers and finish for loop.
  nop                         isubiu vi03, vi03, 1
  nop                         iaddiu vi04, vi04, 2
  nop                         ibne vi03, vi00, next_point
  nop                         nop

  ;; Send GIF packet to the GS
  nop                         xgkick vi01
  nop                         nop
  nop[E]                      nop

