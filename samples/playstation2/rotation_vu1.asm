;; VU1 has 16k of program program memory and 16k data memory.
;; The structure sent from the core unit to VU1 is:
;;
;; struct _object
;; {
;;   float rotation_x, rotation_y, rotation_z, unused_0;
;;   float 0, dz, dy, dx;
;;   int count, unused_1, unused_2, unused_3;
;;   struct _data
;;   {
;;     float 0, z, y, x;
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
;; This structure will start at location 0 in data and starting at location
;; 3 (3 * 16 bytes into data memory) it will be copied to location
;; 512 (8192 bytes into memory).
;;
;; In memory a register is: { w,   z, y, x };
;;                   vf00 = { 1.0, 0, 0, 0 }

.ps2_ee_vu1
.include "playstation2/registers_gs_gp.inc"

.org 0
start:
  ;; vi01 points to the source GIF packet.
  ;; vi02 points to the rotation, position information.
  ;; vi03 holds the count.
  ;; vi04 points to the next point to be processed.
  ;; vi05 is temp
  ;; vi10 is the next point being processed
  ;; vf01 is { 0, 0, uint64_t(REG_XYZ2) }
  ;; vf02 position { 0, dz, dy, dx }
  ;; vf03 Pixel after being modified by position
  ;; vf10 is { 0, 0, 0, 0 }
  sub vf10, vf10, vf10  isub vi02, vi02, vi02
  nop iaddiu vi01, vi02, 3
  nop iaddiu vi04, vi01, 2
  ;nop iaddiu vi05, vi00, 512
  ;nop iaddiu vi06, vi00, 512

  ;; vf01 = { 0, 0, uint64_t(REG_XYZ2) }
  sub.xyzw vf01, vf01, vf01  iaddiu vi05, vi05, REG_XYZ2
  nop mfir.y vf01, vi05

  ;; vf02 = { 0, dz, dy, dx }
  nop lq.xyzw vf02, 1(vi02)

  ;; Setup count/vi03,
  nop ilw.w vi03, 3(vi02)
  ;nop ilw.w vi07, 3(vi02)

next_vertex:
  ;; Load next point from data RAM
  nop lq.xyzw vf10, 0(vi04)

  ;; Transpose point by { 0, dz, dy, dx } vector
  add.xyz vf10, vf10, vf02 nop

  ;; Convert point to 12/4 bit fixed point
  ftoi4.xyzw vf10, vf10, vf10 nop
  add.xyzw vf03, vf10, vf01  mtir.z vi10, vf10
  nop mtir.y vi11, vf10
  nop mtir.x vi12, vf10

  nop mfir.w vf03, vi10

  nop sq.xyzw vf03, 0(vi04)

  nop iaddiu vi04, vi04, 2
  nop isubiu vi03, vi03, 1
  nop ibne vi03, vi00, next_vertex

  ;; Send GIF packet to the GS
  nop nop
  nop xgkick vi01
  nop nop
  nop[E] nop

;; This appears pointless
done:
  nop b done
  nop nop

