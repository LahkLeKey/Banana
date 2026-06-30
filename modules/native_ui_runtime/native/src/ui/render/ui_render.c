#include "ui_internal.h"
#include <string.h>
#include <stdlib.h>

#ifdef _OPENMP
#include <omp.h>
#endif

/* ─────────────────────────────────────────────────────────────────────────
   5×7 Bitmap Font (column-major, public domain)

   95 printable ASCII characters: 0x20 (' ') through 0x7E ('~').
   Each glyph = 5 bytes (one per column, left-to-right).
   In each byte: bit 0 = top row, bit 6 = bottom row.
   ───────────────────────────────────────────────────────────────────────── */

#define FONT_GLYPH_W  5
#define FONT_GLYPH_H  7
#define FONT_SCALE    2   /* scale factor: each logical pixel → NxN screen pixels */
#define FONT_CHAR_W   (FONT_GLYPH_W * FONT_SCALE)
#define FONT_CHAR_H   (FONT_GLYPH_H * FONT_SCALE)
#define FONT_KERN     FONT_SCALE   /* inter-character gap */

static const unsigned char FONT_5X7[95][5] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* 0x20 ' ' */
    { 0x00, 0x00, 0x5F, 0x00, 0x00 }, /* 0x21 '!' */
    { 0x00, 0x07, 0x00, 0x07, 0x00 }, /* 0x22 '"' */
    { 0x14, 0x7F, 0x14, 0x7F, 0x14 }, /* 0x23 '#' */
    { 0x24, 0x2A, 0x7F, 0x2A, 0x12 }, /* 0x24 '$' */
    { 0x23, 0x13, 0x08, 0x64, 0x62 }, /* 0x25 '%' */
    { 0x36, 0x49, 0x55, 0x22, 0x50 }, /* 0x26 '&' */
    { 0x00, 0x05, 0x03, 0x00, 0x00 }, /* 0x27 '\'' */
    { 0x00, 0x1C, 0x22, 0x41, 0x00 }, /* 0x28 '(' */
    { 0x00, 0x41, 0x22, 0x1C, 0x00 }, /* 0x29 ')' */
    { 0x14, 0x08, 0x3E, 0x08, 0x14 }, /* 0x2A '*' */
    { 0x08, 0x08, 0x3E, 0x08, 0x08 }, /* 0x2B '+' */
    { 0x00, 0x50, 0x30, 0x00, 0x00 }, /* 0x2C ',' */
    { 0x08, 0x08, 0x08, 0x08, 0x08 }, /* 0x2D '-' */
    { 0x00, 0x60, 0x60, 0x00, 0x00 }, /* 0x2E '.' */
    { 0x20, 0x10, 0x08, 0x04, 0x02 }, /* 0x2F '/' */
    { 0x3E, 0x51, 0x49, 0x45, 0x3E }, /* 0x30 '0' */
    { 0x00, 0x42, 0x7F, 0x40, 0x00 }, /* 0x31 '1' */
    { 0x42, 0x61, 0x51, 0x49, 0x46 }, /* 0x32 '2' */
    { 0x21, 0x41, 0x45, 0x4B, 0x31 }, /* 0x33 '3' */
    { 0x18, 0x14, 0x12, 0x7F, 0x10 }, /* 0x34 '4' */
    { 0x27, 0x45, 0x45, 0x45, 0x39 }, /* 0x35 '5' */
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 }, /* 0x36 '6' */
    { 0x01, 0x71, 0x09, 0x05, 0x03 }, /* 0x37 '7' */
    { 0x36, 0x49, 0x49, 0x49, 0x36 }, /* 0x38 '8' */
    { 0x06, 0x49, 0x49, 0x29, 0x1E }, /* 0x39 '9' */
    { 0x00, 0x36, 0x36, 0x00, 0x00 }, /* 0x3A ':' */
    { 0x00, 0x56, 0x36, 0x00, 0x00 }, /* 0x3B ';' */
    { 0x08, 0x14, 0x22, 0x41, 0x00 }, /* 0x3C '<' */
    { 0x14, 0x14, 0x14, 0x14, 0x14 }, /* 0x3D '=' */
    { 0x00, 0x41, 0x22, 0x14, 0x08 }, /* 0x3E '>' */
    { 0x02, 0x01, 0x51, 0x09, 0x06 }, /* 0x3F '?' */
    { 0x32, 0x49, 0x79, 0x41, 0x3E }, /* 0x40 '@' */
    { 0x7E, 0x11, 0x11, 0x11, 0x7E }, /* 0x41 'A' */
    { 0x7F, 0x49, 0x49, 0x49, 0x36 }, /* 0x42 'B' */
    { 0x3E, 0x41, 0x41, 0x41, 0x22 }, /* 0x43 'C' */
    { 0x7F, 0x41, 0x41, 0x22, 0x1C }, /* 0x44 'D' */
    { 0x7F, 0x49, 0x49, 0x49, 0x41 }, /* 0x45 'E' */
    { 0x7F, 0x09, 0x09, 0x09, 0x01 }, /* 0x46 'F' */
    { 0x3E, 0x41, 0x49, 0x49, 0x7A }, /* 0x47 'G' */
    { 0x7F, 0x08, 0x08, 0x08, 0x7F }, /* 0x48 'H' */
    { 0x00, 0x41, 0x7F, 0x41, 0x00 }, /* 0x49 'I' */
    { 0x20, 0x40, 0x41, 0x3F, 0x01 }, /* 0x4A 'J' */
    { 0x7F, 0x08, 0x14, 0x22, 0x41 }, /* 0x4B 'K' */
    { 0x7F, 0x40, 0x40, 0x40, 0x40 }, /* 0x4C 'L' */
    { 0x7F, 0x02, 0x04, 0x02, 0x7F }, /* 0x4D 'M' */
    { 0x7F, 0x04, 0x08, 0x10, 0x7F }, /* 0x4E 'N' */
    { 0x3E, 0x41, 0x41, 0x41, 0x3E }, /* 0x4F 'O' */
    { 0x7F, 0x09, 0x09, 0x09, 0x06 }, /* 0x50 'P' */
    { 0x3E, 0x41, 0x51, 0x21, 0x5E }, /* 0x51 'Q' */
    { 0x7F, 0x09, 0x19, 0x29, 0x46 }, /* 0x52 'R' */
    { 0x46, 0x49, 0x49, 0x49, 0x31 }, /* 0x53 'S' */
    { 0x01, 0x01, 0x7F, 0x01, 0x01 }, /* 0x54 'T' */
    { 0x3F, 0x40, 0x40, 0x40, 0x3F }, /* 0x55 'U' */
    { 0x1F, 0x20, 0x40, 0x20, 0x1F }, /* 0x56 'V' */
    { 0x3F, 0x40, 0x38, 0x40, 0x3F }, /* 0x57 'W' */
    { 0x63, 0x14, 0x08, 0x14, 0x63 }, /* 0x58 'X' */
    { 0x07, 0x08, 0x70, 0x08, 0x07 }, /* 0x59 'Y' */
    { 0x61, 0x51, 0x49, 0x45, 0x43 }, /* 0x5A 'Z' */
    { 0x00, 0x7F, 0x41, 0x41, 0x00 }, /* 0x5B '[' */
    { 0x02, 0x04, 0x08, 0x10, 0x20 }, /* 0x5C '\\' */
    { 0x00, 0x41, 0x41, 0x7F, 0x00 }, /* 0x5D ']' */
    { 0x04, 0x02, 0x01, 0x02, 0x04 }, /* 0x5E '^' */
    { 0x40, 0x40, 0x40, 0x40, 0x40 }, /* 0x5F '_' */
    { 0x00, 0x01, 0x02, 0x04, 0x00 }, /* 0x60 '`' */
    { 0x20, 0x54, 0x54, 0x54, 0x78 }, /* 0x61 'a' */
    { 0x7F, 0x48, 0x44, 0x44, 0x38 }, /* 0x62 'b' */
    { 0x38, 0x44, 0x44, 0x44, 0x20 }, /* 0x63 'c' */
    { 0x38, 0x44, 0x44, 0x48, 0x7F }, /* 0x64 'd' */
    { 0x38, 0x54, 0x54, 0x54, 0x18 }, /* 0x65 'e' */
    { 0x08, 0x7E, 0x09, 0x01, 0x02 }, /* 0x66 'f' */
    { 0x0C, 0x52, 0x52, 0x52, 0x3E }, /* 0x67 'g' */
    { 0x7F, 0x08, 0x04, 0x04, 0x78 }, /* 0x68 'h' */
    { 0x00, 0x44, 0x7D, 0x40, 0x00 }, /* 0x69 'i' */
    { 0x20, 0x40, 0x44, 0x3D, 0x00 }, /* 0x6A 'j' */
    { 0x7F, 0x10, 0x28, 0x44, 0x00 }, /* 0x6B 'k' */
    { 0x00, 0x41, 0x7F, 0x40, 0x00 }, /* 0x6C 'l' */
    { 0x7C, 0x04, 0x18, 0x04, 0x78 }, /* 0x6D 'm' */
    { 0x7C, 0x08, 0x04, 0x04, 0x78 }, /* 0x6E 'n' */
    { 0x38, 0x44, 0x44, 0x44, 0x38 }, /* 0x6F 'o' */
    { 0x7C, 0x14, 0x14, 0x14, 0x08 }, /* 0x70 'p' */
    { 0x08, 0x14, 0x14, 0x18, 0x7C }, /* 0x71 'q' */
    { 0x7C, 0x08, 0x04, 0x04, 0x08 }, /* 0x72 'r' */
    { 0x48, 0x54, 0x54, 0x54, 0x20 }, /* 0x73 's' */
    { 0x04, 0x3F, 0x44, 0x40, 0x20 }, /* 0x74 't' */
    { 0x3C, 0x40, 0x40, 0x20, 0x7C }, /* 0x75 'u' */
    { 0x1C, 0x20, 0x40, 0x20, 0x1C }, /* 0x76 'v' */
    { 0x3C, 0x40, 0x30, 0x40, 0x3C }, /* 0x77 'w' */
    { 0x44, 0x28, 0x10, 0x28, 0x44 }, /* 0x78 'x' */
    { 0x0C, 0x50, 0x50, 0x50, 0x3C }, /* 0x79 'y' */
    { 0x44, 0x64, 0x54, 0x4C, 0x44 }, /* 0x7A 'z' */
    { 0x00, 0x08, 0x36, 0x41, 0x00 }, /* 0x7B '{' */
    { 0x00, 0x00, 0x7F, 0x00, 0x00 }, /* 0x7C '|' */
    { 0x00, 0x41, 0x36, 0x08, 0x00 }, /* 0x7D '}' */
    { 0x10, 0x08, 0x08, 0x10, 0x08 }, /* 0x7E '~' */
};

