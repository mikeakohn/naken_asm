.cell
.entry_point main

main:
  ;; Load #5 into 4 slots of the r2 register
  il r2, 5

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

