/*
 * apex_cpu.c
 * Contains APEX cpu pipeline implementation
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State UniveregStatusty of New York at Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"
#include "apex_macros.h"

/* Converts the PC(4000 series) into array index for code memory
 *
 * Note: You are not supposed to edit this function
 */
static int
get_code_memory_index_from_pc(const int pc)
{
    return (pc - 4000) / 4;
}

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
    printf("%-15s: pc(%d) ", name, stage->pc);
    if (strcmp(name, "Fetch"))
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
print_flag_values(int p, int n, int z)
{

    printf("-----------\n%s P = %d, N = %d, Z = %d\n-----------\n", "Flag Values: ", p, n, z);
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
 
static void
print_issueQueue(const APEX_CPU *cpu)
{
    for (int i = 0; i < ISSUE_QUEUE_SIZE ; ++i)
    {
        if(cpu->issueQueue[i].valid_bit)
        {
           print_instruction_physical(&cpu->issueQueue[i].instr);
        }
    }
}

static void
print_ROB(const APEX_CPU *cpu)
{
    for (int i = cpu->ROB_head; i <= cpu->ROB_tail ; ++i)
    {
        if(cpu->RoB[i].established_bit)
        {
           print_instruction_physical(&cpu->RoB[i].instr);
        }
    }

}


static void
initializeRenameTable(APEX_CPU *cpu)
{
    for (int i = 0; i <= REG_FILE_SIZE; ++i)
    {
        cpu->renameTable[i] = NOT_RENAMED;
    }
}

static void
initializeFreeLists(APEX_CPU *cpu)
{
    for (int i = 0; i < PHYSICAL_REG_FILE_SIZE; ++i)
    {
        cpu->freePhysicalRegList[i] = i;
    }

    for (int i = 0; i < CC_REG_FILE_SIZE; ++i)
    {
        cpu->freeCCFlagsRegList[i] = i;
    }
}

static void
initializeIssueQueue(APEX_CPU *cpu)
{
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        cpu->issueQueue[i].valid_bit = ADD_ZERO;
        cpu->issueQueue[i].ready_flag = ADD_ZERO;
        cpu->issueQueue[i].dispatch_time = ADD_ZERO;
    }
}

static void
initializeROB(APEX_CPU *cpu)
{
    for (int i = 0; i < ROB_SIZE; ++i)
    {
        cpu->RoB[i].established_bit = ADD_ZERO;
        cpu->RoB[i].lsq_index = -1;
    }
    cpu->ROB_head = ADD_ZERO;
    cpu->ROB_tail = ROB_SIZE - 1;
    cpu->RoB[cpu->ROB_head].prev_renametable_entry = -1;
}

// get Free physical register from head of free list
static int
getHeadofFreePhysicalRegister(APEX_CPU *cpu)
{
    if (cpu->freePhysicalRegList[0] != -1)
    {
        int head = cpu->freePhysicalRegList[0];

        for (int i = 0; i < PHYSICAL_REG_FILE_SIZE - 1; ++i)
        {
            cpu->freePhysicalRegList[i] = cpu->freePhysicalRegList[i + 1];
        }
        // cpu->freePhysicalRegList[PHYSICAL_REG_FILE_SIZE - 1] = head;
        return head;
    }
    else
    {
        return -1;
    }
}

static int
isIssueQueueFull(APEX_CPU *cpu)
{
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        if (!cpu->issueQueue[i].valid_bit)
        {
            return FALSE;
        }
    }
    return TRUE;
}

static int
isROBFull(APEX_CPU *cpu)
{
    return cpu->ROB_head = cpu->ROB_tail;
}

static void
establishIssueQueueEntry(APEX_CPU *cpu)
{
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        if (!cpu->issueQueue[i].valid_bit)
        {
            cpu->issueQueue[i].valid_bit = 1;
            cpu->issueQueue[i].dispatch_time = cpu->issue_counter++;
            cpu->issueQueue[i].instr = cpu->rename2_dispatch;
            return;
        }
    }
}

static void
establishROBEntry(APEX_CPU *cpu)
{
    cpu->ROB_tail = (cpu->ROB_tail + 1) % ROB_SIZE;
    cpu->RoB[cpu->ROB_tail].instr = cpu->rename2_dispatch;
}

