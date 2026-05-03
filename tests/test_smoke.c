#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

static void test_unity_works(void)
{
    TEST_ASSERT_EQUAL_INT(2, 1 + 1);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, 0xDEADBEEF);
    TEST_ASSERT_TRUE(1);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_unity_works);
    return UNITY_END();
}
