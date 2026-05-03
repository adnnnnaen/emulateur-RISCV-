#ifndef RISCV_EXECUTOR_H
#define RISCV_EXECUTOR_H

#include "cpu.h"
#include "decoder.h"

/* Exécute une instruction décodée sur l'état CPU.
 * Met à jour les registres et le PC. Si l'instruction est invalide
 * ou ECALL/EBREAK, met cpu->halted = true.
 *
 * Convention PC : execute() laisse PC inchangé. C'est l'orchestrateur
 * (cpu_step) qui fait pc += 4, sauf si execute() retourne true pour
 * signaler que PC a déjà été modifié par un branch/jump.
 *
 * Retourne true si le PC a été modifié par l'instruction (branch/jump),
 * false sinon (PC inchangé, à incrémenter de 4 par le caller).
 */
bool execute(Cpu *cpu, const Instruction *instr);

#endif /* RISCV_EXECUTOR_H */
