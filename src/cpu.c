#include "cpu.h"
#include "memory.h"
#include "decoder.h"
#include "executor.h"

#include <string.h> /* pour memset */

void cpu_reset(Cpu *cpu, uint32_t reset_pc)
{
    memset(cpu->regs, 0, sizeof(cpu->regs));
    cpu->pc = reset_pc;
    cpu->halted = false;
}

uint32_t cpu_read_reg(const Cpu *cpu, uint32_t index)
{
    /* x0 est câblé à zéro */
    if (index == 0) {
        return 0;
    }
    return cpu->regs[index];
}

void cpu_write_reg(Cpu *cpu, uint32_t index, uint32_t value)
{
    /* Écriture vers x0 silencieusement ignorée */
    if (index == 0) {
        return;
    }
    cpu->regs[index] = value;
}

void cpu_step(Cpu *cpu)
{
    if (cpu->halted) return;

    uint32_t raw = mem_read32(cpu->pc);
    Instruction instr = decode(raw);
    bool pc_modified = execute(cpu, &instr);

    if (!pc_modified && !cpu->halted) {
        cpu->pc += 4u;
    }
}

void cpu_run(Cpu *cpu, uint64_t max_steps)
{
    for (uint64_t i = 0; i < max_steps && !cpu->halted; i++) {
        cpu_step(cpu);
    }
}
