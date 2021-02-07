#include <unity.h>

#include <definitions.h>
#include <facelets.h>

void test_facelets_color_count() {
    char facelets_correct[N_FACELETS] = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
    TEST_ASSERT_TRUE(verify_valid_facelets(facelets_correct));

    char facelets_wrong[N_FACELETS] = "FUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
    TEST_ASSERT_FALSE(verify_valid_facelets(facelets_wrong));
}

void setUp(void) {}

void tearDown(void) {}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_facelets_color_count);

    return UNITY_END();
}
