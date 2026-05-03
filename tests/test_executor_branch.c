#include "unity.h"
#include "cpu.h"
#include "decoder.h"
#include "executor.h"

static Cpu cpu;

void setUp(void) { cpu_reset(&cpu, 0x80000000); }
void tearDown(void) {}

static bool exec_raw(uint32_t raw)
{
    Instruction instr = decode(raw);
    return execute(&cpu, &instr);
}

static void test_beq_taken(void)
{
    cpu_write_reg(&cpu, 5, 42);
    cpu_write_reg(&cpu, 6, 42);
    /* beq x5, x6, +8 = 0x00628463 */
    bool branched = exec_raw(0x00628463);
    TEST_ASSERT_TRUE(branched);
    TEST_ASSERT_EQUAL_HEX32(0x80000008, cpu.pc);
}

static void test_beq_not_taken(void)
{
    cpu_write_reg(&cpu, 5, 42);
    cpu_write_reg(&cpu, 6, 99);
    bool branched = exec_raw(0x00628463);
    TEST_ASSERT_FALSE(branched);
    TEST_ASSERT_EQUAL_HEX32(0x80000000, cpu.pc);
}

static void test_bne_taken(void)
{
    cpu_write_reg(&cpu, 5, 1);
    cpu_write_reg(&cpu, 6, 2);
    /* bne x5, x6, +8 = 0x00629463 */
    TEST_ASSERT_TRUE(exec_raw(0x00629463));
    TEST_ASSERT_EQUAL_HEX32(0x80000008, cpu.pc);
}

static void test_blt_signed_negative(void)
{
    cpu_write_reg(&cpu, 5, (uint32_t)-1);
    cpu_write_reg(&cpu, 6, 0);
    /* blt x5, x6, +8 = 0x0062C463 */
    TEST_ASSERT_TRUE(exec_raw(0x0062C463));
}

static void test_bltu_unsigned(void)
{
    cpu_write_reg(&cpu, 5, (uint32_t)-1);
    cpu_write_reg(&cpu, 6, 0);
    /* bltu x5, x6, +8 = 0x0062E463 */
    TEST_ASSERT_FALSE(exec_raw(0x0062E463));
}

static void test_branch_backward(void)
{
    cpu.pc = 0x80000010;
    cpu_write_reg(&cpu, 5, 1);
    cpu_write_reg(&cpu, 6, 1);
    /* beq x5, x6, -4 = 0xFE628EE3 */
    TEST_ASSERT_TRUE(exec_raw(0xFE628EE3));
    TEST_ASSERT_EQUAL_HEX32(0x8000000C, cpu.pc);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_beq_taken);
    RUN_TEST(test_beq_not_taken);
    RUN_TEST(test_bne_taken);
    RUN_TEST(test_blt_signed_negative);
    RUN_TEST(test_bltu_unsigned);
    RUN_TEST(test_branch_backward);
    return UNITY_END();
}
