
INCLUDES=-I ../../include
PROGRAMS= \
  sample_no_rsp.z64 \
  sample_rsp_hardcoded.z64 \
  sample_rsp_rectangle.z64 \
  sample_rsp_rectangle_texture.z64 \
  sample_rsp_rotation.z64 \
  sample_rsp_rotation_z.z64 \
  sample_rsp_triangle.z64

default: rsp.bin picture.raw $(PROGRAMS)

rsp:
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o rsp.bin rsp.asm

%.z64: %.asm rsp.asm
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o $*.z64 $<
	./n64crc $*.z64

%.bin: %.asm
	../../naken_asm $(INCLUDES) -dump_symbols -l -type bin -o $*.bin $<

picture.raw:
	python3 picture.py

clean:
	@rm -f *.lst *.z64 rsp.bin picture.raw
	@echo "Clean!"

