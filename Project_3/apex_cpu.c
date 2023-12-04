#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"
#include "apex_macros.h"
#include "apex_sources.c"
#include "apex_issue.c"
#include "apex_print.c"
#include "apex_queues.c"

static int
get_code_memory_index_from_pc(const int pc)
{
    return (pc - 4000) / 4;
}

static void
initializeRenameTable(APEX_CPU *cpu)
{
    for (int i = 0; i <= REG_FILE_SIZE; ++i)
    {
        cpu->renameTable[i] = NOT_RENAMED;
    }
}

/* get Free physical register from head of free list */
static int
headof_free_physicalRegisters(APEX_CPU *cpu)
{
    if (cpu->freePhysicalRegList[0] != -1)
    {
        int head = cpu->freePhysicalRegList[0];

        for (int i = 0; i < PHYSICAL_REG_FILE_SIZE - 1; ++i)
        {
            cpu->freePhysicalRegList[i] = cpu->freePhysicalRegList[i + 1];
        }
        cpu->freePhysicalRegList[PHYSICAL_REG_FILE_SIZE - 1] = -1;
        return head;
    }
    else
    {
        return -1;
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
initializePhysicalRegistersFile(APEX_CPU *cpu)
{
    for (int i = 0; i < PHYSICAL_REG_FILE_SIZE; ++i)
    {
        cpu->physicalRegFile[i].valid_bit = INVALID;
        cpu->physicalRegFile[i].data_field = INVALID;
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
                printf("%-17s:\n", "Fetch");
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
            printf("%-17s:\n", "Fetch");
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
            cpu->decode_rename1.phyrs1 = cpu->renameTable[cpu->decode_rename1.rs1];
            cpu->decode_rename1.phyrs2 = cpu->renameTable[cpu->decode_rename1.rs2];

            set_decode_physical_source1(cpu);
            set_decode_physical_source2(cpu);

            int headOfFreePhyRegister = headof_free_physicalRegisters(cpu);

            if (headOfFreePhyRegister != -1)
            {
                cpu->renameTable[cpu->decode_rename1.rd] = headOfFreePhyRegister;
                cpu->decode_rename1.phyrd = headOfFreePhyRegister;
                cpu->physicalRegFile[cpu->decode_rename1.phyrd].valid_bit = INVALID;
            }
            else
            {
                cpu->decode_rename1.stall = TRUE;
            }
            break;
        }

        case OPCODE_ADDL:
        case OPCODE_SUBL:
        {
            cpu->decode_rename1.phyrs1 = cpu->renameTable[cpu->decode_rename1.rs1];
            set_decode_physical_source1(cpu);

            int headOfFreePhyRegister = headof_free_physicalRegisters(cpu);

            if (headOfFreePhyRegister != -1)
            {
                cpu->renameTable[cpu->decode_rename1.rd] = headOfFreePhyRegister;
                cpu->decode_rename1.phyrd = headOfFreePhyRegister;
                cpu->physicalRegFile[cpu->decode_rename1.phyrd].valid_bit = INVALID;
            }
            else
            {
                cpu->decode_rename1.stall = TRUE;
            }
            break;
        }

        case OPCODE_LOAD:
        {
            cpu->decode_rename1.phyrs1 = cpu->renameTable[cpu->decode_rename1.rs1];
            set_decode_physical_source1(cpu);

            int headOfFreePhyRegister = headof_free_physicalRegisters(cpu);
            if (headOfFreePhyRegister != -1)
            {
                cpu->renameTable[cpu->decode_rename1.rd] = headOfFreePhyRegister;
                cpu->decode_rename1.phyrd = headOfFreePhyRegister;
                cpu->physicalRegFile[cpu->decode_rename1.phyrd].valid_bit = INVALID;
            }
            else
            {
                cpu->decode_rename1.stall = TRUE;
            }
            break;
        }

        case OPCODE_MOVC:
        {
            /* destination reg allocate free physical register*/
            int headOfFreePhyRegister = headof_free_physicalRegisters(cpu);

            if (headOfFreePhyRegister != -1)
            {
                cpu->renameTable[cpu->decode_rename1.rd] = headOfFreePhyRegister;
                cpu->decode_rename1.phyrd = headOfFreePhyRegister;
                cpu->physicalRegFile[cpu->decode_rename1.phyrd].valid_bit = INVALID;
            }
            else
            {
                cpu->decode_rename1.stall = TRUE;
            }
            break;
        }

        case OPCODE_LOADP:
        {
            cpu->decode_rename1.phyrs1 = cpu->renameTable[cpu->decode_rename1.rs1];
            set_decode_physical_source1(cpu);

            int headOfFreePhyRegister = headof_free_physicalRegisters(cpu);

            if (headOfFreePhyRegister != -1)
            {
                cpu->renameTable[cpu->decode_rename1.rd] = headOfFreePhyRegister;
                cpu->decode_rename1.phyrd = headOfFreePhyRegister;
                cpu->physicalRegFile[cpu->decode_rename1.phyrd].valid_bit = INVALID;
            }
            else
            {
                cpu->decode_rename1.stall = TRUE;
            }

            headOfFreePhyRegister = headof_free_physicalRegisters(cpu);

            if (headOfFreePhyRegister != -1)
            {
                cpu->renameTable[cpu->decode_rename1.rs1] = headOfFreePhyRegister;
                cpu->decode_rename1.phyrs3 = headOfFreePhyRegister;
                cpu->physicalRegFile[cpu->decode_rename1.phyrs3].valid_bit = INVALID;
            }
            else
            {
                cpu->decode_rename1.stall = TRUE;
            }
            break;
        }

        case OPCODE_STORE:
        {
            cpu->decode_rename1.phyrs1 = cpu->renameTable[cpu->decode_rename1.rs1];
            cpu->decode_rename1.phyrs2 = cpu->renameTable[cpu->decode_rename1.rs2];

            set_decode_physical_source1(cpu);
            set_decode_physical_source2(cpu);
            break;
        }
        case OPCODE_STOREP:
        {
            cpu->decode_rename1.phyrs1 = cpu->renameTable[cpu->decode_rename1.rs1];
            cpu->decode_rename1.phyrs2 = cpu->renameTable[cpu->decode_rename1.rs2];

            set_decode_physical_source1(cpu);
            set_decode_physical_source2(cpu);

            int headOfFreePhyRegister = headof_free_physicalRegisters(cpu);
            cpu->renameTable[cpu->decode_rename1.rs2] = headOfFreePhyRegister;
            cpu->decode_rename1.phyrs3 = headOfFreePhyRegister;
            cpu->physicalRegFile[cpu->decode_rename1.phyrs3].valid_bit = INVALID;

            break;
        }

        case OPCODE_HALT:
        {
            break;
        }
        }

        if (!cpu->rename2_dispatch.stall)
        {
            cpu->rename2_dispatch = cpu->decode_rename1;
        }

        cpu->decode_rename1.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Decode_rename1", &cpu->decode_rename1);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("%-17s:\n", "Decode_rename1");
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
            set_dispatch_physical_source1(cpu);
            set_dispatch_physical_source2(cpu);

            if (!is_IssueQueue_Full(cpu) && !is_ROB_Full(cpu))
            {
                cpu->issue_queue = cpu->rename2_dispatch;
            }
            else
            {
                cpu->rename2_dispatch.stall = TRUE;
            }

            break;
        }

        case OPCODE_ADDL:
        case OPCODE_SUBL:
        {
            set_dispatch_physical_source1(cpu);

            if (!is_IssueQueue_Full(cpu) && !is_ROB_Full(cpu))
            {
                cpu->issue_queue = cpu->rename2_dispatch;
            }
            else
            {
                cpu->rename2_dispatch.stall = TRUE;
            }

            break;
        }

        case OPCODE_MOVC:
        case OPCODE_HALT:
        {
            if (!is_IssueQueue_Full(cpu) && !is_ROB_Full(cpu))
            {
                cpu->issue_queue = cpu->rename2_dispatch;
            }
            else
            {
                cpu->rename2_dispatch.stall = TRUE;
            }
            break;
        }

        case OPCODE_LOADP:
        case OPCODE_LOAD:
        {
            set_dispatch_physical_source1(cpu);

            if (!is_IssueQueue_Full(cpu) && !is_ROB_Full(cpu) && !is_LSQ_Full(cpu))
            {
                cpu->issue_queue = cpu->rename2_dispatch;
            }
            else
            {
                cpu->rename2_dispatch.stall = TRUE;
            }
            break;
        }

        case OPCODE_STOREP:
        case OPCODE_STORE:
        {
            set_dispatch_physical_source1(cpu);
            set_dispatch_physical_source2(cpu);

            if (!is_IssueQueue_Full(cpu) && !is_ROB_Full(cpu) && !is_LSQ_Full(cpu))
            {
                cpu->issue_queue = cpu->rename2_dispatch;
            }
            else
            {
                cpu->rename2_dispatch.stall = TRUE;
            }
            break;
        }
        }
        cpu->rename2_dispatch.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("Rename2_disptach", &cpu->rename2_dispatch);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("%-17s:\n", "Rename2_disptach");
        }
    }
}

