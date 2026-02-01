#ifndef UI_STYLES_H
#define UI_STYLES_H

#include "lvgl.h"

// Color scheme - dark automotive dashboard
#define COLOR_BG_DARK       lv_color_hex(0x0a0a0f)
#define COLOR_BG_CARD       lv_color_hex(0x1a1a2e)
#define COLOR_BG_CARD_ALT   lv_color_hex(0x16213e)

// Accent colors
#define COLOR_POWER         lv_palette_main(LV_PALETTE_BLUE)
#define COLOR_SOLAR         lv_palette_main(LV_PALETTE_GREEN)
#define COLOR_GRID          lv_palette_main(LV_PALETTE_ORANGE)
#define COLOR_WATER         lv_palette_main(LV_PALETTE_CYAN)
#define COLOR_GAS           lv_palette_main(LV_PALETTE_PINK)
#define COLOR_TEMP          lv_palette_main(LV_PALETTE_TEAL)

// Text colors
#define COLOR_TEXT_PRIMARY  lv_color_hex(0xffffff)
#define COLOR_TEXT_SECONDARY lv_color_hex(0x8892a0)
#define COLOR_TEXT_DIM      lv_color_hex(0x4a5568)

// Status colors
#define COLOR_SUCCESS       lv_palette_main(LV_PALETTE_GREEN)
#define COLOR_WARNING       lv_palette_main(LV_PALETTE_ORANGE)
#define COLOR_ERROR         lv_palette_main(LV_PALETTE_RED)

// Style objects
extern lv_style_t style_screen_bg;
extern lv_style_t style_card;
extern lv_style_t style_card_dark;
extern lv_style_t style_title;
extern lv_style_t style_value_large;
extern lv_style_t style_value_medium;
extern lv_style_t style_value_small;
extern lv_style_t style_label_small;
extern lv_style_t style_unit;
extern lv_style_t style_status_bar;
extern lv_style_t style_btn;
extern lv_style_t style_indicator_bg;

void ui_styles_init(void);

#endif // UI_STYLES_H
