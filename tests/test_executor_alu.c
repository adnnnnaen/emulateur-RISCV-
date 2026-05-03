#include "unity.h"
#include "cpu.h"
#include "decoder.h"
#include "executor.h"

static Cpu cpu;

void setUp(void)
{
    cpu_reset(&cpu, 0x80000000);
}

void tearDown(void) {}

static void exec_raw(uint32_t raw)
{
    Instruction instr = decode(raw);
    execute(&cpu, &instr);
}

/* ===== ADD ===== */

static void test_add_simple(void)
{
    cpu_write_reg(&cpu, 11, 5);
    cpu_write_reg(&cpu, 12, 7);
    /* add x10, x11, x12 = 0x00C58533 */
    exec_raw(0x00C58533);
    TEST_ASSERT_EQUAL_UINT32(12, cpu_read_reg(&cpu, 10));
}

static void test_add_overflow_wraps(void)
{
    cpu_write_reg(&cpu, 11, 0xFFFFFFFF);
    cpu_write_reg(&cpu, 12, 1);
    exec_raw(0x00C58533);
    TEST_ASSERT_EQUAL_HEX32(0, cpu_read_reg(&cpu, 10));
}

/* ===== SUB ===== */

static void test_sub_simple(void)
{
    cpu_write_reg(&cpu, 6, 10);
    cpu_write_reg(&cpu, 7, 3);
    exec_raw(0x407302B3);
    TEST_ASSERT_EQUAL_UINT32(7, cpu_read_reg(&cpu, 5));
}

static void test_sub_underflow(void)
{
    cpu_write_reg(&cpu, 6, 0);
    cpu_write_reg(&cpu, 7, 1);
    exec_raw(0x407302B3);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, cpu_read_reg(&cpu, 5));
}

/* ===== Logical ===== */

static void test_and(void)
{
    cpu_write_reg(&cpu, 11, 0xFF00FF00);
    cpu_write_reg(&cpu, 12, 0x00FFFF00);
    exec_raw(0x00C5F533);
    TEST_ASSERT_EQUAL_HEX32(0x0000FF00, cpu_read_reg(&cpu, 10));
}

static void test_or(void)
{
    cpu_write_reg(&cpu, 11, 0xFF000000);
    cpu_write_reg(&cpu, 12, 0x000000FF);
    exec_raw(0x00C5E533);
    TEST_ASSERT_EQUAL_HEX32(0xFF0000FF, cpu_read_reg(&cpu, 10));
}

static void test_xor(void)
{
    cpu_write_reg(&cpu, 11, 0xAAAAAAAA);
    cpu_write_reg(&cpu, 12, 0x55555555);
    exec_raw(0x00C5C533);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, cpu_read_reg(&cpu, 10));
}

/* ===== Shifts ===== */

static void test_sll(void)
{
    cpu_write_reg(&cpu, 11, 0x00000001);
    cpu_write_reg(&cpu, 12, 4);
    exec_raw(0x00C59533);
    TEST_ASSERT_EQUAL_HEX32(0x10, cpu_read_reg(&cpu, 10));
}

static void test_sll_only_low_5_bits(void)
{
    /* Shift de 33 doit être interprété comme shift de 1 (33 & 0x1F = 1) */
    cpu_write_reg(&cpu, 11, 1);
    cpu_write_reg(&cpu, 12, 33);
    exec_raw(0x00C59533);
    TEST_ASSERT_EQUAL_HEX32(2, cpu_read_reg(&cpu, 10));
}

static void test_srl_logical(void)
{
    cpu_write_reg(&cpu, 11, 0x80000000);
    cpu_write_reg(&cpu, 12, 1);
    exec_raw(0x00C5D533);
    TEST_ASSERT_EQUAL_HEX32(0x40000000, cpu_read_reg(&cpu, 10));
}

static void test_sra_arithmetic(void)
{
    cpu_write_reg(&cpu, 11, 0x80000000);
    cpu_write_reg(&cpu, 12, 1);
    exec_raw(0x40C5D533);
    TEST_ASSERT_EQUAL_HEX32(0xC0000000, cpu_read_reg(&cpu, 10));
}

/* ===== Set Less Than ===== */

static void test_slt_signed(void)
{
    cpu_write_reg(&cpu, 11, (uint32_t)-1);
    cpu_write_reg(&cpu, 12, 1);
    exec_raw(0x00C5A533);
    TEST_ASSERT_EQUAL_UINT32(1, cpu_read_reg(&cpu, 10));
}