static void
issueWakeupInstructionFromIQ(APEX_CPU *cpu)
{
    int min_counter = 999;
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        switch (cpu->decode_rename1.opcode)
        {

        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_XOR:
        case OPCODE_STORE:
        case OPCODE_CMP:
        {
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
        case OPCODE_JALR:
        case OPCODE_JUMP:
        case OPCODE_CML:
        case OPCODE_LOAD:
        {
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
        if (min_counter == cpu->issueQueue[i].dispatch_time)
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
        switch (cpu->decode_rename1.opcode)
        {
        case OPCODE_MUL:
        {
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
        if (min_counter == cpu->issueQueue[i].dispatch_time && cpu->mulcycle_counter == 0)
        {
            cpu->execute_MulFU = cpu->issueQueue[i].instr;
            cpu->issueQueue[i].valid_bit = INVALID;
            cpu->mulcycle_counter = 3;
            break;
        }
    }
}

/*
 * Fetch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_fetch(APEX_CPU *cpu)
{
    APEX_Instruction *current_ins;

    if (cpu->fetch.has_insn)
    {

        if (cpu->fetch_from_next_cycle == TRUE)
        {
            cpu->fetch_from_next_cycle = FALSE;

            if (ENABLE_DEBUG_MESSAGES)
            {
                printf("Fetch          :\n");
            }

            /* Skip this cycle*/
            return;
        }

        cpu->fetch.pc = cpu->pc;

        /* Index into code memory using this pc and copy all instruction fields
         * into fetch latch  */
        current_ins = &cpu->code_memory[get_code_memory_index_from_pc(cpu->pc)];
        strcpy(cpu->fetch.opcode_str, current_ins->opcode_str);
        cpu->fetch.opcode = current_ins->opcode;
        cpu->fetch.rd = current_ins->rd;
        cpu->fetch.rs1 = current_ins->rs1;
        cpu->fetch.rs2 = current_ins->rs2;
        cpu->fetch.imm = current_ins->imm;

        /* Update PC for next instruction */

        /* Copy data from fetch latch to decode latch*/
        if (!cpu->decode_rename1.stall)
        {
            cpu->pc += 4;
            cpu->decode_rename1 = cpu->fetch;
        }

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Fetch", &cpu->fetch);
        }

        /* Stop fetching new instructions if HALT is fetched */
        if (cpu->fetch.opcode == OPCODE_HALT)
        {
            cpu->fetch.has_insn = FALSE;
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("Fetch          :\n");
        }
    }
}

/*
 * Decode Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_decode_rename1(APEX_CPU *cpu)
{

    if (cpu->decode_rename1.has_insn)
    {
        /* Read operands from register file based on the instruction type */

        switch (cpu->decode_rename1.opcode)
        {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_XOR:
        {

            /* Source 1 of reg to reg instruction*/
            cpu->decode_rename1.phyrs1 = cpu->renameTable[cpu->decode_rename1.rs1];

            /* Source 2 of reg to reg instruction*/
            cpu->decode_rename1.phyrs2 = cpu->renameTable[cpu->decode_rename1.rs2];

            /* destination reg allocate free physical register*/
            int headOfFreePhyRegister = getHeadofFreePhysicalRegister(cpu);

            cpu->renameTable[cpu->decode_rename1.rd] = headOfFreePhyRegister;
            cpu->decode_rename1.phyrd = headOfFreePhyRegister;
            cpu->physicalRegFile[cpu->decode_rename1.phyrd].valid_bit = 0;

            break;
        }

        case OPCODE_ADDL:
        case OPCODE_SUBL:
        {
            /* Source 1 of reg to reg instruction*/
            cpu->decode_rename1.phyrs1 = cpu->renameTable[cpu->decode_rename1.rs1];

            /* destination reg allocate free physical register*/
            int headOfFreePhyRegister = getHeadofFreePhysicalRegister(cpu);

            cpu->renameTable[cpu->decode_rename1.rd] = headOfFreePhyRegister;
            cpu->decode_rename1.phyrd = headOfFreePhyRegister;
            cpu->physicalRegFile[cpu->decode_rename1.phyrd].valid_bit = 0;
            break;
        }

        case OPCODE_MOVC:
        {

            /* destination reg allocate free physical register*/
            int headOfFreePhyRegister = getHeadofFreePhysicalRegister(cpu);

            cpu->renameTable[cpu->decode_rename1.rd] = headOfFreePhyRegister;
            cpu->decode_rename1.phyrd = headOfFreePhyRegister;
            cpu->physicalRegFile[cpu->decode_rename1.phyrd].valid_bit = 0;

            break;
        }

        case OPCODE_HALT:
        {
            break;
        }
        }

        if (!cpu->rename2_dispatch.stall || !isIssueQueueFull(cpu))
        {
            cpu->rename2_dispatch = cpu->decode_rename1;
        }

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Decode_rename1", &cpu->decode_rename1);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("Decode_rename1      :\n");
        }
    }
}

