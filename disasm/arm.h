/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_DISASM_ARM_H
#define NAKEN_ASM_DISASM_ARM_H

#include "common/assembler.h"

#define ALU_MASK   0x0c000000
#define ALU_OPCODE 0x00000000
#define MUL_MASK   0x0fc000f0
#define MUL_OPCODE 0x00000090
#define MUL_LONG_MASK   0x0f8000f0
#define MUL_LONG_OPCODE 0x00100090
#define SWAP_MASK   0x0fb00ff0
#define SWAP_OPCODE 0x01000090
#define MRS_MASK   0x0fbf0fff
#define MRS_OPCODE 0x010f0000
#define MSR_ALL_MASK   0x0fbffff0
#define MSR_ALL_OPCODE 0x0129f000
#define MSR_FLAG_MASK   0x0dbff000
#define MSR_FLAG_OPCODE 0x0128f000
#define LDR_STR_MASK   0x0c000000
#define LDR_STR_OPCODE 0x04000000
#define UNDEF_MASK   0x0e000010
#define UNDEF_OPCODE 0x06000010
#define LDM_STM_MASK   0x0e000000
#define LDM_STM_OPCODE 0x08000000
#define BRANCH_MASK   0x0e000000
#define BRANCH_OPCODE 0x0a000000
#define BRANCH_EXCH_MASK   0x0ffffff0
#define BRANCH_EXCH_OPCODE 0x012fff10
#define CO_TRANSFER_MASK   0x0e000000
#define CO_TRANSFER_OPCODE 0x0c000000
#define CO_OP_MASK   0x0f000010
#define CO_OP_OPCODE 0x0e000000
#define CO_RTRANSFER_MASK   0x0f000010
#define CO_RTRANSFER_OPCODE 0x0e000010
#define CO_SWI_MASK   0x0f000000
#define CO_SWI_OPCODE 0x0f000000

//extern char *arm_alu_ops[];
extern char *arm_shift[];

int get_cycle_count_arm(unsigned short int opcode);
int disasm_arm(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_arm(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_arm(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end);

#endif

