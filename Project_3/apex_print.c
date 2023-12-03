
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
    case OPCODE_LOADP:
    {
        printf("%s,P%d,P%d,#%d ", stage->opcode_str, stage->phyrd, stage->phyrs1,
               stage->imm);
        break;
    }

    case OPCODE_STORE:
    case OPCODE_STOREP:
    {
        printf("%s,P%d,P%d,#%d ", stage->opcode_str, stage->phyrs1, stage->phyrs2,
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
print_flag_values(int p, int z)
{

    printf("-----------\n%s P = %d, Z = %d\n-----------\n", "Flag Values: ", p, z);
}

static void
print_forwarding_tags(APEX_CPU *cpu)
{
    if (cpu->intFU_frwded_tag != -1)
    {
        printf("\n--------------\nIntforwarding tag: P%d\n-----------------\n", cpu->intFU_frwded_tag);
    }

    if (cpu->MulFU_frwded_tag != -1)
    {
        printf("\n--------------\nMulforwarding tag: P%d\n-----------------\n", cpu->MulFU_frwded_tag);
    }

    if (cpu->AFU_frwded_tag != -1)
    {
        printf("\n--------------\nAFUforwarding tag: P%d\n-----------------\n", cpu->AFU_frwded_tag);
    }
}
static void
set_flag_values(APEX_CPU *cpu)
{
    if (cpu->physicalRegFile[cpu->execute_IntFU.phyrd].data_field == 0)
    {
        cpu->zero_flag = TRUE;
        cpu->p_flag = FALSE;
    }
    else
    {
        cpu->zero_flag = FALSE;
        cpu->p_flag = TRUE;
    }
}

static void
print_memory_address_values(APEX_CPU *cpu)
{
    printf("Memory Addresses:  ");
    for (int i = 0; i < DATA_MEMORY_SIZE; i++)
    {
        if (cpu->data_memory[i] != 0)
        {
            printf("MEM[%d] = %d ", i, cpu->data_memory[i]);
        }
    }
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

    for (int i = 0; i < REG_FILE_SIZE / 2; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");

    for (i = (REG_FILE_SIZE / 2); i < REG_FILE_SIZE; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_physicalRegisters_file(const APEX_CPU *cpu)
{
    printf("\n----------------\n%s\n----------------\n", "Physical Registers:");

    for (int i = 0; i < PHYSICAL_REG_FILE_SIZE; ++i)
    {
        if (cpu->physicalRegFile[i].valid_bit)
        {
            printf("P%-3d[%-3d] ", i, cpu->physicalRegFile[i].data_field);
        }
    }

    printf("\n");
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