/*
 * Decode Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_rename2_dispatch(APEX_CPU *cpu)
{

    if (cpu->rename2_dispatch.has_insn)
    {
        /* Read operands from register file based on the instruction type */
        switch (cpu->rename2_dispatch.opcode)
        {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_XOR:
        case OPCODE_MUL:
        {

            if (cpu->physicalRegFile[cpu->rename2_dispatch.phyrs1].valid_bit)
            {
                cpu->rename2_dispatch.phyrs1_value = cpu->physicalRegFile[cpu->rename2_dispatch.phyrs1].data_field;
                cpu->rename2_dispatch.phyrs1_valid = 1;
            }

            if (cpu->physicalRegFile[cpu->rename2_dispatch.phyrs2].valid_bit)
            {
                cpu->rename2_dispatch.phyrs2_value = cpu->physicalRegFile[cpu->rename2_dispatch.phyrs2].data_field;
                cpu->rename2_dispatch.phyrs2_valid = 1;
            }

            if (isIssueQueueFull(cpu) || isROBFUll(cpu))
            {
                cpu->rename2_dispatch.stall = TRUE;

            }
            else
            {
                establishIssueQueueEntry(cpu);
                establishROBEntry(cpu);
            }

            break;
        }

        case OPCODE_MOVC:
        case OPCODE_HALT:
        {
            if (isIssueQueueFull(cpu) || isROBFUll(cpu))
            {
                cpu->rename2_dispatch.stall = TRUE;
            }
            else
            {
                establishIssueQueueEntry(cpu);
                establishROBEntry(cpu);
            }
            break;
        }

        }

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Rename2_disptach", &cpu->rename2_dispatch);
            print_issueQueue(cpu);
            print_ROB(cpu);    
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("Rename2_disptach      :\n");
            print_issueQueue(cpu);
            print_ROB(cpu);
        }
    }

}

/*
 * Execute Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_execute_IntFU(APEX_CPU *cpu)
{

    issueWakeupInstructionFromIQ(cpu);

    if (cpu->execute_IntFU.has_insn)
    {
        /* Execute logic based on instruction type */
        switch (cpu->execute_IntFU.opcode)
        {
        case OPCODE_ADD:
        {
            cpu->execute_IntFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs1].data_field;
            cpu->execute_IntFU.phyrs2_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs2].data_field;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].data_field = cpu->execute_IntFU.phyrs1_value + cpu->execute_IntFU.phyrs2_value;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].valid_bit = VALID;

            break;
        }

        case OPCODE_SUB:
        {
            cpu->execute_IntFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs1].data_field;
            cpu->execute_IntFU.phyrs2_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs2].data_field;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].data_field = cpu->execute_IntFU.phyrs1_value - cpu->execute_IntFU.phyrs2_value;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].valid_bit = VALID;

            break;
        }

        case OPCODE_AND:
        {
            cpu->execute_IntFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs1].data_field;
            cpu->execute_IntFU.phyrs2_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs2].data_field;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].data_field = cpu->execute_IntFU.phyrs1_value & cpu->execute_IntFU.phyrs2_value;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].valid_bit = VALID;
            break;
        }

        case OPCODE_OR:
        {
            cpu->execute_IntFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs1].data_field;
            cpu->execute_IntFU.phyrs2_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs2].data_field;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].data_field = cpu->execute_IntFU.phyrs1_value | cpu->execute_IntFU.phyrs2_value;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].valid_bit = VALID;

            break;
        }

        case OPCODE_XOR:
        {
            cpu->execute_IntFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs1].data_field;
            cpu->execute_IntFU.phyrs2_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs2].data_field;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].data_field = cpu->execute_IntFU.phyrs1_value ^ cpu->execute_IntFU.phyrs2_value;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].valid_bit = VALID;
            break;
        }

        case OPCODE_ADDL:
        {
            cpu->execute_IntFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs1].data_field;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].data_field = cpu->execute_IntFU.phyrs1_value + cpu->execute_IntFU.imm;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].valid_bit = VALID;

            break;
        }

        case OPCODE_SUBL:
        {
            cpu->execute_IntFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs1].data_field;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].data_field = cpu->execute_IntFU.phyrs1_value + cpu->execute_IntFU.imm;

            cpu->physicalRegFile[cpu->execute_IntFU.phyrd].valid_bit = VALID;

            break;
        }

        case OPCODE_MOVC:
        {
            cpu->execute_IntFU.result_buffer = cpu->execute_IntFU.imm + ADD_ZERO;
            break;
        }
        }

        /* Copy data from execute latch to memory latch*/
        cpu->execute_IntFU.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("execute_IntFU", &cpu->execute_IntFU);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("execute_IntFU        :\n");
        }
    }
}

