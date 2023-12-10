#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"

static void
initializeIssueQueue(APEX_CPU *cpu)
{
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        cpu->issueQueue[i].valid_bit = ADD_ZERO;
        cpu->issueQueue[i].dispatch_time = ADD_ZERO;
    }
}

static void
initializeBranchQueue(APEX_CPU *cpu)
{
    for (int i = 0; i < BRANCH_QUEUE_SIZE; ++i)
    {
        cpu->issueQueue[i].valid_bit = ADD_ZERO;
    }
}

static void
initializeLSQ(APEX_CPU *cpu)
{
    for (int i = 0; i < LSQ_SIZE; ++i)
    {
        cpu->lsq[i].established_bit = ADD_ZERO;
        cpu->lsq[i].LorS_bit = -1;
    }
    cpu->LSQ_head = ADD_ZERO;
    cpu->LSQ_tail = LSQ_SIZE - 1;
    cpu->LSQ_size = ADD_ZERO;
}

static void
initializeBIS(APEX_CPU *cpu)
{
    for (int i = 0; i < BIS_SIZE; ++i)
    {
        cpu->BIStack[i].established_bit = ADD_ZERO;
        cpu->BIStack[i].branch_tag = ADD_ZERO;
        cpu->BIStack[i].rob_index = ADD_ZERO;
    }
    cpu->BIS_head = ADD_ZERO;
    cpu->BIS_tail = BIS_SIZE - 1;
    cpu->BIS_size = ADD_ZERO;
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
    cpu->ROB_size = ADD_ZERO;
    cpu->RoB[cpu->ROB_head].prev_renametable_entry = -1;
}

static int
is_IssueQueue_Full(APEX_CPU *cpu)
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
is_BranchQueue_Full(APEX_CPU *cpu)
{
    for (int i = 0; i < BRANCH_QUEUE_SIZE; ++i)
    {
        if (!cpu->issueQueue[i].valid_bit)
        {
            return FALSE;
        }
    }
    return TRUE;
}

static int
is_ROB_Full(APEX_CPU *cpu)
{
    return cpu->ROB_size == ROB_SIZE;
}

static int
is_LSQ_Full(APEX_CPU *cpu)
{
    return cpu->LSQ_size == LSQ_SIZE;
}

static void
establish_IssueQueueEntry(APEX_CPU *cpu)
{
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        if (!cpu->issueQueue[i].valid_bit && cpu->issueQueue[i].instr.pc != cpu->issue_queue.pc)
        {
            cpu->issueQueue[i].valid_bit = 1;
            cpu->issueQueue[i].dispatch_time = cpu->issue_counter++;
            cpu->issueQueue[i].instr = cpu->issue_queue;
            break;
        }
    }
}

static void
establish_BranchQueueEntry(APEX_CPU *cpu)
{
    for (int i = 0; i < BRANCH_QUEUE_SIZE; ++i)
    {
        if (!cpu->branchQueue[i].valid_bit && cpu->branchQueue[i].instr.pc != cpu->issue_queue.pc)
        {
            cpu->branchQueue[i].valid_bit = 1;
            cpu->branchQueue[i].dispatch_time = cpu->branch_counter++;
            cpu->branchQueue[i].instr = cpu->issue_queue;
            cpu->branchQueue[i].issuedToBFU = ADD_ZERO;
            cpu->branchQueue[i].issuedToAFU = ADD_ZERO;
            break;
        }
    }
}

static void
establish_ROBEntry(APEX_CPU *cpu, int loadorstore)
{
    if (loadorstore)
    {
        cpu->ROB_tail = (cpu->ROB_tail + 1) % ROB_SIZE;
        cpu->RoB[cpu->ROB_tail].established_bit = VALID;
        cpu->RoB[cpu->ROB_tail].instr = cpu->issue_queue;
        cpu->RoB[cpu->ROB_tail].lsq_index = cpu->LSQ_tail;
        cpu->ROB_size++;
    }
    else
    {
        cpu->ROB_tail = (cpu->ROB_tail + 1) % ROB_SIZE;
        cpu->RoB[cpu->ROB_tail].established_bit = VALID;
        cpu->RoB[cpu->ROB_tail].instr = cpu->issue_queue;
        cpu->ROB_size++;
    }
}

