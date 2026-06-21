#include "ui/ui_internal.h"
#include "../../support/test_support.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef enum BananaUiAllocMode
{
    BANANA_UI_ALLOC_CALLOC_FAIL = 1,
    BANANA_UI_ALLOC_ELEMENTS_FAIL = 2,
} BananaUiAllocMode;

static BananaUiAllocMode g_alloc_mode = BANANA_UI_ALLOC_CALLOC_FAIL;
static int g_malloc_calls = 0;
static int g_free_calls = 0;

static unsigned char g_ui_context_storage[sizeof(UIContext)];
static unsigned char g_framebuffer_storage[4096];

void *banana_test_malloc(size_t size)
{
    (void)size;
    g_malloc_calls += 1;

    if (g_malloc_calls == 1)
        return g_ui_context_storage;

    if (g_alloc_mode == BANANA_UI_ALLOC_ELEMENTS_FAIL)
        return NULL;

    return g_framebuffer_storage;
}

void *banana_test_calloc(size_t count, size_t size)
{
    size_t bytes = count * size;

    if (g_alloc_mode == BANANA_UI_ALLOC_CALLOC_FAIL)
        return NULL;

    if (bytes > sizeof(g_framebuffer_storage))
        return NULL;

    memset(g_framebuffer_storage, 0, bytes);
    return g_framebuffer_storage;
}

void banana_test_free(void *ptr)
{
    (void)ptr;
    g_free_calls += 1;
}

void ui_render_button(UIContext *ctx, UIElement *elem)
{
    (void)ctx;
    (void)elem;
}

void ui_render_text(UIContext *ctx, UIElement *elem)
{
    (void)ctx;
    (void)elem;
}

void ui_render_panel(UIContext *ctx, UIElement *elem)
{
    (void)ctx;
    (void)elem;
}

void ui_render_text_field(UIContext *ctx, UIElement *elem)
{
    (void)ctx;
    (void)elem;
}

void ui_render_inventory_panel(UIContext *ctx, UIElement *elem)
{
    (void)ctx;
    (void)elem;
}

void ui_render_merchant_dialog(UIContext *ctx, UIElement *elem)
{
    (void)ctx;
    (void)elem;
}

static void test_ui_context_create_returns_null_when_framebuffer_allocation_fails(void **state)
{
    UIContext *ctx;
    (void)state;

    g_alloc_mode = BANANA_UI_ALLOC_CALLOC_FAIL;
    g_malloc_calls = 0;
    g_free_calls = 0;

    ctx = ui_context_create(8, 8);

    BANANA_TEST_ASSERT_TRUE(ctx == NULL,
                            "ui_context_create must fail when framebuffer allocation fails");
    BANANA_TEST_ASSERT_INT_EQ(g_free_calls,
                              1,
                              "ui_context_create must free the partially initialized context on framebuffer allocation failure");
}

static void test_ui_context_create_returns_null_when_element_allocation_fails(void **state)
{
    UIContext *ctx;
    (void)state;

    g_alloc_mode = BANANA_UI_ALLOC_ELEMENTS_FAIL;
    g_malloc_calls = 0;
    g_free_calls = 0;

    ctx = ui_context_create(8, 8);

    BANANA_TEST_ASSERT_TRUE(ctx == NULL,
                            "ui_context_create must fail when element allocation fails");
    BANANA_TEST_ASSERT_INT_EQ(g_free_calls,
                              2,
                              "ui_context_create must free framebuffer and context on element allocation failure");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_ui_context_create_returns_null_when_framebuffer_allocation_fails),
    BANANA_TEST_CASE(test_ui_context_create_returns_null_when_element_allocation_fails)
)
