#ifndef BANANA_ENGINE_UI_INTERNAL_H
#define BANANA_ENGINE_UI_INTERNAL_H

/* Internal header: exposes UIContext and UIElement structs to ui_render.c.
   Do NOT include from outside the ui/ directory. */

#include "ui.h"
#include <stdint.h>
#include <stddef.h>

#define MAX_UI_ELEMENTS       512
#define MAX_TEXT_FIELD_BUFFERS 16

typedef enum {
    UI_ELEM_BUTTON,
    UI_ELEM_TEXT,
    UI_ELEM_TEXT_FIELD,
    UI_ELEM_PANEL,
    UI_ELEM_INVENTORY_PANEL,
    UI_ELEM_MERCHANT_DIALOG,
} UIElementType;

typedef struct {
    UIElementType type;
    float x, y, width, height;

    union {
        struct {
            const char *label;
            UICallback callback;
            void       *user_data;
            int         is_hovered;
            int         was_clicked;
        } button;

        struct {
            const char *text;
        } text;

        struct {
            char  *buffer;
            size_t buffer_size;
            size_t max_length;
            int    is_focused;
        } text_field;

        struct {
            uint32_t fill_rgba;
            float    border_width;
        } panel;

        struct {
            int reserved;
        } inventory_panel;

        struct {
            int npc_id;
            int trade_result;
        } merchant_dialog;
    } data;
} UIElement;

struct UIContext {
    int framebuffer_width;
    int framebuffer_height;

    /* Framebuffer: RGBA 32-bit, stride = framebuffer_width * 4 bytes */
    unsigned char *framebuffer;

    /* Element queue for the current frame */
    UIElement *elements;
    int        element_count;
    int        max_elements;

    /* Backing buffers for text fields */
    char *text_field_buffers[MAX_TEXT_FIELD_BUFFERS];
    int   text_field_count;

    /* Pending input state (consumed at ui_end_frame) */
    int last_click_x;
    int last_click_y;
    int has_pending_click;

    /* OpenMP thread budget (0 = auto) */
    int thread_budget;
};

/* ─── Internal render function declarations ──────────────────────────────── */

void ui_render_button(UIContext *ctx, UIElement *elem);
void ui_render_text(UIContext *ctx, UIElement *elem);
void ui_render_panel(UIContext *ctx, UIElement *elem);
void ui_render_text_field(UIContext *ctx, UIElement *elem);
void ui_render_inventory_panel(UIContext *ctx, UIElement *elem);
void ui_render_merchant_dialog(UIContext *ctx, UIElement *elem);

#endif /* BANANA_ENGINE_UI_INTERNAL_H */
