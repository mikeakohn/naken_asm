/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 * 1802 file by Malik Enes Safak
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "asm/1802.h"
#include "disasm/1802.h"
#include "simulate/1802.h"
#include "table/1802.h"

#define READ_RAM(a) memory_read_m(simulate->memory, a)
#define WRITE_RAM(a, b) \
  if (a == simulate->break_io) \
  { \
  exit(b); \
  } \
  memory_write_m(simulate->memory, a, b)
#define READ_IND(a) READ_RAM(simulate_1802->reg_r[a])
#define WRITE_IND(a, b) WRITE_RAM(simulate_1802->reg_r[a], b)
#define REG(a) simulate_1802->reg_r[a]

#define REG_D simulate_1802->reg_d
#define REG_P simulate_1802->reg_p
#define REG_X simulate_1802->reg_x
#define REG_T simulate_1802->reg_t
#define REG_B simulate_1802->reg_b

#define REG_CNTR simulate_1802->reg_cntr
#define REG_CN simulate_1802->reg_cn

#define FLAG_DF simulate_1802->flag_df
#define FLAG_Q simulate_1802->flag_q
#define FLAG_MIE simulate_1802->flag_mie
#define FLAG_CIE simulate_1802->flag_cie
#define FLAG_XIE simulate_1802->flag_xie
#define FLAG_CIL simulate_1802->flag_cil
#define FLAG_ETQ simulate_1802->flag_etq
#define IRQ_CI FLAG_CIL
//#define IRQ_XI simulate_1802->irq_xi

#define PC simulate_1802->reg_r[REG_P]
#define DP simulate_1802->reg_r[REG_D]
#define REG_N simulate_1802->reg_n
#define REG_I simulate_1802->reg_i

static int stop_running = 0;

struct _simulate *simulate_init_1802(struct _memory *memory)
{
  struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_1802)+sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_1802;
  simulate->simulate_free = simulate_free_1802;
  simulate->simulate_dumpram = simulate_dumpram_1802;
  simulate->simulate_push = simulate_push_1802;
  simulate->simulate_set_reg = simulate_set_reg_1802;
  simulate->simulate_get_reg = simulate_get_reg_1802;
  simulate->simulate_set_pc = simulate_set_pc_1802;
  simulate->simulate_reset = simulate_reset_1802;
  simulate->simulate_dump_registers = simulate_dump_registers_1802;
  simulate->simulate_run = simulate_run_1802;

  simulate->memory = memory;
  simulate_reset_1802(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->show = 1; // Show simulation
  simulate->step_mode = 0;

  return simulate;
}

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

void simulate_push_1802(struct _simulate *simulate, uint32_t value)
{
  return;
}

