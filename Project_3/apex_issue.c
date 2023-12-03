#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"


static void
issueWakeupInstructionFromIQ(APEX_CPU *cpu)
{
    int min_counter = 999;
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        switch (cpu->issueQueue[i].instr.opcode)
        {

        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_XOR:
        case OPCODE_CMP:
        {
            /*physical source register 1*/
            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            /*physical source register 2*/

            if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
            {
                cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
            {
                cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
            {
                cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
            }

            if (cpu->issueQueue[i].valid_bit && cpu->issueQueue[i].instr.phyrs1_valid && cpu->issueQueue[i].instr.phyrs2_valid)
            {
                if (min_counter > cpu->issueQueue[i].dispatch_time)
                {
                    min_counter = cpu->issueQueue[i].dispatch_time;
                }
            }
            break;
        }

        case OPCODE_ADDL:
        case OPCODE_SUBL:
        case OPCODE_CML:
        {

            /*physical source register 1*/
            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (cpu->issueQueue[i].valid_bit && cpu->issueQueue[i].instr.phyrs1_valid)
            {
                if (min_counter > cpu->issueQueue[i].dispatch_time)
                {
                    min_counter = cpu->issueQueue[i].dispatch_time;
                }
            }
            break;
        }

        case OPCODE_MOVC:
        case OPCODE_NOP:
        case OPCODE_HALT:
        {
            if (min_counter > cpu->issueQueue[i].dispatch_time)
            {
                min_counter = cpu->issueQueue[i].dispatch_time;
            }
            break;
        }
        }
    }

    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        if (cpu->issueQueue[i].valid_bit && min_counter == cpu->issueQueue[i].dispatch_time)
        {
            cpu->execute_IntFU = cpu->issueQueue[i].instr;
            cpu->issueQueue[i].valid_bit = INVALID;
            break;
        }
    }
}

static void
issueWakeupMulInstructionFromIQ(APEX_CPU *cpu)
{
    int min_counter = 999;
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        switch (cpu->issueQueue[i].instr.opcode)
        {
        case OPCODE_MUL:
        {
            /*physical source register 1*/
            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            /*physical source register 2*/

            if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
            {
                cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
            {
                cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
            {
                cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
            }

            if (cpu->issueQueue[i].valid_bit && cpu->issueQueue[i].instr.phyrs1_valid && cpu->issueQueue[i].instr.phyrs2_valid)
            {
                if (min_counter > cpu->issueQueue[i].dispatch_time)
                {
                    min_counter = cpu->issueQueue[i].dispatch_time;
                }
            }
            break;
        }
        }
    }

    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        if (cpu->issueQueue[i].valid_bit && min_counter == cpu->issueQueue[i].dispatch_time && cpu->mulcycle_counter == 0)
        {
            cpu->execute_MulFU = cpu->issueQueue[i].instr;
            cpu->issueQueue[i].valid_bit = INVALID;
            cpu->mulcycle_counter = 3;
            break;
        }
    }
}

static void
issueWakeupAFUInstructionFromIQ(APEX_CPU *cpu)
{
    int min_counter = 999;
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        switch (cpu->issueQueue[i].instr.opcode)
        {
        case OPCODE_LOAD:
        case OPCODE_LOADP:
        {
            /*physical source register 1*/
            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (cpu->issueQueue[i].valid_bit && cpu->issueQueue[i].instr.phyrs1_valid)
            {
                if (min_counter > cpu->issueQueue[i].dispatch_time)
                {
                    min_counter = cpu->issueQueue[i].dispatch_time;
                }
            }
            break;
        }
        case OPCODE_STORE:
        case OPCODE_STOREP:
        {
            /*physical source register 1*/
            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
            {
                cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
            }

            /*physical source register 2*/
            if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
            {
                cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
            {
                cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
            }

            if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
            {
                cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
            }

            if (cpu->issueQueue[i].valid_bit && (cpu->issueQueue[i].instr.phyrs1_valid && cpu->issueQueue[i].instr.phyrs2_valid))
            {
                if (min_counter > cpu->issueQueue[i].dispatch_time)
                {
                    min_counter = cpu->issueQueue[i].dispatch_time;
                }
            }
            break;
        }
        }
    }

    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        if (cpu->issueQueue[i].valid_bit && min_counter == cpu->issueQueue[i].dispatch_time)
        {
            cpu->execute_AFU = cpu->issueQueue[i].instr;
            cpu->issueQueue[i].valid_bit = INVALID;
            break;
        }
    }
}

static void
updateLSQentrywithForwardedBus(APEX_CPU *cpu)
{
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {

    }

}