/**
 * UI Styles - Dark dashboard theme
 */

#include "ui_styles.h"

// Style definitions
lv_style_t style_screen_bg;
lv_style_t style_card;
lv_style_t style_card_dark;
lv_style_t style_title;
lv_style_t style_value_large;
lv_style_t style_value_medium;
lv_style_t style_value_small;
lv_style_t style_label_small;
lv_style_t style_unit;
lv_style_t style_status_bar;
lv_style_t style_btn;
lv_style_t style_indicator_bg;

void ui_styles_init(void)
{
    // Screen background
    lv_style_init(&style_screen_bg);
    lv_style_set_bg_color(&style_screen_bg, COLOR_BG_DARK);
    lv_style_set_bg_opa(&style_screen_bg, LV_OPA_COVER);

    // Card style
    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, COLOR_BG_CARD);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_radius(&style_card, 12);
    lv_style_set_pad_all(&style_card, 12);
    lv_style_set_border_width(&style_card, 0);

    // Dark card variant
    lv_style_init(&style_card_dark);
    lv_style_set_bg_color(&style_card_dark, COLOR_BG_CARD_ALT);
    lv_style_set_bg_opa(&style_card_dark, LV_OPA_COVER);
    lv_style_set_radius(&style_card_dark, 12);
    lv_style_set_pad_all(&style_card_dark, 12);
    lv_style_set_border_width(&style_card_dark, 0);

    // Title style
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, COLOR_TEXT_SECONDARY);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_14);

    // Large value (temperature, main readings)
    lv_style_init(&style_value_large);
    lv_style_set_text_color(&style_value_large, COLOR_TEXT_PRIMARY);
    lv_style_set_text_font(&style_value_large, &lv_font_montserrat_48);

    // Medium value
    lv_style_init(&style_value_medium);
    lv_style_set_text_color(&style_value_medium, COLOR_TEXT_PRIMARY);
    lv_style_set_text_font(&style_value_medium, &lv_font_montserrat_32);

    // Small value
    lv_style_init(&style_value_small);
    lv_style_set_text_color(&style_value_small, COLOR_TEXT_PRIMARY);
    lv_style_set_text_font(&style_value_small, &lv_font_montserrat_20);

    // Small label
    lv_style_init(&style_label_small);
    lv_style_set_text_color(&style_label_small, COLOR_TEXT_SECONDARY);
    lv_style_set_text_font(&style_label_small, &lv_font_montserrat_14);

    // Unit style
    lv_style_init(&style_unit);
    lv_style_set_text_color(&style_unit, COLOR_TEXT_DIM);
    lv_style_set_text_font(&style_unit, &lv_font_montserrat_20);

    // Status bar
    lv_style_init(&style_status_bar);
    lv_style_set_bg_color(&style_status_bar, lv_color_hex(0x12121a));
    lv_style_set_bg_opa(&style_status_bar, LV_OPA_COVER);
    lv_style_set_border_width(&style_status_bar, 0);
    lv_style_set_pad_all(&style_status_bar, 8);
    lv_style_set_radius(&style_status_bar, 0);

    // Button style
    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, COLOR_BG_CARD);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_radius(&style_btn, 8);
    lv_style_set_border_width(&style_btn, 0);

    // Indicator background
    lv_style_init(&style_indicator_bg);
    lv_style_set_bg_color(&style_indicator_bg, lv_color_hex(0x2a2a3e));
    lv_style_set_bg_opa(&style_indicator_bg, LV_OPA_COVER);
    lv_style_set_radius(&style_indicator_bg, 4);
}