static int operand_exe(struct _simulate *simulate, int opcode)
{
  struct _simulate_1802 *simulate_1802 = (struct _simulate_1802 *)simulate->context;

  REG_N = opcode & 0xF;
  REG_I = (opcode & 0xF0) >> 4;
  uint16_t temp;
  int _temp;
  uint16_t address;

  if (opcode < 0x00 || opcode > 0xFF)
  {
    return -1;
  }

  if (opcode == 0x00)
  {
    //Since this is not handling any dma or irq.
    ++PC;
    return 0;
  }

  int i;
  if (opcode == 0x68)
  {
    int _opcode = READ_RAM(PC + 1);

    for (i = 0; table_1802_16[i].instr != NULL; i++)
    {
      if ((_opcode & table_1802_16[i].mask) == table_1802_16[i].opcode)
      {
        // Every machine cycle is 8 cycle
        simulate->cycle_count += table_1802_16[i].cycles * 8;
        break;
      }
    }
  }
  else
  {
    for (i = 0; table_1802[i].instr != NULL; i++)
    {
      if ((opcode & table_1802[i].mask) == table_1802[i].opcode)
      {
        // Every machine cycle is 8 cycle
        simulate->cycle_count += table_1802[i].cycles * 8;
        break;
      }
    }
  }

  switch(REG_I)
  {
    case 0x0: //LDN(M[R])
      REG_D = READ_IND(REG_N);
      break;

    case 0x1: //INC(R)
      ++REG(REG_N);
      break;

    case 0x2: //DEC(R)
      --REG(REG_N);
      break;

    case 0x3: //SHORT BRANCH
      ++PC;
      address = ((PC & 0xFF00) | READ_RAM(PC)) - 1;
      switch(REG_N)
      {
        case 0x0: //BR
          PC = address;
          break;

        case 0x1: //BQ
          PC = FLAG_Q == 0 ? address : PC;
          break;

        case 0x2: //BZ
          PC = REG_D == 0 ? address : PC;
          break;

        case 0x3: //BDF
          PC = FLAG_DF != 0 ? address : PC;
          break;

        case 0x4: //B1
        case 0x5: //B2
        case 0x6: //B3
        case 0x7: //B4
        case 0x8: //NBR
          break;

        case 0x9: //BNQ
          PC = FLAG_Q != 0 ? address : PC;
          break;

        case 0xA: //BNZ
          PC = REG_D != 0 ? address : PC;
          break;

        case 0xB: //BNF
          PC = FLAG_DF == 0 ? address : PC;
          break;

        case 0xC: //BN1
        case 0xD: //BN2
        case 0xE: //BN3
        case 0xF: //BN4
          break;

        default:
          return -1;
      }
      break;

    case 0x4: //LDA
      REG_D = READ_IND(REG_N);
      ++REG(REG_N);
      break;

    case 0x5: //STR
      WRITE_IND(REG_N, REG_D);
      break;

    case 0x6: // MISC
      switch(REG_N)
      {
        case 0x0: //IRX
          ++REG(REG_X);
          break;

        case 0x1: //OUT1
        case 0x2: //OUT2
        case 0x3: //OUT3
        case 0x4: //OUT4
        case 0x5: //OUT5
        case 0x6: //OUT6
        case 0x7: //OUT7
          break;

        case 0x8:
          ++PC;
          uint8_t _extinstr = READ_RAM(PC);
          uint8_t _I = (_extinstr & 0xF0) >> 4;
          uint8_t _N = _extinstr & 0xF;
          switch(_I)
          {
            case 0x0: //MOSTLY COUNTER INSTRUCTIONS.
              //Counter is always in stop mode, only software decrementing
              // using DTC is possible.
              switch(_N)
              {
                case 0x0: //STPC
                  break;

                case 0x1: //DTC
                  if (REG_CNTR == 1)
                  {
                    if (FLAG_ETQ == 1) { FLAG_Q = !FLAG_Q; }
                    IRQ_CI = 1;
                    REG_CNTR = REG_CN;
                  }
                  else { --REG_CNTR; }
                  break;

                case 0x2: //SPM2
                case 0x3: //SCM2
                case 0x4: //SPM1
                case 0x5: //SCM1
                  break;

                case 0x6: //LDC
                  REG_CNTR = REG_D;
                  REG_CN = REG_D;
                  FLAG_ETQ = 0;
                  break;

                case 0x7: //STM
                  break;

                case 0x8: //GEC
                  REG_D = REG_CNTR;
                  break;

                case 0x9: //ETQ
                  FLAG_ETQ = 1;
                  break;

                case 0xA: //XIE
                  FLAG_XIE = 1;
                  break;

                case 0xB: //XID
                  FLAG_XIE = 0;
                  break;

                case 0xC: //CIE
                  FLAG_CIE = 1;
                  break;

                case 0xD: //CID
                  FLAG_CIE = 0;
                  break;

                default:
                  return -1;
              }
              break;

            case 0x2: //DBNZ
              PC += 2;
              address = ((READ_RAM(PC - 1) << 8) | (READ_RAM(PC))) - 1;
              PC = --REG(_N) != 0 ? address : PC;
              break;

            case 0x3:
              ++PC;
              address = ((PC & 0xFF00) | READ_RAM(PC)) - 1;
              switch(_N)
              {
                case 0xE: //BCI
                  PC = IRQ_CI == 0 ? address : PC;
                  break;

                case 0xF: //BXI
                  //PC = IRQ_XI == 0 ? address : PC;
                  break;

                default:
                  return -1;
              }
              break;

            case 0x6: //RLXA
              REG(_N) = READ_IND(REG_X) << 8;
              ++REG(REG_X);
              REG(_N) |= READ_IND(REG_X);
              ++REG(REG_X);
              break;

            case 0x7:
              switch(_N)
              {
                case 0x4: //DADC
                  _temp =  REG_D + READ_IND(REG_X) + FLAG_DF;
                  FLAG_DF = ((_temp > 99) | (_temp < 0)) ? 1 : 0;
                  _temp = _temp > 99 ? _temp - 100 : _temp < 0 ? 100 + _temp : _temp;
                  REG_D = ((_temp % 10) | (_temp / 10)) << 4;
                  break;

                case 0x6: //DSAV
                  --REG(REG_X);
                  WRITE_IND(REG_X, REG_T);
                  --REG(REG_X);
                  WRITE_IND(REG_X, REG_D);
                  --REG(REG_X);
                  REG_D >>= 1;
                  REG_D |= FLAG_DF << 7;
                  WRITE_IND(REG_X, REG_D);
                  break;

                case 0x7: //DSMB
                  _temp =  REG_D - READ_IND(REG_X) - !FLAG_DF;
                  FLAG_DF = ((_temp > 99) | (_temp < 0)) ? 0 : 1;
                  _temp = _temp > 99 ? _temp - 100 : _temp < 0 ? 100 + _temp : _temp;
                  REG_D = (_temp % 10) | (_temp / 10) << 4;
                  break;

                case 0xC: //DACI
                  ++PC;
                  _temp =  REG_D + READ_RAM(PC) + FLAG_DF;
                  FLAG_DF = ((_temp > 99) | (_temp < 0)) ? 1 : 0;
                  _temp = _temp > 99 ? _temp - 100 : _temp < 0 ? 100 + _temp : _temp;
                  REG_D = (_temp % 10) | (_temp / 10) << 4;
                  break;

                case 0xF: //DSBI
                  ++PC;
                  _temp =  REG_D - READ_RAM(PC) - !FLAG_DF;
                  FLAG_DF = ((_temp > 99) | (_temp < 0)) ? 0 : 1;
                  _temp = _temp > 99 ? _temp - 100 : _temp < 0 ? 100 + _temp : _temp;
                  REG_D = (_temp % 10) | (_temp / 10) << 4;
                  break;

                default:
                  return -1;
              }
              break;

            case 0x8: //SCAL
              WRITE_IND(REG_X, REG(_N) & 0xFF);
              --REG(REG_X);
              WRITE_IND(REG_X, (REG(_N) & 0xFF00) >> 8);
              --REG(REG_X);
              REG(_N) = REG(REG_P);
              REG(REG_P) = READ_IND(_N) << 8;
              ++REG(_N);
              REG(REG_P) |= READ_IND(_N);
              ++REG(_N);
              break;

            case 0x9: //SRET
              REG(REG_P) = REG(_N);
              ++REG(REG_X);
              REG(_N) = READ_IND(REG_X) << 8;
              ++REG(REG_X);
              REG(_N) |= READ_IND(REG_X);
            case 0xA: //RSXD
              WRITE_IND(REG_X, REG(_N) & 0xFF);
              --REG(REG_X);
              WRITE_IND(REG_X, (REG(_N) & 0xFF00) >> 8);
              --REG(REG_X);
              break;

            case 0xB: //RNX
              REG(REG_X) = REG(_N);
              break;

            case 0xC: //RLDI
              ++PC;
              REG(_N) = READ_RAM(PC) << 8;
              ++PC;
              REG(_N) |= READ_RAM(PC);
              break;

            case 0xF:
              switch(_N)
              {
                case 0x4: //DADD
                  _temp =  REG_D + READ_IND(REG_X);
                  FLAG_DF = ((_temp > 99) | (_temp < 0)) ? 1 : 0;
                  _temp = _temp > 99 ? _temp - 100 : _temp < 0 ? 100 + _temp : _temp;
                  REG_D = (_temp % 10) | (_temp / 10) << 4;
                  break;

                case 0x7: //DSM
                  _temp =  REG_D - READ_IND(REG_X);
                  FLAG_DF = ((_temp > 99) | (_temp < 0)) ? 0 : 1;
                  _temp = _temp > 99 ? _temp - 100 : _temp < 0 ? 100 + _temp : _temp;
                  REG_D = (_temp % 10) | (_temp / 10) << 4;
                  break;

                case 0xC: //DADI
                  ++PC;
                  _temp =  REG_D + READ_RAM(PC);
                  FLAG_DF = ((_temp > 99) | (_temp < 0)) ? 1 : 0;
                  _temp = _temp > 99 ? _temp - 100 : _temp < 0 ? 100 + _temp : _temp;
                  REG_D = (_temp % 10) | (_temp / 10) << 4;
                  break;

                case 0xF: //DSMI
                  ++PC;
                  _temp =  REG_D - READ_RAM(PC);
                  FLAG_DF = ((_temp > 99) | (_temp < 0)) ? 0 : 1;
                  _temp = _temp > 99 ? _temp - 100 : _temp < 0 ? 100 + _temp : _temp;
                  REG_D = (_temp % 10) | (_temp / 10) << 4;
                  break;
                default:
                  return -1;
              }
              break;
            default:
              return -1;
          }
          break;

        case 0x9: //IN1
        case 0xA: //IN2
        case 0xB: //IN3
        case 0xC: //IN4
        case 0xD: //IN5
        case 0xE: //IN6
        case 0xF: //IN7
          break;

        default:
          return -1;
      }
      break;

    case 0x7: //MOSTLY ARITHMETIC
      switch(REG_N)
      {
        case 0x0: //RET
          temp = READ_IND(REG_X);
          REG_X = (temp & 0xF0) >> 4;
          REG_P = temp & 0xF;
          ++REG(REG_X);
          FLAG_MIE = 1;
          break;

        case 0x1: //DIS
          temp = READ_IND(REG_X);
          REG_X = (temp & 0xF0) >> 4;
          REG_P = temp & 0xF;
          ++REG(REG_X);
          FLAG_MIE = 0;
          break;

        case 0x2: //LDXA
          REG_D = READ_IND(REG_X);
          ++REG(REG_X);
          break;

        case 0x3: //STXD
          WRITE_IND(REG_X, REG_D);
          --REG(REG_X);
          break;

        case 0x4: //ADC
          temp = READ_IND(REG_X) + REG_D + FLAG_DF;
          FLAG_DF = temp & 0x100 ? 1 : 0;
          REG_D = temp;
          break;

        case 0x5: //SDB
          temp = READ_IND(REG_X) - REG_D - !FLAG_DF;
          FLAG_DF = temp & 0x100 ? 0 : 1;
          REG_D = temp;
          break;

        case 0x6: //SHRC
          temp = REG_D & 0x1;
          REG_D >>= 1;
          REG_D |= FLAG_DF << 7;
          FLAG_DF = temp;
          break;

        case 0x7: //SMB
          temp = REG_D - READ_IND(REG_X) - !FLAG_DF;
          FLAG_DF = temp & 0x100 ? 0 : 1;
          REG_D = temp;
          break;

        case 0x8: //SAV
          WRITE_IND(REG_X, REG_T);
          break;

        case 0x9: //MARK
          REG_T = REG_X << 4 | REG_P;
          WRITE_IND(0x2, REG_T);
          REG_X = REG_P;
          --REG(2);
          break;

        case 0xA: //REQ
          FLAG_Q = 0;
          break;

        case 0xB: //SEQ
          FLAG_Q = 1;
          break;

        case 0xC: //ADCI
          ++PC;
          temp = READ_RAM(PC) + REG_D + FLAG_DF;
          FLAG_DF = temp & 0x100 ? 1 : 0;
          REG_D = temp;
          break;

        case 0xD: //SDBI
          ++PC;
          temp = READ_RAM(PC) - REG_D - !FLAG_DF;
          FLAG_DF = temp & 0x100 ? 0 : 1;
          REG_D = temp;
          break;

        case 0xE: //SHLC
          temp = REG_D & 0x80;
          REG_D <<= 1;
          REG_D |= FLAG_DF;
          FLAG_DF = temp;
          break;

        case 0xF: //SMBI
          ++PC;
          temp = REG_D - READ_RAM(PC) - !FLAG_DF;
          FLAG_DF = temp & 0x100 ? 0 : 1;
          REG_D = temp;
          break;
        default:
          return -1;
      }
      break;

    case 0x8: //GLO
      REG_D = REG(REG_N) & 0xFF;
      break;

    case 0x9: //GHI
      REG_D = (REG(REG_N) & 0xFF00) >> 8;
      break;

    case 0xA: //PLO
      REG(REG_N) &= 0xFF00;
      REG(REG_N) |= REG_D;
      break;

    case 0xB: //PHI
      REG(REG_N) &= 0xFF;
      REG(REG_N) |= REG_D << 8;
      break;

    case 0xC: //LONG BRANCH
      PC += 2;
      address = ((READ_RAM(PC - 1) << 8) | (READ_RAM(PC))) - 1;
      switch(REG_N)
      {
        case 0x0:
          PC = address;
          break;

        case 0x1: //LBQ
          PC = FLAG_Q == 0 ? address : PC;
          break;

        case 0x2: //LBZ
          PC = REG_D == 0 ? address : PC;
          break;

        case 0x3: //LBDF
          PC = FLAG_DF != 0 ? address : PC;
          break;

        case 0x4: //NOP
          PC -= 2;
          break;

        case 0x5: //LSNQ
          PC = FLAG_Q == 0 ? PC : PC - 2;
          break;

        case 0x6: //LSNZ
          PC = REG_D != 0 ? PC : PC - 2;
          break;

        case 0x7: //LSNF
          PC = FLAG_DF == 0 ? PC : PC - 2;
          break;

        case 0x8: //LSKP
          break;

        case 0x9: //LBNQ
          PC = FLAG_Q == 0 ? address : PC;
          break;

        case 0xA: //LBNZ
          PC = REG_D != 0 ? address : PC;
          break;

        case 0xB: //LBNF
          PC = FLAG_DF == 0 ? address : PC;
          break;

        case 0xC: //LSIE
          PC = FLAG_MIE != 0 ? PC : PC - 2;
          break;

        case 0xD: //LSQ
          PC = FLAG_Q != 0 ? PC : PC - 2;
          break;

        case 0xE: //LSZ
          PC = REG_D == 0 ? PC : PC - 2;
          break;

        case 0xF: //LSDF
          PC = FLAG_DF != 0 ? PC : PC - 2;
          break;
        default:
          return -1;
      }
      break;

    case 0xD: //SEP
      REG_P = REG_N;
      break;

    case 0xE: //SEX
      REG_X = REG_N;
      break;

    case 0xF: //MOSTLY LOGIC
      switch(REG_N)
      {
        case 0x0: //LDX
          REG_D = READ_IND(REG_X);
          break;

        case 0x1: //OR
          REG_D = READ_IND(REG_X) | REG_D;
          break;

        case 0x2: //AND
          REG_D = READ_IND(REG_X) & REG_D;
          break;

        case 0x3: //XOR
          REG_D = READ_IND(REG_X) ^ REG_D;
          break;

        case 0x4: //ADD
          temp = READ_IND(REG_X) + REG_D;
          FLAG_DF = temp & 0x100 ? 1 : 0;
          REG_D = temp;
          break;

        case 0x5: //SD
          temp = READ_IND(REG_X) - REG_D;
          FLAG_DF = temp & 0x100 ? 0 : 1;
          REG_D = temp;
          break;

        case 0x6: //SHR
          FLAG_DF = REG_D & 0x1;
          REG_D >>= 1;
          break;

        case 0x7: //SM
          temp = REG_D - READ_IND(REG_X);
          FLAG_DF = temp & 0x100 ? 0 : 1;
          REG_D = temp;
          break;

        case 0x8: //LDI
          ++PC;
          REG_D = READ_RAM(PC);
          break;

        case 0x9: //ORI
          ++PC;
          REG_D = READ_RAM(PC) | REG_D;
          break;

        case 0xA: //ANI
          ++PC;
          REG_D = READ_RAM(PC) & REG_D;
          break;

        case 0xB: //XRI
          ++PC;
          REG_D = READ_RAM(PC) ^ REG_D;
          break;

        case 0xC: //ADI
          ++PC;
          temp = READ_RAM(PC) + REG_D;
          FLAG_DF = temp & 0x100 ? 1 : 0;
          REG_D = temp;
          break;

        case 0xD: //SDI
          ++PC;
          temp = READ_RAM(PC) - REG_D;
          FLAG_DF = temp & 0x100 ? 0 : 1;
          REG_D = temp;
          break;

        case 0xE: //SHL
          temp = REG_D & 0x80;
          REG_D <<= 1;
          FLAG_DF = temp;
          break;

        case 0xF: //SMI
          ++PC;
          temp = REG_D - READ_RAM(PC);
          FLAG_DF = temp & 0x100 ? 0 : 1;
          REG_D = temp;
          break;
        default:
          return -1;
      }
      break;

    default:
      return -1;
  }
  ++PC;
  return 0;
}

