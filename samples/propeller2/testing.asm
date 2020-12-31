.propeller2

main:
  nop
  rol 0x4, #5
  rol 0x4, 0xff
  modcz _nc_and_nz, _c_eq_z
  modcz 2, _c_eq_z
  mov dira, #5
  rdbyte 0x134, ptra
  rdbyte 0x134, ptrb[3]
  rdbyte 0x134, ptra++
  rdbyte 0x134, ++ptra
  rdbyte 0x134, ptra--
  rdbyte 0x134, --ptra
  rdbyte 0x134, ptra++[16]
  rdbyte 0x134, ptra++[11]
  rdbyte 0x134, ++ptra[11]
  rdbyte 0x134, ptra--[5]
  rdbyte 0x134, --ptra[5]

  ;; It's interesting in the documentation it looks like the index is
  ;; supposed to be negative, but instead it's written positive and
  ;; encoded in the opcode as negative.
  wrbyte 0x90, ++ptrb[4]
  wrword 0x90, ptra--[4]
  wrword 0x90, --ptra[4]

  ;; Some instructions that add AUG instrucitons.
  add 0x90, ##0xff000
  rdbyte 0x90, ##0xff000
  rdbyte 0x90, ++ptrb[##0x12345]
  rdbyte 0x90, --ptrb[##0x12345]
  rdbyte 0x90, ptrb++[##0x12345]
  rdbyte 0x90, ptrb--[##0x12345]
  rdbyte 0x90, ptrb[##0x12345]


