#include "../../../src/native/engine/ui/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TEST_PASS(name) printf("✓ %s\n", name)
#define TEST_FAIL(name, msg) do { fprintf(stderr, "✗ %s: %s\n", name, msg); exit(1); } while (0)
#define ASSERT_NE(a, b, name) do { if ((a) == (b)) TEST_FAIL(name, "Expected non-equal"); } while (0)
#define ASSERT_EQ(a, b, name) do { if ((a) != (b)) TEST_FAIL(name, "Expected equal"); } while (0)

void test_context_creation(void) {
    UIContext *ctx = ui_context_create(800, 600);
    ASSERT_NE(ctx, NULL, "ui_context_create");
    ui_context_destroy(ctx);
    TEST_PASS("context_creation");
}

void test_context_creation_invalid_dimensions(void) {
    UIContext *ctx = ui_context_create(0, 600);
    ASSERT_EQ(ctx, NULL, "ui_context_create_invalid_width");
    TEST_PASS("context_creation_invalid_dimensions");
}

void test_framebuffer_allocation(void) {
    UIContext *ctx = ui_context_create(800, 600);
    const unsigned char *fb = ui_get_framebuffer(ctx);
    ASSERT_NE(fb, NULL, "ui_get_framebuffer");
    ui_context_destroy(ctx);
    TEST_PASS("framebuffer_allocation");
}

void test_frame_lifecycle(void) {
    UIContext *ctx = ui_context_create(800, 600);
    ui_begin_frame(ctx);
    const unsigned char *fb = ui_get_framebuffer(ctx);
    ASSERT_NE(fb, NULL, "framebuffer_after_begin");
    ui_end_frame(ctx);
    ui_context_destroy(ctx);
    TEST_PASS("frame_lifecycle");
}

void test_button_element(void) {
    UIContext *ctx = ui_context_create(800, 600);
    ui_begin_frame(ctx);
    int clicked = ui_button(ctx, 100.0f, 100.0f, 80.0f, 30.0f, "Test Button", NULL, NULL);
    ASSERT_EQ(clicked, 0, "button_not_clicked_first_frame");
    ui_end_frame(ctx);
    ui_context_destroy(ctx);
    TEST_PASS("button_element");
}

void test_text_element(void) {
    UIContext *ctx = ui_context_create(800, 600);
    ui_begin_frame(ctx);
    ui_text(ctx, 50.0f, 50.0f, "Hello World");
    ui_end_frame(ctx);
    ui_context_destroy(ctx);
    TEST_PASS("text_element");
}

void test_inventory_panel(void) {
    UIContext *ctx = ui_context_create(800, 600);
    ui_begin_frame(ctx);
    int result = ui_inventory_panel(ctx, 700.0f, 50.0f);
    ASSERT_EQ(result, 0, "inventory_panel_result");
    ui_end_frame(ctx);
    ui_context_destroy(ctx);
    TEST_PASS("inventory_panel");
}

void test_merchant_dialog(void) {
    UIContext *ctx = ui_context_create(800, 600);
    ui_begin_frame(ctx);
    int result = ui_merchant_dialog(ctx, 400.0f, 300.0f, 1);
    ASSERT_EQ(result, 0, "merchant_dialog_result");
    ui_end_frame(ctx);
    ui_context_destroy(ctx);
    TEST_PASS("merchant_dialog");
}

void test_resize_framebuffer(void) {
    UIContext *ctx = ui_context_create(800, 600);
    ui_resize(ctx, 1024, 768);
    const unsigned char *fb = ui_get_framebuffer(ctx);
    ASSERT_NE(fb, NULL, "framebuffer_after_resize");
    ui_context_destroy(ctx);
    TEST_PASS("resize_framebuffer");
}

void test_set_thread_budget(void) {
    UIContext *ctx = ui_context_create(800, 600);
    ui_set_thread_budget(ctx, 4);
    ui_begin_frame(ctx);
    ui_end_frame(ctx);
    ui_context_destroy(ctx);
    TEST_PASS("set_thread_budget");
}

void test_multiple_frames(void) {
    UIContext *ctx = ui_context_create(800, 600);
    for (int frame = 0; frame < 10; frame++) {
        ui_begin_frame(ctx);
        ui_button(ctx, 100.0f, 100.0f, 80.0f, 30.0f, "Frame Button", NULL, NULL);
        ui_text(ctx, 50.0f, 50.0f, "Frame");
        ui_end_frame(ctx);
    }
    ui_context_destroy(ctx);
    TEST_PASS("multiple_frames");
}

void test_null_context_handling(void) {
    ui_begin_frame(NULL);
    ui_end_frame(NULL);
    ui_input_click(NULL, 100.0f, 100.0f);
    ui_input_key(NULL, 65);
    int result = ui_button(NULL, 100.0f, 100.0f, 80.0f, 30.0f, "Test", NULL, NULL);
    ASSERT_EQ(result, 0, "null_button");
    ui_text(NULL, 50.0f, 50.0f, "Test");
    ui_panel(NULL, 100.0f, 100.0f, 200.0f, 150.0f, 0xFFFFFFFF, 2.0f);
    const unsigned char *fb = ui_get_framebuffer(NULL);
    ASSERT_EQ(fb, NULL, "null_framebuffer");
    TEST_PASS("null_context_handling");
}

/* ─── Tier 2b: Pixel rendering validation ────────────────────────────────── */

/* Helper: count non-zero bytes in the framebuffer */
static int framebuffer_has_pixels(const unsigned char *fb, int w, int h) {
    int total = w * h * 4;
    for (int i = 0; i < total; i++) {
        if (fb[i] != 0) return 1;
    }
    return 0;
}

void test_panel_renders_pixels(void) {
    UIContext *ctx = ui_context_create(200, 100);
    ui_begin_frame(ctx);
    ui_panel(ctx, 10.0f, 10.0f, 80.0f, 50.0f, 0xFF4080FF, 2.0f);
    ui_end_frame(ctx);
    const unsigned char *fb = ui_get_framebuffer(ctx);
    ASSERT_NE(framebuffer_has_pixels(fb, 200, 100), 0, "panel_wrote_pixels");
    ui_context_destroy(ctx);
    TEST_PASS("panel_renders_pixels");
}

void test_button_renders_pixels(void) {
    UIContext *ctx = ui_context_create(400, 200);
    ui_begin_frame(ctx);
    ui_button(ctx, 50.0f, 50.0f, 120.0f, 40.0f, "OK", NULL, NULL);
    ui_end_frame(ctx);
    const unsigned char *fb = ui_get_framebuffer(ctx);
    ASSERT_NE(framebuffer_has_pixels(fb, 400, 200), 0, "button_wrote_pixels");
    ui_context_destroy(ctx);
    TEST_PASS("button_renders_pixels");
}

void test_text_renders_pixels(void) {
    UIContext *ctx = ui_context_create(300, 100);
    ui_begin_frame(ctx);
    ui_text(ctx, 10.0f, 10.0f, "Hello");
    ui_end_frame(ctx);
    const unsigned char *fb = ui_get_framebuffer(ctx);
    ASSERT_NE(framebuffer_has_pixels(fb, 300, 100), 0, "text_wrote_pixels");
    ui_context_destroy(ctx);
    TEST_PASS("text_renders_pixels");
}

void test_button_click_detection(void) {
    UIContext *ctx = ui_context_create(800, 600);
    /* Feed click at center of button before ui_begin_frame */
    ui_input_click(ctx, 140.0f, 115.0f); /* inside 100,100 80x30 */
    ui_begin_frame(ctx);
    int clicked = ui_button(ctx, 100.0f, 100.0f, 80.0f, 30.0f, "Click Me", NULL, NULL);
    ASSERT_EQ(clicked, 1, "button_returned_clicked");
    ui_end_frame(ctx);
    /* Verify click was consumed: subsequent button at same coords not re-triggered */
    ui_begin_frame(ctx);
    int clicked2 = ui_button(ctx, 100.0f, 100.0f, 80.0f, 30.0f, "Click Me", NULL, NULL);
    ASSERT_EQ(clicked2, 0, "click_consumed_after_first_frame");
    ui_end_frame(ctx);
    ui_context_destroy(ctx);
    TEST_PASS("button_click_detection");
}

void test_text_field_renders_pixels(void) {
    UIContext *ctx = ui_context_create(400, 200);
    char buf[256] = {0};
    ui_begin_frame(ctx);
    ui_text_field(ctx, 20.0f, 20.0f, 200.0f, 30.0f, "Type here...", buf, sizeof(buf), 64);
    ui_end_frame(ctx);
    const unsigned char *fb = ui_get_framebuffer(ctx);
    ASSERT_NE(framebuffer_has_pixels(fb, 400, 200), 0, "text_field_wrote_pixels");
    ui_context_destroy(ctx);
    TEST_PASS("text_field_renders_pixels");
}

void test_render_clipping(void) {
    /* Elements at negative / out-of-bounds positions must not crash or corrupt */
    UIContext *ctx = ui_context_create(100, 100);
    ui_begin_frame(ctx);
    ui_panel(ctx, -50.0f, -50.0f, 80.0f, 80.0f, 0xFFFFFFFF, 0.0f); /* partially off-screen */
    ui_button(ctx, 90.0f,  90.0f, 80.0f, 30.0f, "Off", NULL, NULL); /* mostly off-screen */
    ui_text(ctx, -10.0f,  10.0f, "clip");                            /* starts off-screen */
    ui_end_frame(ctx);
    /* No crash = pass */
    ui_context_destroy(ctx);
    TEST_PASS("render_clipping");
}

int main(void) {
    printf("=== UI System Tests ===\n\n");

    test_context_creation();
    test_context_creation_invalid_dimensions();
    test_framebuffer_allocation();
    test_frame_lifecycle();
    test_button_element();
    test_text_element();
    test_inventory_panel();
    test_merchant_dialog();
    test_resize_framebuffer();
    test_set_thread_budget();
    test_multiple_frames();
    test_null_context_handling();

    /* Tier 2b: rendering */
    test_panel_renders_pixels();
    test_button_renders_pixels();
    test_text_renders_pixels();
    test_button_click_detection();
    test_text_field_renders_pixels();
    test_render_clipping();

    printf("\n=== All Tests Passed ===\n");
    return 0;
}
