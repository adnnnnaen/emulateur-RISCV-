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

static void test_jal_forward(void)
{
    cpu.pc = 0x80000010;
    /* jal x1, +16 = 0x010000EF */
    bool jumped = exec_raw(0x010000EF);
    TEST_ASSERT_TRUE(jumped);
    TEST_ASSERT_EQUAL_HEX32(0x80000020, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x80000014, cpu_read_reg(&cpu, 1));
}

static void test_jal_backward(void)
{
    cpu.pc = 0x80000020;
    /* jal x1, -8 = 0xFF9FF0EF */
    bool jumped = exec_raw(0xFF9FF0EF);
    TEST_ASSERT_TRUE(jumped);
    TEST_ASSERT_EQUAL_HEX32(0x80000018, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x80000024, cpu_read_reg(&cpu, 1));
}

static void test_jal_x0_no_link(void)
{
    /* jal x0, +8 = 0x0080006F */
    cpu.pc = 0x80000010;
    bool jumped = exec_raw(0x0080006F);
    TEST_ASSERT_TRUE(jumped);
    TEST_ASSERT_EQUAL_HEX32(0x80000018, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0, cpu_read_reg(&cpu, 0));
}

static void test_jalr(void)
{
    cpu.pc = 0x80000100;
    cpu_write_reg(&cpu, 5, 0x80001000);
    /* jalr x1, 0(x5) = 0x000280E7 */
    bool jumped = exec_raw(0x000280E7);
    TEST_ASSERT_TRUE(jumped);
    TEST_ASSERT_EQUAL_HEX32(0x80001000, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x80000104, cpu_read_reg(&cpu, 1));
}

static void test_jalr_lsb_cleared(void)
{
    cpu.pc = 0x80000100;
    cpu_write_reg(&cpu, 5, 0x80001003);
    exec_raw(0x000280E7);
    TEST_ASSERT_EQUAL_HEX32(0x80001002, cpu.pc);
}

static void test_jal_uses_old_pc_for_return(void)
{
    cpu.pc = 0x80000050;
    /* jal x1, +0x100 = 0x100000EF */
    exec_raw(0x100000EF);
    TEST_ASSERT_EQUAL_HEX32(0x80000054, cpu_read_reg(&cpu, 1));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_jal_forward);
    RUN_TEST(test_jal_backward);
    RUN_TEST(test_jal_x0_no_link);
    RUN_TEST(test_jalr);
    RUN_TEST(test_jalr_lsb_cleared);
    RUN_TEST(test_jal_uses_old_pc_for_return);
    return UNITY_END();
}
