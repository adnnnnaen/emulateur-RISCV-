#include "unity.h"
#include "cpu.h"
#include "memory.h"

static Cpu cpu;

void setUp(void)
{
    mem_init();
    cpu_reset(&cpu, 0x80000000);
}

void tearDown(void) {}

static void test_step_advances_pc_simple(void)
{
    /* addi x10, x0, 1 = 0x00100513 */
    mem_write32(0x80000000, 0x00100513);
    cpu_step(&cpu);
    TEST_ASSERT_EQUAL_HEX32(0x80000004, cpu.pc);
    TEST_ASSERT_EQUAL_UINT32(1, cpu_read_reg(&cpu, 10));
}

static void test_step_branch_modifies_pc(void)
{
    /* beq x0, x0, +16 = 0x01000063 (offset 16 → bit12=0, bit11=0, bits[10:5]=0, bits[4:1]=8) */
    /* Encodage correct : beq x0, x0, +16 */
    /* opcode=0x63, funct3=0, rs1=0, rs2=0, imm=16
     * imm[12]=0, imm[11]=0, imm[10:5]=0, imm[4:1]=8 (1000)
     * raw = (0<<31) | (0<<25) | (0<<20) | (0<<15) | (0<<12) | (8<<8) | (0<<7) | 0x63
     *     = 0x00000863 */
    mem_write32(0x80000000, 0x00000863);
    cpu_step(&cpu);
    TEST_ASSERT_EQUAL_HEX32(0x80000010, cpu.pc); /* +16, pas +4 */
}

static void test_run_two_instructions(void)
{
    /* addi x10, x0, 5 = 0x00500513
     * addi x10, x10, 3 = 0x00350513 */
    mem_write32(0x80000000, 0x00500513);
    mem_write32(0x80000004, 0x00350513);
    cpu_run(&cpu, 2);
    TEST_ASSERT_EQUAL_UINT32(8, cpu_read_reg(&cpu, 10));
    TEST_ASSERT_EQUAL_HEX32(0x80000008, cpu.pc);
}

static void test_ecall_halts_run(void)
{
    /* addi x10, x0, 42 = 0x02A00513
     * ecall = 0x00000073 */
    mem_write32(0x80000000, 0x02A00513);
    mem_write32(0x80000004, 0x00000073);
    cpu_run(&cpu, 100);
    TEST_ASSERT_TRUE(cpu.halted);
    TEST_ASSERT_EQUAL_UINT32(42, cpu_read_reg(&cpu, 10));
}

static void test_loop_with_jump(void)
{
    /* Programme :
     *   addi x10, x0, 0       # x10 = 0           = 0x00000513
     * loop:
     *   addi x10, x10, 1      # x10++             = 0x00150513
     *   addi x12, x0, 5       # x12 = 5           = 0x00500613
     *   beq  x10, x12, end    # si x10 == 5 → end = 0x00C50863 (+16)
     *   jal  x0, loop         # else → loop (-12) = 0xFF5FF06F
     * end:
     *   ecall                 #                   = 0x00000073
     */
    mem_write32(0x80000000, 0x00000513);
    mem_write32(0x80000004, 0x00150513); /* loop */
    mem_write32(0x80000008, 0x00500613);
    mem_write32(0x8000000C, 0x00C50863); /* beq x10, x12, +16 → end (à 0x8000001C) */
    mem_write32(0x80000010, 0xFF5FF06F); /* jal x0, -12 → loop */
    mem_write32(0x80000014, 0x00000013); /* nop (addi x0, x0, 0) */
    mem_write32(0x80000018, 0x00000013); /* nop */
    mem_write32(0x8000001C, 0x00000073); /* end: ecall */

    cpu_run(&cpu, 1000);
    TEST_ASSERT_TRUE(cpu.halted);
    TEST_ASSERT_EQUAL_UINT32(5, cpu_read_reg(&cpu, 10));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_step_advances_pc_simple);
    RUN_TEST(test_step_branch_modifies_pc);
    RUN_TEST(test_run_two_instructions);
    RUN_TEST(test_ecall_halts_run);
    RUN_TEST(test_loop_with_jump);
    return UNITY_END();
}
