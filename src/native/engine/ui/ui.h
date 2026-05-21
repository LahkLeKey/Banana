#ifndef BANANA_ENGINE_UI_H
#define BANANA_ENGINE_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* ─────────────────────────────────────────────────────────────────────────
   Immediate-Mode UI Context
   
   The UI system is immediate-mode: call ui_begin_frame(), define elements
   via function calls (which queue rendering commands), then ui_end_frame()
   to render to the framebuffer. State (button hover, input focus, etc.) is
   maintained by the context across frames.
   
   Rendering is parallelized with OpenMP where applicable.
   ───────────────────────────────────────────────────────────────────────── */

typedef struct UIContext UIContext;

/* Callback function signature: called when UI element is interacted with.
   user_data: arbitrary pointer passed at element creation
   return: 0 to consume event, non-zero to propagate */
typedef int (*UICallback)(void *user_data);

/* ─── Initialization ─────────────────────────────────────────────────────── */

/* Create a UI context. Must be called after OpenGL context is ready.
   framebuffer_width/height: dimensions of target framebuffer
   Returns NULL on failure. */
UIContext *ui_context_create(int framebuffer_width, int framebuffer_height);

/* Destroy UI context and all queued elements. */
void ui_context_destroy(UIContext *ctx);

/* ─── Frame Lifecycle ────────────────────────────────────────────────────── */

/* Begin a new UI frame. Clears queued elements, processes input from last frame.
   Call once per render frame, before defining any UI elements. */
void ui_begin_frame(UIContext *ctx);

/* End the current UI frame: flatten element queue, render to framebuffer.
   Call once per render frame, after all ui_*() element calls. */
void ui_end_frame(UIContext *ctx);

/* ─── Input Events ───────────────────────────────────────────────────────── */

/* Queue a mouse click event (world-space coordinates).
   Called by engine/window to feed input to UI system. */
void ui_input_click(UIContext *ctx, float x, float y);

/* Queue a key press event (for text input, menu navigation).
   key_code: ASCII code or special key ID */
void ui_input_key(UIContext *ctx, int key_code);

/* ─── Basic Elements (Immediate-Mode) ────────────────────────────────────── */

/* Draw a button. Returns 1 if clicked this frame, 0 otherwise.
   x, y: screen-space position (top-left)
   width, height: dimensions
   label: button text
   callback: function to call on click (can be NULL)
   user_data: arbitrary data passed to callback */
int ui_button(UIContext *ctx, float x, float y, float width, float height,
              const char *label, UICallback callback, void *user_data);

/* Draw text. Always visible (no interaction).
   x, y: screen-space position
   text: string to render */
void ui_text(UIContext *ctx, float x, float y, const char *text);

/* Draw a text field (for input). Returns 1 if just activated, 0 otherwise.
   x, y: screen-space position
   width, height: dimensions
   placeholder: hint text when empty
   out_buffer: pointer to char[256] buffer to hold input
   buffer_size: size of out_buffer (typically 256)
   max_length: maximum characters to accept */
int ui_text_field(UIContext *ctx, float x, float y, float width, float height,
                   const char *placeholder, char *out_buffer, size_t buffer_size,
                   size_t max_length);

/* Draw a panel (rounded rect with optional fill/border).
   x, y: screen-space position
   width, height: dimensions
   fill_rgba: fill color as 0xRRGGBBAA (0 for transparent)
   border_width: border thickness in pixels (0 for no border) */
void ui_panel(UIContext *ctx, float x, float y, float width, float height,
              uint32_t fill_rgba, float border_width);

/* ─── Widgets (Specialized Elements) ──────────────────────────────────────── */

/* Draw inventory panel showing player resources (wood, ore, gold).
   x, y: screen-space position (top-right anchor point)
   Returns: non-zero if any resource was clicked for interaction
   
   Queries player_registry directly via native engine integration. */
int ui_inventory_panel(UIContext *ctx, float x, float y);

/* Draw merchant trading dialog.
   x, y: screen-space position (center)
   npc_id: merchant NPC to trade with
   
   Shows merchant inventory (buy options) and player gold.
   Returns: trade result if a transaction was initiated, 0 otherwise
   
   Calls npc_merchant_trade_buy/sell directly via callbacks. */
int ui_merchant_dialog(UIContext *ctx, float x, float y, int npc_id);

/* Draw quest log panel (list of active quests with status).
   x, y: screen-space position
   
   For future expansion; currently stub. */
void ui_quest_log_panel(UIContext *ctx, float x, float y);

/* ─── Rendering ──────────────────────────────────────────────────────────── */

/* Get pointer to rendered UI framebuffer (RGBA 32-bit pixels).
   Size = framebuffer_width * framebuffer_height * 4 bytes
   Valid until next ui_end_frame() call. */
const unsigned char *ui_get_framebuffer(UIContext *ctx);

/* Resize UI framebuffer (call on window resize). */
void ui_resize(UIContext *ctx, int new_width, int new_height);

/* ─── Parallel Configuration ─────────────────────────────────────────────── */

/* Set OpenMP thread budget for UI rendering.
   max_threads: maximum threads to use (1 = single-threaded, 0 = auto)
   
   Default: auto (uses omp_get_max_threads()).
   Call before ui_begin_frame() for effect. */
void ui_set_thread_budget(UIContext *ctx, int max_threads);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_UI_H */