static void
APEX_execute_MulFU(APEX_CPU *cpu)
{

    issueWakeupMulInstructionFromIQ(cpu);

    if (cpu->execute_MulFU.has_insn)
    {
        if(--cpu->mulcycle_counter == 0)
        {
                    
          cpu->execute_IntFU.phyrs1_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs1].data_field;
          cpu->execute_IntFU.phyrs2_value = cpu->physicalRegFile[cpu->execute_IntFU.phyrs2].data_field;
          cpu->physicalRegFile[cpu->execute_IntFU.phyrd].data_field = cpu->execute_IntFU.phyrs1_value * cpu->execute_IntFU.phyrs2_value;
          cpu->physicalRegFile[cpu->execute_IntFU.phyrd].valid_bit = VALID;

        }

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("execute_MulFU", &cpu->execute_MulFU);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("execute_MulFU        :\n");
        }
    }
}

static void
APEX_execute_AFU(APEX_CPU *cpu)
{
    if (cpu->execute_AFU.has_insn)
    {
        /* Execute logic based on instruction type */
        switch (cpu->execute_AFU.opcode)
        {
        }
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("execute_AFU", &cpu->execute_AFU);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("execute_AFU        :\n");
        }
    }
}

static void
APEX_execute_BFU(APEX_CPU *cpu)
{
    if (cpu->execute_BFU.has_insn)
    {
        /* Execute logic based on instruction type */
        switch (cpu->execute_BFU.opcode)
        {

        }
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("execute_BFU", &cpu->execute_BFU);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("execute_BFU        :\n");
        }
    }
}

/*
 * Memory Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_execute_MAU(APEX_CPU *cpu)
{
    if (cpu->execute_MAU.has_insn)
    {
        switch (cpu->execute_MAU.opcode)
        {
        case OPCODE_LOAD:
        case OPCODE_LOADP:
        {
            /* Read from data memory */
            cpu->execute_MAU.result_buffer = cpu->data_memory[cpu->execute_MAU.memory_address];
            break;
        }

        case OPCODE_STORE:
        case OPCODE_STOREP:
        {
            cpu->data_memory[cpu->execute_MAU.memory_address] = cpu->execute_MAU.rs1_value;
            break;
        }

        case OPCODE_JALR:
        case OPCODE_ADDL:
        case OPCODE_SUBL:
        {
            break;
        }

        case OPCODE_BZ:
        case OPCODE_BNZ:
        case OPCODE_HALT:
        case OPCODE_CML:
        case OPCODE_CMP:
        case OPCODE_NOP:
        case OPCODE_BP:
        case OPCODE_BNP:
        case OPCODE_JUMP:
        {
            break;
        }
        }

        /* Copy data from memory latch to writeback latch*/
        cpu->execute_MAU.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Memory", &cpu->execute_MAU);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("Memory         :\n");
        }
    }
}

/*
 * Writeback Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static int
APEX_commit_to_ARF(APEX_CPU *cpu)
{
    cpu->commit_ARF = cpu->RoB[cpu->ROB_head].instr;

    if (cpu->commit_ARF.has_insn)
    {
        /* Write result to register file based on instruction type */
        switch (cpu->commit_ARF.opcode)
        {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_XOR:
        case OPCODE_ADDL:
        case OPCODE_SUBL:
        case OPCODE_LOAD:
        case OPCODE_MOVC:
        {
            if (cpu->physicalRegFile[cpu->commit_ARF.phyrd].valid_bit)
            {
                cpu->regs[cpu->commit_ARF.rd] = cpu->physicalRegFile[cpu->commit_ARF.phyrd].data_field;

                if (cpu->RoB[cpu->ROB_head].prev_renametable_entry != -1)
                {
                    cpu->physicalRegFile[cpu->commit_ARF.rd].valid_bit = INVALID;
                }
                cpu->RoB[cpu->ROB_head].prev_renametable_entry = cpu->commit_ARF.phyrd;

                cpu->ROB_head = (cpu->ROB_head + 1) % ROB_SIZE;
            }
            break;
        }

        case OPCODE_LOADP:
        {
            break;
        }

        case OPCODE_STOREP:
        {
            break;
        }

        case OPCODE_JALR:
        {

            break;
        }

        case OPCODE_STORE:
        case OPCODE_BZ:
        case OPCODE_BNZ:
        case OPCODE_BP:
        case OPCODE_BNP:
        case OPCODE_NOP:
        case OPCODE_HALT:
        case OPCODE_CML:
        case OPCODE_CMP:
        case OPCODE_JUMP:
        {

            break;
        }
        }

        cpu->commit_ARF.has_insn = FALSE;
        cpu->insn_completed++;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("commit_ARF", &cpu->commit_ARF);
        }

        if (cpu->commit_ARF.opcode == OPCODE_HALT)
        {
            /* Stop the APEX simulator */
            return TRUE;
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("Writeback      :\n");
        }
    }

    /* Default */
    return 0;
}