static void
establish_LSQEntry(APEX_CPU *cpu)
{
    switch (cpu->issue_queue.opcode)
    {
    case OPCODE_LOADP:
    case OPCODE_LOAD:
    {
        cpu->LSQ_tail = (cpu->LSQ_tail + 1) % LSQ_SIZE;
        cpu->lsq[cpu->LSQ_tail].established_bit = VALID;
        cpu->lsq[cpu->LSQ_tail].LorS_bit = LOAD;
        cpu->lsq[cpu->LSQ_tail].memory_address_valid = INVALID;
        cpu->lsq[cpu->LSQ_tail].memory_address = INVALID;
        cpu->lsq[cpu->LSQ_tail].src_data_valid = VALID;
        cpu->lsq[cpu->LSQ_tail].phyrd = cpu->issue_queue.phyrd;
        cpu->lsq[cpu->LSQ_tail].instr = cpu->issue_queue;
        cpu->LSQ_size++;
        break;
    }
    case OPCODE_STORE:
    case OPCODE_STOREP:
    {
        cpu->LSQ_tail = (cpu->LSQ_tail + 1) % LSQ_SIZE;
        cpu->lsq[cpu->LSQ_tail].established_bit = VALID;
        cpu->lsq[cpu->LSQ_tail].LorS_bit = STORE;
        cpu->lsq[cpu->LSQ_tail].memory_address_valid = INVALID;
        cpu->lsq[cpu->LSQ_tail].memory_address = INVALID;

        if (cpu->issue_queue.phyrs1_valid)
        {
            cpu->lsq[cpu->LSQ_tail].src_data_valid = VALID;
            cpu->lsq[cpu->LSQ_tail].valueToStore = cpu->issue_queue.phyrs1_value;
        }
        else
        {
            cpu->lsq[cpu->LSQ_tail].src_data_valid = INVALID;
        }

        cpu->lsq[cpu->LSQ_tail].src_tag = cpu->issue_queue.phyrs1;
        cpu->lsq[cpu->LSQ_tail].instr = cpu->issue_queue;
        cpu->LSQ_size++;
        break;
    }
    }
}

static void
addBackToFreePhysicalList(APEX_CPU *cpu, int phyRegister)
{
    for (int i = 0; i < PHYSICAL_REG_FILE_SIZE; ++i)
    {
        if (cpu->freePhysicalRegList[i] == -1)
        {
            cpu->freePhysicalRegList[i] = phyRegister;
            break;
        }
    }
}

static void
removeBranchQueueEntry(APEX_CPU *cpu, int branchPc)
{
    for (int i = 0; i < BRANCH_QUEUE_SIZE; ++i)
    {
        if (branchPc == cpu->branchQueue[i].instr.pc)
        {
            cpu->branchQueue[i].valid_bit = ADD_ZERO;
            cpu->branchQueue[i].dispatch_time = ADD_ZERO;
            cpu->branchQueue[i].instr.pc = ADD_ZERO;
            break;
        }
    }
}

static void
removeIssueQueueEntry(APEX_CPU *cpu, int instrPc)
{
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i)
    {
        if (instrPc == cpu->issueQueue[i].instr.pc)
        {
            cpu->issueQueue[i].valid_bit = ADD_ZERO;
            cpu->issueQueue[i].dispatch_time = ADD_ZERO;
            cpu->issueQueue[i].instr.pc = ADD_ZERO;
            break;
        }
    }
}

static void
removeLSQTail(APEX_CPU *cpu)
{
    cpu->lsq[cpu->RoB[cpu->ROB_tail].lsq_index].established_bit = INVALID;
    cpu->lsq[cpu->RoB[cpu->ROB_tail].lsq_index].memory_address_valid = INVALID;
    cpu->LSQ_size--;
    cpu->LSQ_tail = (cpu->LSQ_tail - 1) % LSQ_SIZE;
}

