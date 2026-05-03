#include "decoder.h"
#include "utils.h"

/* Helpers d'extraction de champs depuis le mot 32 bits brut */
static inline uint32_t opcode_of(uint32_t raw) { return raw & 0x7Fu; }
static inline uint32_t rd_of    (uint32_t raw) { return (raw >> 7)  & 0x1Fu; }
static inline uint32_t funct3_of(uint32_t raw) { return (raw >> 12) & 0x07u; }
static inline uint32_t rs1_of   (uint32_t raw) { return (raw >> 15) & 0x1Fu; }
static inline uint32_t rs2_of   (uint32_t raw) { return (raw >> 20) & 0x1Fu; }
static inline uint32_t funct7_of(uint32_t raw) { return (raw >> 25) & 0x7Fu; }

/* Immediate I-type : bits [31:20], sign-extended depuis bit 11 */
static inline int32_t imm_i(uint32_t raw)
{
    return sign_extend((raw >> 20) & 0xFFFu, 12);
}

/* Immediate S-type : bits [31:25] = imm[11:5], bits [11:7] = imm[4:0] */
static inline int32_t imm_s(uint32_t raw)
{
    uint32_t imm = (((raw >> 25) & 0x7Fu) << 5)
                 | ((raw >> 7)  & 0x1Fu);
    return sign_extend(imm, 12);
}

/* Immediate B-type : bits éparpillés [31|7|30:25|11:8] = imm[12|11|10:5|4:1], bit 0 toujours 0 */
static inline int32_t imm_b(uint32_t raw)
{
    uint32_t imm = (((raw >> 31) & 0x1u)  << 12)
                 | (((raw >> 7)  & 0x1u)  << 11)
                 | (((raw >> 25) & 0x3Fu) << 5)
                 | (((raw >> 8)  & 0xFu)  << 1);
    return sign_extend(imm, 13);
}

/* Immediate U-type : bits [31:12] forment imm[31:12], les 12 bits bas sont 0 */
static inline int32_t imm_u(uint32_t raw)
{
    return (int32_t)(raw & 0xFFFFF000u);
}

/* Immediate J-type : bits éparpillés [31|19:12|20|30:21] = imm[20|19:12|11|10:1], bit 0 = 0 */
static inline int32_t imm_j(uint32_t raw)
{
    uint32_t imm = (((raw >> 31) & 0x1u)   << 20)
                 | (((raw >> 12) & 0xFFu)  << 12)
                 | (((raw >> 20) & 0x1u)   << 11)
                 | (((raw >> 21) & 0x3FFu) << 1);
    return sign_extend(imm, 21);
}

static InstrType decode_op_imm(uint32_t funct3, uint32_t funct7)
{
    switch (funct3) {
        case 0x0: return INSTR_ADDI;
        case 0x2: return INSTR_SLTI;
        case 0x3: return INSTR_SLTIU;
        case 0x4: return INSTR_XORI;
        case 0x6: return INSTR_ORI;
        case 0x7: return INSTR_ANDI;
        case 0x1: return (funct7 == 0) ? INSTR_SLLI : INSTR_INVALID;
        case 0x5:
            if (funct7 == 0x00) return INSTR_SRLI;
            if (funct7 == 0x20) return INSTR_SRAI;
            return INSTR_INVALID;
        default: return INSTR_INVALID;
    }
}

static InstrType decode_op(uint32_t funct3, uint32_t funct7)
{
    switch (funct3) {
        case 0x0:
            if (funct7 == 0x00) return INSTR_ADD;
            if (funct7 == 0x20) return INSTR_SUB;
            return INSTR_INVALID;
        case 0x1: return (funct7 == 0) ? INSTR_SLL : INSTR_INVALID;
        case 0x2: return (funct7 == 0) ? INSTR_SLT : INSTR_INVALID;
        case 0x3: return (funct7 == 0) ? INSTR_SLTU : INSTR_INVALID;
        case 0x4: return (funct7 == 0) ? INSTR_XOR : INSTR_INVALID;
        case 0x5:
            if (funct7 == 0x00) return INSTR_SRL;
            if (funct7 == 0x20) return INSTR_SRA;
            return INSTR_INVALID;
        case 0x6: return (funct7 == 0) ? INSTR_OR : INSTR_INVALID;
        case 0x7: return (funct7 == 0) ? INSTR_AND : INSTR_INVALID;
        default: return INSTR_INVALID;
    }
}