void simulate_free_1802(struct _simulate *simulate)
{
  free(simulate);
}

void simulate_dump_registers_1802(struct _simulate *simulate)
{
  struct _simulate_1802 *simulate_1802 = (struct _simulate_1802 *)simulate->context;

  printf("\nSimulation Register Dump                                    \n");
  printf("------------------------------------------------------------\n");
  printf(" R0 = %04x,  R1 = %04x,  R2 = %04x,  R3 = %04x | D = %02x\n",
    REG(0),
    REG(1),
    REG(2),
    REG(3),
    REG_D);

  printf(" R4 = %04x,  R5 = %04x,  R6 = %04x,  R7 = %04x | P = %01x X = %01x\n",
    REG(4),
    REG(5),
    REG(6),
    REG(7),
    REG_P,
    REG_X);

  printf(" R8 = %04x,  R9 = %04x, R10 = %04x, R11 = %04x | I = %01x N = %01x\n",
    REG(8),
    REG(9),
    REG(10),
    REG(11),
    REG_I,
    REG_N);

  printf("R12 = %04x, R13 = %04x, R14 = %04x, R15 = %04x | T = %02x\n",
    REG(12),
    REG(13),
    REG(14),
    REG(15),
    REG_T);

  printf(" DF = %d,     IE = %d,      Q = %d      PC = %04x\n",
    FLAG_DF,
    FLAG_MIE,
    FLAG_Q,
    PC);

  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_1802(struct _simulate *simulate, int cycles, int step)
{
  struct _simulate_1802 *simulate_1802 = (struct _simulate_1802 *)simulate->context;
  char instruction[128];
  char bytes[16];

  stop_running = 0;
  signal(SIGINT, handle_signal);

  printf("Running... Press Ctl-C to break.\n");

  while (stop_running == 0)
  {
    int pc = PC;
    int opcode = READ_RAM(pc);
    int ret = operand_exe(simulate, opcode);

    if (ret == -1)
    {
      printf("Illegal instruction at address 0x%04x\n", pc);
      return -1;
    }

    if (simulate->show == 1)
    {
      printf("\x1b[1J\x1b[1;1H");
      simulate_dump_registers_1802(simulate);

      int cycles_min, cycles_max;
      int n = 0;
      while (n < 6)
      {
        int count = disasm_1802(simulate->memory, pc, instruction, &cycles_min, &cycles_max);
        int i;

        bytes[0] = 0;
        for (i = 0; i < count; i++)
        {
          char temp[4];
          sprintf(temp, "%02x ", READ_RAM(pc + i));
          strcat(bytes, temp);
        }

        if (cycles_min == -1) break;

        if (pc == simulate->break_point) { printf("*"); }
        else { printf(" "); }

        if (n == 0)
          { printf("! "); }
        else if (pc == PC)
          { printf("> "); }
        else
          { printf("  "); }

        printf("0x%04x: %-10s %-40s %d-%d\n", pc, bytes, instruction, cycles_min, cycles_max);

        if (count == 0) { break; }

        n++;
        pc += count;
      }
    }

    if (simulate->break_point == PC)
    {
      printf("Breakpoint hit at 0x%04x\n", simulate->break_point);
      break;
    }

    if ((simulate->usec == 0) || (step == 1))
    {
      signal(SIGINT, SIG_DFL);
      return 0;
    }
    usleep(simulate->usec > 999999 ? 999999 : simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  printf("Stopped.  PC=0x%04x.\n", PC);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}

int simulate_dumpram_1802(struct _simulate *simulate, int start, int end)
{
  //struct _simulate_1802 *simulate_1802 = (struct _simulate_1802 *)simulate->context;

  printf("\n                       Simulation RAM Dump                         \n");
  printf("---------------------------------------------------------------------\n");
  printf("       x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF\n");

  int i = 0;
  while (i != (end - start))
  {
    if (i % 16 == 0)
    {
      printf("%04x:", start + i);
    }

    printf("  %02x", READ_RAM(start + i));

    if (i % 16 == 15)
    {
      printf("\n");
    }

    ++i;
  }
  printf("\n\n");
  return 0;
}

int simulate_set_reg_1802(
  struct _simulate *simulate,
  char *reg_string,
  uint32_t value)
{
  struct _simulate_1802 *simulate_1802 = (struct _simulate_1802 *)simulate->context;

  while (*reg_string==' ') { reg_string++; }

  char *pos = reg_string;

  // d, p, x, t, i, n, r0, r1, r2..., df, ie, q

  if ((pos[0] == 'd' || pos[0] == 'D') && (pos[1] == 'f' || pos[1] == 'F')) { FLAG_DF = value & 0x1; }
  else if (pos[0] == 'd' || pos[0] == 'D') { REG_D = value & 0xFF; }
  else if ((pos[0] == 'p' || pos[0] == 'P') && (pos[1] == 'c' || pos[1] == 'C')) { PC = value & 0xFFFF; }
  else if ((pos[0] == 'i' || pos[0] == 'I') && (pos[1] == 'e' || pos[1] == 'E')) { FLAG_MIE = value & 0x1; }
  else if ((pos[0] == 'q' || pos[0] == 'Q')) { FLAG_Q = value & 0x1; }
  else if (pos[0] == 'p' || pos[0] == 'P') { REG_P = value & 0xF; }
  else if (pos[0] == 'x' || pos[0] == 'X') { REG_X = value & 0xF; }
  else if (pos[0] == 't' || pos[0] == 'T') { REG_T = value & 0xFF; }
  else if (pos[0] == 'i' || pos[0] == 'I') { REG_I = value & 0xF; }
  else if (pos[0] == 'n' || pos[0] == 'N') { REG_N = value & 0xF; }

  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '0')) { REG(10) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '1')) { REG(11) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '2')) { REG(12) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '3')) { REG(13) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '4')) { REG(14) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '5')) { REG(15) = value & 0xFFFF; }

  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '0')) { REG(0) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1')) { REG(1) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '2')) { REG(2) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '3')) { REG(3) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '4')) { REG(4) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '5')) { REG(5) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '6')) { REG(6) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '7')) { REG(7) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '8')) { REG(8) = value & 0xFFFF; }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '9')) { REG(9) = value & 0xFFFF; }

  else { return -1; }

  return 0;
}

