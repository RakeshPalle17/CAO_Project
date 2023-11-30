/*
 * apex_cpu.h
 * Contains APEX cpu pipeline declarations
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_

#include "apex_macros.h"

/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
    char opcode_str[128];
    int opcode;
    int rd;
    int rs1;
    int rs2;
    int imm;
} APEX_Instruction;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
    int pc;
    char opcode_str[128];
    int opcode;
    int rs1;
    int rs2;
    int rd;
    int imm;
    int rs1_value;
    int rs2_value;
    int result_buffer;
    int incrementor;
    int memory_address;
    int has_insn;
    int stall;
    int btb_hit;
    int taken;

    int phyrs1;
    int phyrs2;
    int phyrs1_valid;
    int phyrs2_valid;
    int phyrd;
    int phyrs1_value;
    int phyrs2_value;

} CPU_Stage;


typedef struct RegisterStatusIndicator
{
    int isInvalid;
} RegisterStatusIndicator;

typedef struct BranchTargetBuffer
{
    int branch_pc;
    int recent_outcomes[2];
    int target_pc;

} BranchTargetBuffer;

typedef struct PhysicalRegistrationFile
{
    int valid_bit;
    int data_field;
    int delivery_list[64];

} PhysicalRegistrationFile;

typedef struct CCRegistrationFile
{
    int valid_bit;
    int flag_value;

} CCRegistrationFile;

typedef struct RenameTableEntry{
    int physicalReg;
} RenameTableEntry;

typedef struct IssueQueue
{
   int valid_bit;
   int ready_flag;
   char FU_Type[64];
   CPU_Stage instr;
   int dispatch_time;
   
}IssueQueue;

typedef struct ReorderBuffer {
    int established_bit;
    int lsq_index;
    int prev_renametable_entry;
    CPU_Stage instr;
} ReorderBuffer;


/* Model of APEX CPU */
typedef struct APEX_CPU
{
    int pc;                            /* Current program counter */
    int clock;                         /* Clock cycles elapsed */
    int insn_completed;                /* Instructions retired */
    int regs[REG_FILE_SIZE];           /* Integer register file */
    int code_memory_size;              /* Number of instruction in the input file */
    APEX_Instruction *code_memory;     /* Code Memory */
    int data_memory[DATA_MEMORY_SIZE]; /* Data Memory */
    int single_step;                   /* Wait for user input after every cycle */
    int zero_flag;                     /* {TRUE, FALSE} Used by BZ and BNZ to branch */
    int p_flag;
    int n_flag;
    int fetch_from_next_cycle;
    int decode_from_next_cycle;

    RegisterStatusIndicator regStatus[REG_FILE_SIZE];
    int writeToDestination;
    int writeToSource1;
    int writeToSource2;
    int num_of_cycles_to_run;

    BranchTargetBuffer BTBEntry[BTB_SIZE];
    int btb_full; 
    int counter;

    PhysicalRegistrationFile physicalRegFile[PHYSICAL_REG_FILE_SIZE];
    CCRegistrationFile ccRegFile[CC_REG_FILE_SIZE];
    IssueQueue issueQueue[ISSUE_QUEUE_SIZE];
    int issue_counter;

    ReorderBuffer RoB[ROB_SIZE];
    int ROB_head;
    int ROB_tail;

    int freePhysicalRegList[PHYSICAL_REG_FILE_SIZE];
    int freeCCFlagsRegList[CC_REG_FILE_SIZE];
    int renameTable[REG_FILE_SIZE + 1];


    int mulcycle_counter;
    /* Pipeline stages */
    CPU_Stage fetch;
    CPU_Stage decode_rename1;
    CPU_Stage rename2_dispatch;
    CPU_Stage execute_IntFU;
    CPU_Stage execute_AFU;
    CPU_Stage execute_BFU;
    CPU_Stage execute_MulFU;
    CPU_Stage execute_MAU;
    CPU_Stage commit_ARF;
} APEX_CPU;

APEX_Instruction *create_code_memory(const char *filename, int *size);
APEX_CPU *APEX_cpu_init(const char *filename);
void APEX_cpu_run(APEX_CPU *cpu);
void APEX_cpu_stop(APEX_CPU *cpu);
#endif
