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
    int phyrs3;

    int phyrs1_valid;
    int phyrs2_valid;
    int phyrs3_valid;

    int phyrd;
    int phyrs1_value;
    int phyrs2_value;
    int phyrs3_value;

    int flag;
    int target_address;
    int target_address_valid;

} CPU_Stage;

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
} PhysicalRegistrationFile;

typedef struct Flags
{
   int zero;
   int positive;
}Flags;

typedef struct CCRegistrationFile
{
    int valid_bit;
    Flags flag;
} CCRegistrationFile;

typedef struct RenameTableEntry
{
    int physicalReg;
} RenameTableEntry;

typedef struct IssueQueue
{
    int valid_bit;
    CPU_Stage instr;
    int dispatch_time;
    int branch_tag;
} IssueQueue;

typedef struct BranchQueue
{
    int valid_bit;
    int issuedToAFU;
    int issuedToBFU;
    int prediction;
    int target_address;
    int cc_tag;
    int cc_content;
    CPU_Stage instr;
    int dispatch_time;
} BranchQueue;

typedef struct LoadStoreQueue
{
    int established_bit;
    int LorS_bit;
    int memory_address;
    int memory_address_valid;
    int phyrd;
    int src_data_valid;
    int src_tag;
    int valueToStore;
    int branch_tag;
    CPU_Stage instr;
} LoadStoreQueue;

typedef struct BranchInstructionStack
{
    int established_bit;
    int rob_index;
    int branch_tag;

} BranchInstructionStack;

typedef struct ReorderBuffer
{
    int established_bit;
    int lsq_index;
    int branch_tag;
    int prev_renametable_entry;
    CPU_Stage instr;
} ReorderBuffer;

typedef struct ArchitecturalRegisterFiles
{
    int value;
    Flags flags;
}ArchitecturalRegisterFiles;

/* Model of APEX CPU */
typedef struct APEX_CPU
{
    int pc;                            /* Current program counter */
    int clock;                         /* Clock cycles elapsed */
    int insn_completed;                /* Instructions retired */
    ArchitecturalRegisterFiles regs[REG_FILE_SIZE];/* Integer register file */
    int code_memory_size;              /* Number of instruction in the input file */
    APEX_Instruction *code_memory;     /* Code Memory */
    int data_memory[DATA_MEMORY_SIZE]; /* Data Memory */
    int single_step;                   /* Wait for user input after every cycle */
    int zero_flag;                     /* {TRUE, FALSE} Used by BZ and BNZ to branch */
    int p_flag;
    int fetch_from_next_cycle;

    PhysicalRegistrationFile physicalRegFile[PHYSICAL_REG_FILE_SIZE];
    int freePhysicalRegList[PHYSICAL_REG_FILE_SIZE];

    CCRegistrationFile ccRegFile[CC_REG_FILE_SIZE];
    int freeCCFlagsRegList[CC_REG_FILE_SIZE];

    int renameTable[REG_FILE_SIZE];

    IssueQueue issueQueue[ISSUE_QUEUE_SIZE];
    int issue_counter;

    BranchQueue branchQueue[BRANCH_QUEUE_SIZE];
    int branch_counter;

    BranchInstructionStack BIStack[BIS_SIZE];
    int BIS_head;
    int BIS_tail;
    int BIS_size;
    int delete_BIS_head;
    int branch_tag;
    int miss_branch_tag;

    int intFU_frwded_tag;
    int intFU_frwded_value;

    int intFu_frwded_ccTag;
    int intFu_frwded_ccValue;

    int MulFU_frwded_tag;
    int MulFU_frwded_value;

    int AFU_frwded_tag;
    int AFU_frwded_value;

    int AFU_frwded_address;
    int AFU_frwded_pc;

    int MAU_frwded_tag;
    int MAU_frwded_value;
    
    int BFU_frwded_pc;
    int BFU_frwded_tag;
    int BFU_frwded_value;

    LoadStoreQueue lsq[LSQ_SIZE];
    int LSQ_head;
    int LSQ_tail;
    int LSQ_size;
    int delete_LSQ_head;

    ReorderBuffer RoB[ROB_SIZE];
    int ROB_head;
    int ROB_tail;
    int ROB_size;
    int delete_ROB_head;

    BranchTargetBuffer BTBEntry[BTB_SIZE];
    int btb_full;
    int counter;


    int mulcycle_counter;
    int mau_cycle_latency;
    int stored_in_memory;
    int num_of_cycles_to_run;
    int compare_value;
    int HALT;

    /* Pipeline stages */
    CPU_Stage fetch;
    CPU_Stage decode_rename1;
    CPU_Stage rename2_dispatch;
    CPU_Stage issue_queue;
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