uint32_t simulate_get_reg_1802(struct _simulate *simulate, char *reg_string)
{
  struct _simulate_1802 *simulate_1802 = (struct _simulate_1802 *)simulate->context;

  while (*reg_string==' ') { reg_string++; }

  char *pos = reg_string;

  // d, p, x, t, i, n, r0, r1, r2..., df, ie, q

  if (pos[0] == 'd' || pos[0] == 'D') { return REG_D; }
  else if (pos[0] == 'p' || pos[0] == 'P') { return REG_P; }
  else if (pos[0] == 'x' || pos[0] == 'X') { return REG_X; }
  else if (pos[0] == 't' || pos[0] == 'T') { return REG_T; }
  else if (pos[0] == 'i' || pos[0] == 'I') { return REG_I; }
  else if (pos[0] == 'n' || pos[0] == 'N') { return REG_N; }

  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '0')) { return REG(10); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '1')) { return REG(11); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '2')) { return REG(12); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '3')) { return REG(13); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '4')) { return REG(14); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1') && (pos[2] == '5')) { return REG(15); }

  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '0')) { return REG(0); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '1')) { return REG(1); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '2')) { return REG(2); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '3')) { return REG(3); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '4')) { return REG(4); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '5')) { return REG(5); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '6')) { return REG(6); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '7')) { return REG(7); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '8')) { return REG(8); }
  else if ((pos[0] == 'r' || pos[0] == 'R') && (pos[1] == '9')) { return REG(9); }

  else if ((pos[0] == 'd' || pos[0] == 'D') && (pos[1] == 'f' || pos[1] == 'F')) { return FLAG_DF; }
  else if ((pos[0] == 'i' || pos[0] == 'I') && (pos[1] == 'e' || pos[1] == 'E')) { return FLAG_MIE; }
  else if ((pos[0] == 'q' || pos[0] == 'Q')) { return FLAG_Q; }
  else { return -1; }
}

void simulate_set_pc_1802(struct _simulate *simulate, uint32_t value)
{
  struct _simulate_1802 *simulate_1802 = (struct _simulate_1802 *)simulate->context;
  PC = value;
}

void simulate_reset_1802(struct _simulate *simulate)
{
  struct _simulate_1802 *simulate_1802 = (struct _simulate_1802 *)simulate->context;

  simulate->cycle_count = 0;
  simulate->nested_call_count = 0;

  // d, b, p, x, t, i, n, r0, r1, r2..., df, ie, q
  REG_D = 0;
  REG_B = 0;
  REG_P = 0;
  REG_X = 0;
  REG_T = 0;
  REG_I = 0;
  REG_N = 0;

  int i;
  for (i = 0; i < 16; i++)
  {
    REG(i) = 0;
  }

  REG_CNTR = 0;
  REG_CN = 0;

  FLAG_DF = 0;
  FLAG_MIE = 1;
  FLAG_CIE = 1;
  FLAG_XIE = 1;
  FLAG_CIL = 0;
  FLAG_Q = 0;
  simulate->break_point = -1;
}