static InstrType decode_load(uint32_t funct3)
{
    switch (funct3) {
        case 0x0: return INSTR_LB;
        case 0x1: return INSTR_LH;
        case 0x2: return INSTR_LW;
        case 0x4: return INSTR_LBU;
        case 0x5: return INSTR_LHU;
        default:  return INSTR_INVALID;
    }
}

static InstrType decode_store(uint32_t funct3)
{
    switch (funct3) {
        case 0x0: return INSTR_SB;
        case 0x1: return INSTR_SH;
        case 0x2: return INSTR_SW;
        default:  return INSTR_INVALID;
    }
}

static InstrType decode_branch(uint32_t funct3)
{
    switch (funct3) {
        case 0x0: return INSTR_BEQ;
        case 0x1: return INSTR_BNE;
        case 0x4: return INSTR_BLT;
        case 0x5: return INSTR_BGE;
        case 0x6: return INSTR_BLTU;
        case 0x7: return INSTR_BGEU;
        default:  return INSTR_INVALID;
    }
}

static InstrType decode_system(uint32_t raw)
{
    /* Pour Phase 1 : seulement ECALL (imm=0) et EBREAK (imm=1) */
    uint32_t imm12 = (raw >> 20) & 0xFFFu;
    uint32_t funct3 = funct3_of(raw);
    if (funct3 != 0) return INSTR_INVALID;  /* CSR vient en Phase 3 */
    if (imm12 == 0x000) return INSTR_ECALL;
    if (imm12 == 0x001) return INSTR_EBREAK;
    return INSTR_INVALID;
}

Instruction decode(uint32_t raw)
{
    Instruction instr = {0};
    instr.raw = raw;

    uint32_t opcode = opcode_of(raw);
    uint32_t funct3 = funct3_of(raw);
    uint32_t funct7 = funct7_of(raw);

    switch (opcode) {
        case 0x33: /* OP — R-type */
            instr.type = decode_op(funct3, funct7);
            instr.rd  = rd_of(raw);
            instr.rs1 = rs1_of(raw);
            instr.rs2 = rs2_of(raw);
            break;

        case 0x13: /* OP-IMM — I-type */
            instr.type = decode_op_imm(funct3, funct7);
            instr.rd  = rd_of(raw);
            instr.rs1 = rs1_of(raw);
            /* Pour shifts immediate (SLLI/SRLI/SRAI), imm[4:0] = shamt */
            if (funct3 == 1 || funct3 == 5) {
                instr.imm = (int32_t)((raw >> 20) & 0x1Fu);  /* shamt */
            } else {
                instr.imm = imm_i(raw);
            }
            break;

        case 0x03: /* LOAD — I-type */
            instr.type = decode_load(funct3);
            instr.rd  = rd_of(raw);
            instr.rs1 = rs1_of(raw);
            instr.imm = imm_i(raw);
            break;

        case 0x23: /* STORE — S-type */
            instr.type = decode_store(funct3);
            instr.rs1 = rs1_of(raw);
            instr.rs2 = rs2_of(raw);
            instr.imm = imm_s(raw);
            break;

        case 0x63: /* BRANCH — B-type */
            instr.type = decode_branch(funct3);
            instr.rs1 = rs1_of(raw);
            instr.rs2 = rs2_of(raw);
            instr.imm = imm_b(raw);
            break;

        case 0x37: /* LUI — U-type */
            instr.type = INSTR_LUI;
            instr.rd  = rd_of(raw);
            instr.imm = imm_u(raw);
            break;

        case 0x17: /* AUIPC — U-type */
            instr.type = INSTR_AUIPC;
            instr.rd  = rd_of(raw);
            instr.imm = imm_u(raw);
            break;

        case 0x6F: /* JAL — J-type */
            instr.type = INSTR_JAL;
            instr.rd  = rd_of(raw);
            instr.imm = imm_j(raw);
            break;

        case 0x67: /* JALR — I-type */
            instr.type = (funct3 == 0) ? INSTR_JALR : INSTR_INVALID;
            instr.rd  = rd_of(raw);
            instr.rs1 = rs1_of(raw);
            instr.imm = imm_i(raw);
            break;

        case 0x73: /* SYSTEM */
            instr.type = decode_system(raw);
            break;

        case 0x0F: /* MISC-MEM (FENCE) */
            instr.type = INSTR_FENCE;
            break;

        default:
            instr.type = INSTR_INVALID;
            break;
    }

    return instr;
}
