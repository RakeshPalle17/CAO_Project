
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

    /*MAU forwarding bus src1*/
    if (!cpu->decode_rename1.phyrs1_valid && cpu->MAU_frwded_tag == cpu->decode_rename1.phyrs1)
    {
        cpu->decode_rename1.phyrs1_valid = VALID;
        cpu->decode_rename1.phyrs1_value = cpu->MAU_frwded_value;
    }

    /*BFU forwarding bus src1*/
    if (!cpu->decode_rename1.phyrs1_valid && cpu->BFU_frwded_tag == cpu->decode_rename1.phyrs1)
    {
        cpu->decode_rename1.phyrs1_valid = VALID;
        cpu->decode_rename1.phyrs1_value = cpu->BFU_frwded_value;
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

    /*MAU forwarding bus src2*/
    if (!cpu->decode_rename1.phyrs2_valid && cpu->MAU_frwded_tag == cpu->decode_rename1.phyrs2)
    {
        cpu->decode_rename1.phyrs2_valid = VALID;
        cpu->decode_rename1.phyrs2_value = cpu->MAU_frwded_value;
    }

    
    /*BFU forwarding bus src2*/
    if (!cpu->decode_rename1.phyrs2_valid && cpu->BFU_frwded_tag == cpu->decode_rename1.phyrs2)
    {
        cpu->decode_rename1.phyrs2_valid = VALID;
        cpu->decode_rename1.phyrs2_value = cpu->BFU_frwded_value;
    }
}

static void
set_dispatch_physical_source1(APEX_CPU *cpu)
{
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

    /*MAU forwarding bus src1*/
    if (!cpu->rename2_dispatch.phyrs1_valid && cpu->MAU_frwded_tag == cpu->rename2_dispatch.phyrs1)
    {
        cpu->rename2_dispatch.phyrs1_valid = VALID;
        cpu->rename2_dispatch.phyrs1_value = cpu->MAU_frwded_value;
    }

    /*BFU forwarding bus src1*/
    if (!cpu->rename2_dispatch.phyrs1_valid && cpu->BFU_frwded_tag == cpu->rename2_dispatch.phyrs1)
    {
        cpu->rename2_dispatch.phyrs1_valid = VALID;
        cpu->rename2_dispatch.phyrs1_value = cpu->BFU_frwded_value;
    }

    /* physical registers src1*/
    if (!cpu->rename2_dispatch.phyrs1_valid && cpu->physicalRegFile[cpu->rename2_dispatch.phyrs1].valid_bit)
    {
        cpu->rename2_dispatch.phyrs1_value = cpu->physicalRegFile[cpu->rename2_dispatch.phyrs1].data_field;
        cpu->rename2_dispatch.phyrs1_valid = VALID;
    }
}

static void
set_dispatch_physical_source2(APEX_CPU *cpu)
{
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

    /*MAU forwarding bus src2*/
    if (!cpu->rename2_dispatch.phyrs2_valid && cpu->MAU_frwded_tag == cpu->rename2_dispatch.phyrs2)
    {
        cpu->rename2_dispatch.phyrs2_valid = VALID;
        cpu->rename2_dispatch.phyrs2_value = cpu->MAU_frwded_value;
    }

    /*BFU forwarding bus src2*/
    if (!cpu->rename2_dispatch.phyrs2_valid && cpu->BFU_frwded_tag == cpu->rename2_dispatch.phyrs2)
    {
        cpu->rename2_dispatch.phyrs2_valid = VALID;
        cpu->rename2_dispatch.phyrs2_value = cpu->BFU_frwded_value;
    }

    /* physical registers src2*/
    if (!cpu->rename2_dispatch.phyrs2_valid && cpu->physicalRegFile[cpu->rename2_dispatch.phyrs2].valid_bit)
    {
        cpu->rename2_dispatch.phyrs2_value = cpu->physicalRegFile[cpu->rename2_dispatch.phyrs2].data_field;
        cpu->rename2_dispatch.phyrs2_valid = VALID;
    }
}

static void
set_issueQueue_src1_physical(APEX_CPU *cpu, int i)
{

    if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
    {
        cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs1_value = cpu->intFU_frwded_value;
    }

    if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
    {
        cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs1_value = cpu->MulFU_frwded_value;
    }

    if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
    {
        cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs1_value = cpu->AFU_frwded_value;
    }

    if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->MAU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
    {
        cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs1_value = cpu->MAU_frwded_value;
    }

    if (!cpu->issueQueue[i].instr.phyrs1_valid && cpu->BFU_frwded_tag == cpu->issueQueue[i].instr.phyrs1)
    {
        cpu->issueQueue[i].instr.phyrs1_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs1_value = cpu->BFU_frwded_value;
    }
}

static void
set_issueQueue_src2_physical(APEX_CPU *cpu, int i)
{

    if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->intFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
    {
        cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs2_value = cpu->intFU_frwded_value;
    }

    if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->MulFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
    {
        cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs2_value = cpu->MulFU_frwded_value;
    }

    if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->AFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
    {
        cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs2_value = cpu->AFU_frwded_value;
    }

    if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->MAU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
    {
        cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs2_value = cpu->MAU_frwded_value;
    }
    
    if (!cpu->issueQueue[i].instr.phyrs2_valid && cpu->BFU_frwded_tag == cpu->issueQueue[i].instr.phyrs2)
    {
        cpu->issueQueue[i].instr.phyrs2_valid = TRUE;
        cpu->issueQueue[i].instr.phyrs2_value = cpu->BFU_frwded_value;
    }
}

static void
update_physical_register_file(APEX_CPU *cpu)
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

    if (cpu->MAU_frwded_tag != -1)
    {
        cpu->physicalRegFile[cpu->MAU_frwded_tag].data_field = cpu->MAU_frwded_value;
        cpu->physicalRegFile[cpu->MAU_frwded_tag].valid_bit = VALID;
        cpu->MAU_frwded_tag = -1;
    }

   if (cpu->BFU_frwded_tag != -1)
    {
        cpu->physicalRegFile[cpu->BFU_frwded_tag].data_field = cpu->BFU_frwded_value;
        cpu->physicalRegFile[cpu->BFU_frwded_tag].valid_bit = VALID;
        cpu->BFU_frwded_tag = -1;
    }
}



static void
update_CCphysical_register_file(APEX_CPU *cpu)
{
    if (cpu->intFu_frwded_ccTag != -1)
    {
        if(cpu->intFu_frwded_ccValue)
        {
        cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag.positive = cpu->intFu_frwded_ccValue;
        cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag.zero = ADD_ZERO;
        cpu->ccRegFile[cpu->intFu_frwded_ccTag].valid_bit = VALID;
        cpu->intFu_frwded_ccTag = -1;
        }
        else
        {
        cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag.positive = cpu->intFu_frwded_ccValue;
        cpu->ccRegFile[cpu->intFu_frwded_ccTag].flag.zero = VALID;
        cpu->intFu_frwded_ccTag = -1;
        }
    }
}