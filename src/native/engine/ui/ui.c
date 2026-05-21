#include "ui_internal.h"
#include <stdlib.h>
#include <string.h>

/* ─────────────────────────────────────────────────────────────────────────
   Initialization
   ───────────────────────────────────────────────────────────────────────── */

UIContext *ui_context_create(int framebuffer_width, int framebuffer_height) {
    if (framebuffer_width <= 0 || framebuffer_height <= 0) {
        return NULL;
    }
    
    UIContext *ctx = (UIContext *)malloc(sizeof(UIContext));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(UIContext));
    
    ctx->framebuffer_width = framebuffer_width;
    ctx->framebuffer_height = framebuffer_height;
    
    /* Allocate framebuffer: RGBA 32-bit */
    size_t fb_size = (size_t)framebuffer_width * framebuffer_height * 4;
    ctx->framebuffer = (unsigned char *)calloc(fb_size, 1);
    if (!ctx->framebuffer) {
        free(ctx);
        return NULL;
    }
    
    /* Allocate element queue */
    ctx->elements = (UIElement *)malloc(MAX_UI_ELEMENTS * sizeof(UIElement));
    if (!ctx->elements) {
        free(ctx->framebuffer);
        free(ctx);
        return NULL;
    }
    
    ctx->max_elements = MAX_UI_ELEMENTS;
    ctx->element_count = 0;
    ctx->thread_budget = 0; /* 0 = auto */
    
    return ctx;
}

void ui_context_destroy(UIContext *ctx) {
    if (!ctx) return;
    
    /* Free text field buffers */
    for (int i = 0; i < ctx->text_field_count; i++) {
        free(ctx->text_field_buffers[i]);
    }
    
    free(ctx->framebuffer);
    free(ctx->elements);
    free(ctx);
}

/* ─────────────────────────────────────────────────────────────────────────
   Frame Lifecycle
   ───────────────────────────────────────────────────────────────────────── */

void ui_begin_frame(UIContext *ctx) {
    if (!ctx) return;
    
    /* Clear element queue */
    ctx->element_count = 0;
    
    /* Clear framebuffer to transparent black */
    memset(ctx->framebuffer, 0, 
           (size_t)ctx->framebuffer_width * ctx->framebuffer_height * 4);
}

void ui_end_frame(UIContext *ctx) {
    if (!ctx) return;

    /* Discard any click that was not consumed by a button this frame */
    ctx->has_pending_click = 0;

    /* Render all queued elements to framebuffer (back-to-front) */
    for (int i = 0; i < ctx->element_count; i++) {
        UIElement *elem = &ctx->elements[i];
        switch (elem->type) {
            case UI_ELEM_BUTTON:           ui_render_button(ctx, elem);           break;
            case UI_ELEM_TEXT:             ui_render_text(ctx, elem);             break;
            case UI_ELEM_PANEL:            ui_render_panel(ctx, elem);            break;
            case UI_ELEM_TEXT_FIELD:       ui_render_text_field(ctx, elem);       break;
            case UI_ELEM_INVENTORY_PANEL:  ui_render_inventory_panel(ctx, elem);  break;
            case UI_ELEM_MERCHANT_DIALOG:  ui_render_merchant_dialog(ctx, elem);  break;
            default: break;
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────────
   Input Events
   ───────────────────────────────────────────────────────────────────────── */

void ui_input_click(UIContext *ctx, float x, float y) {
    if (!ctx) return;
    
    ctx->last_click_x = (int)x;
    ctx->last_click_y = (int)y;
    ctx->has_pending_click = 1;
}

void ui_input_key(UIContext *ctx, int key_code) {
    if (!ctx) return;
    
    /* TODO: Route key input to focused text field or menu navigation */
}

/* ─────────────────────────────────────────────────────────────────────────
   Basic Elements
   ───────────────────────────────────────────────────────────────────────── */

int ui_button(UIContext *ctx, float x, float y, float width, float height,
              const char *label, UICallback callback, void *user_data) {
    if (!ctx || ctx->element_count >= ctx->max_elements) return 0;

    UIElement *elem = &ctx->elements[ctx->element_count++];
    memset(elem, 0, sizeof(UIElement));

    elem->type = UI_ELEM_BUTTON;
    elem->x = x;
    elem->y = y;
    elem->width = width;
    elem->height = height;
    elem->data.button.label = label;
    elem->data.button.callback = callback;
    elem->data.button.user_data = user_data;

    /* Hit-test: if a click is pending and lands inside this button, consume it */
    if (ctx->has_pending_click) {
        float cx = (float)ctx->last_click_x;
        float cy = (float)ctx->last_click_y;
        if (cx >= x && cx < x + width && cy >= y && cy < y + height) {
            ctx->has_pending_click = 0;
            elem->data.button.was_clicked = 1;
            if (callback) callback(user_data);
            return 1;
        }
    }

    return 0;
}

void ui_text(UIContext *ctx, float x, float y, const char *text) {
    if (!ctx || !text || ctx->element_count >= ctx->max_elements) return;
    
    UIElement *elem = &ctx->elements[ctx->element_count++];
    memset(elem, 0, sizeof(UIElement));
    
    elem->type = UI_ELEM_TEXT;
    elem->x = x;
    elem->y = y;
    elem->data.text.text = text;
}

int ui_text_field(UIContext *ctx, float x, float y, float width, float height,
                   const char *placeholder, char *out_buffer, size_t buffer_size,
                   size_t max_length) {
    if (!ctx || !out_buffer || buffer_size == 0 || ctx->element_count >= ctx->max_elements) {
        return 0;
    }
    
    UIElement *elem = &ctx->elements[ctx->element_count++];
    memset(elem, 0, sizeof(UIElement));
    
    elem->type = UI_ELEM_TEXT_FIELD;
    elem->x = x;
    elem->y = y;
    elem->width = width;
    elem->height = height;
    elem->data.text_field.buffer = out_buffer;
    elem->data.text_field.buffer_size = buffer_size;
    elem->data.text_field.max_length = max_length;
    
    return 0; /* Would return 1 if newly focused this frame */
}

void ui_panel(UIContext *ctx, float x, float y, float width, float height,
              uint32_t fill_rgba, float border_width) {
    if (!ctx || ctx->element_count >= ctx->max_elements) return;
    
    UIElement *elem = &ctx->elements[ctx->element_count++];
    memset(elem, 0, sizeof(UIElement));
    
    elem->type = UI_ELEM_PANEL;
    elem->x = x;
    elem->y = y;
    elem->width = width;
    elem->height = height;
    elem->data.panel.fill_rgba = fill_rgba;
    elem->data.panel.border_width = border_width;
}

/* ─────────────────────────────────────────────────────────────────────────
   Widgets
   ───────────────────────────────────────────────────────────────────────── */

int ui_inventory_panel(UIContext *ctx, float x, float y) {
    if (!ctx || ctx->element_count >= ctx->max_elements) return 0;
    
    UIElement *elem = &ctx->elements[ctx->element_count++];
    memset(elem, 0, sizeof(UIElement));
    
    elem->type = UI_ELEM_INVENTORY_PANEL;
    elem->x = x;
    elem->y = y;
    
    return 0;
}

int ui_merchant_dialog(UIContext *ctx, float x, float y, int npc_id) {
    if (!ctx || ctx->element_count >= ctx->max_elements) return 0;
    
    UIElement *elem = &ctx->elements[ctx->element_count++];
    memset(elem, 0, sizeof(UIElement));
    
    elem->type = UI_ELEM_MERCHANT_DIALOG;
    elem->x = x;
    elem->y = y;
    elem->data.merchant_dialog.npc_id = npc_id;
    
    return 0;
}

void ui_quest_log_panel(UIContext *ctx, float x, float y) {
    /* Stub for future quest system */
    if (!ctx) return;
}

/* ─────────────────────────────────────────────────────────────────────────
   Rendering
   ───────────────────────────────────────────────────────────────────────── */

const unsigned char *ui_get_framebuffer(UIContext *ctx) {
    return ctx ? ctx->framebuffer : NULL;
}

void ui_resize(UIContext *ctx, int new_width, int new_height) {
    if (!ctx || new_width <= 0 || new_height <= 0) return;
    
    size_t new_size = (size_t)new_width * new_height * 4;
    unsigned char *new_fb = (unsigned char *)realloc(ctx->framebuffer, new_size);
    
    if (!new_fb) return; /* Keep old framebuffer on allocation failure */
    
    ctx->framebuffer = new_fb;
    ctx->framebuffer_width = new_width;
    ctx->framebuffer_height = new_height;
    
    /* Clear to transparent black */
    memset(ctx->framebuffer, 0, new_size);
}

/* ─────────────────────────────────────────────────────────────────────────
   Parallel Configuration
   ───────────────────────────────────────────────────────────────────────── */

void ui_set_thread_budget(UIContext *ctx, int max_threads) {
    if (!ctx) return;
    
    if (max_threads <= 0) {
        ctx->thread_budget = 0; /* Auto */
    } else {
        ctx->thread_budget = max_threads;
    }
}
