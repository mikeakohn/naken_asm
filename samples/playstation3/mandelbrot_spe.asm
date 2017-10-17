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
  il r16, 0
  il r14, 1
  il r17, 2
  il r18, 3

  ; r3 = [ 4.0, 4.0, 4.0, 4.0 ]
  ; r8 = [ 2.0, 2.0, 2.0, 2.0 ]
  lqa r3, mandel_max
  lqa r8, mul_by_2

  ; r20 = [ picture, ?, ?, ? ]
  il r20, picture

  ;; Wait for data on channel 29 
  rdch r1, 29 

  ;; Add: r1 = r1 + r2
  a r1, r1, r2

  ;; Write back to PPE
  wrch 28, r1

  ;bra main

  ;; Stop the cell program
  sync
  stop 0

.align_bytes 16
picture:

