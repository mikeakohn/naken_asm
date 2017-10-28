/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#include "cpu_list.h"

#include "asm/4004.h"
#include "asm/6502.h"
#include "asm/65816.h"
#include "asm/6800.h"
#include "asm/6809.h"
#include "asm/68hc08.h"
#include "asm/68000.h"
#include "asm/8051.h"
#include "asm/arc.h"
#include "asm/arm.h"
#include "asm/avr8.h"
#include "asm/cell.h"
#include "asm/common.h"
#include "asm/dspic.h"
#include "asm/epiphany.h"
#include "asm/lc3.h"
#include "asm/mips.h"
#include "asm/msp430.h"
#include "asm/pdp8.h"
#include "asm/pic14.h"
#include "asm/powerpc.h"
#include "asm/propeller.h"
#include "asm/ps2_ee_vu.h"
#include "asm/riscv.h"
#include "asm/stm8.h"
#include "asm/super_fx.h"
#include "asm/thumb.h"
#include "asm/tms1000.h"
#include "asm/tms9900.h"
#include "asm/z80.h"
#include "disasm/4004.h"
#include "disasm/6502.h"
#include "disasm/65816.h"
#include "disasm/6800.h"
#include "disasm/6809.h"
#include "disasm/68hc08.h"
#include "disasm/68000.h"
#include "disasm/8051.h"
#include "disasm/arc.h"
#include "disasm/arm.h"
#include "disasm/avr8.h"
#include "disasm/cell.h"
#include "disasm/dspic.h"
#include "disasm/epiphany.h"
#include "disasm/lc3.h"
#include "disasm/mips.h"
#include "disasm/msp430.h"
#include "disasm/pdp8.h"
#include "disasm/pic14.h"
#include "disasm/powerpc.h"
#include "disasm/propeller.h"
#include "disasm/ps2_ee_vu.h"
#include "disasm/riscv.h"
#include "disasm/stm8.h"
#include "disasm/super_fx.h"
#include "disasm/thumb.h"
#include "disasm/tms1000.h"
#include "disasm/tms9900.h"
#include "disasm/z80.h"
#include "simulate/6502.h"
#include "simulate/65816.h"
#include "simulate/avr8.h"
#include "simulate/lc3.h"
#include "simulate/mips.h"
#include "simulate/msp430.h"
#include "simulate/tms9900.h"
#include "simulate/z80.h"

#define NO_FLAGS 0

struct _cpu_list cpu_list[] =
{
#ifdef ENABLE_MSP430
  { "msp430", CPU_TYPE_MSP430, ENDIAN_LITTLE, 1, ALIGN_2, 0, 0, 1, SREC_16, parse_instruction_msp430, NULL, list_output_msp430, disasm_range_msp430, simulate_init_msp430, NO_FLAGS },
  { "msp430x", CPU_TYPE_MSP430X, ENDIAN_LITTLE, 1, ALIGN_2, 0, 0, 1, SREC_24, parse_instruction_msp430, NULL, list_output_msp430x, disasm_range_msp430x, simulate_init_msp430, NO_FLAGS },
#endif
#ifdef ENABLE_4004
  { "4004", CPU_TYPE_4004, ENDIAN_LITTLE, 1, ALIGN_1, 1, 0, 0, SREC_16, parse_instruction_4004, NULL, list_output_4004, disasm_range_4004, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_6502
  { "6502", CPU_TYPE_6502, ENDIAN_LITTLE, 1, ALIGN_1, 1, 0, 1, SREC_16, parse_instruction_6502, NULL, list_output_6502, disasm_range_6502, simulate_init_6502, NO_FLAGS },
#endif
#ifdef ENABLE_65816
  { "65816", CPU_TYPE_65816, ENDIAN_LITTLE, 1, ALIGN_1, 1, 0, 1, SREC_16, parse_instruction_65816, NULL, list_output_65816, disasm_range_65816, simulate_init_65816, NO_FLAGS },
#endif
#ifdef ENABLE_6800
  { "6800", CPU_TYPE_6800, ENDIAN_BIG, 1, ALIGN_1, 1, 0, 0, SREC_16, parse_instruction_6800, NULL, list_output_6800, disasm_range_6800, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_6809
  { "6809", CPU_TYPE_6809, ENDIAN_BIG, 1, ALIGN_1, 1, 0, 0, SREC_16, parse_instruction_6809, NULL, list_output_6809, disasm_range_6809, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_68HC08
  { "68hc08", CPU_TYPE_68HC08, ENDIAN_BIG, 1, ALIGN_1, 1, 0, 0, SREC_16, parse_instruction_68hc08, NULL, list_output_68hc08, disasm_range_68hc08, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_68000
  { "68000", CPU_TYPE_68000, ENDIAN_BIG, 1, ALIGN_2, 1, 0, 1, SREC_32, parse_instruction_68000, NULL, list_output_68000, disasm_range_68000, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_8051
  { "8051", CPU_TYPE_8051, ENDIAN_LITTLE, 1, ALIGN_1, 0, 0, 0, SREC_16, parse_instruction_8051, NULL, list_output_8051, disasm_range_8051, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_ARC
  { "arc", CPU_TYPE_ARC, ENDIAN_LITTLE, 1, ALIGN_4, 0, 0, 0, SREC_32, parse_instruction_arc, NULL, list_output_arc, disasm_range_arc, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_ARM
  { "arm", CPU_TYPE_ARM, ENDIAN_LITTLE, 1, ALIGN_4, 0, 0, 0, SREC_32, parse_instruction_arm, NULL, list_output_arm, disasm_range_arm, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_AVR8
  { "avr8", CPU_TYPE_AVR8, ENDIAN_LITTLE, 2, ALIGN_2, 0, 0, 0, SREC_16, parse_instruction_avr8, NULL, list_output_avr8, disasm_range_avr8, simulate_init_avr8, NO_FLAGS },
#endif
#ifdef ENABLE_CELL
  { "cell", CPU_TYPE_CELL, ENDIAN_BIG, 1, ALIGN_4, 0, 0, 0, SREC_32, parse_instruction_cell, NULL, list_output_cell, disasm_range_cell, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_DSPIC
  { "dspic", CPU_TYPE_DSPIC, ENDIAN_LITTLE, 2, ALIGN_2, 0, 0, 0, SREC_16, parse_instruction_dspic, NULL, list_output_dspic, disasm_range_dspic, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_EPIPHANY
  { "epiphany", CPU_TYPE_EPIPHANY, ENDIAN_LITTLE, 1, ALIGN_4, 0, 0, 1, SREC_32, parse_instruction_epiphany, NULL, list_output_epiphany, disasm_range_epiphany, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_LC3
  { "lc3", CPU_TYPE_LC3, ENDIAN_BIG, 2, ALIGN_2, 0, 0, 0, SREC_16, parse_instruction_lc3, NULL, list_output_lc3, disasm_range_lc3,  simulate_init_lc3, NO_FLAGS },
#endif
#ifdef ENABLE_MIPS
  { "mips", CPU_TYPE_MIPS32, ENDIAN_LITTLE, 1, ALIGN_4, 0, 0, 0, SREC_32, parse_instruction_mips, NULL, list_output_mips, disasm_range_mips, simulate_init_mips, MIPS_I | MIPS_II | MIPS_III },
  { "mips32", CPU_TYPE_MIPS32, ENDIAN_LITTLE, 1, ALIGN_4, 0, 0, 0, SREC_32, parse_instruction_mips, NULL, list_output_mips, disasm_range_mips, simulate_init_mips, MIPS_I | MIPS_II | MIPS_III | MIPS_FPU | MIPS_MSA },
  { "pic32", CPU_TYPE_MIPS32, ENDIAN_LITTLE, 1, ALIGN_4, 0, 0, 0, SREC_32, parse_instruction_mips, NULL, list_output_mips, disasm_range_mips, simulate_init_mips, MIPS_I | MIPS_II | MIPS_III | MIPS_32 },
  { "ps2_ee", CPU_TYPE_EMOTION_ENGINE, ENDIAN_LITTLE, 1, ALIGN_16, 0, 0, 0, SREC_32, parse_instruction_mips, NULL, list_output_mips, disasm_range_mips, simulate_init_mips, MIPS_I | MIPS_II | MIPS_III | MIPS_IV | MIPS_FPU | MIPS_EE_CORE | MIPS_EE_VU },
#endif
#ifdef ENABLE_PDP8
  { "pdp8", CPU_TYPE_PDP8, ENDIAN_LITTLE, 2, ALIGN_2, 0, 0, 0, SREC_16, parse_instruction_pdp8, NULL, list_output_pdp8, disasm_range_pdp8, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_PIC14
  { "pic14", CPU_TYPE_PIC14, ENDIAN_LITTLE, 2, ALIGN_2, 0, 0, 0, SREC_16, parse_instruction_pic14, NULL, list_output_pic14, disasm_range_pic14, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_DSPIC
  { "pic24", CPU_TYPE_PIC24, ENDIAN_LITTLE, 2, ALIGN_2, 0, 0, 0, SREC_24, parse_instruction_dspic, NULL, list_output_dspic, disasm_range_dspic, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_POWERPC
  { "powerpc", CPU_TYPE_POWERPC, ENDIAN_BIG, 1, ALIGN_4, 0, 0, 0, SREC_32, parse_instruction_powerpc, NULL, list_output_powerpc, disasm_range_powerpc, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_PROPELLER
  { "propeller", CPU_TYPE_PROPELLER, ENDIAN_LITTLE, 4, ALIGN_4, 0, 0, 0, SREC_16, parse_instruction_propeller, NULL, list_output_propeller, disasm_range_propeller, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_EMOTION_ENGINE
  { "ps2_ee_vu0", CPU_TYPE_PS2_EE_VU, ENDIAN_LITTLE, 1, ALIGN_16, 0, 0, 0, SREC_32, parse_instruction_ps2_ee_vu, NULL, list_output_ps2_ee_vu, disasm_range_ps2_ee_vu, NULL, PS2_EE_VU0 },
  { "ps2_ee_vu1", CPU_TYPE_PS2_EE_VU, ENDIAN_LITTLE, 1, ALIGN_16, 0, 0, 0, SREC_32, parse_instruction_ps2_ee_vu, NULL, list_output_ps2_ee_vu, disasm_range_ps2_ee_vu, NULL, PS2_EE_VU1 },
#endif
#ifdef ENABLE_RISCV
  { "riscv", CPU_TYPE_RISCV, ENDIAN_LITTLE, 1, ALIGN_4, 1, 0, 1, SREC_32, parse_instruction_riscv, NULL, list_output_riscv, disasm_range_riscv, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_STM8
  { "stm8", CPU_TYPE_STM8, ENDIAN_BIG, 1, ALIGN_1, 1, 0, 1, SREC_16, parse_instruction_stm8, NULL, list_output_stm8, disasm_range_stm8, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_SUPER_FX
  { "super_fx", CPU_TYPE_SUPER_FX, ENDIAN_LITTLE, 1, ALIGN_1, 1, 0, 1, SREC_16, parse_instruction_super_fx, NULL, list_output_super_fx, disasm_range_super_fx, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_THUMB
  { "thumb", CPU_TYPE_THUMB, ENDIAN_LITTLE, 1, ALIGN_2, 0, 0, 0, SREC_32, parse_instruction_thumb, NULL, list_output_thumb, disasm_range_thumb, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_TMS1000
  { "tms1000", CPU_TYPE_TMS1000, ENDIAN_LITTLE, 1, ALIGN_1, 0, 0, 0, SREC_16, parse_instruction_tms1000, NULL, list_output_tms1000, disasm_range_tms1000, NULL, NO_FLAGS },
  { "tms1100", CPU_TYPE_TMS1100, ENDIAN_LITTLE, 1, ALIGN_1, 0, 0, 0, SREC_16, parse_instruction_tms1100, NULL, list_output_tms1100, disasm_range_tms1100, NULL, NO_FLAGS },
#endif
#ifdef ENABLE_TMS9900
  { "tms9900", CPU_TYPE_TMS9900, ENDIAN_BIG, 1, ALIGN_2, 0, 0, 0, SREC_16, parse_instruction_tms9900, NULL, list_output_tms9900, disasm_range_tms9900, simulate_init_tms9900, NO_FLAGS },
#endif
#ifdef ENABLE_Z80
  { "z80", CPU_TYPE_Z80, ENDIAN_LITTLE, 1, ALIGN_1, 0, 1, 0, SREC_16, parse_instruction_z80, NULL, list_output_z80, disasm_range_z80, simulate_init_z80, NO_FLAGS },
#endif
  { NULL },
};


