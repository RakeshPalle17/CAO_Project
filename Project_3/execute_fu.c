
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"

static void
set_Source1_value_IntFU(APEX_CPU *cpu)
{
    if (cpu->intFU_frwded_tag == cpu->execute_IntFU.phyrs1)
    {
        cpu->execute_IntFU.phyrs1_value = cpu->intFU_frwded_value;
    }
    else if (cpu->MulFU_frwded_tag == cpu->execute_IntFU.phyrs1)
    {
        cpu->execute_IntFU.phyrs1_value = cpu->MulFU_frwded_value;
    }
    else if (cpu->AFU_frwded_tag == cpu->execute_IntFU.phyrs1)
    {
        cpu->execute_IntFU.phyrs1_value = cpu->AFU_frwded_value;
    }
    else
    {
        cpu->execute_IntFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs1].data_field;
    }
}

static void
set_Source2_value_IntFU(APEX_CPU *cpu)
{
    if (cpu->intFU_frwded_tag == cpu->execute_IntFU.phyrs2)
    {
        cpu->execute_IntFU.phyrs2_value = cpu->intFU_frwded_value;
    }
    else if (cpu->MulFU_frwded_tag == cpu->execute_IntFU.phyrs2)
    {
        cpu->execute_IntFU.phyrs2_value = cpu->MulFU_frwded_value;
    }
    else if (cpu->AFU_frwded_tag == cpu->execute_IntFU.phyrs2)
    {
        cpu->execute_IntFU.phyrs2_value = cpu->AFU_frwded_value;
    }
    else
    {
        cpu->execute_IntFU.phyrs2_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs2].data_field;
    }
}

static void
set_Source1_value_MulFU(APEX_CPU *cpu)
{
    if (cpu->intFU_frwded_tag == cpu->execute_MulFU.phyrs1)
    {
        cpu->execute_MulFU.phyrs1_value = cpu->intFU_frwded_value;
    }
    else if (cpu->MulFU_frwded_tag == cpu->execute_MulFU.phyrs1)
    {
        cpu->execute_MulFU.phyrs1_value = cpu->MulFU_frwded_value;
    }
    else if (cpu->AFU_frwded_tag == cpu->execute_MulFU.phyrs1)
    {
        cpu->execute_MulFU.phyrs1_value = cpu->AFU_frwded_value;
    }
    else
    {
        cpu->execute_MulFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_MulFU.phyrs1].data_field;
    }
}

static void
set_Source2_value_MulFU(APEX_CPU *cpu)
{
    if (cpu->intFU_frwded_tag == cpu->execute_MulFU.phyrs2)
    {
        cpu->execute_MulFU.phyrs2_value = cpu->intFU_frwded_value;
    }
    else if (cpu->MulFU_frwded_tag == cpu->execute_MulFU.phyrs2)
    {
        cpu->execute_MulFU.phyrs2_value = cpu->MulFU_frwded_value;
    }
    else if (cpu->AFU_frwded_tag == cpu->execute_MulFU.phyrs2)
    {
        cpu->execute_MulFU.phyrs2_value = cpu->AFU_frwded_value;
    }
    else
    {
        cpu->execute_MulFU.phyrs2_value = cpu->physicalRegFile[cpu->execute_MulFU.phyrs2].data_field;
    }
}



static void
set_Source1_value_AFU(APEX_CPU *cpu)
{
    if (cpu->intFU_frwded_tag == cpu->execute_AFU.phyrs1)
    {
        cpu->execute_AFU.phyrs1_value = cpu->intFU_frwded_value;
    }
    else if (cpu->MulFU_frwded_tag == cpu->execute_AFU.phyrs1)
    {
        cpu->execute_AFU.phyrs1_value = cpu->MulFU_frwded_value;
    }
    else if (cpu->AFU_frwded_tag == cpu->execute_AFU.phyrs1)
    {
        cpu->execute_AFU.phyrs1_value = cpu->AFU_frwded_value;
    }
    else
    {
        cpu->execute_AFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_AFU.phyrs1].data_field;
    }
}

static void
set_Source2_value_AFU(APEX_CPU *cpu)
{
    if (cpu->intFU_frwded_tag == cpu->execute_AFU.phyrs2)
    {
        cpu->execute_AFU.phyrs2_value = cpu->intFU_frwded_value;
    }
    else if (cpu->MulFU_frwded_tag == cpu->execute_AFU.phyrs2)
    {
        cpu->execute_AFU.phyrs2_value = cpu->MulFU_frwded_value;
    }
    else if (cpu->AFU_frwded_tag == cpu->execute_AFU.phyrs2)
    {
        cpu->execute_AFU.phyrs2_value = cpu->AFU_frwded_value;
    }
    else
    {
        cpu->execute_AFU.phyrs2_value = cpu->physicalRegFile[cpu->execute_AFU.phyrs2].data_field;
    }
}

