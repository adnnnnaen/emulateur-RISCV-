#ifndef RISCV_MEMORY_H
#define RISCV_MEMORY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Taille de la RAM : 1 MB. Suffisant pour les tests Phase 1. */
#define MEMORY_SIZE (1024u * 1024u)

/* Adresse de base de la RAM dans l'espace adressable RISC-V */
#define MEMORY_BASE 0x80000000u

/* Initialise la RAM à 0. À appeler avant toute autre fonction. */
void mem_init(void);

/* Charge un buffer en RAM à partir de l'adresse 'base'.
 * Retourne false si le chargement déborde de la RAM. */
bool mem_load(uint32_t base, const uint8_t *buf, size_t len);

/* Lectures little-endian. addr est une adresse RISC-V (typiquement 0x80000000+). */
uint8_t  mem_read8 (uint32_t addr);
uint16_t mem_read16(uint32_t addr);
uint32_t mem_read32(uint32_t addr);

/* Écritures little-endian. */
void mem_write8 (uint32_t addr, uint8_t  value);
void mem_write16(uint32_t addr, uint16_t value);
void mem_write32(uint32_t addr, uint32_t value);

/* True si l'adresse + size octets tombent dans la RAM. Utile pour diag. */
bool mem_in_range(uint32_t addr, uint32_t size);

#endif /* RISCV_MEMORY_H */
