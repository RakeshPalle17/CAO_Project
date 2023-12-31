
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"

static void
print_instruction_physical(const CPU_Stage *stage)
{
    switch (stage->opcode)
    {
    case OPCODE_ADD:
    case OPCODE_SUB:
    case OPCODE_MUL:
    case OPCODE_DIV:
    case OPCODE_AND:
    case OPCODE_OR:
    case OPCODE_XOR:
    {
        printf("%s,P%d,P%d,P%d ", stage->opcode_str, stage->phyrd, stage->phyrs1,
               stage->phyrs2);
        break;
    }

    case OPCODE_MOVC:
    {
        printf("%s,P%d,#%d ", stage->opcode_str, stage->phyrd, stage->imm);
        break;
    }

    case OPCODE_LOAD:
    {
        printf("%s,P%d,P%d,#%d ", stage->opcode_str, stage->phyrd, stage->phyrs1,
               stage->imm);
        break;
    }
    case OPCODE_LOADP:
    {
        printf("%s,P%d,P%d,#%d ", stage->opcode_str, stage->phyrd, stage->phyrs3,
               stage->imm);
        break;
    }

    case OPCODE_STORE:
    {
        printf("%s,P%d,P%d,#%d ", stage->opcode_str, stage->phyrs1, stage->phyrs2,
               stage->imm);
        break;
    }
    case OPCODE_STOREP:
    {
        printf("%s,P%d,P%d,#%d ", stage->opcode_str, stage->phyrs1, stage->phyrs3,
               stage->imm);
        break;
    }

    case OPCODE_BZ:
    case OPCODE_BNZ:
    case OPCODE_BP:
    case OPCODE_BNP:
    {
        printf("%s,#%d ", stage->opcode_str, stage->imm);
        break;
    }

    case OPCODE_ADDL:
    case OPCODE_SUBL:
    case OPCODE_JALR:
    {
        printf("%s,P%d,P%d,#%d ", stage->opcode_str, stage->phyrd, stage->phyrs1,
               stage->imm);
        break;
    }

    case OPCODE_CML:
    case OPCODE_JUMP:
    {
        printf("%s,P%d,#%d ", stage->opcode_str, stage->phyrs1,
               stage->imm);
        break;
    }

    case OPCODE_CMP:
    {
        printf("%s,P%d,P%d ", stage->opcode_str, stage->phyrs1,
               stage->phyrs2);
        break;
    }

    case OPCODE_NOP:
    {
        printf("%s", stage->opcode_str);
        break;
    }

    case OPCODE_HALT:
    {
        printf("%s", stage->opcode_str);
        break;
    }
    }
}

static void
print_instruction_arch(const CPU_Stage *stage)
{
    switch (stage->opcode)
    {
    case OPCODE_ADD:
    case OPCODE_SUB:
    case OPCODE_MUL:
    case OPCODE_DIV:
    case OPCODE_AND:
    case OPCODE_OR:
    case OPCODE_XOR:
    {
        printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
               stage->rs2);
        break;
    }

    case OPCODE_MOVC:
    {
        printf("%s,R%d,#%d ", stage->opcode_str, stage->rd, stage->imm);
        break;
    }

    case OPCODE_LOAD:
    case OPCODE_LOADP:
    {
        printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
               stage->imm);
        break;
    }

    case OPCODE_STORE:
    case OPCODE_STOREP:
    {
        printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
               stage->imm);
        break;
    }

    case OPCODE_BZ:
    case OPCODE_BNZ:
    case OPCODE_BP:
    case OPCODE_BNP:
    {
        printf("%s,#%d ", stage->opcode_str, stage->imm);
        break;
    }

    case OPCODE_ADDL:
    case OPCODE_SUBL:
    case OPCODE_JALR:
    {
        printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
               stage->imm);
        break;
    }

    case OPCODE_CML:
    case OPCODE_JUMP:
    {
        printf("%s,R%d,#%d ", stage->opcode_str, stage->rs1,
               stage->imm);
        break;
    }

    case OPCODE_CMP:
    {
        printf("%s,R%d,R%d ", stage->opcode_str, stage->rs1,
               stage->rs2);
        break;
    }

    case OPCODE_NOP:
    {
        printf("%s", stage->opcode_str);
        break;
    }

    case OPCODE_HALT:
    {
        printf("%s", stage->opcode_str);
        break;
    }
    }
}

/* Debug function which prints the CPU stage content
 *
 * Note: You can edit this function to print in more detail
 */
static void
print_stage_content(const char *name, const CPU_Stage *stage)
{
    printf("%-17s: pc(%d) ", name, stage->pc);
    if (strcmp(name, "Fetch") == 0)
    {
        print_instruction_arch(stage);
    }
    else
    {
        print_instruction_physical(stage);
    }
    printf("\n");
}


static void
print_forwarding_tags(APEX_CPU *cpu)
{
    printf("\n--------------\n");

    if (cpu->intFU_frwded_tag != -1)
    {
        printf("IntFU Forwarding-Tag: P%d ", cpu->intFU_frwded_tag);
    }

    if (cpu->MulFU_frwded_tag != -1)
    {
        printf("MulFU Forwarding-Tag: P%d ", cpu->MulFU_frwded_tag);
    }

    if (cpu->AFU_frwded_tag != -1)
    {
        printf("AFU Forwarding-Tag: P%d ", cpu->AFU_frwded_tag);
    }

    if (cpu->MAU_frwded_tag != -1)
    {
        printf("MAU Forwarding-Tag: P%d ", cpu->MAU_frwded_tag);
    }

    if (cpu->BFU_frwded_tag != -1)
    {
        printf("BFU Forwarding-Tag: P%d ", cpu->BFU_frwded_tag);
    }

    if (cpu->intFu_frwded_ccTag != -1)
    {
        printf("\nCC Forwarding-Tag: cp%d ", cpu->intFu_frwded_ccTag);
    }
}

static void
print_memory_address_values(APEX_CPU *cpu)
{
    printf("\n-----------------\n%s","Memory Addresses: ");
    for (int i = 0; i < DATA_MEMORY_SIZE; i++)
    {
        if (cpu->data_memory[i] != -1)
        {
            printf("MEM[%d] = %d ", i, cpu->data_memory[i]);
        }
    }
    printf("\n-----------------");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_reg_file(const APEX_CPU *cpu)
{
    int i;

    printf("\n----------\n%s\n----------\n", "Registers:");

    for (int i = 0; i <= REG_FILE_SIZE / 2; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i].value);
    }

    printf("\n");

    for (i = (REG_FILE_SIZE / 2) + 1; i < REG_FILE_SIZE - 2; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i].value);
    }

    printf("R%-3d[Z = %-3d, P = %-d] ", 17, cpu->regs[i].flags.zero, cpu->regs[i].flags.positive);
    printf("R%-3d[%-3d] ", 18, cpu->regs[i+1].value);

    printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_physicalRegisters_file(const APEX_CPU *cpu)
{
    printf("\n------------------\n%s\n------------------\n", "Physical Registers:");

    for (int i = 0; i < PHYSICAL_REG_FILE_SIZE; ++i)
    {
        if (cpu->physicalRegFile[i].valid_bit)
        {
            printf("P%-3d[%-3d] ", i, cpu->physicalRegFile[i].data_field);
        }
    }

    printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_CCPhysicalRegisters_file(const APEX_CPU *cpu)
{
    printf("\n----------------\n%s","CC Flag values: ");

    for (int i = 0; i < CC_REG_FILE_SIZE; ++i)
    {
        if (cpu->ccRegFile[i].valid_bit)
        {
            printf("cp%i {Z=%d, P=%d} ", i, cpu->ccRegFile[i].flag.zero, cpu->ccRegFile[i].flag.positive);
        }
    }
    printf("\n----------------");
}

static void
print_issueQueue(const APEX_CPU *cpu)
{
    printf("%-17s:", "IQ");
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        if (cpu->issueQueue[i].valid_bit)
        {
            printf(" pc(%d) ", cpu->issueQueue[i].instr.pc);
            print_instruction_physical(&cpu->issueQueue[i].instr);
        }
    }
    printf("\n");
}

static void
print_BQ(const APEX_CPU *cpu)
{
    printf("%-17s:", "BQ");
    for (int i = 0; i < BRANCH_QUEUE_SIZE; ++i)
    {
        if (cpu->branchQueue[i].valid_bit)
        {
            printf(" pc(%d) ", cpu->branchQueue[i].instr.pc);
            print_instruction_physical(&cpu->branchQueue[i].instr);
        }
    }
    printf("\n");

}

static void
print_ROB(const APEX_CPU *cpu)
{
    printf("%-17s:", "ROB");
    for (int i = cpu->ROB_head; i <= cpu->ROB_tail; ++i)
    {
        if (cpu->RoB[i].established_bit)
        {
            printf(" pc(%d) ", cpu->RoB[i].instr.pc);
            print_instruction_physical(&cpu->RoB[i].instr);
        }
    }

    printf("\n");
}

static void
print_LSQ(const APEX_CPU *cpu)
{
    printf("%-17s:", "LSQ");
    for (int i = cpu->LSQ_head; i <= cpu->LSQ_tail; ++i)
    {
        if (cpu->lsq[i].established_bit)
        {
            printf(" pc(%d) ", cpu->lsq[i].instr.pc);
            print_instruction_physical(&cpu->lsq[i].instr);
        }
    }

    printf("\n");
}

static void
print_BTB(const APEX_CPU *cpu)
{
    printf("%-17s:", "BTB");
    for (int i = 0; i < BTB_SIZE; ++i)
    {
        if (cpu->BTBEntry[i].branch_pc != 0)
        {
            printf(" pc(%d) Prediction(%d%d)  Target[%d]", cpu->BTBEntry[i].branch_pc,
                   cpu->BTBEntry[i].recent_outcomes[0], cpu->BTBEntry[i].recent_outcomes[0],
                   cpu->BTBEntry[i].target_pc);
        }
    }

    printf("\n");
}


// static void
// print_BTB(const APEX_CPU *cpu)
// {
//     printf("--------------\n%s\n--------------\n","BTB");
//     printf("%-9s %-9s %-9s \n", "pc", "bits", "Target");
//     for (int i = 0; i < BTB_SIZE; ++i)
//     {
//         if (cpu->BTBEntry[i].branch_pc != 0)
//         {
//             printf("%-9d %-d%-9d %-9d \n",cpu->BTBEntry[i].branch_pc, cpu->BTBEntry[i].recent_outcomes[0], cpu->BTBEntry[i].recent_outcomes[0],
//                    cpu->BTBEntry[i].target_pc);

//         }
//     }

//     printf("\n");
// }
