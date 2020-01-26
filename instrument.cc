//===================================================================================================================
//
//  instrument.cc -- Bochs instrumentation for CenuryOS
//
/////////////////////////////////////////////////////////////////////////
// $Id: instrument.cc 12655 2015-02-19 20:23:08Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2006-2015 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
//  -----------------------------------------------------------------------------------------------------------------
//
//     Date      Tracker  Version  Pgmr  Description
//  -----------  -------  -------  ----  ---------------------------------------------------------------------------
//  2020-Jan-11  Initial  v0.0.1   ADCL  Initial version -- copied from bochs-2.6.11/instrument/example1
//
//===================================================================================================================



#include <assert.h>

#include "bochs.h"
#include "cpu/cpu.h"
#include "disasm/disasm.h"

bxInstrumentation *icpu = NULL;

static disassembler bx_disassembler;

static logfunctions *instrument_log = new logfunctions ();
#define LOG_THIS instrument_log->

void bx_instr_init_env(void) {}
void bx_instr_exit_env(void) {}

void bx_instr_initialize(unsigned cpu)
{
  assert(cpu < BX_SMP_PROCESSORS);

  if (icpu == NULL)
      icpu = new bxInstrumentation[BX_SMP_PROCESSORS];

  icpu[cpu].set_cpu_id(cpu);

  BX_INFO(("Initialize cpu %u", cpu));
}

void bxInstrumentation::bx_instr_reset(unsigned type)
{
  ready = is_branch = 0;
  num_data_accesses = 0;
  active = 0;
}

void bxInstrumentation::bx_print_instruction(void)
{
  char disasm_tbuf[512];	// buffer for instruction disassembly
  char opcode_hex[100] = {0};   // buffer for the instruction hex
  char buf[5];            // buffer for sprintf
  bx_disassembler.disasm(is32, is64, 0, 0, opcode, disasm_tbuf);

  if(opcode_length != 0)
  {
    unsigned n;

    BX_INFO(("----------------------------------------------------------"));
    BX_INFO(("CPU %d: %s", cpu_id, disasm_tbuf));

    for(n=0;n < opcode_length;n++) {
      sprintf(buf, "%02x", opcode[n]);
      strcat(opcode_hex, buf);
    }

    BX_INFO(("LEN %d\tBYTES: %s", opcode_length, opcode_hex));
    if(is_branch)
    {
      if(is_taken)
        BX_INFO(("\tBRANCH TARGET " FMT_ADDRX " (TAKEN)", target_linear));
      else
        BX_INFO(("\tBRANCH (NOT TAKEN)"));
    }

    for(n=0;n < num_data_accesses;n++)
    {
      BX_INFO(("MEM ACCESS[%u]: 0x" FMT_ADDRX " (linear) 0x" FMT_PHY_ADDRX " (physical) %s SIZE: %d", n,
                    data_access[n].laddr,
                    data_access[n].paddr,
                    data_access[n].rw == BX_READ ? "RD":"WR",
                    data_access[n].size));
    }
    BX_INFO((""));
  }
}

void bxInstrumentation::bx_instr_before_execution(bxInstruction_c *i)
{
  //
  // -- check if we have `xchg edx,edx` and if so we will toggle the active flag
  //    ------------------------------------------------------------------------
  if (i->get_opcode_bytes()[0] == 0x87 && i->get_opcode_bytes()[1] == 0xd2) {
    toggle_active();
    if (is_active()) {
      BX_INFO(("Instrumentation is now ON"));
    } else {
      BX_INFO(("Instrumentation is now off"));
    }
  }

  if (!active) return;

  if (ready) bx_print_instruction();

  // prepare instruction_t structure for new instruction
  ready = 1;
  num_data_accesses = 0;
  is_branch = 0;

  is32 = BX_CPU(cpu_id)->sregs[BX_SEG_REG_CS].cache.u.segment.d_b;
  is64 = BX_CPU(cpu_id)->long64_mode();
  opcode_length = i->ilen();
  memcpy(opcode, i->get_opcode_bytes(), opcode_length);
}

void bxInstrumentation::bx_instr_after_execution(bxInstruction_c *i)
{
  if (!active) return;

  if (ready) {
    bx_print_instruction();
    ready = 0;
  }
}

void bxInstrumentation::branch_taken(bx_address new_eip)
{
  if (!active || !ready) return;

  is_branch = 1;
  is_taken = 1;

  // find linear address
  target_linear = BX_CPU(cpu_id)->get_laddr(BX_SEG_REG_CS, new_eip);
}

void bxInstrumentation::bx_instr_cnear_branch_taken(bx_address branch_eip, bx_address new_eip)
{
  branch_taken(new_eip);
}

void bxInstrumentation::bx_instr_cnear_branch_not_taken(bx_address branch_eip)
{
  if (!active || !ready) return;

  is_branch = 1;
  is_taken = 0;
}

void bxInstrumentation::bx_instr_ucnear_branch(unsigned what, bx_address branch_eip, bx_address new_eip)
{
  branch_taken(new_eip);
}

void bxInstrumentation::bx_instr_far_branch(unsigned what, Bit16u prev_cs, bx_address prev_eip, Bit16u new_cs, bx_address new_eip)
{
  branch_taken(new_eip);
}

void bxInstrumentation::bx_instr_interrupt(unsigned vector)
{
  if(active)
  {
    BX_INFO(("CPU %u: interrupt %02xh", cpu_id, vector));
  }
}

void bxInstrumentation::bx_instr_exception(unsigned vector, unsigned error_code)
{
  if(active)
  {
    BX_INFO(("CPU %u: exception %02xh error_code=%x", cpu_id, vector, error_code));
    BX_CPU_C *cpu;
#if BX_SUPPORT_SMP
    cpu = bx_cpu_array[cpu_id];
#else
    cpu = &bx_cpu;
#endif

    cpu->debug(0);
  }
}

void bxInstrumentation::bx_instr_hwinterrupt(unsigned vector, Bit16u cs, bx_address eip)
{
  if(active)
  {
    BX_INFO(("CPU %u: hardware interrupt %02xh", cpu_id, vector));
  }
}

void bxInstrumentation::bx_instr_lin_access(bx_address lin, bx_phy_address phy, unsigned len, unsigned memtype, unsigned rw)
{
  if(!active || !ready) return;

  if (num_data_accesses < MAX_DATA_ACCESSES) {
    data_access[num_data_accesses].laddr = lin;
    data_access[num_data_accesses].paddr = phy;
    data_access[num_data_accesses].rw    = rw;
    data_access[num_data_accesses].size  = len;
    data_access[num_data_accesses].memtype = memtype;
    num_data_accesses++;
  }
}
