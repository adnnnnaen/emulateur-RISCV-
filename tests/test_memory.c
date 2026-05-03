#include "unity.h"
#include "memory.h"

void setUp(void) { mem_init(); }
void tearDown(void) {}

static void test_init_zeroes_ram(void)
{
    /* Toute la RAM doit être à 0 après init */
    for (uint32_t addr = MEMORY_BASE; addr < MEMORY_BASE + 1024; addr += 4) {
        TEST_ASSERT_EQUAL_HEX32(0, mem_read32(addr));
    }
}

static void test_write_read_byte(void)
{
    mem_write8(MEMORY_BASE + 0x10, 0xAB);
    TEST_ASSERT_EQUAL_HEX8(0xAB, mem_read8(MEMORY_BASE + 0x10));
}

static void test_write_read_word(void)
{
    mem_write32(MEMORY_BASE + 0x20, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, mem_read32(MEMORY_BASE + 0x20));
}

static void test_little_endian_byte_order(void)
{
    /* Écrire 0x12345678 en mot 32 bits, vérifier l'ordre des octets */
    mem_write32(MEMORY_BASE, 0x12345678);
    TEST_ASSERT_EQUAL_HEX8(0x78, mem_read8(MEMORY_BASE + 0));  /* LSB */
    TEST_ASSERT_EQUAL_HEX8(0x56, mem_read8(MEMORY_BASE + 1));
    TEST_ASSERT_EQUAL_HEX8(0x34, mem_read8(MEMORY_BASE + 2));
    TEST_ASSERT_EQUAL_HEX8(0x12, mem_read8(MEMORY_BASE + 3));  /* MSB */
}

static void test_little_endian_byte_to_word(void)
{
    /* Écrire octet par octet, lire en mot */
    mem_write8(MEMORY_BASE + 0, 0xEF);
    mem_write8(MEMORY_BASE + 1, 0xBE);
    mem_write8(MEMORY_BASE + 2, 0xAD);
    mem_write8(MEMORY_BASE + 3, 0xDE);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, mem_read32(MEMORY_BASE));
}

static void test_halfword(void)
{
    mem_write16(MEMORY_BASE + 0x100, 0xABCD);
    TEST_ASSERT_EQUAL_HEX16(0xABCD, mem_read16(MEMORY_BASE + 0x100));
    TEST_ASSERT_EQUAL_HEX8(0xCD, mem_read8(MEMORY_BASE + 0x100));
    TEST_ASSERT_EQUAL_HEX8(0xAB, mem_read8(MEMORY_BASE + 0x101));
}

static void test_in_range_boundaries(void)
{
    TEST_ASSERT_TRUE(mem_in_range(MEMORY_BASE, 4));
    TEST_ASSERT_TRUE(mem_in_range(MEMORY_BASE + MEMORY_SIZE - 4, 4));
    TEST_ASSERT_FALSE(mem_in_range(MEMORY_BASE + MEMORY_SIZE - 3, 4));
    TEST_ASSERT_FALSE(mem_in_range(MEMORY_BASE - 1, 1));
}

static void test_load_buffer(void)
{
    const uint8_t data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    bool ok = mem_load(MEMORY_BASE + 0x40, data, sizeof(data));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(0xDDCCBBAA, mem_read32(MEMORY_BASE + 0x40));
}

static void test_load_overflow_rejected(void)
{
    const uint8_t data[8] = {0};
    bool ok = mem_load(MEMORY_BASE + MEMORY_SIZE - 4, data, 8);
    TEST_ASSERT_FALSE(ok);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_zeroes_ram);
    RUN_TEST(test_write_read_byte);
    RUN_TEST(test_write_read_word);
    RUN_TEST(test_little_endian_byte_order);
    RUN_TEST(test_little_endian_byte_to_word);
    RUN_TEST(test_halfword);
    RUN_TEST(test_in_range_boundaries);
    RUN_TEST(test_load_buffer);
    RUN_TEST(test_load_overflow_rejected);
    return UNITY_END();
}
