/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include "table/riscv.h"

struct _table_riscv table_riscv[] =
{
  { "lui",     0x00000037, 0x0000007f, OP_U_TYPE,  -1, -1 },
  { "auipc",   0x00000017, 0x0000007f, OP_U_TYPE,  -1, -1 },
  { "jal",     0x0000006f, 0x0000007f, OP_UJ_TYPE, -1, -1 },
  { "jalr",    0x00000067, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "beq",     0x00000063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "bne",     0x00001063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "blt",     0x00004063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "bge",     0x00005063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "bltu",    0x00006063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "bgeu",    0x00007063, 0x0000707f, OP_SB_TYPE, -1, -1 },
  { "lb",      0x00000003, 0x0000707f, OP_RD_INDEX_R,  -1, -1 },
  { "lh",      0x00001003, 0x0000707f, OP_RD_INDEX_R,  -1, -1 },
  { "lw",      0x00002003, 0x0000707f, OP_RD_INDEX_R,  -1, -1 },
  { "lbu",     0x00004003, 0x0000707f, OP_RD_INDEX_R,  -1, -1 },
  { "lhu",     0x00005003, 0x0000707f, OP_RD_INDEX_R,  -1, -1 },
  { "sb",      0x00000023, 0x0000707f, OP_RS_INDEX_R,  -1, -1 },
  { "sh",      0x00001023, 0x0000707f, OP_RS_INDEX_R,  -1, -1 },
  { "sw",      0x00002023, 0x0000707f, OP_RS_INDEX_R,  -1, -1 },
  { "addi",    0x00000013, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "slti",    0x00002013, 0x0000707f, OP_UI_TYPE,  -1, -1 },
  { "sltiu",   0x00003013, 0x0000707f, OP_UI_TYPE,  -1, -1 },
  { "xori",    0x00004013, 0x0000707f, OP_UI_TYPE,  -1, -1 },
  { "ori",     0x00006013, 0x0000707f, OP_UI_TYPE,  -1, -1 },
  { "andi",    0x00007013, 0x0000707f, OP_UI_TYPE,  -1, -1 },
  { "slli",    0x00001013, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "srli",    0x00005013, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "srai",    0x40005013, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "add",     0x00000033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sub",     0x40000033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sll",     0x00001033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "slt",     0x00002033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sltu",    0x00003033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "xor",     0x00004033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "srl",     0x00005033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sra",     0x40005033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "or",      0x00006033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "and",     0x00007033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "fence",   0x0000000f, 0xf00fffff, OP_FENCE,   -1, -1 },
  { "fence.i", 0x0000100f, 0xffffffff, OP_FFFF,    -1, -1 },
  { "scall",   0x00000073, 0xffffffff, OP_FFFF,    -1, -1 },
  { "sbreak",  0x00100073, 0xffffffff, OP_FFFF,    -1, -1 },
  { "rdcycle",   0xc0002073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdcycleh",  0xc8002073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdtime",    0xc0102073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdtimeh",   0xc8102073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdinstret", 0xc0202073, 0xfffff07f, OP_READ,    -1, -1 },
  { "rdinstreth",0xc8202073, 0xfffff07f, OP_READ,    -1, -1 },
  { "lwu",     0x00006003, 0x0000707f, OP_RD_INDEX_R,  -1, -1 },
  { "ld",      0x00003003, 0x0000707f, OP_RD_INDEX_R,  -1, -1 },
  { "sd",      0x00003023, 0x0000707f, OP_RS_INDEX_R,  -1, -1 },
  //{ "slli",    0x00001013, 0xfe00707f, OP_SHIFT,   -1, -1 },
  //{ "srli",    0x00005013, 0xfe00707f, OP_SHIFT,   -1, -1 },
  //{ "srai",    0x40005013, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "addiw",   0x0000001b, 0x0000707f, OP_I_TYPE,  -1, -1 },
  { "slliw",   0x0000101b, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "srliw",   0x0000501b, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "sraiw",   0x4000501b, 0xfe00707f, OP_SHIFT,   -1, -1 },
  { "addw",    0x0000003b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "subw",    0x4000003b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sllw",    0x0000103b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "srlw",    0x0000503b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "sraw",    0x4000503b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "mul",     0x02000033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "mulh",    0x02001033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "mulhsu",  0x02002033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "mulhu",   0x02003033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "div",     0x02004033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "divu",    0x02005033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "rem",     0x02006033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "remu",    0x02007033, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "mulw",    0x0200003b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "divw",    0x0200403b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "divuw",   0x0200503b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "remw",    0x0200603b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "remuw",   0x0200703b, 0xfe00707f, OP_R_TYPE,  -1, -1 },
  { "lr.w",      0x1000202f, 0xf800707f, OP_LR,    -1, -1 },
  { "sc.w",      0x1800202f, 0xf800707f, OP_STD_EXT,    -1, -1 },
  { "amoswap.w", 0x0800202f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amoadd.w",  0x0000202f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amoxor.w",  0x2000202f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amoand.w",  0x6000202f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amoor.w",   0x4000202f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amomin.w",  0x8000202f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amomax.w",  0xa000202f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amominu.w", 0xc000202f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amomaxu.w", 0xe000202f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "lr.d",      0x1000302f, 0xf800707f, OP_LR,      -1, -1 },
  { "sc.d",      0x1800302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amoswap.d", 0x0800302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amoadd.d",  0x0000302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amoxor.d",  0x2000302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amoand.d",  0x6000302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amoor.d",   0x4000302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amomin.d",  0x8000302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amomax.d",  0xa000302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amominu.d", 0xc000302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "amomaxu.d", 0xe000302f, 0xf800707f, OP_STD_EXT, -1, -1 },
  { "flw",       0x00002007, 0x0000707f, OP_F_INDEX_R,  -1, -1 },
  { "fsw",       0x00002027, 0x0000707f, OP_F_INDEX_R,  -1, -1 },
  { "fmadd.s",   0x00000043, 0x0600007f, OP_FP_FP_FP_FP_RM, -1, -1 },
  { "fmsub.s",   0x00000047, 0x0600007f, OP_FP_FP_FP_FP_RM, -1, -1 },
  { "fnmsub.s",  0x0000004b, 0x0600007f, OP_FP_FP_FP_FP_RM, -1, -1 },
  { "fnmadd.s",  0x0000004f, 0x0600007f, OP_FP_FP_FP_FP_RM, -1, -1 },
  { "fadd.s",    0x00000053, 0xfe00007f, OP_FP_FP_FP_RM, -1, -1 },
  { "fsub.s",    0x08000053, 0xfe00007f, OP_FP_FP_FP_RM, -1, -1 },
  { "fmul.s",    0x10000053, 0xfe00007f, OP_FP_FP_FP_RM, -1, -1 },
  { "fdiv.s",    0x18000053, 0xfe00007f, OP_FP_FP_FP_RM, -1, -1 },
  { "fsqrt.s",   0x58000053, 0xfff0007f, OP_FP_FP_RM, -1, -1 },
  { "fsgnj.s",   0x20000053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fsgnjn.s",  0x20001053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fsgnjx.s",  0x20002053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fmin.s",    0x28000053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fmax.s",    0x28001053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fcvt.w.s",  0xc0000053, 0xfff0007f, OP_R_FP_RM, -1, -1 },
  { "fcvt.wu.s", 0xc0100053, 0xfff0007f, OP_R_FP_RM, -1, -1 },
  { "fmv.x.s",   0xe0000053, 0xfe00707f, OP_R_FP, -1, -1 },
  { "feq.s",     0xa0002053, 0xfe00707f, OP_R_FP_FP, -1, -1 },
  { "flt.s",     0xa0001053, 0xfe00707f, OP_R_FP_FP, -1, -1 },
  { "fle.s",     0xa0000053, 0xfe00707f, OP_R_FP_FP, -1, -1 },
  { "fclass.s",  0xe0001053, 0xfff0707f, OP_R_FP, -1, -1 },
  { "fcvt.s.w",  0xd0000053, 0xfff0007f, OP_FP_R_RM, -1, -1 },
  { "fcvt.s.wu", 0xd0100053, 0xfff0007f, OP_FP_R_RM, -1, -1 },
  { "fmv.s.x",   0xf0000053, 0xfff0707f, OP_FP_R, -1, -1 },
  { "frcsr",     0x00302073, 0xfffff07f, OP_READ, -1, -1 },
  { "frrm",      0x00202073, 0xfffff07f, OP_READ, -1, -1 },
  { "frflags",   0x00102073, 0xfffff07f, OP_READ, -1, -1 },
  { "fscsr",     0x00301073, 0xfff0707f, OP_R_R,  -1, -1 },
  { "fsrm",      0x00201073, 0xfff0707f, OP_R_R,  -1, -1 },
  { "fsflags",   0x00101073, 0xfff0707f, OP_R_R,  -1, -1 },
  //{ "fsrmi",     0x00102073, 0xfffff07f, OP_FP, -1, -1 },
  //{ "fsflagsi",  0x00302073, 0xfffff07f, OP_FP, -1, -1 },
  { "fcvt.l.s",  0xc0200053, 0xfff0007f, OP_R_FP_RM, -1, -1 },
  { "fcvt.lu.s", 0xc0300053, 0xfff0007f, OP_R_FP_RM, -1, -1 },
  { "fcvt.s.l",  0xd0200053, 0xfff0007f, OP_FP_R_RM, -1, -1 },
  { "fcvt.s.lu", 0xd0300053, 0xfff0007f, OP_FP_R_RM, -1, -1 },
  { "fld",       0x00003007, 0x0000707f, OP_F_INDEX_R,  -1, -1 },
  { "fsd",       0x00003027, 0x0000707f, OP_F_INDEX_R,  -1, -1 },
  { "fmadd.d",   0x02000043, 0x0600007f, OP_FP_FP_FP_FP_RM, -1, -1 },
  { "fmsub.d",   0x02000047, 0x0600007f, OP_FP_FP_FP_FP_RM, -1, -1 },
  { "fnmsub.d",  0x0200004b, 0x0600007f, OP_FP_FP_FP_FP_RM, -1, -1 },
  { "fnmadd.d",  0x0200004f, 0x0600007f, OP_FP_FP_FP_FP_RM, -1, -1 },
  { "fadd.d",    0x02000053, 0xfe00007f, OP_FP_FP_FP_RM, -1, -1 },
  { "fsub.d",    0x09000053, 0xfe00007f, OP_FP_FP_FP_RM, -1, -1 },
  { "fmul.d",    0x11000053, 0xfe00007f, OP_FP_FP_FP_RM, -1, -1 },
  { "fdiv.d",    0x19000053, 0xfe00007f, OP_FP_FP_FP_RM, -1, -1 },
  { "fsqrt.d",   0x59000053, 0xfff0007f, OP_FP_FP_RM, -1, -1 },
  { "fsgnj.d",   0x21000053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fsgnjn.d",  0x21001053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fsgnjx.d",  0x21002053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fmin.d",    0x29000053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fmax.d",    0x29001053, 0xfe00707f, OP_FP_FP_FP, -1, -1 },
  { "fcvt.s.d",  0x40100053, 0xfff0007f, OP_FP_FP_RM, -1, -1 },
  { "fcvt.d.s",  0x42000053, 0xfff0007f, OP_FP_FP_RM, -1, -1 },
  { "feq.d",     0xa2002053, 0xfe00707f, OP_R_FP_FP, -1, -1 },
  { "flt.d",     0xa2001053, 0xfe00707f, OP_R_FP_FP, -1, -1 },
  { "fle.d",     0xa2000053, 0xfe00707f, OP_R_FP_FP, -1, -1 },
  { "fclass.d",  0xe2001053, 0xfff0707f, OP_R_FP,    -1, -1 },
  { "fcvt.w.d",  0xc2000053, 0xfff0007f, OP_R_FP_RM, -1, -1 },
  { "fcvt.wu.d", 0xc0100053, 0xfff0007f, OP_R_FP_RM, -1, -1 },
  { "fcvt.d.w",  0xd2000053, 0xfff0007f, OP_FP_R_RM, -1, -1 },
  { "fcvt.d.wu", 0xd2100053, 0xfff0007f, OP_FP_R_RM, -1, -1 },
  { "fcvt.l.d",  0xc2200053, 0xfff0007f, OP_R_FP_RM, -1, -1 },
  { "fcvt.lu.d", 0xc2300053, 0xfff0007f, OP_R_FP_RM, -1, -1 },
  { "fmv.x.d",   0xe2000053, 0xfff0707f, OP_R_FP,    -1, -1 },
  { "fcvt.d.l",  0xd2200053, 0xfff0007f, OP_FP_R_RM, -1, -1 },
  { "fcvt.d.lu", 0xd2300053, 0xfff0007f, OP_FP_R_RM, -1, -1 },
  { "fmv.d.x",   0xf2000053, 0xfff0707f, OP_FP_R,    -1, -1 },
  { NULL, 0, 0, 0, 0 }
};

