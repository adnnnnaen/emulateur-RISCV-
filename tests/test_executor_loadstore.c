#include "unity.h"
#include "cpu.h"
#include "decoder.h"
#include "executor.h"
#include "memory.h"

static Cpu cpu;

void setUp(void)
{
    cpu_reset(&cpu, 0x80000000);
    mem_init();
}

void tearDown(void) {}

static void exec_raw(uint32_t raw)
{
    Instruction instr = decode(raw);
    execute(&cpu, &instr);
}

/* ===== Stores ===== */

static void test_sw_basic(void)
{
    cpu_write_reg(&cpu, 10, 0x80000100);
    cpu_write_reg(&cpu, 12, 0xDEADBEEF);
    /* sw x12, 0(x10) = 0x00C52023 */
    exec_raw(0x00C52023);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, mem_read32(0x80000100));
}

static void test_sb_writes_only_low_byte(void)
{
    mem_write32(0x80000100, 0xFFFFFFFF);
    cpu_write_reg(&cpu, 10, 0x80000100);
    cpu_write_reg(&cpu, 12, 0xCAFEAB12);
    /* sb x12, 0(x10) = 0x00C50023 */
    exec_raw(0x00C50023);
    TEST_ASSERT_EQUAL_HEX8(0x12, mem_read8(0x80000100));
    TEST_ASSERT_EQUAL_HEX8(0xFF, mem_read8(0x80000101));
}

static void test_sh_writes_only_low_halfword(void)
{
    mem_write32(0x80000100, 0xFFFFFFFF);
    cpu_write_reg(&cpu, 10, 0x80000100);
    cpu_write_reg(&cpu, 12, 0xCAFEAB12);
    /* sh x12, 0(x10) = 0x00C51023 */
    exec_raw(0x00C51023);
    TEST_ASSERT_EQUAL_HEX16(0xAB12, mem_read16(0x80000100));
    TEST_ASSERT_EQUAL_HEX16(0xFFFF, mem_read16(0x80000102));
}

/* ===== Loads ===== */

static void test_lw(void)
{
    mem_write32(0x80000200, 0x12345678);
    cpu_write_reg(&cpu, 10, 0x80000200);
    /* lw x12, 0(x10) = 0x00052603 */
    exec_raw(0x00052603);
    TEST_ASSERT_EQUAL_HEX32(0x12345678, cpu_read_reg(&cpu, 12));
}

static void test_lb_sign_extend_negative(void)
{
    mem_write8(0x80000200, 0xFF);
    cpu_write_reg(&cpu, 10, 0x80000200);
    /* lb x12, 0(x10) = 0x00050603 */
    exec_raw(0x00050603);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, cpu_read_reg(&cpu, 12));
}

static void test_lbu_zero_extend(void)
{
    mem_write8(0x80000200, 0xFF);
    cpu_write_reg(&cpu, 10, 0x80000200);
    /* lbu x12, 0(x10) = 0x00054603 */
    exec_raw(0x00054603);
    TEST_ASSERT_EQUAL_HEX32(0x000000FF, cpu_read_reg(&cpu, 12));
}

static void test_lh_sign_extend(void)
{
    mem_write16(0x80000200, 0xFFFE);
    cpu_write_reg(&cpu, 10, 0x80000200);
    /* lh x12, 0(x10) = 0x00051603 */
    exec_raw(0x00051603);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFE, cpu_read_reg(&cpu, 12));
}

static void test_lhu_zero_extend(void)
{
    mem_write16(0x80000200, 0xFFFE);
    cpu_write_reg(&cpu, 10, 0x80000200);
    /* lhu x12, 0(x10) = 0x00055603 */
    exec_raw(0x00055603);
    TEST_ASSERT_EQUAL_HEX32(0x0000FFFE, cpu_read_reg(&cpu, 12));
}

static void test_load_with_negative_offset(void)
{
    mem_write32(0x80000200, 0xCAFEBABE);
    cpu_write_reg(&cpu, 10, 0x80000204);
    /* lw x12, -4(x10) = 0xFFC52603 */
    exec_raw(0xFFC52603);
    TEST_ASSERT_EQUAL_HEX32(0xCAFEBABE, cpu_read_reg(&cpu, 12));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_sw_basic);
    RUN_TEST(test_sb_writes_only_low_byte);
    RUN_TEST(test_sh_writes_only_low_halfword);
    RUN_TEST(test_lw);
    RUN_TEST(test_lb_sign_extend_negative);
    RUN_TEST(test_lbu_zero_extend);
    RUN_TEST(test_lh_sign_extend);
    RUN_TEST(test_lhu_zero_extend);
    RUN_TEST(test_load_with_negative_offset);
    return UNITY_END();
}