static void
APEX_issue_queue(APEX_CPU *cpu)
{
    if (cpu->issue_queue.has_insn)
    {
        int loadorstore = 0;
        switch (cpu->issue_queue.opcode)
        {
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_XOR:
        case OPCODE_MUL:
        case OPCODE_MOVC:
        case OPCODE_HALT:
        case OPCODE_ADDL:
        case OPCODE_SUBL:
        {
            establish_IssueQueueEntry(cpu);
            establish_ROBEntry(cpu, loadorstore);
            break;
        }
        case OPCODE_LOADP:
        case OPCODE_LOAD:
        case OPCODE_STORE:
        case OPCODE_STOREP:
        {
            loadorstore = 1;
            establish_IssueQueueEntry(cpu);
            establish_LSQEntry(cpu);
            establish_ROBEntry(cpu, loadorstore);
            break;
        }
        }

        cpu->issue_queue.has_insn = FALSE;
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_ROB(cpu);
            print_LSQ(cpu);
            print_issueQueue(cpu);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            print_ROB(cpu);
            print_LSQ(cpu);
            print_issueQueue(cpu);
        }
    }

    issueWakeupInstructionFromIQ(cpu);
    issueWakeupMulInstructionFromIQ(cpu);
    updateLSQentrywithForwardedBus(cpu);
    issueWakeupAFUInstructionFromIQ(cpu);
}

