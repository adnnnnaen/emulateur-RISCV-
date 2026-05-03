#include "unity.h"
#include "cpu.h"

static Cpu cpu;

void setUp(void)
{
    cpu_reset(&cpu, 0x80000000);
}

void tearDown(void) {}

static void test_reset_zeroes_all_regs(void)
{
    for (uint32_t i = 0; i < CPU_NUM_REGS; i++) {
        TEST_ASSERT_EQUAL_HEX32(0, cpu_read_reg(&cpu, i));
    }
}

static void test_reset_sets_pc(void)
{
    TEST_ASSERT_EQUAL_HEX32(0x80000000, cpu.pc);
}

static void test_reset_clears_halted(void)
{
    cpu.halted = true;
    cpu_reset(&cpu, 0);
    TEST_ASSERT_FALSE(cpu.halted);
}

static void test_x0_always_reads_zero(void)
{
    cpu.regs[0] = 0xDEADBEEF; /* tentative d'écriture directe */
    TEST_ASSERT_EQUAL_HEX32(0, cpu_read_reg(&cpu, 0));
}

static void test_write_x0_is_ignored(void)
{
    cpu_write_reg(&cpu, 0, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0, cpu_read_reg(&cpu, 0));
    TEST_ASSERT_EQUAL_HEX32(0, cpu.regs[0]);
}

static void test_write_read_normal_reg(void)
{
    cpu_write_reg(&cpu, 5, 0xCAFEBABE);
    TEST_ASSERT_EQUAL_HEX32(0xCAFEBABE, cpu_read_reg(&cpu, 5));
}

static void test_all_regs_independent(void)
{
    /* Écrire valeur unique dans chaque registre */
    for (uint32_t i = 1; i < CPU_NUM_REGS; i++) {
        cpu_write_reg(&cpu, i, i * 0x100u);
    }
    /* Vérifier que rien n'a été écrasé */
    for (uint32_t i = 1; i < CPU_NUM_REGS; i++) {
        TEST_ASSERT_EQUAL_HEX32(i * 0x100u, cpu_read_reg(&cpu, i));
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_reset_zeroes_all_regs);
    RUN_TEST(test_reset_sets_pc);
    RUN_TEST(test_reset_clears_halted);
    RUN_TEST(test_x0_always_reads_zero);
    RUN_TEST(test_write_x0_is_ignored);
    RUN_TEST(test_write_read_normal_reg);
    RUN_TEST(test_all_regs_independent);
    return UNITY_END();
}
