
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"


static void
set_decode_physical_source1(APEX_CPU *cpu)
{
    /*int forwarding bus src1*/
    if (!cpu->decode_rename1.phyrs1_valid && cpu->intFU_frwded_tag == cpu->decode_rename1.phyrs1)
    {
        cpu->decode_rename1.phyrs1_valid = VALID;
        cpu->decode_rename1.phyrs1_value = cpu->intFU_frwded_value;
    }

    /*Mul forwarding bus src1*/
    if (!cpu->decode_rename1.phyrs1_valid && cpu->MulFU_frwded_tag == cpu->decode_rename1.phyrs1)
    {
        cpu->decode_rename1.phyrs1_valid = VALID;
        cpu->decode_rename1.phyrs1_value = cpu->MulFU_frwded_value;
    }

    /*AFU forwarding bus src1*/
    if (!cpu->decode_rename1.phyrs1_valid && cpu->AFU_frwded_tag == cpu->decode_rename1.phyrs1)
    {
        cpu->decode_rename1.phyrs1_valid = VALID;
        cpu->decode_rename1.phyrs1_value = cpu->AFU_frwded_value;
    }
}

static void
set_decode_physical_source2(APEX_CPU *cpu)
{

    /*int forwarding bus src2*/
    if (!cpu->decode_rename1.phyrs2_valid && cpu->intFU_frwded_tag == cpu->decode_rename1.phyrs2)
    {
        cpu->decode_rename1.phyrs2_valid = VALID;
        cpu->decode_rename1.phyrs2_value = cpu->intFU_frwded_value;
    }

    /*Mul forwarding bus src2*/
    if (!cpu->decode_rename1.phyrs2_valid && cpu->MulFU_frwded_tag == cpu->decode_rename1.phyrs2)
    {
        cpu->decode_rename1.phyrs2_valid = VALID;
        cpu->decode_rename1.phyrs2_value = cpu->MulFU_frwded_value;
    }

    /*AFU forwarding bus src2*/
    if (!cpu->decode_rename1.phyrs2_valid && cpu->AFU_frwded_tag == cpu->decode_rename1.phyrs2)
    {
        cpu->decode_rename1.phyrs2_valid = VALID;
        cpu->decode_rename1.phyrs2_value = cpu->AFU_frwded_value;
    }
}

static void
set_dispatch_physical_source1(APEX_CPU *cpu)
{
    /* physical registers src1*/
    if (cpu->physicalRegFile[cpu->rename2_dispatch.phyrs1].valid_bit)
    {
        cpu->rename2_dispatch.phyrs1_value = cpu->physicalRegFile[cpu->rename2_dispatch.phyrs1].data_field;
        cpu->rename2_dispatch.phyrs1_valid = VALID;
    }

    /*int forwarding bus src1*/
    if (!cpu->rename2_dispatch.phyrs1_valid && cpu->intFU_frwded_tag == cpu->rename2_dispatch.phyrs1)
    {
        cpu->rename2_dispatch.phyrs1_valid = VALID;
        cpu->rename2_dispatch.phyrs1_value = cpu->intFU_frwded_value;
    }

    /*Mul forwarding bus src1*/
    if (!cpu->rename2_dispatch.phyrs1_valid && cpu->MulFU_frwded_tag == cpu->rename2_dispatch.phyrs1)
    {
        cpu->rename2_dispatch.phyrs1_valid = VALID;
        cpu->rename2_dispatch.phyrs1_value = cpu->MulFU_frwded_value;
    }

    /*AFU forwarding bus src1*/
    if (!cpu->rename2_dispatch.phyrs1_valid && cpu->AFU_frwded_tag == cpu->rename2_dispatch.phyrs1)
    {
        cpu->rename2_dispatch.phyrs1_valid = VALID;
        cpu->rename2_dispatch.phyrs1_value = cpu->AFU_frwded_value;
    }
}

static void
set_dispatch_physical_source2(APEX_CPU *cpu)
{
    /* physical registers src2*/
    if (cpu->physicalRegFile[cpu->rename2_dispatch.phyrs2].valid_bit)
    {
        cpu->rename2_dispatch.phyrs2_value = cpu->physicalRegFile[cpu->rename2_dispatch.phyrs2].data_field;
        cpu->rename2_dispatch.phyrs2_valid = VALID;
    }

    /*int forwarding bus src2*/
    if (!cpu->rename2_dispatch.phyrs2_valid && cpu->intFU_frwded_tag == cpu->rename2_dispatch.phyrs2)
    {
        cpu->rename2_dispatch.phyrs2_valid = VALID;
        cpu->rename2_dispatch.phyrs2_value = cpu->intFU_frwded_value;
    }

    /*Mul forwarding bus src2*/
    if (!cpu->rename2_dispatch.phyrs2_valid && cpu->MulFU_frwded_tag == cpu->rename2_dispatch.phyrs2)
    {
        cpu->rename2_dispatch.phyrs2_valid = VALID;
        cpu->rename2_dispatch.phyrs2_value = cpu->MulFU_frwded_value;
    }

    /*AFU forwarding bus src2*/
    if (!cpu->rename2_dispatch.phyrs2_valid && cpu->AFU_frwded_tag == cpu->rename2_dispatch.phyrs2)
    {
        cpu->rename2_dispatch.phyrs2_valid = VALID;
        cpu->rename2_dispatch.phyrs2_value = cpu->AFU_frwded_value;
    }
}

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


static void
update_PRF(APEX_CPU *cpu)
{
    if (cpu->intFU_frwded_tag != -1)
    {
        cpu->physicalRegFile[cpu->intFU_frwded_tag].data_field = cpu->intFU_frwded_value;
        cpu->physicalRegFile[cpu->intFU_frwded_tag].valid_bit = VALID;
        cpu->intFU_frwded_tag = -1;
    }

    if (cpu->MulFU_frwded_tag != -1)
    {
        cpu->physicalRegFile[cpu->MulFU_frwded_tag].data_field = cpu->MulFU_frwded_value;
        cpu->physicalRegFile[cpu->MulFU_frwded_tag].valid_bit = VALID;
        cpu->MulFU_frwded_tag = -1;
    }

    if (cpu->AFU_frwded_tag != -1)
    {
        cpu->physicalRegFile[cpu->AFU_frwded_tag].data_field = cpu->AFU_frwded_value;
        cpu->physicalRegFile[cpu->AFU_frwded_tag].valid_bit = VALID;
        cpu->AFU_frwded_tag = -1;
    }
}
