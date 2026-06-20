#include "ui/ui.h"

#include "../../support/test_support.h"

#include <stdint.h>

int ui_merchant_dialog_impl(UIContext *ctx, float x, float y, int npc_id);

static void test_merchant_impl_returns_zero_for_null_ctx(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(ui_merchant_dialog_impl(NULL, 10.0f, 20.0f, 9),
                              0,
                              "merchant impl must return zero for null context");
}

static void test_merchant_impl_returns_zero_for_valid_ctx_stub(void **state)
{
    UIContext *fake_ctx = (UIContext *)(uintptr_t)0x1;

    (void)state;

    BANANA_TEST_ASSERT_INT_EQ(ui_merchant_dialog_impl(fake_ctx, 4.0f, 5.0f, 3),
                              0,
                              "merchant impl stub should return zero with non-null context");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_merchant_impl_returns_zero_for_null_ctx),
    BANANA_TEST_CASE(test_merchant_impl_returns_zero_for_valid_ctx_stub)
)