/*
 * Execute Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_execute_IntFU(APEX_CPU *cpu)
{
    if (cpu->execute_IntFU.has_insn)
    {
        switch (cpu->execute_IntFU.opcode)
        {
        case OPCODE_ADD:
        {
            set_Source1_value_IntFU(cpu);
            set_Source2_value_IntFU(cpu);

            cpu->intFU_frwded_tag = cpu->execute_IntFU.phyrd;
            cpu->intFU_frwded_value = cpu->execute_IntFU.phyrs1_value + cpu->execute_IntFU.phyrs2_value;

            set_flag_values(cpu);
            break;
        }

        case OPCODE_SUB:
        {
            set_Source1_value_IntFU(cpu);
            set_Source2_value_IntFU(cpu);

            cpu->intFU_frwded_tag = cpu->execute_IntFU.phyrd;
            cpu->intFU_frwded_value = cpu->execute_IntFU.phyrs1_value - cpu->execute_IntFU.phyrs2_value;

            set_flag_values(cpu);
            break;
        }

        case OPCODE_AND:
        {
            set_Source1_value_IntFU(cpu);
            set_Source2_value_IntFU(cpu);

            cpu->intFU_frwded_tag = cpu->execute_IntFU.phyrd;
            cpu->intFU_frwded_value = cpu->execute_IntFU.phyrs1_value & cpu->execute_IntFU.phyrs2_value;

            set_flag_values(cpu);
            break;
        }

        case OPCODE_OR:
        {
            set_Source1_value_IntFU(cpu);
            set_Source2_value_IntFU(cpu);

            cpu->intFU_frwded_tag = cpu->execute_IntFU.phyrd;
            cpu->intFU_frwded_value = cpu->execute_IntFU.phyrs1_value | cpu->execute_IntFU.phyrs2_value;

            set_flag_values(cpu);
            break;
        }

        case OPCODE_XOR:
        {
            set_Source1_value_IntFU(cpu);
            set_Source2_value_IntFU(cpu);

            cpu->intFU_frwded_tag = cpu->execute_IntFU.phyrd;
            cpu->intFU_frwded_value = cpu->execute_IntFU.phyrs1_value ^ cpu->execute_IntFU.phyrs2_value;

            set_flag_values(cpu);
            break;
        }

        case OPCODE_ADDL:
        {
            set_Source1_value_IntFU(cpu);

            cpu->intFU_frwded_tag = cpu->execute_IntFU.phyrd;
            cpu->intFU_frwded_value = cpu->execute_IntFU.phyrs1_value + cpu->execute_IntFU.imm;

            set_flag_values(cpu);
            break;
        }

        case OPCODE_SUBL:
        {
            set_Source1_value_IntFU(cpu);

            cpu->intFU_frwded_tag = cpu->execute_IntFU.phyrd;
            cpu->intFU_frwded_value = cpu->execute_IntFU.phyrs1_value - cpu->execute_IntFU.imm;

            set_flag_values(cpu);
            break;
        }

        case OPCODE_MOVC:
        {
            cpu->intFU_frwded_tag = cpu->execute_IntFU.phyrd;
            cpu->intFU_frwded_value = cpu->execute_IntFU.imm + ADD_ZERO;

            set_flag_values(cpu);
            break;
        }

        case OPCODE_HALT:
        {
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

            printf("%-17s:\n", "execute_IntFU");
        }
    }
}

static void
APEX_execute_MulFU(APEX_CPU *cpu)
{
    if (cpu->execute_MulFU.has_insn)
    {
        if (--cpu->mulcycle_counter == 0)
        {
            set_Source1_value_MulFU(cpu);
            set_Source2_value_MulFU(cpu);

            cpu->MulFU_frwded_tag = cpu->execute_MulFU.phyrd;
            cpu->MulFU_frwded_value = cpu->execute_MulFU.phyrs1_value * cpu->execute_MulFU.phyrs2_value;

            set_flag_values(cpu);

            cpu->execute_MulFU.has_insn = FALSE;
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
            printf("%-17s:\n", "execute_MulFU");
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

        case OPCODE_LOAD:
        {
            set_Source1_value_AFU(cpu);

            cpu->AFU_frwded_address = cpu->execute_AFU.phyrs1_value + cpu->execute_AFU.imm;
            cpu->AFU_frwded_pc = cpu->execute_AFU.pc;

            break;
        }

        case OPCODE_LOADP:
        {
            set_Source1_value_AFU(cpu);

            cpu->AFU_frwded_address = cpu->execute_AFU.phyrs1_value + cpu->execute_AFU.imm;
            cpu->AFU_frwded_pc = cpu->execute_AFU.pc;

            cpu->AFU_frwded_tag = cpu->execute_AFU.phyrs3;
            cpu->AFU_frwded_value = cpu->execute_AFU.phyrs1_value + INCREMENTOR;

            break;
        }
        case OPCODE_STORE:
        {
            set_Source2_value_AFU(cpu);

            cpu->AFU_frwded_address = cpu->execute_AFU.phyrs2_value + cpu->execute_AFU.imm;
            cpu->AFU_frwded_pc = cpu->execute_AFU.pc;

            break;
        }
        case OPCODE_STOREP:
        {

            set_Source2_value_AFU(cpu);

            cpu->AFU_frwded_address = cpu->execute_AFU.phyrs2_value + cpu->execute_AFU.imm;
            cpu->AFU_frwded_pc = cpu->execute_AFU.pc;

            cpu->AFU_frwded_tag = cpu->execute_AFU.phyrs3;
            cpu->AFU_frwded_value = cpu->execute_AFU.phyrs2_value + INCREMENTOR;

            break;
        }
        }
        cpu->execute_AFU.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("execute_AFU", &cpu->execute_AFU);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("%-17s:\n", "execute_AFU");
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
    if (cpu->RoB[cpu->ROB_head].lsq_index == cpu->LSQ_head && cpu->mau_cycle_latency == 0)
    {
        cpu->execute_MAU = cpu->lsq[cpu->LSQ_head].instr;
        cpu->mau_cycle_latency = 2;
    }

    if (cpu->execute_MAU.has_insn)
    {
        if (--cpu->mau_cycle_latency == 0)
        {
            if (cpu->lsq[cpu->LSQ_head].LorS_bit)
            {
                if (cpu->lsq[cpu->LSQ_head].memory_address_valid)
                {
                    /* Read from data memory */
                    cpu->MAU_frwded_value = cpu->data_memory[cpu->lsq[cpu->LSQ_head].memory_address];
                    cpu->MAU_frwded_tag = cpu->lsq[cpu->LSQ_head].phyrd;
                }
            }
            else
            {
                if (cpu->lsq[cpu->LSQ_head].memory_address_valid)
                {
                    cpu->data_memory[cpu->lsq[cpu->LSQ_head].memory_address] = cpu->lsq[cpu->LSQ_head].valueToStore;
                }
            }
            cpu->execute_MAU.has_insn = FALSE;
        }

        if (ENABLE_DEBUG_MESSAGES)
        {
            print_stage_content("execute_MAU", &cpu->execute_MAU);
        }
    }
    else
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("%-17s:\n", "execute_MAU");
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

            printf("%-17s:\n", "execute_BFU");
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

    if (cpu->delete_ROB_head)
    {
        cpu->RoB[cpu->ROB_head].established_bit = INVALID;
        cpu->ROB_head = (cpu->ROB_head + 1) % ROB_SIZE;
        cpu->ROB_size--;
        cpu->delete_ROB_head = ADD_ZERO;
    }

    if (cpu->delete_LSQ_head)
    {
        cpu->lsq[cpu->LSQ_head].src_data_valid = INVALID;
        cpu->lsq[cpu->LSQ_head].established_bit = INVALID;
        cpu->LSQ_head = (cpu->LSQ_head + 1) % LSQ_SIZE;
        cpu->LSQ_size--;
        cpu->delete_LSQ_head = ADD_ZERO;
    }

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
        case OPCODE_MOVC:
        {
            if (cpu->physicalRegFile[cpu->commit_ARF.phyrd].valid_bit)
            {
                cpu->regs[cpu->commit_ARF.rd] = cpu->physicalRegFile[cpu->commit_ARF.phyrd].data_field;

                if (cpu->RoB[cpu->ROB_head].prev_renametable_entry != -1)
                {
                    cpu->freePhysicalRegList[PHYSICAL_REG_FILE_SIZE - 1] = cpu->commit_ARF.phyrd;
                }

                cpu->RoB[cpu->ROB_head].prev_renametable_entry = cpu->commit_ARF.phyrd;
                cpu->delete_ROB_head = 1;
            }
            break;
        }

        case OPCODE_LOAD:
        {
            if (cpu->physicalRegFile[cpu->commit_ARF.phyrd].valid_bit && cpu->RoB[cpu->ROB_head].lsq_index == cpu->LSQ_head && cpu->mau_cycle_latency)
            {
                cpu->regs[cpu->commit_ARF.rd] = cpu->physicalRegFile[cpu->commit_ARF.phyrd].data_field;

                if (cpu->RoB[cpu->ROB_head].prev_renametable_entry != -1)
                {
                    cpu->freePhysicalRegList[PHYSICAL_REG_FILE_SIZE - 1] = cpu->commit_ARF.phyrd;
                }

                cpu->RoB[cpu->ROB_head].prev_renametable_entry = cpu->commit_ARF.phyrd;
                cpu->delete_ROB_head = 1;
                cpu->delete_LSQ_head = 1;
            }
            break;
        }

        case OPCODE_LOADP:
        {
            if (cpu->physicalRegFile[cpu->commit_ARF.phyrd].valid_bit && cpu->physicalRegFile[cpu->commit_ARF.phyrs3].valid_bit && cpu->RoB[cpu->ROB_head].lsq_index == cpu->LSQ_head && cpu->mau_cycle_latency)
            {
                cpu->regs[cpu->commit_ARF.rd] = cpu->physicalRegFile[cpu->commit_ARF.phyrd].data_field;
                cpu->regs[cpu->commit_ARF.rs1] = cpu->physicalRegFile[cpu->commit_ARF.phyrs3].data_field;

                if (cpu->RoB[cpu->ROB_head].prev_renametable_entry != -1)
                {
                    cpu->freePhysicalRegList[PHYSICAL_REG_FILE_SIZE - 1] = cpu->commit_ARF.phyrd;
                }

                cpu->RoB[cpu->ROB_head].prev_renametable_entry = cpu->commit_ARF.phyrd;
                cpu->delete_ROB_head = 1;
                cpu->delete_LSQ_head = 1;
            }
            break;
        }
        case OPCODE_STOREP:
        {
            if (cpu->physicalRegFile[cpu->commit_ARF.phyrs3].valid_bit && cpu->RoB[cpu->ROB_head].lsq_index == cpu->LSQ_head && cpu->mau_cycle_latency)
            {
                cpu->regs[cpu->commit_ARF.rs2] = cpu->physicalRegFile[cpu->commit_ARF.phyrs3].data_field;

                if (cpu->RoB[cpu->ROB_head].prev_renametable_entry != -1)
                {
                    cpu->freePhysicalRegList[PHYSICAL_REG_FILE_SIZE - 1] = cpu->commit_ARF.phyrd;
                }

                cpu->RoB[cpu->ROB_head].prev_renametable_entry = cpu->commit_ARF.phyrd;
                cpu->delete_ROB_head = 1;
                cpu->delete_LSQ_head = 1;
            }
            break;
        }
        case OPCODE_STORE:
        {
            if (cpu->RoB[cpu->ROB_head].lsq_index == cpu->LSQ_head && cpu->mau_cycle_latency)
            {
                cpu->delete_ROB_head = 1;
                cpu->delete_LSQ_head = 1;
            }
            break;
        }

        case OPCODE_JALR:
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
            printf("%-17s:\n", "commit_ARF");
        }
    }

    update_physical_register_file(cpu);

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
    cpu->single_step = ENABLE_SINGLE_STEP;

    cpu->clock = 1;
    cpu->issue_counter = ADD_ZERO;
    cpu->delete_ROB_head = ADD_ZERO;
    cpu->delete_LSQ_head = ADD_ZERO;
    cpu->mau_cycle_latency = ADD_ZERO;

    cpu->intFU_frwded_tag = -1;
    cpu->intFU_frwded_value = -1;

    cpu->MulFU_frwded_tag = -1;
    cpu->MulFU_frwded_value = -1;

    cpu->AFU_frwded_tag = -1;
    cpu->AFU_frwded_value = -1;

    cpu->MAU_frwded_tag = -1;
    cpu->MAU_frwded_value = -1;

    cpu->AFU_frwded_address = -1;
    // cpu->AFU_frwded_phyrd = -1;
    // cpu->AFU_frwded_srcTag = -1;

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
    initializePhysicalRegistersFile(cpu);
    initializeRenameTable(cpu);
    initializeROB(cpu);
    initializeIssueQueue(cpu);
    initializeLSQ(cpu);

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
            print_forwarding_tags(cpu);
            print_flag_values(cpu->p_flag, cpu->zero_flag);
            print_memory_address_values(cpu);
            print_physicalRegisters_file(cpu);
            print_reg_file(cpu);
            printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
            break;
        }

        APEX_execute_MAU(cpu);
        APEX_execute_MulFU(cpu);
        APEX_execute_BFU(cpu);
        APEX_execute_AFU(cpu);
        APEX_execute_IntFU(cpu);
        APEX_issue_queue(cpu);
        APEX_rename2_dispatch(cpu);
        APEX_decode_rename1(cpu);
        APEX_fetch(cpu);

        print_forwarding_tags(cpu);
        print_flag_values(cpu->p_flag, cpu->zero_flag);
        print_memory_address_values(cpu);
        print_physicalRegisters_file(cpu);
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

void APEX_cpu_stop(APEX_CPU *cpu)
{
    free(cpu->code_memory);
    free(cpu);
}