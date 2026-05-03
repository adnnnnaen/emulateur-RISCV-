#include "memory.h"
#include <string.h>
#include <assert.h>

/* RAM allouée statiquement — pas de malloc dans le hot path */
static uint8_t ram[MEMORY_SIZE];

void mem_init(void)
{
    memset(ram, 0, sizeof(ram));
}

bool mem_in_range(uint32_t addr, uint32_t size)
{
    if (addr < MEMORY_BASE) return false;
    uint32_t offset = addr - MEMORY_BASE;
    return (offset + size) <= MEMORY_SIZE;
}

bool mem_load(uint32_t base, const uint8_t *buf, size_t len)
{
    if (!mem_in_range(base, (uint32_t)len)) return false;
    memcpy(&ram[base - MEMORY_BASE], buf, len);
    return true;
}

uint8_t mem_read8(uint32_t addr)
{
    assert(mem_in_range(addr, 1));
    return ram[addr - MEMORY_BASE];
}

uint16_t mem_read16(uint32_t addr)
{
    assert(mem_in_range(addr, 2));
    uint32_t off = addr - MEMORY_BASE;
    return (uint16_t)((uint16_t)ram[off]
                    | ((uint16_t)ram[off + 1] << 8));
}

uint32_t mem_read32(uint32_t addr)
{
    assert(mem_in_range(addr, 4));
    uint32_t off = addr - MEMORY_BASE;
    return (uint32_t)ram[off]
         | ((uint32_t)ram[off + 1] << 8)
         | ((uint32_t)ram[off + 2] << 16)
         | ((uint32_t)ram[off + 3] << 24);
}

void mem_write8(uint32_t addr, uint8_t value)
{
    assert(mem_in_range(addr, 1));
    ram[addr - MEMORY_BASE] = value;
}

void mem_write16(uint32_t addr, uint16_t value)
{
    assert(mem_in_range(addr, 2));
    uint32_t off = addr - MEMORY_BASE;
    ram[off]     = (uint8_t)(value & 0xFFu);
    ram[off + 1] = (uint8_t)((value >> 8) & 0xFFu);
}

void mem_write32(uint32_t addr, uint32_t value)
{
    assert(mem_in_range(addr, 4));
    uint32_t off = addr - MEMORY_BASE;
    ram[off]     = (uint8_t)(value & 0xFFu);
    ram[off + 1] = (uint8_t)((value >> 8) & 0xFFu);
    ram[off + 2] = (uint8_t)((value >> 16) & 0xFFu);
    ram[off + 3] = (uint8_t)((value >> 24) & 0xFFu);
}
