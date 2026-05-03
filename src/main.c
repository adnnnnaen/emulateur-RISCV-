#include "cpu.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_MAX_STEPS 100000000ULL  /* safeguard contre boucle infinie */

static int load_bin_file(const char *path, uint32_t base_addr)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "error: cannot open '%s'\n", path);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0) {
        fprintf(stderr, "error: empty or invalid file\n");
        fclose(f);
        return -1;
    }

    uint8_t *buf = malloc((size_t)size);
    if (!buf) {
        fprintf(stderr, "error: malloc failed\n");
        fclose(f);
        return -1;
    }

    size_t nread = fread(buf, 1, (size_t)size, f);
    fclose(f);

    if (nread != (size_t)size) {
        fprintf(stderr, "error: short read\n");
        free(buf);
        return -1;
    }

    bool ok = mem_load(base_addr, buf, (size_t)size);
    free(buf);

    if (!ok) {
        fprintf(stderr, "error: program does not fit in RAM\n");
        return -1;
    }

    fprintf(stderr, "loaded %ld bytes at 0x%08X\n", size, base_addr);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <program.bin>\n", argv[0]);
        return 1;
    }

    mem_init();

    if (load_bin_file(argv[1], MEMORY_BASE) != 0) {
        return 1;
    }

    Cpu cpu;
    cpu_reset(&cpu, MEMORY_BASE);

    cpu_run(&cpu, DEFAULT_MAX_STEPS);

    /* Code de retour conventionnel riscv-tests : a0 == 0 → succès */
    uint32_t a0 = cpu_read_reg(&cpu, 10);

    if (cpu.halted) {
        fprintf(stderr, "halted: PC=0x%08X a0=0x%08X (%u)\n", cpu.pc, a0, a0);
    } else {
        fprintf(stderr, "max steps reached, suspected infinite loop\n");
        return 2;
    }

    return (a0 == 0) ? 0 : 1;
}
