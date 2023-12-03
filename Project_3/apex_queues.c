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
        if (!cpu->issueQueue[i].valid_bit)
        {
            cpu->issueQueue[i].valid_bit = 1;
            cpu->issueQueue[i].dispatch_time = cpu->issue_counter++;
            cpu->issueQueue[i].instr = cpu->issue_queue;
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