static void
removeROBTail(APEX_CPU *cpu)
{
    while (TRUE)
    {
        if (cpu->ROB_size != 0 && cpu->miss_branch_tag != cpu->RoB[cpu->ROB_tail].instr.pc && cpu->RoB[cpu->ROB_tail].established_bit)
        {
            switch (cpu->RoB[cpu->ROB_tail].instr.opcode)
            {
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_MUL:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_XOR:
            case OPCODE_ADDL:
            case OPCODE_SUBL:
            {
                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;

                addBackToFreePhysicalList(cpu, cpu->RoB[cpu->ROB_tail].instr.phyrd);
                addBackToFreeCCList(cpu, cpu->RoB[cpu->ROB_tail].instr.prev_cc);
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);

                cpu->renameTable[cpu->RoB[cpu->ROB_tail].instr.rd] = cpu->RoB[cpu->ROB_tail].instr.prev_pd;
                cpu->renameTable[REG_FILE_SIZE - 1] = cpu->RoB[cpu->ROB_tail].instr.prev_cc;

                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }

            case OPCODE_MOVC:
            {
                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;

                addBackToFreePhysicalList(cpu, cpu->RoB[cpu->ROB_tail].instr.phyrd);
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);

                cpu->renameTable[cpu->RoB[cpu->ROB_tail].instr.rd] = cpu->RoB[cpu->ROB_tail].instr.prev_pd;

                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }
            case OPCODE_LOAD:
            {

                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;

                addBackToFreePhysicalList(cpu, cpu->RoB[cpu->ROB_tail].instr.phyrd);
                removeLSQTail(cpu);
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);

                cpu->renameTable[cpu->RoB[cpu->ROB_tail].instr.rd] = cpu->RoB[cpu->ROB_tail].instr.prev_pd;

                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }

            case OPCODE_LOADP:
            {
                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;

                addBackToFreePhysicalList(cpu, cpu->RoB[cpu->ROB_tail].instr.phyrd);
                addBackToFreePhysicalList(cpu, cpu->RoB[cpu->ROB_tail].instr.phyrs3);
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);
                removeLSQTail(cpu);

                cpu->renameTable[cpu->RoB[cpu->ROB_tail].instr.rd] = cpu->RoB[cpu->ROB_tail].instr.prev_pd;
                cpu->renameTable[cpu->RoB[cpu->ROB_tail].instr.rs1] = cpu->RoB[cpu->ROB_tail].instr.prev_prs;

                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }
            case OPCODE_STOREP:
            {
                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;

                addBackToFreePhysicalList(cpu, cpu->commit_ARF.phyrs3);
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);
                removeLSQTail(cpu);
                cpu->renameTable[cpu->RoB[cpu->ROB_tail].instr.rs2] = cpu->RoB[cpu->ROB_tail].instr.prev_prs;

                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }
            case OPCODE_STORE:
            {
                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);
                removeLSQTail(cpu);
                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }
            case OPCODE_CML:
            case OPCODE_CMP:
            {
                addBackToFreeCCList(cpu, cpu->RoB[cpu->ROB_tail].instr.prev_cc);
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);
                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;
                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }

            case OPCODE_NOP:
            case OPCODE_HALT:
            {
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);
                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;
                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }

            case OPCODE_BNZ:
            case OPCODE_BP:
            case OPCODE_BZ:
            case OPCODE_BNP:
            case OPCODE_JUMP:
            {
                removeBranchQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);

                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;
                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }

            case OPCODE_JALR:
            {
                cpu->RoB[cpu->ROB_tail].established_bit = INVALID;

                removeBranchQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);
                removeIssueQueueEntry(cpu, cpu->RoB[cpu->ROB_tail].instr.pc);
                addBackToFreePhysicalList(cpu, cpu->RoB[cpu->ROB_tail].instr.phyrd);

                cpu->renameTable[cpu->RoB[cpu->ROB_tail].instr.rd] = cpu->RoB[cpu->ROB_tail].instr.prev_pd;

                cpu->ROB_size--;
                cpu->ROB_tail = (cpu->ROB_tail - 1) % ROB_SIZE;
                break;
            }
            }
        }
        else
        {
            break;
        }
    }
}