/*
 * This function creates and initializes APEX cpu.
 *
 * Note: You are free to edit this function according to your implementation
 */
APEX_CPU *
APEX_cpu_init(const char *filename)
{
    int i;
    APEX_CPU *cpu;

    if (!filename)
    {
        return NULL;
    }

    cpu = calloc(1, sizeof(APEX_CPU));

    if (!cpu)
    {
        return NULL;
    }

    /* Initialize PC, Registers and all pipeline stages */
    cpu->pc = 4000;
    memset(cpu->regs, 0, sizeof(int) * REG_FILE_SIZE);
    memset(cpu->data_memory, 0, sizeof(int) * DATA_MEMORY_SIZE);
    memset(cpu->regStatus, 0, sizeof(int) * REG_FILE_SIZE);
    cpu->single_step = ENABLE_SINGLE_STEP;
    cpu->issue_counter = ADD_ZERO;

    /* Parse input file and create code memory */
    cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);
    if (!cpu->code_memory)
    {
        free(cpu);
        return NULL;
    }

    if (ENABLE_DEBUG_MESSAGES)
    {
        fprintf(stderr,
                "APEX_CPU: Initialized APEX CPU, loaded %d instructions\n",
                cpu->code_memory_size);
        fprintf(stderr, "APEX_CPU: PC initialized to %d\n", cpu->pc);
        fprintf(stderr, "APEX_CPU: Printing Code Memory\n");
        printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode_str", "rd", "rs1", "rs2",
               "imm");

        for (i = 0; i < cpu->code_memory_size; ++i)
        {
            printf("%-9s %-9d %-9d %-9d %-9d\n", cpu->code_memory[i].opcode_str,
                   cpu->code_memory[i].rd, cpu->code_memory[i].rs1,
                   cpu->code_memory[i].rs2, cpu->code_memory[i].imm);
        }
    }

    /* To start fetch stage */
    cpu->fetch.has_insn = TRUE;

    initializeFreeLists(cpu);
    initializeRenameTable(cpu);

    return cpu;
}

/*
 * APEX CPU simulation loop
 *
 * Note: You are free to edit this function according to your implementation
 */
void APEX_cpu_run(APEX_CPU *cpu)
{
    char user_prompt_val;
    cpu->clock = 1;

    while (TRUE)
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("--------------------------------------------\n");
            printf("Clock Cycle #: %d\n", cpu->clock);
            printf("--------------------------------------------\n");
        }

        if (APEX_commit_to_ARF(cpu) || cpu->num_of_cycles_to_run == cpu->clock)
        {
            print_flag_values(cpu->p_flag, cpu->n_flag, cpu->zero_flag);
            print_memory_address_values(cpu);
            print_reg_file(cpu);
            /* Halt in writeback stage */
            printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
            break;
        }

        APEX_execute_MAU(cpu);
        APEX_execute_MulFU(cpu);
        APEX_execute_BFU(cpu);
        APEX_execute_AFU(cpu);
        APEX_execute_IntFU(cpu);
        APEX_rename2_dispatch(cpu);
        APEX_decode_rename1(cpu);
        APEX_fetch(cpu);

        print_flag_values(cpu->p_flag, cpu->n_flag, cpu->zero_flag);
        print_memory_address_values(cpu);
        print_reg_file(cpu);

        if (cpu->single_step)
        {
            printf("Press any key to advance CPU Clock or <q> to quit:\n");
            scanf("%c", &user_prompt_val);

            if ((user_prompt_val == 'Q') || (user_prompt_val == 'q'))
            {
                printf("APEX_CPU: Simulation Stopped, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
                break;
            }
        }

        cpu->clock++;
    }
}

/*
 * This function deallocates APEX CPU.

 *
 * Note: You are free to edit this function according to your implementation
 */
void APEX_cpu_stop(APEX_CPU *cpu)
{
    free(cpu->code_memory);
    free(cpu);
}