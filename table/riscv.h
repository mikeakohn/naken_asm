/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_RISCV_H
#define NAKEN_ASM_TABLE_RISCV_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_R_TYPE,
  OP_R_R,
  OP_I_TYPE,
  OP_UI_TYPE,
  OP_SB_TYPE,
  OP_U_TYPE,
  OP_UJ_TYPE,
  OP_SHIFT,
  OP_FENCE,
  OP_FFFF,
  OP_READ,
  OP_RD_INDEX_R,
  OP_FD_INDEX_R,
  OP_RS_INDEX_R,
  OP_FS_INDEX_R,
  OP_LR,
  OP_STD_EXT,
  OP_R_FP,
  OP_R_FP_RM,
  OP_R_FP_FP,
  OP_FP,
  OP_FP_FP,
  OP_FP_FP_RM,
  OP_FP_R,
  OP_FP_R_RM,
  OP_FP_FP_FP,
  OP_FP_FP_FP_RM,
  OP_FP_FP_FP_FP_RM,
  OP_ALIAS_RD_RS1,
  OP_ALIAS_RD_RS2,
  OP_ALIAS_FP_FP,
  OP_ALIAS_BR_RS_X0,
  OP_ALIAS_BR_X0_RS,
  OP_ALIAS_BR_RS_RT,
  OP_ALIAS_JAL,
  OP_ALIAS_JALR,
  // RISC-V C Extension (compressed).
  OP_COMP_RD_NZUIMM,
  OP_COMP_UIMM53_76,
  OP_COMP_UIMM548_76,
  OP_COMP_UIMM53_26,
  OP_COMP_JUMP,
  OP_COMP_9_46875,
  OP_COMP_RD_NZIMM5,
  OP_COMP_RD_IMM5,
  OP_COMP_RD_17_1612,
  OP_COMP_RD_NZ5_40,
  OP_COMP_RD_5_40,
  OP_COMP_RD,
  OP_COMP_RD_RS2,
  OP_COMP_BRANCH,

  OP_COMP_RD_RS1,
  OP_COMP_RD_5_4386,
  OP_COMP_RD_5_496,
  OP_COMP_RD_5_4276,
  OP_COMP_RS1,
  OP_COMP_RS1_RS2,
  OP_COMP_5386_RS2,
  OP_COMP_5496_RS2,
  OP_COMP_5276_RS2,
};

#define RISCV64  0x01
#define RISCV128 0x02
#define RISCV_FP 0x04

struct _table_riscv
{
  const char *instr;
  uint32_t opcode;
  uint32_t mask;
  uint8_t type;
  uint8_t flags;
};

struct _table_riscv_comp
{
  const char *instr;
  uint16_t opcode;
  uint16_t mask;
  uint8_t type;
  uint8_t flags;
};

extern struct _table_riscv table_riscv[];
extern struct _table_riscv_comp table_riscv_comp[];

class RiscvPerm
{
public:
  static int8_t nzuimm[11];
  static int8_t uimm53_76[11];
  static int8_t uimm548_76[11];
  static int8_t uimm53_26[11];
  static int8_t jump[11];
  static int8_t imm9_46875[11];
  static int8_t nzimm5[11];
  static int8_t imm5[11];
  static int8_t imm17_1612[11];
  static int8_t branch[11];

private:
  RiscvPerm() { }
  ~RiscvPerm() { }
};

#endif

