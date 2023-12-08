#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
initialize_default_btb(APEX_CPU *cpu)
{
    cpu->btb_full = TRUE;
    for (int i = 0; i < BTB_SIZE; i++)
    {
        cpu->BTBEntry[i].recent_outcomes[0] = TAKEN;
        cpu->BTBEntry[i].recent_outcomes[1] = TAKEN;

        cpu->BTBEntry[i].branch_pc = ADD_ZERO;
        cpu->BTBEntry[i].target_pc = ADD_ZERO;
    }
}

static void
btb_is_full(APEX_CPU *cpu)
{
    if (!cpu->rename2_dispatch.btb_hit)
    {
        cpu->BTBEntry[cpu->counter].branch_pc = cpu->rename2_dispatch.pc;
        cpu->BTBEntry[cpu->counter].target_pc = ADD_ZERO;
        cpu->BTBEntry[cpu->counter].recent_outcomes[0] = NOT_TAKEN;
        cpu->BTBEntry[cpu->counter].recent_outcomes[1] = NOT_TAKEN;
        cpu->counter++;
    }
    if (cpu->counter == BTB_SIZE)
    {
        cpu->counter = ADD_ZERO;
    }
}

static void
btb_is_full_bnz_bp(APEX_CPU *cpu)
{
    if (!cpu->rename2_dispatch.btb_hit)
    {
        cpu->BTBEntry[cpu->counter].branch_pc = cpu->rename2_dispatch.pc;
        cpu->BTBEntry[cpu->counter].target_pc = ADD_ZERO;
        cpu->BTBEntry[cpu->counter].recent_outcomes[0] = TAKEN;
        cpu->BTBEntry[cpu->counter].recent_outcomes[1] = TAKEN;
        cpu->counter++;
    }
    if (cpu->counter == BTB_SIZE)
    {
        cpu->counter = ADD_ZERO;
    }
}

static void
establish_btb_entry(APEX_CPU *cpu)
{
    cpu->btb_full = TRUE;
    for (int i = 0; i < BTB_SIZE; i++)
    {
        if (cpu->BTBEntry[i].branch_pc == 0 || cpu->BTBEntry[i].branch_pc == cpu->rename2_dispatch.pc)
        {
            if (!cpu->rename2_dispatch.btb_hit)
            {
                cpu->BTBEntry[i].branch_pc = cpu->rename2_dispatch.pc;
                cpu->BTBEntry[i].recent_outcomes[0] = NOT_TAKEN;
                cpu->BTBEntry[i].recent_outcomes[1] = NOT_TAKEN;
            }

            cpu->btb_full = FALSE;
            break;
        }
    }

    if (cpu->btb_full)
    {
        btb_is_full(cpu);
    }
}

static void
establish_btb_entry_bnz_bp(APEX_CPU *cpu)
{
    cpu->btb_full = TRUE;
    for (int i = 0; i < BTB_SIZE; i++)
    {
        if (cpu->BTBEntry[i].branch_pc == 0 || cpu->BTBEntry[i].branch_pc == cpu->rename2_dispatch.pc)
        {
            if (!cpu->rename2_dispatch.btb_hit)
            {
                cpu->BTBEntry[i].branch_pc = cpu->rename2_dispatch.pc;
                cpu->BTBEntry[i].recent_outcomes[0] = TAKEN;
                cpu->BTBEntry[i].recent_outcomes[1] = TAKEN;
            }
            cpu->btb_full = FALSE;
            break;
        }
    }

    if (cpu->btb_full)
    {
        btb_is_full_bnz_bp(cpu);
    }
}


static void
set_flag_values(APEX_CPU *cpu)
{
    if (cpu->intFU_frwded_value == 0)
    {
        cpu->intFu_frwded_ccTag = cpu->execute_IntFU.flag;
        cpu->intFu_frwded_ccValue = ZERO_FLAG;
    }
    else
    {
        cpu->intFu_frwded_ccTag = cpu->execute_IntFU.flag;
        cpu->intFu_frwded_ccValue = POSITIVE_FLAG;
    }
}

static void
set_flag_values_cmpAndcml(APEX_CPU *cpu)
{
    if (cpu->compare_value == 0)
    {
        cpu->intFu_frwded_ccTag = cpu->execute_IntFU.flag;
        cpu->intFu_frwded_ccValue = ZERO_FLAG;
    }
    else
    {
        cpu->intFu_frwded_ccTag = cpu->execute_IntFU.flag;
        cpu->intFu_frwded_ccValue = POSITIVE_FLAG;
    }
}