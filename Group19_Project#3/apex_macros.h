/*
 * apex_macros.h
 * Contains APEX cpu pipeline macros
 *
 */
#ifndef _MACROS_H_
#define _MACROS_H_

#define FALSE 0x0
#define TRUE 0x1

/* Integers */
#define DATA_MEMORY_SIZE 4096

/* Size of integer register file */
#define REG_FILE_SIZE 19

#define PHYSICAL_REG_FILE_SIZE 25

#define CC_REG_FILE_SIZE 16

#define ISSUE_QUEUE_SIZE 24

#define BRANCH_QUEUE_SIZE 16

#define LSQ_SIZE 16

#define ROB_SIZE 32

/* Size of BTB */
#define BTB_SIZE 4

#define BIS_SIZE 4

/* number of prediction bits*/
#define PREDICTION_BITS 2

/* prediction bit 1 for taken 0 for nottaken*/
#define TAKEN 1
#define NOT_TAKEN 0

#define VALID 1
#define INVALID 0

#define LOAD 1
#define STORE 0

#define NOT_RENAMED -1

#define ZERO_FLAG 0
#define POSITIVE_FLAG 1

/* Numeric OPCODE identifiers for instructions */
#define OPCODE_ADD 0x0
#define OPCODE_SUB 0x1
#define OPCODE_MUL 0x2
#define OPCODE_DIV 0x3
#define OPCODE_AND 0x4
#define OPCODE_OR 0x5
#define OPCODE_XOR 0x6
#define OPCODE_MOVC 0x7
#define OPCODE_LOAD 0x8
#define OPCODE_STORE 0x9
#define OPCODE_BZ 0xa
#define OPCODE_BNZ 0xb
#define OPCODE_HALT 0xc

// Rakesh Added
#define OPCODE_ADDL 0x10
#define OPCODE_SUBL 0x11
#define OPCODE_LOADP 0x12
#define OPCODE_STOREP 0x13
#define OPCODE_CML 0x14
#define OPCODE_CMP 0x15
#define OPCODE_NOP 0x16
#define OPCODE_BP 0x17
#define OPCODE_BNP 0x18
#define OPCODE_JUMP 0xd
#define OPCODE_JALR 0xe

#define FU_

/*Incrementor for LOADP and STOREP*/
#define INCREMENTOR 4

/*Add zero to MOVC*/
#define ADD_ZERO 0

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1

/* Set this flag to 1 to enable cycle single-step mode */
#define ENABLE_SINGLE_STEP 1

#endif
