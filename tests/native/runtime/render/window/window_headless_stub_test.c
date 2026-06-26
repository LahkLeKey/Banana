#include "render/window.h"
#include "runtime/support/test_support.h"

static void test_window_headless_stub_contract(void **state)
{
    Window *window = NULL;
    int width = 0;
    int height = 0;
    float click_x = 123.0f;
    float click_y = 456.0f;

    (void)state;

    window = window_create(320, 200, "headless-test");
    BANANA_TEST_ASSERT_TRUE(window != NULL,
                            "headless stub must allocate a window object");

    BANANA_TEST_ASSERT_TRUE(window_is_open(window) == 1,
                            "headless stub window must report open after creation");

    window_poll_events(window);
    window_swap_buffers(window);

    window_get_size(window, &width, &height);
    BANANA_TEST_ASSERT_INT_EQ(width, 320,
                              "headless stub must preserve requested framebuffer width");
    BANANA_TEST_ASSERT_INT_EQ(height, 200,
                              "headless stub must preserve requested framebuffer height");

    width = 0;
    height = 0;
    window_get_input_size(window, &width, &height);
    BANANA_TEST_ASSERT_INT_EQ(width, 320,
                              "headless stub must preserve requested input width");
    BANANA_TEST_ASSERT_INT_EQ(height, 200,
                              "headless stub must preserve requested input height");

    BANANA_TEST_ASSERT_TRUE(window_take_right_click(window, &click_x, &click_y) == 0,
                            "headless stub must report no queued right click");
    BANANA_TEST_ASSERT_FLOAT_EQ(click_x, 0.0f, 0.0001f,
                                "headless stub must zero click x when no input is queued");
    BANANA_TEST_ASSERT_FLOAT_EQ(click_y, 0.0f, 0.0001f,
                                "headless stub must zero click y when no input is queued");

    BANANA_TEST_ASSERT_TRUE(window_get_native_handle(window) == NULL,
                            "headless stub must not expose a native window handle");

    window_destroy(window);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_window_headless_stub_contract)
)