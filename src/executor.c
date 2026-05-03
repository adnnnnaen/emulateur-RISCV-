#include "executor.h"
#include "memory.h"

#include <stdbool.h>
#include <stdint.h>

bool execute(Cpu *cpu, const Instruction *instr)
{
    uint32_t a = cpu_read_reg(cpu, instr->rs1);
    uint32_t b = cpu_read_reg(cpu, instr->rs2);
    uint32_t result = 0;
    bool pc_modified = false;

    switch (instr->type) {

    /* ===== Group A : ALU register-register ===== */
    case INSTR_ADD:  result = a + b;                                 break;
    case INSTR_SUB:  result = a - b;                                 break;
    case INSTR_AND:  result = a & b;                                 break;
    case INSTR_OR:   result = a | b;                                 break;
    case INSTR_XOR:  result = a ^ b;                                 break;
    case INSTR_SLL:  result = a << (b & 0x1Fu);                      break;
    case INSTR_SRL:  result = a >> (b & 0x1Fu);                      break;
    case INSTR_SRA:  result = (uint32_t)((int32_t)a >> (b & 0x1Fu)); break;
    case INSTR_SLT:  result = ((int32_t)a < (int32_t)b) ? 1u : 0u;   break;
    case INSTR_SLTU: result = (a < b) ? 1u : 0u;                     break;

    /* ===== Group B : ALU register-immediate ===== */
    case INSTR_ADDI:  result = a + (uint32_t)instr->imm;                                   break;
    case INSTR_ANDI:  result = a & (uint32_t)instr->imm;                                   break;
    case INSTR_ORI:   result = a | (uint32_t)instr->imm;                                   break;
    case INSTR_XORI:  result = a ^ (uint32_t)instr->imm;                                   break;
    case INSTR_SLLI:  result = a << ((uint32_t)instr->imm & 0x1Fu);                        break;
    case INSTR_SRLI:  result = a >> ((uint32_t)instr->imm & 0x1Fu);                        break;
    case INSTR_SRAI:  result = (uint32_t)((int32_t)a >> ((uint32_t)instr->imm & 0x1Fu));   break;
    case INSTR_SLTI:  result = ((int32_t)a < instr->imm) ? 1u : 0u;                        break;
    case INSTR_SLTIU: result = (a < (uint32_t)instr->imm) ? 1u : 0u;                       break;

    /* ===== Group C : Upper immediate ===== */
    case INSTR_LUI:   result = (uint32_t)instr->imm;                                       break;
    case INSTR_AUIPC: result = cpu->pc + (uint32_t)instr->imm;                             break;

    /* ===== Group D : Loads (sign-extended) ===== */
    case INSTR_LB: {
        uint32_t addr = a + (uint32_t)instr->imm;
        int32_t v = (int8_t)mem_read8(addr);   /* sign-extend depuis 8 bits */
        result = (uint32_t)v;
        break;
    }
    case INSTR_LH: {
        uint32_t addr = a + (uint32_t)instr->imm;
        int32_t v = (int16_t)mem_read16(addr); /* sign-extend depuis 16 bits */
        result = (uint32_t)v;
        break;
    }
    case INSTR_LW: {
        uint32_t addr = a + (uint32_t)instr->imm;
        result = mem_read32(addr);
        break;
    }
    case INSTR_LBU: {
        uint32_t addr = a + (uint32_t)instr->imm;
        result = (uint32_t)mem_read8(addr);    /* zero-extend */
        break;
    }
    case INSTR_LHU: {
        uint32_t addr = a + (uint32_t)instr->imm;
        result = (uint32_t)mem_read16(addr);   /* zero-extend */
        break;
    }

    /* ===== Group D : Stores (n'écrivent pas rd) ===== */
    case INSTR_SB: {
        uint32_t addr = a + (uint32_t)instr->imm;
        mem_write8(addr, (uint8_t)(b & 0xFFu));
        return false;
    }
    case INSTR_SH: {
        uint32_t addr = a + (uint32_t)instr->imm;
        mem_write16(addr, (uint16_t)(b & 0xFFFFu));
        return false;
    }
    case INSTR_SW: {
        uint32_t addr = a + (uint32_t)instr->imm;
        mem_write32(addr, b);
        return false;
    }

    /* ===== Group E : Branches ===== */
    case INSTR_BEQ: case INSTR_BNE:
    case INSTR_BLT: case INSTR_BGE:
    case INSTR_BLTU: case INSTR_BGEU: {
        bool taken = false;
        switch (instr->type) {
            case INSTR_BEQ:  taken = (a == b); break;
            case INSTR_BNE:  taken = (a != b); break;
            case INSTR_BLT:  taken = ((int32_t)a <  (int32_t)b); break;
            case INSTR_BGE:  taken = ((int32_t)a >= (int32_t)b); break;
            case INSTR_BLTU: taken = (a <  b); break;
            case INSTR_BGEU: taken = (a >= b); break;
            default: break;
        }
        if (taken) {
            cpu->pc = cpu->pc + (uint32_t)instr->imm;
            pc_modified = true;
        }
        return pc_modified;
    }

    /* ===== Group F : Jumps ===== */
    case INSTR_JAL: {
        uint32_t return_addr = cpu->pc + 4u;
        cpu->pc = cpu->pc + (uint32_t)instr->imm;
        cpu_write_reg(cpu, instr->rd, return_addr);
        return true;
    }
    case INSTR_JALR: {
        uint32_t return_addr = cpu->pc + 4u;
        cpu->pc = (a + (uint32_t)instr->imm) & ~1u; /* LSB forcé à 0 */
        cpu_write_reg(cpu, instr->rd, return_addr);
        return true;
    }

    /* ===== Group G : System (Phase 1 stubs, vrais traps en Phase 3) ===== */
    case INSTR_ECALL:
    case INSTR_EBREAK:
        cpu->halted = true;
        return false;

    case INSTR_FENCE:
        /* NOP en émulateur instruction-accurate single-thread */
        return false;

    case INSTR_INVALID:
    default:
        cpu->halted = true;
        return false;
    }

    cpu_write_reg(cpu, instr->rd, result);
    return pc_modified;
}
