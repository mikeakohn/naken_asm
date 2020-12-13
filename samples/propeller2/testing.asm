.propeller2

main:
  nop
  rol 0x4, #5
  rol 0x4, 0xff
  modcz _nc_and_nz, _c_eq_z
  modcz 2, _c_eq_z
  mov dira, #5

