#include "runtime/ui_abi.h"

#include <stdio.h>

static int expect_int(const char *label, int actual, int expected)
{
    if (actual == expected)
        return 1;

    fprintf(stderr, "%s expected=%d actual=%d\n", label, expected, actual);
    return 0;
}

int main(void)
{
    const unsigned char *fb = NULL;

    runtime_ui_abi_shutdown();

    if (!expect_int("invalid init rejected", runtime_ui_abi_init(0, 360), -1))
        return 1;

    if (!expect_int("framebuffer null after invalid init",
                    runtime_ui_abi_get_framebuffer() == NULL ? 1 : 0,
                    1))
        return 1;

    runtime_ui_abi_begin_frame();
    runtime_ui_abi_end_frame();

    if (!expect_int("inventory panel without init",
                    runtime_ui_abi_inventory_panel(100.f, 50.f),
                    0))
        return 1;

    if (!expect_int("merchant dialog without init",
                    runtime_ui_abi_merchant_dialog(400.f, 300.f, 1),
                    0))
        return 1;

    if (!expect_int("init success", runtime_ui_abi_init(640, 360), 0))
        return 1;

    if (!expect_int("repeat init is idempotent", runtime_ui_abi_init(640, 360), 0))
        return 1;

    runtime_ui_abi_begin_frame();
    if (!expect_int("inventory panel with init",
                    runtime_ui_abi_inventory_panel(100.f, 50.f),
                    0))
        return 1;
    if (!expect_int("merchant dialog with init",
                    runtime_ui_abi_merchant_dialog(400.f, 300.f, 1),
                    0))
        return 1;
    runtime_ui_abi_end_frame();

    fb = runtime_ui_abi_get_framebuffer();
    if (!expect_int("framebuffer available", fb != NULL ? 1 : 0, 1))
        return 1;

    runtime_ui_abi_shutdown();
    runtime_ui_abi_shutdown();

    fb = runtime_ui_abi_get_framebuffer();
    if (!expect_int("framebuffer null after shutdown", fb == NULL ? 1 : 0, 1))
        return 1;

    return 0;
}