/* ─────────────────────────────────────────────────────────────────────────
   Low-level pixel / rect helpers
   ───────────────────────────────────────────────────────────────────────── */

static inline void ui_set_pixel(UIContext *ctx, int x, int y,
                                unsigned char r, unsigned char g,
                                unsigned char b, unsigned char a) {
    if (x < 0 || y < 0 || x >= ctx->framebuffer_width || y >= ctx->framebuffer_height)
        return;
    unsigned char *p = ctx->framebuffer + (y * ctx->framebuffer_width + x) * 4;
    p[0] = r;
    p[1] = g;
    p[2] = b;
    p[3] = a;
}

/* Fill an axis-aligned rectangle. Rows parallelized with OpenMP when large. */
static void ui_fill_rect(UIContext *ctx, int x, int y, int w, int h,
                         unsigned char r, unsigned char g,
                         unsigned char b, unsigned char a) {
    if (w <= 0 || h <= 0) return;

    /* Clamp to framebuffer bounds */
    int x1 = x     < 0                       ? 0                       : x;
    int y1 = y     < 0                       ? 0                       : y;
    int x2 = x + w > ctx->framebuffer_width  ? ctx->framebuffer_width  : x + w;
    int y2 = y + h > ctx->framebuffer_height ? ctx->framebuffer_height : y + h;
    if (x1 >= x2 || y1 >= y2) return;

    int row_w = x2 - x1;

#ifdef _OPENMP
    int py;
    #pragma omp parallel for schedule(static) if(h > 16)
    for (py = y1; py < y2; py++) {
#else
    for (int py = y1; py < y2; py++) {
#endif
        unsigned char *row = ctx->framebuffer + (py * ctx->framebuffer_width + x1) * 4;
        for (int i = 0; i < row_w; i++) {
            row[i * 4 + 0] = r;
            row[i * 4 + 1] = g;
            row[i * 4 + 2] = b;
            row[i * 4 + 3] = a;
        }
    }
}

/* Draw a rectangle outline with given thickness. */
static void ui_stroke_rect(UIContext *ctx, int x, int y, int w, int h,
                           int thickness,
                           unsigned char r, unsigned char g,
                           unsigned char b, unsigned char a) {
    if (thickness <= 0 || w <= 0 || h <= 0) return;
    int t = thickness;
    /* Top / bottom */
    ui_fill_rect(ctx, x,         y,         w,  t, r, g, b, a);
    ui_fill_rect(ctx, x,         y + h - t, w,  t, r, g, b, a);
    /* Left / right (avoid corner overlap) */
    ui_fill_rect(ctx, x,         y + t,     t,  h - 2 * t, r, g, b, a);
    ui_fill_rect(ctx, x + w - t, y + t,     t,  h - 2 * t, r, g, b, a);
}

/* ─────────────────────────────────────────────────────────────────────────
   Text rendering helpers
   ───────────────────────────────────────────────────────────────────────── */

/* Pixel width of a string at FONT_SCALE. */
static int ui_string_pixel_width(const char *text) {
    if (!text) return 0;
    int len = 0;
    while (*text++) len++;
    if (len == 0) return 0;
    return len * (FONT_CHAR_W + FONT_KERN) - FONT_KERN;
}

/* Draw a single ASCII character at screen coordinates (cx, cy) = top-left.
   Characters outside ASCII 32–126 are skipped. */
static void ui_draw_char(UIContext *ctx, int cx, int cy, char ch,
                         unsigned char r, unsigned char g,
                         unsigned char b, unsigned char a) {
    unsigned char code = (unsigned char)ch;
    if (code < 0x20 || code > 0x7E) return;

    const unsigned char *glyph = FONT_5X7[code - 0x20];

    for (int col = 0; col < FONT_GLYPH_W; col++) {
        unsigned char bits = glyph[col];
        for (int row = 0; row < FONT_GLYPH_H; row++) {
            if (bits & (1 << row)) {
                /* Each logical pixel → FONT_SCALE × FONT_SCALE screen pixels */
                int sx = cx + col * FONT_SCALE;
                int sy = cy + row * FONT_SCALE;
                for (int dy = 0; dy < FONT_SCALE; dy++)
                    for (int dx = 0; dx < FONT_SCALE; dx++)
                        ui_set_pixel(ctx, sx + dx, sy + dy, r, g, b, a);
            }
        }
    }
}

/* Draw a string starting at (x, y). */
static void ui_draw_string(UIContext *ctx, int x, int y, const char *text,
                           unsigned char r, unsigned char g,
                           unsigned char b, unsigned char a) {
    if (!text) return;
    int pen_x = x;
    while (*text) {
        ui_draw_char(ctx, pen_x, y, *text, r, g, b, a);
        pen_x += FONT_CHAR_W + FONT_KERN;
        text++;
    }
}

/* ─────────────────────────────────────────────────────────────────────────
   Element renderers
   ───────────────────────────────────────────────────────────────────────── */

/* Button color palette */
#define BTN_BG_R  0x2A
#define BTN_BG_G  0x3A
#define BTN_BG_B  0x5A
#define BTN_BG_A  0xFF
#define BTN_BD_R  0x6A
#define BTN_BD_G  0x9A
#define BTN_BD_B  0xCA
#define BTN_BD_A  0xFF
#define BTN_TX_R  0xFF
#define BTN_TX_G  0xFF
#define BTN_TX_B  0xFF
#define BTN_TX_A  0xFF

void ui_render_button(UIContext *ctx, UIElement *elem) {
    int x = (int)elem->x;
    int y = (int)elem->y;
    int w = (int)elem->width;
    int h = (int)elem->height;

    /* Background */
    ui_fill_rect(ctx, x, y, w, h, BTN_BG_R, BTN_BG_G, BTN_BG_B, BTN_BG_A);
    /* Border */
    ui_stroke_rect(ctx, x, y, w, h, 1, BTN_BD_R, BTN_BD_G, BTN_BD_B, BTN_BD_A);

    /* Label centered */
    const char *label = elem->data.button.label;
    if (label && *label) {
        int text_w = ui_string_pixel_width(label);
        int tx = x + (w - text_w) / 2;
        int ty = y + (h - FONT_CHAR_H) / 2;
        ui_draw_string(ctx, tx, ty, label, BTN_TX_R, BTN_TX_G, BTN_TX_B, BTN_TX_A);
    }
}

/* Panel color: fill_rgba = 0xRRGGBBAA */
void ui_render_panel(UIContext *ctx, UIElement *elem) {
    int x = (int)elem->x;
    int y = (int)elem->y;
    int w = (int)elem->width;
    int h = (int)elem->height;

    uint32_t fill = elem->data.panel.fill_rgba;
    unsigned char fr = (unsigned char)((fill >> 24) & 0xFF);
    unsigned char fg = (unsigned char)((fill >> 16) & 0xFF);
    unsigned char fb = (unsigned char)((fill >>  8) & 0xFF);
    unsigned char fa = (unsigned char)((fill)        & 0xFF);

    if (fa > 0)
        ui_fill_rect(ctx, x, y, w, h, fr, fg, fb, fa);

    int border = (int)elem->data.panel.border_width;
    if (border > 0) {
        /* Border is slightly brighter than fill */
        unsigned char br = fr < 235 ? fr + 20 : 255;
        unsigned char bg = fg < 235 ? fg + 20 : 255;
        unsigned char bb = fb < 235 ? fb + 20 : 255;
        ui_stroke_rect(ctx, x, y, w, h, border, br, bg, bb, fa);
    }
}

/* Text element: white text */
void ui_render_text(UIContext *ctx, UIElement *elem) {
    const char *text = elem->data.text.text;
    if (!text) return;
    ui_draw_string(ctx, (int)elem->x, (int)elem->y, text,
                   0xFF, 0xFF, 0xFF, 0xFF);
}

/* Text field: dark background + border + placeholder or content */
void ui_render_text_field(UIContext *ctx, UIElement *elem) {
    int x = (int)elem->x;
    int y = (int)elem->y;
    int w = (int)elem->width;
    int h = (int)elem->height;

    /* Background */
    ui_fill_rect(ctx, x, y, w, h, 0x1A, 0x1A, 0x2A, 0xFF);
    /* Border */
    unsigned char bd_r = elem->data.text_field.is_focused ? 0x80 : 0x4A;
    unsigned char bd_g = elem->data.text_field.is_focused ? 0xC0 : 0x6A;
    unsigned char bd_b = elem->data.text_field.is_focused ? 0xFF : 0x8A;
    ui_stroke_rect(ctx, x, y, w, h, 1, bd_r, bd_g, bd_b, 0xFF);

    /* Content or cursor */
    char *buf = elem->data.text_field.buffer;
    if (buf && buf[0] != '\0') {
        ui_draw_string(ctx, x + 4, y + (h - FONT_CHAR_H) / 2, buf,
                       0xFF, 0xFF, 0xFF, 0xFF);
    }
}

/* Inventory panel: dark HUD bar with resource labels */
void ui_render_inventory_panel(UIContext *ctx, UIElement *elem) {
    int w = 160, h = 56;
    int x = (int)elem->x - w;
    int y = (int)elem->y;

    /* Panel backing */
    ui_fill_rect(ctx, x, y, w, h, 0x10, 0x18, 0x28, 0xE0);
    ui_stroke_rect(ctx, x, y, w, h, 1, 0x40, 0x60, 0x90, 0xFF);

    /* Resource labels (stacked) */
    ui_draw_string(ctx, x + 8, y + 6,  "Wood:  --", 0xC8, 0x9A, 0x4A, 0xFF);
    ui_draw_string(ctx, x + 8, y + 22, "Ore:   --", 0xA0, 0xB0, 0xC8, 0xFF);
    ui_draw_string(ctx, x + 8, y + 38, "Gold:  --", 0xFF, 0xD7, 0x00, 0xFF);
}

/* Merchant dialog: centered trading window */
void ui_render_merchant_dialog(UIContext *ctx, UIElement *elem) {
    int w = 300, h = 200;
    int x = (int)elem->x - w / 2;
    int y = (int)elem->y - h / 2;

    /* Window background */
    ui_fill_rect(ctx, x, y, w, h, 0x1C, 0x24, 0x38, 0xF0);
    ui_stroke_rect(ctx, x, y, w, h, 2, 0x50, 0x78, 0xB0, 0xFF);

    /* Title bar */
    ui_fill_rect(ctx, x + 2, y + 2, w - 4, FONT_CHAR_H + 8,
                 0x28, 0x38, 0x58, 0xFF);
    ui_draw_string(ctx, x + 8, y + 6, "Merchant", 0xFF, 0xD7, 0x00, 0xFF);

    /* Divider */
    ui_fill_rect(ctx, x + 2, y + FONT_CHAR_H + 12, w - 4, 1,
                 0x50, 0x78, 0xB0, 0xFF);

    /* Body text */
    ui_draw_string(ctx, x + 8, y + FONT_CHAR_H + 20,
                   "Buy  /  Sell", 0xCC, 0xCC, 0xCC, 0xFF);
}