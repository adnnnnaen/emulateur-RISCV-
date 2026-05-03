#ifndef RISCV_CPU_H
#define RISCV_CPU_H

#include <stdint.h>
#include <stdbool.h>

/* Nombre de registres généraux — fixé par l'ISA RISC-V */
#define CPU_NUM_REGS 32

/* État complet d'un cœur CPU RV32I */
typedef struct {
    uint32_t regs[CPU_NUM_REGS]; /* x0-x31, x0 toujours 0 */
    uint32_t pc;                  /* program counter */
    bool halted;                  /* flag : ECALL/EBREAK met halted=true */
} Cpu;

/* Initialise un CPU : registres à 0, PC à reset_pc, halted=false */
void cpu_reset(Cpu *cpu, uint32_t reset_pc);

/* Lecture d'un registre. read_reg(0) retourne toujours 0. */
uint32_t cpu_read_reg(const Cpu *cpu, uint32_t index);

/* Écriture d'un registre. write_reg(0, ...) est silencieusement ignorée. */
void cpu_write_reg(Cpu *cpu, uint32_t index, uint32_t value);

/* Exécute un cycle complet : fetch instruction depuis PC, décode, execute,
 * met à jour PC (+4 sauf si branch/jump l'a modifié). Met halted=true si
 * ECALL/EBREAK/INVALID rencontré.
 *
 * Implémenté en Task 12 (après que decoder/executor existent).
 */
void cpu_step(Cpu *cpu);

/* Fait tourner cpu_step() jusqu'à halted=true ou max_steps atteint. */
void cpu_run(Cpu *cpu, uint64_t max_steps);

#endif /* RISCV_CPU_H */
