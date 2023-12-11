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
        if (cpu->BTBEntry[i].branch_pc == 0 || cpu->BTBEntry[i].branch_pc == cpu->decode_rename1.pc)
        {
            if (!cpu->decode_rename1.btb_hit)
            {
                cpu->BTBEntry[i].branch_pc = cpu->decode_rename1.pc;
                cpu->BTBEntry[i].recent_outcomes[0] = NOT_TAKEN;
                cpu->BTBEntry[i].recent_outcomes[1] = NOT_TAKEN;
                cpu->decode_rename1.stall = TRUE;
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
        if (cpu->BTBEntry[i].branch_pc == 0 || cpu->BTBEntry[i].branch_pc == cpu->decode_rename1.pc)
        {
            if (!cpu->decode_rename1.btb_hit)
            {
                cpu->BTBEntry[i].branch_pc = cpu->decode_rename1.pc;
                cpu->BTBEntry[i].recent_outcomes[0] = TAKEN;
                cpu->BTBEntry[i].recent_outcomes[1] = TAKEN;
                cpu->decode_rename1.stall = TRUE;
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
branch_taken_flow(APEX_CPU *cpu)
{
    for (int i = 0; i < BTB_SIZE; i++)
    {
        if (cpu->execute_BFU.pc == cpu->BTBEntry[i].branch_pc)
        {
            cpu->BTBEntry[i].target_pc = cpu->execute_BFU.target_address;

            if (!cpu->execute_BFU.taken)
            {
                cpu->BFU_frwded_pc = cpu->execute_BFU.target_address;
                cpu->miss_branch_tag = cpu->execute_BFU.pc;
                cpu->fetch.has_insn = TRUE;
            }

            if (!cpu->BTBEntry[i].recent_outcomes[0] && !cpu->BTBEntry[i].recent_outcomes[1])
            {
                cpu->BTBEntry[i].recent_outcomes[1] = TAKEN;
            }
            else if (!cpu->BTBEntry[i].recent_outcomes[0] && cpu->BTBEntry[i].recent_outcomes[1])
            {
                cpu->BTBEntry[i].recent_outcomes[0] = TAKEN;
                cpu->BTBEntry[i].recent_outcomes[1] = NOT_TAKEN;
            }
            else if (cpu->BTBEntry[i].recent_outcomes[0] && !cpu->BTBEntry[i].recent_outcomes[1])
            {
                cpu->BTBEntry[i].recent_outcomes[0] = TAKEN;
                cpu->BTBEntry[i].recent_outcomes[1] = TAKEN;
            }
            break;
        }
    }
}

static void
branch_not_taken_flow(APEX_CPU *cpu)
{
    for (int i = 0; i < BTB_SIZE; i++)
    {
        if (cpu->execute_BFU.pc == cpu->BTBEntry[i].branch_pc)
        {
            cpu->BTBEntry[i].target_pc = cpu->execute_BFU.target_address;

            if (cpu->execute_BFU.taken)
            {
                cpu->BFU_frwded_pc = cpu->execute_BFU.pc + INCREMENTOR;
                cpu->miss_branch_tag = cpu->execute_BFU.pc;
                cpu->fetch.has_insn = TRUE;
            }

            if (cpu->BTBEntry[i].recent_outcomes[0] && cpu->BTBEntry[i].recent_outcomes[1])
            {
                cpu->BTBEntry[i].recent_outcomes[1] = NOT_TAKEN;
            }
            else if (cpu->BTBEntry[i].recent_outcomes[0] && !cpu->BTBEntry[i].recent_outcomes[1])
            {
                cpu->BTBEntry[i].recent_outcomes[0] = NOT_TAKEN;
                cpu->BTBEntry[i].recent_outcomes[1] = TAKEN;
            }
            else if (!cpu->BTBEntry[i].recent_outcomes[0] && cpu->BTBEntry[i].recent_outcomes[1])
            {
                cpu->BTBEntry[i].recent_outcomes[0] = NOT_TAKEN;
                cpu->BTBEntry[i].recent_outcomes[1] = NOT_TAKEN;
            }

            break;
        }
    }
}