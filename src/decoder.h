#ifndef RISCV_DECODER_H
#define RISCV_DECODER_H

#include <stdint.h>

/* Catégorie d'instruction — détermine quelle opération l'executor exécute */
typedef enum {
    INSTR_INVALID = 0,

    /* ALU register-register (R-type, opcode 0x33) */
    INSTR_ADD, INSTR_SUB, INSTR_AND, INSTR_OR, INSTR_XOR,
    INSTR_SLL, INSTR_SRL, INSTR_SRA, INSTR_SLT, INSTR_SLTU,

    /* ALU register-immediate (I-type, opcode 0x13) */
    INSTR_ADDI, INSTR_ANDI, INSTR_ORI, INSTR_XORI,
    INSTR_SLLI, INSTR_SRLI, INSTR_SRAI, INSTR_SLTI, INSTR_SLTIU,

    /* Upper immediate (U-type) */
    INSTR_LUI, INSTR_AUIPC,

    /* Loads (I-type, opcode 0x03) */
    INSTR_LB, INSTR_LH, INSTR_LW, INSTR_LBU, INSTR_LHU,

    /* Stores (S-type, opcode 0x23) */
    INSTR_SB, INSTR_SH, INSTR_SW,

    /* Branches (B-type, opcode 0x63) */
    INSTR_BEQ, INSTR_BNE, INSTR_BLT, INSTR_BGE, INSTR_BLTU, INSTR_BGEU,

    /* Jumps */
    INSTR_JAL,   /* J-type, opcode 0x6F */
    INSTR_JALR,  /* I-type, opcode 0x67 */

    /* System (opcode 0x73) */
    INSTR_ECALL, INSTR_EBREAK,

    /* Fence (opcode 0x0F) — NOP en émulateur */
    INSTR_FENCE
} InstrType;

/* Représentation décodée d'une instruction RV32I.
 * Contient TOUS les champs possibles, certains seront ignorés selon le type.
 */
typedef struct {
    InstrType type;
    uint32_t  rd;     /* registre destination (5 bits, 0-31) */
    uint32_t  rs1;    /* registre source 1 */
    uint32_t  rs2;    /* registre source 2 */
    int32_t   imm;    /* immediate déjà sign-extended sur 32 bits */
    uint32_t  raw;    /* mot 32 bits original — utile pour debug */
} Instruction;

/* Décode un mot 32 bits en Instruction.
 * Si l'instruction est invalide (opcode inconnu, encodage illégal),
 * retourne une Instruction avec type = INSTR_INVALID.
 */
Instruction decode(uint32_t raw);

#endif /* RISCV_DECODER_H */
