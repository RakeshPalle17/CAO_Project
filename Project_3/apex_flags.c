#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
initializeCCRegistersFile(APEX_CPU *cpu)
{
    for (int i = 0; i < CC_REG_FILE_SIZE; ++i)
    {
        cpu->ccRegFile[i].valid_bit = INVALID;
    }
}

/* get Free physical register from head of free list */
static int
headof_free_CCRegisters(APEX_CPU *cpu)
{
    if (cpu->freeCCFlagsRegList[0] != -1)
    {
        int head = cpu->freeCCFlagsRegList[0];
        for (int i = 0; i < CC_REG_FILE_SIZE - 1; ++i)
        {
            cpu->freeCCFlagsRegList[i] = cpu->freeCCFlagsRegList[i + 1];
        }
        cpu->freeCCFlagsRegList[CC_REG_FILE_SIZE - 1] = -1;

        return head;
    }
    else
    {
        return -1;
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
set_flag_values_mul(APEX_CPU *cpu)
{
    if (cpu->MulFU_frwded_value == 0)
    {
        cpu->MulFu_frwded_ccTag = cpu->execute_MulFU.flag;
        cpu->MUlFu_frwded_ccValue = ZERO_FLAG;
    }
    else
    {
        cpu->MulFu_frwded_ccTag = cpu->execute_MulFU.flag;
        cpu->MUlFu_frwded_ccValue = POSITIVE_FLAG;
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

static void
update_CCphysical_register_file(APEX_CPU *cpu)
{
    if (cpu->intFu_frwded_ccTag != -1)
    {
        if (cpu->intFu_frwded_ccValue)
        {
            cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag.positive = cpu->intFu_frwded_ccValue;
            cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag.zero = ADD_ZERO;
            cpu->ccRegFile[cpu->intFu_frwded_ccTag].valid_bit = VALID;
            cpu->CCHead = cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag;
            cpu->intFu_frwded_ccTag = -1;
        }
        else
        {
            cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag.positive = cpu->intFu_frwded_ccValue;
            cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag.zero = VALID;
            cpu->ccRegFile[cpu->intFu_frwded_ccTag].valid_bit = VALID;
            cpu->CCHead = cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag;
            cpu->intFu_frwded_ccTag = -1;
        }
    }

    if (cpu->MulFu_frwded_ccTag != -1)
    {
        if (cpu->MUlFu_frwded_ccValue)
        {
            cpu->ccRegFile[cpu->MulFu_frwded_ccTag].flag.positive = cpu->MUlFu_frwded_ccValue;
            cpu->ccRegFile[cpu->MulFu_frwded_ccTag].flag.zero = ADD_ZERO;
            cpu->ccRegFile[cpu->MulFu_frwded_ccTag].valid_bit = VALID;
            cpu->CCHead = cpu->ccRegFile[cpu->MulFu_frwded_ccTag].flag;
            cpu->MulFu_frwded_ccTag = -1;
        }
        else
        {
            cpu->ccRegFile[cpu->MulFu_frwded_ccTag].flag.positive = cpu->MUlFu_frwded_ccValue;
            cpu->ccRegFile[cpu->MulFu_frwded_ccTag].flag.zero = VALID;
            cpu->ccRegFile[cpu->MulFu_frwded_ccTag].valid_bit = VALID;
            cpu->CCHead = cpu->ccRegFile[cpu->MulFu_frwded_ccTag].flag;
            cpu->MulFu_frwded_ccTag = -1;
        }
    }
}

static void
addBackToFreeCCList(APEX_CPU *cpu, int CCRegister)
{
    for (int i = 0; i < CC_REG_FILE_SIZE; ++i)
    {
        if (cpu->freeCCFlagsRegList[i] == -1)
        {
            cpu->freeCCFlagsRegList[i] = CCRegister;
            break;
        }
    }
}