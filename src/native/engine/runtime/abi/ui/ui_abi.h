#ifndef BANANA_ENGINE_RUNTIME_UI_ABI_H
#define BANANA_ENGINE_RUNTIME_UI_ABI_H

#ifdef __cplusplus
extern "C"
{
#endif

    int runtime_ui_abi_init(int width, int height);

    void runtime_ui_abi_begin_frame(void);

    int runtime_ui_abi_inventory_panel(float x, float y);

    int runtime_ui_abi_merchant_dialog(float x, float y, int npc_id);

    void runtime_ui_abi_panel(float x,
                              float y,
                              float width,
                              float height,
                              unsigned int fill_rgba,
                              float border_width);

    void runtime_ui_abi_text(float x, float y, const char *text);

    void runtime_ui_abi_end_frame(void);

    const unsigned char *runtime_ui_abi_get_framebuffer(void);

    void runtime_ui_abi_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif