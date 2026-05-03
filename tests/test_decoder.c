#include "unity.h"
#include "decoder.h"

void setUp(void) {}
void tearDown(void) {}

/* ===== R-type ===== */

static void test_decode_add(void)
{
    /* add x10, x11, x12 = 0x00C58533 */
    Instruction i = decode(0x00C58533);
    TEST_ASSERT_EQUAL(INSTR_ADD, i.type);
    TEST_ASSERT_EQUAL_UINT32(10, i.rd);
    TEST_ASSERT_EQUAL_UINT32(11, i.rs1);
    TEST_ASSERT_EQUAL_UINT32(12, i.rs2);
}

static void test_decode_sub(void)
{
    /* sub x5, x6, x7 = 0x407302B3 */
    Instruction i = decode(0x407302B3);
    TEST_ASSERT_EQUAL(INSTR_SUB, i.type);
    TEST_ASSERT_EQUAL_UINT32(5, i.rd);
}

/* ===== I-type ===== */

static void test_decode_addi_positive(void)
{
    /* addi x10, x11, 100 = 0x06458513 */
    Instruction i = decode(0x06458513);
    TEST_ASSERT_EQUAL(INSTR_ADDI, i.type);
    TEST_ASSERT_EQUAL_UINT32(10, i.rd);
    TEST_ASSERT_EQUAL_UINT32(11, i.rs1);
    TEST_ASSERT_EQUAL_INT32(100, i.imm);
}

static void test_decode_addi_negative(void)
{
    /* addi x5, x6, -1 = 0xFFF30293 */
    Instruction i = decode(0xFFF30293);
    TEST_ASSERT_EQUAL(INSTR_ADDI, i.type);
    TEST_ASSERT_EQUAL_INT32(-1, i.imm);
}

static void test_decode_slli_shamt(void)
{
    /* slli x5, x6, 5 = 0x00531293 */
    Instruction i = decode(0x00531293);
    TEST_ASSERT_EQUAL(INSTR_SLLI, i.type);
    TEST_ASSERT_EQUAL_INT32(5, i.imm);
}

static void test_decode_srai_shamt(void)
{
    /* srai x5, x6, 1 = 0x40135293 */
    Instruction i = decode(0x40135293);
    TEST_ASSERT_EQUAL(INSTR_SRAI, i.type);
    TEST_ASSERT_EQUAL_INT32(1, i.imm);
}

/* ===== S-type ===== */

static void test_decode_sw_positive_offset(void)
{
    /* sw x12, 16(x10) = 0x00C52823 */
    Instruction i = decode(0x00C52823);
    TEST_ASSERT_EQUAL(INSTR_SW, i.type);
    TEST_ASSERT_EQUAL_UINT32(10, i.rs1);
    TEST_ASSERT_EQUAL_UINT32(12, i.rs2);
    TEST_ASSERT_EQUAL_INT32(16, i.imm);
}

static void test_decode_sw_negative_offset(void)
{
    /* sw x12, -4(x10) = 0xFEC52E23 */
    Instruction i = decode(0xFEC52E23);
    TEST_ASSERT_EQUAL(INSTR_SW, i.type);
    TEST_ASSERT_EQUAL_INT32(-4, i.imm);
}

/* ===== B-type ===== */

static void test_decode_beq_forward(void)
{
    /* beq x5, x6, +8 = 0x00628463 */
    Instruction i = decode(0x00628463);
    TEST_ASSERT_EQUAL(INSTR_BEQ, i.type);
    TEST_ASSERT_EQUAL_UINT32(5, i.rs1);
    TEST_ASSERT_EQUAL_UINT32(6, i.rs2);
    TEST_ASSERT_EQUAL_INT32(8, i.imm);
}

static void test_decode_beq_backward(void)
{
    /* beq x5, x6, -4 = 0xFE628EE3 */
    Instruction i = decode(0xFE628EE3);
    TEST_ASSERT_EQUAL(INSTR_BEQ, i.type);
    TEST_ASSERT_EQUAL_INT32(-4, i.imm);
}

/* ===== U-type ===== */

static void test_decode_lui(void)
{
    /* lui x10, 0x12345 = 0x12345537 */
    Instruction i = decode(0x12345537);
    TEST_ASSERT_EQUAL(INSTR_LUI, i.type);
    TEST_ASSERT_EQUAL_UINT32(10, i.rd);
    TEST_ASSERT_EQUAL_HEX32(0x12345000, (uint32_t)i.imm);
}

static void test_decode_auipc(void)
{
    /* auipc x5, 0x1 = 0x00001297 */
    Instruction i = decode(0x00001297);
    TEST_ASSERT_EQUAL(INSTR_AUIPC, i.type);
    TEST_ASSERT_EQUAL_UINT32(5, i.rd);
    TEST_ASSERT_EQUAL_HEX32(0x00001000, (uint32_t)i.imm);
}

/* ===== J-type ===== */

static void test_decode_jal_forward(void)
{
    /* jal x1, +16 = 0x010000EF */
    Instruction i = decode(0x010000EF);
    TEST_ASSERT_EQUAL(INSTR_JAL, i.type);
    TEST_ASSERT_EQUAL_UINT32(1, i.rd);
    TEST_ASSERT_EQUAL_INT32(16, i.imm);
}

static void test_decode_jal_backward(void)
{
    /* jal x1, -8 = 0xFF9FF0EF */
    Instruction i = decode(0xFF9FF0EF);
    TEST_ASSERT_EQUAL(INSTR_JAL, i.type);
    TEST_ASSERT_EQUAL_INT32(-8, i.imm);
}

static void test_decode_jalr(void)
{
    /* jalr x1, 0(x5) = 0x000280E7 */
    Instruction i = decode(0x000280E7);
    TEST_ASSERT_EQUAL(INSTR_JALR, i.type);
    TEST_ASSERT_EQUAL_UINT32(1, i.rd);
    TEST_ASSERT_EQUAL_UINT32(5, i.rs1);
    TEST_ASSERT_EQUAL_INT32(0, i.imm);
}

/* ===== System ===== */

static void test_decode_ecall(void)
{
    Instruction i = decode(0x00000073);
    TEST_ASSERT_EQUAL(INSTR_ECALL, i.type);
}

static void test_decode_ebreak(void)
{
    Instruction i = decode(0x00100073);
    TEST_ASSERT_EQUAL(INSTR_EBREAK, i.type);
}

static void test_decode_fence(void)
{
    Instruction i = decode(0x0FF0000F);
    TEST_ASSERT_EQUAL(INSTR_FENCE, i.type);
}

/* ===== Invalid ===== */

static void test_decode_invalid_opcode(void)
{
    /* opcode 0x7F = inutilisé en RV32I */
    Instruction i = decode(0x0000007F);
    TEST_ASSERT_EQUAL(INSTR_INVALID, i.type);
}

static void test_decode_invalid_funct3(void)
{
    /* opcode store (0x23) avec funct3=3 → invalide en RV32 */
    Instruction i = decode(0x00003023);
    TEST_ASSERT_EQUAL(INSTR_INVALID, i.type);
}

int main(void)
{
    UNITY_BEGIN();

    /* R-type */
    RUN_TEST(test_decode_add);
    RUN_TEST(test_decode_sub);

    /* I-type */
    RUN_TEST(test_decode_addi_positive);
    RUN_TEST(test_decode_addi_negative);
    RUN_TEST(test_decode_slli_shamt);
    RUN_TEST(test_decode_srai_shamt);

    /* S-type */
    RUN_TEST(test_decode_sw_positive_offset);
    RUN_TEST(test_decode_sw_negative_offset);

    /* B-type */
    RUN_TEST(test_decode_beq_forward);
    RUN_TEST(test_decode_beq_backward);

    /* U-type */
    RUN_TEST(test_decode_lui);
    RUN_TEST(test_decode_auipc);

    /* J-type */
    RUN_TEST(test_decode_jal_forward);
    RUN_TEST(test_decode_jal_backward);
    RUN_TEST(test_decode_jalr);

    /* System */
    RUN_TEST(test_decode_ecall);
    RUN_TEST(test_decode_ebreak);
    RUN_TEST(test_decode_fence);

    /* Invalid */
    RUN_TEST(test_decode_invalid_opcode);
    RUN_TEST(test_decode_invalid_funct3);

    return UNITY_END();
}
