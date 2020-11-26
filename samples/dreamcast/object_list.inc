object_list_start:

;; Paramter control word is:
;; [bit 32-24 para control] [23-16 group control] [15-0 obj control]
;;   Para control is:
;;   [31-29 para type] [28 end of strip] [27 reserved] [26-24 list type]
;;      Para types:
;;      0: end of list
;;      1: user tile clip
;;      2: object set list
;;      4: polygon/modifier volume
;;      5: sprite
;;      7: vertex
;;
;;      List types:
;;      0: opaque
;;      1: opaque modifier voume
;;      2: translucent
;;      3: translucent modifier volume
;;      4: punch through
vertex_0:
  .dc32 (7 << 29)
  .dc32 100.0      ;; X
  .dc32 100.0      ;; Y
  .dc32   0.0      ;; Z
  .dc32     0      ;; ignored
  .dc32     0      ;; ignored
  .dc32 0x00ff0000 ;; Base Color
  .dc32     0      ;; ignored

vertex_1:
  .dc32 (7 << 29)
  .dc32 100.0      ;; X
  .dc32 200.0      ;; Y
  .dc32   0.0      ;; Z
  .dc32     0      ;; ignored
  .dc32     0      ;; ignored
  .dc32 0x0000ff00 ;; Base Color
  .dc32     0      ;; ignored

vertex_2:
  .dc32 (7 << 29) | (1 << 28)
  .dc32 200.0      ;; X
  .dc32 200.0      ;; Y
  .dc32   0.0      ;; Z
  .dc32     0      ;; ignored
  .dc32     0      ;; ignored
  .dc32 0x000000ff ;; Base Color
  .dc32     0      ;; ignored

end_of_list:
  .dc32 0
  .dc32     0      ;; ignored
  .dc32     0      ;; ignored
  .dc32     0      ;; ignored
  .dc32     0      ;; ignored
  .dc32     0      ;; ignored
  .dc32     0      ;; ignored
  .dc32     0      ;; ignored

object_list_end:


