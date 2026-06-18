#include "ui/ui.h"

#if defined(BANANA_USE_CMOCKA)
#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

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
    char buffer[16] = {0};
    int value = 7;

    assert_non_null(ctx);

    ui_begin_frame(ctx);
    assert_int_equal(ui_button(ctx, 0.0f, 0.0f, 4.0f, 2.0f, "Go", test_callback, &value), 0);

    ui_input_click(ctx, 1.0f, 1.0f);
    assert_int_equal(ui_button(ctx, 0.0f, 0.0f, 4.0f, 2.0f, "Go", test_callback, &value), 1);

    assert_int_equal(callback_called, 7);

    ui_text(ctx, 1.0f, 3.0f, "Hello");
    ui_panel(ctx, 0.0f, 4.0f, 6.0f, 2.0f, 0x11223344, 1.0f);
    ui_text_field(ctx, 0.0f, 6.0f, 6.0f, 2.0f, "Name", buffer, sizeof(buffer), 8);
    ui_inventory_panel(ctx, 8.0f, 1.0f);
    ui_merchant_dialog(ctx, 4.0f, 4.0f, 3);

    ui_end_frame(ctx);
    assert_non_null(ui_get_framebuffer(ctx));

    ui_set_thread_budget(ctx, 2);
    ui_resize(ctx, 16, 16);

    ui_context_destroy(ctx);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_ui_context_and_widgets),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#else
#include <stdio.h>

static int fail_if(int condition, const char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s\n", message);
        return 1;
    }
    return 0;
}

static int callback_called = 0;

static int test_callback(void *user_data)
{
    callback_called = *(int *)user_data;
    return 0;
}

static int test_ui_context_and_widgets(void)
{
    UIContext *ctx = ui_context_create(8, 8);
    char buffer[16] = {0};
    int value = 7;

    if (fail_if(!ctx, "ui_context_create must allocate a usable UI context"))
        return 1;

    ui_begin_frame(ctx);
    if (fail_if(ui_button(ctx, 0.0f, 0.0f, 4.0f, 2.0f, "Go", test_callback, &value) != 0,
                "ui_button should report a click only after input is queued"))
    {
        ui_context_destroy(ctx);
        return 1;
    }

    ui_input_click(ctx, 1.0f, 1.0f);
    if (fail_if(ui_button(ctx, 0.0f, 0.0f, 4.0f, 2.0f, "Go", test_callback, &value) != 1,
                "ui_button should consume a click that lands inside its bounds"))
    {
        ui_context_destroy(ctx);
        return 1;
    }

    if (fail_if(callback_called != 7, "ui_button callback should receive the user data"))
    {
        ui_context_destroy(ctx);
        return 1;
    }

    ui_text(ctx, 1.0f, 3.0f, "Hello");
    ui_panel(ctx, 0.0f, 4.0f, 6.0f, 2.0f, 0x11223344, 1.0f);
    ui_text_field(ctx, 0.0f, 6.0f, 6.0f, 2.0f, "Name", buffer, sizeof(buffer), 8);
    ui_inventory_panel(ctx, 8.0f, 1.0f);
    ui_merchant_dialog(ctx, 4.0f, 4.0f, 3);

    ui_end_frame(ctx);
    if (fail_if(!ui_get_framebuffer(ctx), "ui_get_framebuffer must return a non-null framebuffer"))
    {
        ui_context_destroy(ctx);
        return 1;
    }

    ui_set_thread_budget(ctx, 2);
    ui_resize(ctx, 16, 16);

    ui_context_destroy(ctx);
    return 0;
}

int main(void)
{
    return test_ui_context_and_widgets();
}
#endif
