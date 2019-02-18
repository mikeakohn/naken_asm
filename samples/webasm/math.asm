.webasm

.dc8 0, "asm"
.dc32 1

;;;;;; Type section     (1) ;;;;;;
.varuint 1
.varuint32 end_type - start_type
start_type:
;; Type count.
.varuint 1
;; Type is function.
.varuint 0x60 
;; Parameter count is 2.
.varuint 2
;; Parameter types are (i32, i32).
.varuint 0x7f 
.varuint 0x7f 
;; Return count is 1.
.varuint 1
;; Return type is i32.
.varuint 0x7f 
end_type:

;;;;;; Function section (3) ;;;;;;
.varuint 3
.varuint32 end_function - start_function
start_function:
;; Function count.
.varuint 1
;; Function index 0.
.varuint 0
end_function:

;;;;;; Memory section (5) ;;;;;;
.varuint 5
.varuint32 end_memory - start_memory
start_memory:
;; Memory count.
.varuint 1
;; Memory 0, flags = 0, initial = 1.
.varuint 0
.varuint 1
end_memory:

;;;;;; Global section (6) ;;;;;;
.varuint 6
.varuint32 end_global - start_global
start_global:
;; Global count.
.varuint 0
end_global:

;;;;;; Export section   (7) ;;;;;;
.varuint 7
.varuint32 end_export - start_export
start_export:
;; Export count.
.varuint 2
;; Field length
.varuint32 field0_name_end - field0_name_start
field0_name_start:
.db "memory"
field0_name_end:
.varuint 2
.varuint 0
.varuint32 field1_name_end - field1_name_start
field1_name_start:
.db "add_nums"
field1_name_end:
.varuint 0
.varuint 0
end_export:

;;;;;; Code section    (10) ;;;;;;
.varuint 10
.varuint32 code_end - code_start
code_start:

;; Function count.
.varuint 1

;; Function 0, body_size, local_count=0
.varuint32 code_end - function_0
function_0:
.varuint 0

start:
get_local 0
get_local 1
i32.add
end

code_end:

