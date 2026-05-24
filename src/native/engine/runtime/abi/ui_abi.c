#include "ui_abi.h"

#include "../../ui/ui.h"

static UIContext *s_ui_context = NULL;

int runtime_ui_abi_init(int width, int height)
{
    if (s_ui_context)
        return 0;

    s_ui_context = ui_context_create(width, height);
    return s_ui_context ? 0 : -1;
}

void runtime_ui_abi_begin_frame(void)
{
    if (!s_ui_context)
        return;

    ui_begin_frame(s_ui_context);
}

int runtime_ui_abi_inventory_panel(float x, float y)
{
    if (!s_ui_context)
        return 0;

    return ui_inventory_panel(s_ui_context, x, y);
}

int runtime_ui_abi_merchant_dialog(float x, float y, int npc_id)
{
    if (!s_ui_context)
        return 0;

    return ui_merchant_dialog(s_ui_context, x, y, npc_id);
}

void runtime_ui_abi_panel(float x,
                          float y,
                          float width,
                          float height,
                          unsigned int fill_rgba,
                          float border_width)
{
    if (!s_ui_context)
        return;

    ui_panel(s_ui_context, x, y, width, height, fill_rgba, border_width);
}

void runtime_ui_abi_text(float x, float y, const char *text)
{
    if (!s_ui_context || !text)
        return;

    ui_text(s_ui_context, x, y, text);
}

void runtime_ui_abi_end_frame(void)
{
    if (!s_ui_context)
        return;

    ui_end_frame(s_ui_context);
}

const unsigned char *runtime_ui_abi_get_framebuffer(void)
{
    return s_ui_context ? ui_get_framebuffer(s_ui_context) : NULL;
}

void runtime_ui_abi_shutdown(void)
{
    if (!s_ui_context)
        return;

    ui_context_destroy(s_ui_context);
    s_ui_context = NULL;
}