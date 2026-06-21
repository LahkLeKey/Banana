#include "ui/ui.h"
#include "ui/ui_internal.h"
#include "../../support/test_support.h"

#include <stdlib.h>

static int callback_called = 0;

static int test_callback(void *user_data)
{
    callback_called = *(int *)user_data;
    return 0;
}

static void test_ui_context_and_widgets(void **state)
{
    (void)state;
    UIContext *ctx = ui_context_create(8, 8);
    UIContext *invalid_ctx = ui_context_create(0, 8);
    char buffer[16] = {0};
    int value = 7;

    BANANA_TEST_ASSERT_TRUE(invalid_ctx == NULL,
                            "ui_context_create must reject non-positive framebuffer dimensions");
    BANANA_TEST_ASSERT_TRUE(ctx != NULL, "ui_context_create must allocate a usable UI context");

    ui_begin_frame(ctx);
    BANANA_TEST_ASSERT_INT_EQ(ui_button(ctx, 0.0f, 0.0f, 4.0f, 2.0f, "Go", test_callback, &value), 0,
                              "ui_button should report click only after input is queued");

    ui_input_click(ctx, 1.0f, 1.0f);
    BANANA_TEST_ASSERT_INT_EQ(ui_button(ctx, 0.0f, 0.0f, 4.0f, 2.0f, "Go", test_callback, &value), 1,
                              "ui_button should consume click inside bounds");

    BANANA_TEST_ASSERT_INT_EQ(callback_called, 7,
                              "ui_button callback should receive user data");

    ui_text(ctx, 1.0f, 3.0f, "Hello");
    ui_input_key(ctx, 13);

    BANANA_TEST_ASSERT_INT_EQ(ui_text_field(ctx,
                                            0.0f,
                                            0.0f,
                                            2.0f,
                                            1.0f,
                                            "Guard",
                                            NULL,
                                            sizeof(buffer),
                                            4),
                              0,
                              "ui_text_field must reject null output buffers");

    ui_panel(ctx, 0.0f, 4.0f, 6.0f, 2.0f, 0x11223344, 1.0f);
    buffer[0] = 'X';
    buffer[1] = '\0';
    ui_text_field(ctx, 0.0f, 6.0f, 6.0f, 2.0f, "Name", buffer, sizeof(buffer), 8);
    ui_inventory_panel(ctx, 8.0f, 1.0f);
    ui_merchant_dialog(ctx, 4.0f, 4.0f, 3);
    ui_quest_log_panel(ctx, 2.0f, 2.0f);
    ui_quest_log_panel(NULL, 2.0f, 2.0f);

    ui_end_frame(ctx);
    BANANA_TEST_ASSERT_TRUE(ui_get_framebuffer(ctx) != NULL,
                            "ui_get_framebuffer must return non-null framebuffer");

    ui_set_thread_budget(ctx, 2);
    ui_set_thread_budget(ctx, 0);
    ui_resize(ctx, 16, 16);

    ctx->text_field_buffers[0] = (char *)malloc(8);
    BANANA_TEST_ASSERT_TRUE(ctx->text_field_buffers[0] != NULL,
                            "ui test must allocate a text-field buffer for cleanup coverage");
    ctx->text_field_count = 1;

    ui_context_destroy(ctx);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_ui_context_and_widgets)
)
