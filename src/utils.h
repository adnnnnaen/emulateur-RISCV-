#ifndef RISCV_UTILS_H
#define RISCV_UTILS_H

#include <stdint.h>

/* Étend le signe d'une valeur de num_bits bits vers 32 bits.
 * Exemple: sign_extend(0xFFF, 12) = 0xFFFFFFFF (la valeur signée -1).
 * Exemple: sign_extend(0x7FF, 12) = 0x000007FF (la valeur signée +2047).
 */
static inline int32_t sign_extend(uint32_t value, uint32_t num_bits)
{
    uint32_t shift = 32u - num_bits;
    return ((int32_t)(value << shift)) >> shift;
}

#endif /* RISCV_UTILS_H */