static void test_sltu_unsigned(void)
{
    cpu_write_reg(&cpu, 11, (uint32_t)-1);
    cpu_write_reg(&cpu, 12, 1);
    exec_raw(0x00C5B533);
    TEST_ASSERT_EQUAL_UINT32(0, cpu_read_reg(&cpu, 10));
}

/* ===== Group B : Immediate ===== */

static void test_addi_positive(void)
{
    cpu_write_reg(&cpu, 11, 100);
    /* addi x10, x11, 50 = 0x03258513 */
    exec_raw(0x03258513);
    TEST_ASSERT_EQUAL_UINT32(150, cpu_read_reg(&cpu, 10));
}

static void test_addi_negative(void)
{
    cpu_write_reg(&cpu, 11, 100);
    /* addi x10, x11, -50 = 0xFCE58513 */
    exec_raw(0xFCE58513);
    TEST_ASSERT_EQUAL_UINT32(50, cpu_read_reg(&cpu, 10));
}

static void test_andi(void)
{
    cpu_write_reg(&cpu, 11, 0xFFFFFFFF);
    /* andi x10, x11, 0xFF = 0x0FF5F513 */
    exec_raw(0x0FF5F513);
    TEST_ASSERT_EQUAL_HEX32(0xFF, cpu_read_reg(&cpu, 10));
}

static void test_slli(void)
{
    cpu_write_reg(&cpu, 11, 1);
    /* slli x10, x11, 4 = 0x00459513 */
    exec_raw(0x00459513);
    TEST_ASSERT_EQUAL_HEX32(0x10, cpu_read_reg(&cpu, 10));
}

static void test_srai(void)
{
    cpu_write_reg(&cpu, 11, 0x80000000);
    /* srai x10, x11, 1 = 0x4015D513 */
    exec_raw(0x4015D513);
    TEST_ASSERT_EQUAL_HEX32(0xC0000000, cpu_read_reg(&cpu, 10));
}

/* ===== Group C : Upper immediate ===== */

static void test_lui(void)
{
    /* lui x10, 0x12345 = 0x12345537 */
    exec_raw(0x12345537);
    TEST_ASSERT_EQUAL_HEX32(0x12345000, cpu_read_reg(&cpu, 10));
}

static void test_auipc(void)
{
    cpu.pc = 0x80001000;
    /* auipc x10, 0x1 = 0x00001517 */
    exec_raw(0x00001517);
    TEST_ASSERT_EQUAL_HEX32(0x80002000, cpu_read_reg(&cpu, 10));
}

/* ===== Group G : System ===== */

static void test_ecall_halts(void)
{
    exec_raw(0x00000073);
    TEST_ASSERT_TRUE(cpu.halted);
}

static void test_ebreak_halts(void)
{
    exec_raw(0x00100073);
    TEST_ASSERT_TRUE(cpu.halted);
}

static void test_fence_is_nop(void)
{
    cpu.halted = false;
    exec_raw(0x0FF0000F);
    TEST_ASSERT_FALSE(cpu.halted);
}

static void test_invalid_halts(void)
{
    exec_raw(0xFFFFFFFF);
    TEST_ASSERT_TRUE(cpu.halted);
}

int main(void)
{
    UNITY_BEGIN();

    /* Group A */
    RUN_TEST(test_add_simple);
    RUN_TEST(test_add_overflow_wraps);
    RUN_TEST(test_sub_simple);
    RUN_TEST(test_sub_underflow);
    RUN_TEST(test_and);
    RUN_TEST(test_or);
    RUN_TEST(test_xor);
    RUN_TEST(test_sll);
    RUN_TEST(test_sll_only_low_5_bits);
    RUN_TEST(test_srl_logical);
    RUN_TEST(test_sra_arithmetic);
    RUN_TEST(test_slt_signed);
    RUN_TEST(test_sltu_unsigned);

    /* Group B */
    RUN_TEST(test_addi_positive);
    RUN_TEST(test_addi_negative);
    RUN_TEST(test_andi);
    RUN_TEST(test_slli);
    RUN_TEST(test_srai);

    /* Group C */
    RUN_TEST(test_lui);
    RUN_TEST(test_auipc);

    /* Group G */
    RUN_TEST(test_ecall_halts);
    RUN_TEST(test_ebreak_halts);
    RUN_TEST(test_fence_is_nop);
    RUN_TEST(test_invalid_halts);

    return UNITY_END();
}
