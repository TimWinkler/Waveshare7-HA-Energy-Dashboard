/**
 * UI Screens - Modern card-based dashboard layout
 */

#include "ui_screens.h"
#include "ui_styles.h"
#include "ui.h"
#include "config.h"
#include <stdio.h>

// Screen objects
lv_obj_t *screen_today = NULL;
lv_obj_t *screen_ytd = NULL;
lv_obj_t *screen_forecast = NULL;

// Widget handles
ui_widgets_t ui_widgets = {0};

// Current screen index
static int current_screen = 0;

// Forward declarations
static void screen_gesture_cb(lv_event_t *e);

//=============================================================================
// Helper: Create a metric card
//=============================================================================
static lv_obj_t *create_metric_card(lv_obj_t *parent, int x, int y, int w, int h)
{
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, w, h);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    return card;
}

//=============================================================================
// Power Cards
//=============================================================================
static void create_power_card(lv_obj_t *parent)
{
    // LEFT: Power Consumption Card
    lv_obj_t *card_consumption = create_metric_card(parent, 16, 50, 140, 185);

    lv_obj_t *title1 = lv_label_create(card_consumption);
    lv_obj_add_style(title1, &style_title, 0);
    lv_label_set_text(title1, "GRID");
    lv_obj_align(title1, LV_ALIGN_TOP_MID, 0, 0);

    ui_widgets.arc_power = lv_arc_create(card_consumption);
    lv_obj_set_size(ui_widgets.arc_power, 100, 100);
    lv_obj_align(ui_widgets.arc_power, LV_ALIGN_CENTER, 0, 5);
    lv_arc_set_rotation(ui_widgets.arc_power, 135);
    lv_arc_set_bg_angles(ui_widgets.arc_power, 0, 270);
    lv_arc_set_range(ui_widgets.arc_power, 0, 5000);
    lv_arc_set_value(ui_widgets.arc_power, 0);

    lv_obj_set_style_arc_color(ui_widgets.arc_power, COLOR_GRID, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_widgets.arc_power, 10, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_widgets.arc_power, true, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ui_widgets.arc_power, lv_color_hex(0x2a2a3e), LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_widgets.arc_power, 10, LV_PART_MAIN);
    lv_obj_remove_style(ui_widgets.arc_power, NULL, LV_PART_KNOB);

    ui_widgets.label_power_value = lv_label_create(card_consumption);
    lv_obj_add_style(ui_widgets.label_power_value, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_power_value, "----");
    lv_obj_align_to(ui_widgets.label_power_value, ui_widgets.arc_power, LV_ALIGN_CENTER, -10, -5);

    ui_widgets.label_power_unit = lv_label_create(card_consumption);
    lv_obj_add_style(ui_widgets.label_power_unit, &style_label_small, 0);
    lv_label_set_text(ui_widgets.label_power_unit, "W");
    lv_obj_align_to(ui_widgets.label_power_unit, ui_widgets.label_power_value, LV_ALIGN_OUT_BOTTOM_MID, 10, 0);

    // RIGHT: Solar Power Card
    lv_obj_t *card_solar = create_metric_card(parent, 162, 50, 140, 185);

    lv_obj_t *title2 = lv_label_create(card_solar);
    lv_obj_add_style(title2, &style_title, 0);
    lv_label_set_text(title2, "SOLAR");
    lv_obj_align(title2, LV_ALIGN_TOP_MID, 0, 0);

    ui_widgets.arc_solar = lv_arc_create(card_solar);
    lv_obj_set_size(ui_widgets.arc_solar, 100, 100);
    lv_obj_align(ui_widgets.arc_solar, LV_ALIGN_CENTER, 0, 5);
    lv_arc_set_rotation(ui_widgets.arc_solar, 135);
    lv_arc_set_bg_angles(ui_widgets.arc_solar, 0, 270);
    lv_arc_set_range(ui_widgets.arc_solar, 0, 800);
    lv_arc_set_value(ui_widgets.arc_solar, 0);

    lv_obj_set_style_arc_color(ui_widgets.arc_solar, COLOR_SOLAR, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_widgets.arc_solar, 10, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_widgets.arc_solar, true, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ui_widgets.arc_solar, lv_color_hex(0x2a2a3e), LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_widgets.arc_solar, 10, LV_PART_MAIN);
    lv_obj_remove_style(ui_widgets.arc_solar, NULL, LV_PART_KNOB);

    ui_widgets.label_solar_power_value = lv_label_create(card_solar);
    lv_obj_add_style(ui_widgets.label_solar_power_value, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_solar_power_value, "----");
    lv_obj_align_to(ui_widgets.label_solar_power_value, ui_widgets.arc_solar, LV_ALIGN_CENTER, 0, -5);

    ui_widgets.label_solar_power_unit = lv_label_create(card_solar);
    lv_obj_add_style(ui_widgets.label_solar_power_unit, &style_label_small, 0);
    lv_label_set_text(ui_widgets.label_solar_power_unit, "W");
    lv_obj_align_to(ui_widgets.label_solar_power_unit, ui_widgets.label_solar_power_value, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    // Daily Summary Card
    lv_obj_t *summary_card = create_metric_card(parent, 16, 241, 286, 149);

    lv_obj_t *summary_title = lv_label_create(summary_card);
    lv_obj_add_style(summary_title, &style_title, 0);
    lv_label_set_text(summary_title, "TODAY");
    lv_obj_align(summary_title, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *grid_icon = lv_label_create(summary_card);
    lv_obj_set_style_text_font(grid_icon, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(grid_icon, COLOR_GRID, 0);
    lv_label_set_text(grid_icon, LV_SYMBOL_DOWN);
    lv_obj_align(grid_icon, LV_ALIGN_TOP_LEFT, 15, 40);

    lv_obj_t *grid_label = lv_label_create(summary_card);
    lv_obj_add_style(grid_label, &style_label_small, 0);
    lv_label_set_text(grid_label, "Grid");
    lv_obj_align(grid_label, LV_ALIGN_TOP_LEFT, 50, 45);

    ui_widgets.label_grid_value = lv_label_create(summary_card);
    lv_obj_add_style(ui_widgets.label_grid_value, &style_value_small, 0);
    lv_obj_set_style_text_color(ui_widgets.label_grid_value, COLOR_GRID, 0);
    lv_label_set_text(ui_widgets.label_grid_value, "-- kWh");
    lv_obj_align(ui_widgets.label_grid_value, LV_ALIGN_TOP_LEFT, 15, 70);

    ui_widgets.label_grid_cost = lv_label_create(summary_card);
    lv_obj_add_style(ui_widgets.label_grid_cost, &style_label_small, 0);
    lv_obj_set_style_text_color(ui_widgets.label_grid_cost, COLOR_GRID, 0);
    lv_label_set_text(ui_widgets.label_grid_cost, "-- EUR");
    lv_obj_align(ui_widgets.label_grid_cost, LV_ALIGN_TOP_LEFT, 15, 95);

    lv_obj_t *solar_icon = lv_label_create(summary_card);
    lv_obj_set_style_text_font(solar_icon, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(solar_icon, COLOR_SOLAR, 0);
    lv_label_set_text(solar_icon, LV_SYMBOL_UP);
    lv_obj_align(solar_icon, LV_ALIGN_TOP_LEFT, 150, 40);

    lv_obj_t *solar_label = lv_label_create(summary_card);
    lv_obj_add_style(solar_label, &style_label_small, 0);
    lv_label_set_text(solar_label, "Solar");
    lv_obj_align(solar_label, LV_ALIGN_TOP_LEFT, 185, 45);

    ui_widgets.label_solar_value = lv_label_create(summary_card);
    lv_obj_add_style(ui_widgets.label_solar_value, &style_value_small, 0);
    lv_obj_set_style_text_color(ui_widgets.label_solar_value, COLOR_SOLAR, 0);
    lv_label_set_text(ui_widgets.label_solar_value, "-- kWh");
    lv_obj_align(ui_widgets.label_solar_value, LV_ALIGN_TOP_LEFT, 150, 70);
}

//=============================================================================
// Gas Card
//=============================================================================
static void create_gas_card(lv_obj_t *parent)
{
    lv_obj_t *card = create_metric_card(parent, 312, 50, 236, 165);

    lv_obj_t *title = lv_label_create(card);
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_style_text_color(title, COLOR_GAS, 0);
    lv_label_set_text(title, "GAS");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    ui_widgets.arc_gas = lv_arc_create(card);
    lv_obj_set_size(ui_widgets.arc_gas, 100, 100);
    lv_obj_align(ui_widgets.arc_gas, LV_ALIGN_LEFT_MID, 10, 10);
    lv_arc_set_rotation(ui_widgets.arc_gas, 135);
    lv_arc_set_bg_angles(ui_widgets.arc_gas, 0, 270);
    lv_arc_set_range(ui_widgets.arc_gas, 0, 20);
    lv_arc_set_value(ui_widgets.arc_gas, 0);

    lv_obj_set_style_arc_color(ui_widgets.arc_gas, COLOR_GAS, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_widgets.arc_gas, 10, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_widgets.arc_gas, true, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ui_widgets.arc_gas, lv_color_hex(0x2a2a3e), LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_widgets.arc_gas, 10, LV_PART_MAIN);
    lv_obj_remove_style(ui_widgets.arc_gas, NULL, LV_PART_KNOB);

    ui_widgets.label_gas_value = lv_label_create(card);
    lv_obj_add_style(ui_widgets.label_gas_value, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_gas_value, "-.-");
    lv_obj_align_to(ui_widgets.label_gas_value, ui_widgets.arc_gas, LV_ALIGN_CENTER, 0, -8);

    lv_obj_t *gas_unit = lv_label_create(card);
    lv_obj_add_style(gas_unit, &style_label_small, 0);
    lv_label_set_text(gas_unit, "m3");
    lv_obj_align_to(gas_unit, ui_widgets.label_gas_value, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t *cost_label = lv_label_create(card);
    lv_obj_add_style(cost_label, &style_label_small, 0);
    lv_label_set_text(cost_label, "Cost");
    lv_obj_align(cost_label, LV_ALIGN_TOP_RIGHT, -30, 35);

    ui_widgets.label_gas_cost = lv_label_create(card);
    lv_obj_add_style(ui_widgets.label_gas_cost, &style_value_medium, 0);
    lv_obj_set_style_text_color(ui_widgets.label_gas_cost, COLOR_GAS, 0);
    lv_label_set_text(ui_widgets.label_gas_cost, "---");
    lv_obj_align(ui_widgets.label_gas_cost, LV_ALIGN_RIGHT_MID, -20, 10);

    lv_obj_t *euro = lv_label_create(card);
    lv_obj_add_style(euro, &style_label_small, 0);
    lv_label_set_text(euro, "EUR");
    lv_obj_align_to(euro, ui_widgets.label_gas_cost, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
}

//=============================================================================
// Water Card
//=============================================================================
static void create_water_card(lv_obj_t *parent)
{
    lv_obj_t *card = create_metric_card(parent, 312, 225, 236, 165);

    lv_obj_t *title = lv_label_create(card);
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_style_text_color(title, COLOR_WATER, 0);
    lv_label_set_text(title, "WATER");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    ui_widgets.arc_water = lv_arc_create(card);
    lv_obj_set_size(ui_widgets.arc_water, 100, 100);
    lv_obj_align(ui_widgets.arc_water, LV_ALIGN_CENTER, 0, 15);
    lv_arc_set_rotation(ui_widgets.arc_water, 135);
    lv_arc_set_bg_angles(ui_widgets.arc_water, 0, 270);
    lv_arc_set_range(ui_widgets.arc_water, 0, 500);
    lv_arc_set_value(ui_widgets.arc_water, 0);

    lv_obj_set_style_arc_color(ui_widgets.arc_water, COLOR_WATER, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(ui_widgets.arc_water, 10, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(ui_widgets.arc_water, true, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ui_widgets.arc_water, lv_color_hex(0x2a2a3e), LV_PART_MAIN);
    lv_obj_set_style_arc_width(ui_widgets.arc_water, 10, LV_PART_MAIN);
    lv_obj_remove_style(ui_widgets.arc_water, NULL, LV_PART_KNOB);

    ui_widgets.label_water_value = lv_label_create(card);
    lv_obj_add_style(ui_widgets.label_water_value, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_water_value, "---");
    lv_obj_align_to(ui_widgets.label_water_value, ui_widgets.arc_water, LV_ALIGN_CENTER, 0, -8);

    lv_obj_t *water_unit = lv_label_create(card);
    lv_obj_add_style(water_unit, &style_label_small, 0);
    lv_label_set_text(water_unit, "L");
    lv_obj_align_to(water_unit, ui_widgets.label_water_value, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}

//=============================================================================
// Outdoor Card
//=============================================================================
static void create_outdoor_card(lv_obj_t *parent)
{
    lv_obj_t *card = create_metric_card(parent, 558, 50, 226, 165);

    lv_obj_t *icon = lv_label_create(card);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(icon, COLOR_SOLAR, 0);
    lv_label_set_text(icon, LV_SYMBOL_EYE_OPEN);
    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 5, 5);

    lv_obj_t *title = lv_label_create(card);
    lv_obj_add_style(title, &style_title, 0);
    lv_label_set_text(title, "OUTDOOR");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 25, 3);

    ui_widgets.label_weather_condition = lv_label_create(card);
    lv_obj_add_style(ui_widgets.label_weather_condition, &style_label_small, 0);
    lv_obj_set_style_text_color(ui_widgets.label_weather_condition, COLOR_SOLAR, 0);
    lv_label_set_text(ui_widgets.label_weather_condition, "");
    lv_obj_align(ui_widgets.label_weather_condition, LV_ALIGN_TOP_RIGHT, -5, 22);

    ui_widgets.label_outdoor_temp = lv_label_create(card);
    lv_obj_add_style(ui_widgets.label_outdoor_temp, &style_value_large, 0);
    lv_label_set_text(ui_widgets.label_outdoor_temp, "--.-");
    lv_obj_align(ui_widgets.label_outdoor_temp, LV_ALIGN_CENTER, -20, -5);

    lv_obj_t *outdoor_unit = lv_label_create(card);
    lv_obj_add_style(outdoor_unit, &style_unit, 0);
    lv_label_set_text(outdoor_unit, "\xc2\xb0" "C");
    lv_obj_align_to(outdoor_unit, ui_widgets.label_outdoor_temp, LV_ALIGN_OUT_RIGHT_TOP, 8, 10);

    lv_obj_t *humid_icon = lv_label_create(card);
    lv_obj_set_style_text_font(humid_icon, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(humid_icon, COLOR_WATER, 0);
    lv_label_set_text(humid_icon, LV_SYMBOL_TINT);
    lv_obj_align(humid_icon, LV_ALIGN_BOTTOM_LEFT, 20, -15);

    ui_widgets.label_humidity = lv_label_create(card);
    lv_obj_add_style(ui_widgets.label_humidity, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_humidity, "-- %");
    lv_obj_align(ui_widgets.label_humidity, LV_ALIGN_BOTTOM_LEFT, 45, -15);
}

//=============================================================================
// Indoor Card
//=============================================================================
static void create_indoor_card(lv_obj_t *parent)
{
    lv_obj_t *card = create_metric_card(parent, 558, 225, 226, 165);

    lv_obj_t *icon = lv_label_create(card);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(icon, COLOR_TEMP, 0);
    lv_label_set_text(icon, LV_SYMBOL_HOME);
    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 5, 5);

    lv_obj_t *title = lv_label_create(card);
    lv_obj_add_style(title, &style_title, 0);
    lv_label_set_text(title, "INDOOR");
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 25, 3);

    ui_widgets.label_indoor_temp = lv_label_create(card);
    lv_obj_add_style(ui_widgets.label_indoor_temp, &style_value_large, 0);
    lv_label_set_text(ui_widgets.label_indoor_temp, "--.-");
    lv_obj_align(ui_widgets.label_indoor_temp, LV_ALIGN_CENTER, -20, -5);

    lv_obj_t *indoor_unit = lv_label_create(card);
    lv_obj_add_style(indoor_unit, &style_unit, 0);
    lv_label_set_text(indoor_unit, "\xc2\xb0" "C");
    lv_obj_align_to(indoor_unit, ui_widgets.label_indoor_temp, LV_ALIGN_OUT_RIGHT_TOP, 8, 10);

    lv_obj_t *humid_icon = lv_label_create(card);
    lv_obj_set_style_text_font(humid_icon, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(humid_icon, COLOR_WATER, 0);
    lv_label_set_text(humid_icon, LV_SYMBOL_TINT);
    lv_obj_align(humid_icon, LV_ALIGN_BOTTOM_LEFT, 20, -15);

    ui_widgets.label_humidity_indoor = lv_label_create(card);
    lv_obj_add_style(ui_widgets.label_humidity_indoor, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_humidity_indoor, "-- %");
    lv_obj_align(ui_widgets.label_humidity_indoor, LV_ALIGN_BOTTOM_LEFT, 45, -15);
}

//=============================================================================
// Status Bar
//=============================================================================
static void create_status_bar(lv_obj_t *parent)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_set_size(bar, LCD_WIDTH, 40);
    lv_obj_set_pos(bar, 0, 0);
    lv_obj_add_style(bar, &style_status_bar, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    ui_widgets.label_date = lv_label_create(bar);
    lv_obj_add_style(ui_widgets.label_date, &style_label_small, 0);
    lv_label_set_text(ui_widgets.label_date, "-- --. ---");
    lv_obj_align(ui_widgets.label_date, LV_ALIGN_LEFT_MID, 10, 0);

    ui_widgets.label_time = lv_label_create(bar);
    lv_obj_add_style(ui_widgets.label_time, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_time, "--:--");
    lv_obj_align(ui_widgets.label_time, LV_ALIGN_CENTER, 0, 0);

    ui_widgets.label_wifi_status = lv_label_create(bar);
    lv_obj_add_style(ui_widgets.label_wifi_status, &style_label_small, 0);
    lv_label_set_text(ui_widgets.label_wifi_status, LV_SYMBOL_WIFI);
    lv_obj_align(ui_widgets.label_wifi_status, LV_ALIGN_RIGHT_MID, -10, 0);
}

//=============================================================================
// Navigation Bar
//=============================================================================
static void create_nav_bar(lv_obj_t *parent, int active_page)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_set_size(bar, LCD_WIDTH, 40);
    lv_obj_set_pos(bar, 0, LCD_HEIGHT - 40);
    lv_obj_add_style(bar, &style_status_bar, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    const char *page_titles[] = {"Today", "This Year", "Forecast"};
    ui_widgets.label_page_title = lv_label_create(bar);
    lv_obj_add_style(ui_widgets.label_page_title, &style_label_small, 0);
    lv_label_set_text(ui_widgets.label_page_title, page_titles[active_page]);
    lv_obj_align(ui_widgets.label_page_title, LV_ALIGN_CENTER, 0, -5);

    // 3 page indicators
    ui_widgets.page_indicator_0 = lv_obj_create(bar);
    lv_obj_set_size(ui_widgets.page_indicator_0, 8, 8);
    lv_obj_align(ui_widgets.page_indicator_0, LV_ALIGN_CENTER, -20, 12);
    lv_obj_set_style_radius(ui_widgets.page_indicator_0, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui_widgets.page_indicator_0,
        active_page == 0 ? COLOR_TEXT_PRIMARY : COLOR_TEXT_DIM, 0);
    lv_obj_set_style_bg_opa(ui_widgets.page_indicator_0, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(ui_widgets.page_indicator_0, 0, 0);

    ui_widgets.page_indicator_1 = lv_obj_create(bar);
    lv_obj_set_size(ui_widgets.page_indicator_1, 8, 8);
    lv_obj_align(ui_widgets.page_indicator_1, LV_ALIGN_CENTER, 0, 12);
    lv_obj_set_style_radius(ui_widgets.page_indicator_1, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui_widgets.page_indicator_1,
        active_page == 1 ? COLOR_TEXT_PRIMARY : COLOR_TEXT_DIM, 0);
    lv_obj_set_style_bg_opa(ui_widgets.page_indicator_1, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(ui_widgets.page_indicator_1, 0, 0);

    ui_widgets.page_indicator_2 = lv_obj_create(bar);
    lv_obj_set_size(ui_widgets.page_indicator_2, 8, 8);
    lv_obj_align(ui_widgets.page_indicator_2, LV_ALIGN_CENTER, 20, 12);
    lv_obj_set_style_radius(ui_widgets.page_indicator_2, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ui_widgets.page_indicator_2,
        active_page == 2 ? COLOR_TEXT_PRIMARY : COLOR_TEXT_DIM, 0);
    lv_obj_set_style_bg_opa(ui_widgets.page_indicator_2, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(ui_widgets.page_indicator_2, 0, 0);
}

//=============================================================================
// Gesture handler
//=============================================================================
static void screen_gesture_cb(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

    if (dir == LV_DIR_LEFT) {
        current_screen = (current_screen + 1) % 3;
        ui_switch_screen(current_screen);
    } else if (dir == LV_DIR_RIGHT) {
        current_screen = (current_screen + 2) % 3;
        ui_switch_screen(current_screen);
    }
}

//=============================================================================
// Create Today Screen
//=============================================================================
void ui_create_screen_today(void)
{
    screen_today = lv_obj_create(NULL);
    lv_obj_add_style(screen_today, &style_screen_bg, 0);
    lv_obj_clear_flag(screen_today, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(screen_today, screen_gesture_cb, LV_EVENT_GESTURE, NULL);
    lv_obj_clear_flag(screen_today, LV_OBJ_FLAG_GESTURE_BUBBLE);

    create_status_bar(screen_today);
    create_power_card(screen_today);
    create_gas_card(screen_today);
    create_water_card(screen_today);
    create_outdoor_card(screen_today);
    create_indoor_card(screen_today);
    create_nav_bar(screen_today, 0);
}

//=============================================================================
// Helper: Create a YTD column
//=============================================================================
static lv_obj_t *create_ytd_column(lv_obj_t *parent, int x, int y, int w, int h,
                                    const char *title, lv_color_t color,
                                    const char *icon, const char *unit,
                                    lv_obj_t **label_value, lv_obj_t **label_cost)
{
    lv_obj_t *card = create_metric_card(parent, x, y, w, h);

    lv_obj_t *lbl_title = lv_label_create(card);
    lv_obj_add_style(lbl_title, &style_title, 0);
    lv_obj_set_style_text_color(lbl_title, color, 0);
    lv_label_set_text(lbl_title, title);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 5);

    lv_obj_t *lbl_icon = lv_label_create(card);
    lv_obj_set_style_text_font(lbl_icon, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(lbl_icon, color, 0);
    lv_label_set_text(lbl_icon, icon);
    lv_obj_align(lbl_icon, LV_ALIGN_TOP_MID, 0, 35);

    *label_value = lv_label_create(card);
    lv_obj_add_style(*label_value, &style_value_medium, 0);
    lv_obj_set_style_text_color(*label_value, COLOR_TEXT_PRIMARY, 0);
    lv_label_set_text(*label_value, "----");
    lv_obj_align(*label_value, LV_ALIGN_CENTER, 0, 25);

    lv_obj_t *lbl_unit = lv_label_create(card);
    lv_obj_add_style(lbl_unit, &style_label_small, 0);
    lv_label_set_text(lbl_unit, unit);
    lv_obj_align_to(lbl_unit, *label_value, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);

    *label_cost = lv_label_create(card);
    lv_obj_add_style(*label_cost, &style_value_small, 0);
    lv_obj_set_style_text_color(*label_cost, color, 0);
    lv_label_set_text(*label_cost, "---.-");
    lv_obj_align(*label_cost, LV_ALIGN_BOTTOM_MID, 0, -30);

    lv_obj_t *lbl_eur = lv_label_create(card);
    lv_obj_add_style(lbl_eur, &style_label_small, 0);
    lv_label_set_text(lbl_eur, "EUR");
    lv_obj_align_to(lbl_eur, *label_cost, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);

    return card;
}

//=============================================================================
// Create YTD Screen - 4 column layout
//=============================================================================
void ui_create_screen_ytd(void)
{
    screen_ytd = lv_obj_create(NULL);
    lv_obj_add_style(screen_ytd, &style_screen_bg, 0);
    lv_obj_clear_flag(screen_ytd, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(screen_ytd, screen_gesture_cb, LV_EVENT_GESTURE, NULL);
    lv_obj_clear_flag(screen_ytd, LV_OBJ_FLAG_GESTURE_BUBBLE);

    // Status bar
    lv_obj_t *bar = lv_obj_create(screen_ytd);
    lv_obj_set_size(bar, LCD_WIDTH, 40);
    lv_obj_set_pos(bar, 0, 0);
    lv_obj_add_style(bar, &style_status_bar, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    ui_widgets.label_date_ytd = lv_label_create(bar);
    lv_obj_add_style(ui_widgets.label_date_ytd, &style_label_small, 0);
    lv_label_set_text(ui_widgets.label_date_ytd, "-- --. ---");
    lv_obj_align(ui_widgets.label_date_ytd, LV_ALIGN_LEFT_MID, 10, 0);

    ui_widgets.label_time_ytd = lv_label_create(bar);
    lv_obj_add_style(ui_widgets.label_time_ytd, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_time_ytd, "--:--");
    lv_obj_align(ui_widgets.label_time_ytd, LV_ALIGN_CENTER, 0, 0);

    ui_widgets.label_wifi_status_ytd = lv_label_create(bar);
    lv_obj_add_style(ui_widgets.label_wifi_status_ytd, &style_label_small, 0);
    lv_label_set_text(ui_widgets.label_wifi_status_ytd, LV_SYMBOL_WIFI);
    lv_obj_align(ui_widgets.label_wifi_status_ytd, LV_ALIGN_RIGHT_MID, -10, 0);

    // 4 columns: 180px wide each
    const int col_width = 180;
    const int col_height = 340;
    const int col_y = 50;
    const int col_x[4] = {16, 212, 408, 604};

    create_ytd_column(screen_ytd, col_x[0], col_y, col_width, col_height,
                      "GRID", COLOR_GRID, LV_SYMBOL_CHARGE, "kWh",
                      &ui_widgets.label_grid_ytd, &ui_widgets.label_grid_cost_ytd);

    create_ytd_column(screen_ytd, col_x[1], col_y, col_width, col_height,
                      "PV", COLOR_SOLAR, LV_SYMBOL_UP, "kWh",
                      &ui_widgets.label_solar_ytd, &ui_widgets.label_solar_cost_ytd);

    create_ytd_column(screen_ytd, col_x[2], col_y, col_width, col_height,
                      "GAS", COLOR_GAS, LV_SYMBOL_WARNING, "m3",
                      &ui_widgets.label_gas_ytd, &ui_widgets.label_gas_cost_ytd);

    create_ytd_column(screen_ytd, col_x[3], col_y, col_width, col_height,
                      "WATER", COLOR_WATER, LV_SYMBOL_TINT, "L",
                      &ui_widgets.label_water_ytd, &ui_widgets.label_water_cost_ytd);

    // Navigation bar
    lv_obj_t *nav = lv_obj_create(screen_ytd);
    lv_obj_set_size(nav, LCD_WIDTH, 40);
    lv_obj_set_pos(nav, 0, LCD_HEIGHT - 40);
    lv_obj_add_style(nav, &style_status_bar, 0);
    lv_obj_clear_flag(nav, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *nav_title = lv_label_create(nav);
    lv_obj_add_style(nav_title, &style_label_small, 0);
    lv_label_set_text(nav_title, "This Year");
    lv_obj_align(nav_title, LV_ALIGN_CENTER, 0, -5);

    for (int i = 0; i < 3; i++) {
        lv_obj_t *dot = lv_obj_create(nav);
        lv_obj_set_size(dot, 8, 8);
        lv_obj_align(dot, LV_ALIGN_CENTER, (i - 1) * 20, 12);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dot, (i == 1) ? COLOR_TEXT_PRIMARY : COLOR_TEXT_DIM, 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(dot, 0, 0);
    }
}

//=============================================================================
// Create Forecast Screen
//=============================================================================
void ui_create_screen_forecast(void)
{
    screen_forecast = lv_obj_create(NULL);
    lv_obj_add_style(screen_forecast, &style_screen_bg, 0);
    lv_obj_clear_flag(screen_forecast, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(screen_forecast, screen_gesture_cb, LV_EVENT_GESTURE, NULL);
    lv_obj_clear_flag(screen_forecast, LV_OBJ_FLAG_GESTURE_BUBBLE);

    lv_obj_t *current_card = create_metric_card(screen_forecast, 16, 50, 768, 150);

    ui_widgets.label_forecast_temp = lv_label_create(current_card);
    lv_obj_add_style(ui_widgets.label_forecast_temp, &style_value_large, 0);
    lv_label_set_text(ui_widgets.label_forecast_temp, "--.-");
    lv_obj_align(ui_widgets.label_forecast_temp, LV_ALIGN_TOP_LEFT, 80, 20);

    lv_obj_t *temp_unit = lv_label_create(current_card);
    lv_obj_add_style(temp_unit, &style_unit, 0);
    lv_label_set_text(temp_unit, "C");
    lv_obj_align_to(temp_unit, ui_widgets.label_forecast_temp, LV_ALIGN_OUT_RIGHT_TOP, 5, 5);

    ui_widgets.label_forecast_condition = lv_label_create(current_card);
    lv_obj_add_style(ui_widgets.label_forecast_condition, &style_label_small, 0);
    lv_label_set_text(ui_widgets.label_forecast_condition, "---");
    lv_obj_align(ui_widgets.label_forecast_condition, LV_ALIGN_TOP_LEFT, 80, 75);

    lv_obj_t *humid_icon = lv_label_create(current_card);
    lv_obj_set_style_text_font(humid_icon, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(humid_icon, COLOR_WATER, 0);
    lv_label_set_text(humid_icon, LV_SYMBOL_TINT);
    lv_obj_align(humid_icon, LV_ALIGN_TOP_LEFT, 200, 20);

    ui_widgets.label_forecast_humidity = lv_label_create(current_card);
    lv_obj_add_style(ui_widgets.label_forecast_humidity, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_forecast_humidity, "-- %");
    lv_obj_align(ui_widgets.label_forecast_humidity, LV_ALIGN_TOP_LEFT, 220, 18);

    ui_widgets.label_forecast_pressure = lv_label_create(current_card);
    lv_obj_add_style(ui_widgets.label_forecast_pressure, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_forecast_pressure, "----");
    lv_obj_align(ui_widgets.label_forecast_pressure, LV_ALIGN_TOP_LEFT, 240, 48);

    lv_obj_t *sunrise_icon = lv_label_create(current_card);
    lv_obj_add_style(sunrise_icon, &style_label_small, 0);
    lv_obj_set_style_text_color(sunrise_icon, COLOR_SOLAR, 0);
    lv_label_set_text(sunrise_icon, LV_SYMBOL_UP);
    lv_obj_align(sunrise_icon, LV_ALIGN_TOP_LEFT, 350, 20);

    ui_widgets.label_forecast_sunrise = lv_label_create(current_card);
    lv_obj_add_style(ui_widgets.label_forecast_sunrise, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_forecast_sunrise, "--:--");
    lv_obj_align(ui_widgets.label_forecast_sunrise, LV_ALIGN_TOP_LEFT, 375, 18);

    lv_obj_t *sunset_icon = lv_label_create(current_card);
    lv_obj_add_style(sunset_icon, &style_label_small, 0);
    lv_obj_set_style_text_color(sunset_icon, COLOR_GRID, 0);
    lv_label_set_text(sunset_icon, LV_SYMBOL_DOWN);
    lv_obj_align(sunset_icon, LV_ALIGN_TOP_LEFT, 350, 50);

    ui_widgets.label_forecast_sunset = lv_label_create(current_card);
    lv_obj_add_style(ui_widgets.label_forecast_sunset, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_forecast_sunset, "--:--");
    lv_obj_align(ui_widgets.label_forecast_sunset, LV_ALIGN_TOP_LEFT, 375, 48);

    ui_widgets.label_forecast_wind = lv_label_create(current_card);
    lv_obj_add_style(ui_widgets.label_forecast_wind, &style_value_small, 0);
    lv_label_set_text(ui_widgets.label_forecast_wind, "-- km/h");
    lv_obj_align(ui_widgets.label_forecast_wind, LV_ALIGN_TOP_LEFT, 550, 18);

    // 7-day forecast
    lv_obj_t *forecast_card = create_metric_card(screen_forecast, 16, 210, 768, 200);

    const char *day_names[] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
    int x_start = 20;
    int x_spacing = 105;

    for (int i = 0; i < 7; i++) {
        int x_pos = x_start + (i * x_spacing);

        ui_widgets.label_day[i] = lv_label_create(forecast_card);
        lv_obj_add_style(ui_widgets.label_day[i], &style_label_small, 0);
        lv_label_set_text(ui_widgets.label_day[i], day_names[i]);
        lv_obj_align(ui_widgets.label_day[i], LV_ALIGN_TOP_LEFT, x_pos, 10);

        ui_widgets.label_day_high[i] = lv_label_create(forecast_card);
        lv_obj_add_style(ui_widgets.label_day_high[i], &style_value_small, 0);
        lv_obj_set_style_text_color(ui_widgets.label_day_high[i], COLOR_GRID, 0);
        lv_label_set_text(ui_widgets.label_day_high[i], "--");
        lv_obj_align(ui_widgets.label_day_high[i], LV_ALIGN_TOP_LEFT, x_pos, 35);

        ui_widgets.label_day_low[i] = lv_label_create(forecast_card);
        lv_obj_add_style(ui_widgets.label_day_low[i], &style_value_small, 0);
        lv_obj_set_style_text_color(ui_widgets.label_day_low[i], COLOR_WATER, 0);
        lv_label_set_text(ui_widgets.label_day_low[i], "--");
        lv_obj_align(ui_widgets.label_day_low[i], LV_ALIGN_TOP_LEFT, x_pos, 60);

        ui_widgets.bar_precip[i] = lv_bar_create(forecast_card);
        lv_obj_set_size(ui_widgets.bar_precip[i], 30, 80);
        lv_obj_align(ui_widgets.bar_precip[i], LV_ALIGN_TOP_LEFT, x_pos + 15, 90);
        lv_bar_set_range(ui_widgets.bar_precip[i], 0, 20);
        lv_bar_set_value(ui_widgets.bar_precip[i], 0, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(ui_widgets.bar_precip[i], lv_color_hex(0x2a2a3e), LV_PART_MAIN);
        lv_obj_set_style_bg_color(ui_widgets.bar_precip[i], COLOR_WATER, LV_PART_INDICATOR);
    }

    // Navigation bar
    lv_obj_t *nav = lv_obj_create(screen_forecast);
    lv_obj_set_size(nav, LCD_WIDTH, 50);
    lv_obj_set_pos(nav, 0, LCD_HEIGHT - 60);
    lv_obj_add_style(nav, &style_status_bar, 0);
    lv_obj_clear_flag(nav, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *nav_title = lv_label_create(nav);
    lv_obj_add_style(nav_title, &style_label_small, 0);
    lv_label_set_text(nav_title, "Forecast");
    lv_obj_align(nav_title, LV_ALIGN_CENTER, 0, 0);

    for (int i = 0; i < 3; i++) {
        lv_obj_t *dot = lv_obj_create(nav);
        lv_obj_set_size(dot, 8, 8);
        lv_obj_align(dot, LV_ALIGN_CENTER, (i - 1) * 20, 15);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dot, (i == 2) ? COLOR_TEXT_PRIMARY : COLOR_TEXT_DIM, 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(dot, 0, 0);
    }
}

//=============================================================================
// Update screen values
//=============================================================================
void ui_screens_update(const sensor_data_t *data)
{
    char buf[32];

    // Power arc and value
    if (ui_widgets.arc_power) {
        int power_val = (int)data->current_power;
        if (power_val < 0) power_val = 0;
        if (power_val > 5000) power_val = 5000;
        lv_arc_set_value(ui_widgets.arc_power, power_val);
    }

    if (ui_widgets.label_power_value) {
        snprintf(buf, sizeof(buf), "%d", (int)data->current_power);
        lv_label_set_text(ui_widgets.label_power_value, buf);
    }

    // Solar power
    if (ui_widgets.arc_solar) {
        int solar_val = (int)data->solar_power;
        if (solar_val < 0) solar_val = 0;
        if (solar_val > 800) solar_val = 800;
        lv_arc_set_value(ui_widgets.arc_solar, solar_val);
    }

    if (ui_widgets.label_solar_power_value) {
        snprintf(buf, sizeof(buf), "%d", (int)data->solar_power);
        lv_label_set_text(ui_widgets.label_solar_power_value, buf);
    }

    // Daily values
    if (ui_widgets.label_solar_value) {
        snprintf(buf, sizeof(buf), "%.1f kWh", data->solar_daily);
        lv_label_set_text(ui_widgets.label_solar_value, buf);
    }

    if (ui_widgets.label_grid_value) {
        snprintf(buf, sizeof(buf), "%.1f kWh", data->grid_daily);
        lv_label_set_text(ui_widgets.label_grid_value, buf);
    }

    if (ui_widgets.label_grid_cost) {
        float grid_cost = data->grid_daily * 0.30f;
        snprintf(buf, sizeof(buf), "%.2f EUR", grid_cost);
        lv_label_set_text(ui_widgets.label_grid_cost, buf);
    }

    // Gas
    if (ui_widgets.arc_gas) {
        int gas_val = (int)(data->gas_consumption);
        if (gas_val < 0) gas_val = 0;
        if (gas_val > 20) gas_val = 20;
        lv_arc_set_value(ui_widgets.arc_gas, gas_val);
    }

    if (ui_widgets.label_gas_value) {
        snprintf(buf, sizeof(buf), "%.1f", data->gas_consumption);
        lv_label_set_text(ui_widgets.label_gas_value, buf);
    }

    if (ui_widgets.label_gas_cost) {
        float gas_cost = data->gas_consumption * 2.2f;
        snprintf(buf, sizeof(buf), "%.2f", gas_cost);
        lv_label_set_text(ui_widgets.label_gas_cost, buf);
    }

    // Water
    if (ui_widgets.arc_water) {
        int water_val = (int)data->water_daily;
        if (water_val < 0) water_val = 0;
        if (water_val > 500) water_val = 500;
        lv_arc_set_value(ui_widgets.arc_water, water_val);
    }

    if (ui_widgets.label_water_value) {
        snprintf(buf, sizeof(buf), "%d", (int)data->water_daily);
        lv_label_set_text(ui_widgets.label_water_value, buf);
    }

    // Temperatures
    if (ui_widgets.label_indoor_temp) {
        snprintf(buf, sizeof(buf), "%.1f", data->temp_indoor);
        lv_label_set_text(ui_widgets.label_indoor_temp, buf);
    }

    if (ui_widgets.label_humidity_indoor) {
        snprintf(buf, sizeof(buf), "%d %%", (int)data->humidity_indoor);
        lv_label_set_text(ui_widgets.label_humidity_indoor, buf);
    }

    if (ui_widgets.label_outdoor_temp) {
        snprintf(buf, sizeof(buf), "%.1f", data->temp_outdoor);
        lv_label_set_text(ui_widgets.label_outdoor_temp, buf);
    }

    if (ui_widgets.label_humidity) {
        snprintf(buf, sizeof(buf), "%d %%", (int)data->humidity_outdoor);
        lv_label_set_text(ui_widgets.label_humidity, buf);
    }

    if (ui_widgets.label_weather_condition && data->weather_condition[0] != '\0') {
        lv_label_set_text(ui_widgets.label_weather_condition, data->weather_condition);
    }

    // YTD values
    if (ui_widgets.label_grid_ytd) {
        snprintf(buf, sizeof(buf), "%.0f", data->grid_ytd);
        lv_label_set_text(ui_widgets.label_grid_ytd, buf);
    }
    if (ui_widgets.label_grid_cost_ytd) {
        float grid_cost_ytd = data->grid_ytd * 0.30f;
        snprintf(buf, sizeof(buf), "%.2f", grid_cost_ytd);
        lv_label_set_text(ui_widgets.label_grid_cost_ytd, buf);
    }

    if (ui_widgets.label_solar_ytd) {
        snprintf(buf, sizeof(buf), "%.1f", data->solar_ytd);
        lv_label_set_text(ui_widgets.label_solar_ytd, buf);
    }
    if (ui_widgets.label_solar_cost_ytd) {
        float solar_savings_ytd = data->solar_ytd * 0.30f;
        snprintf(buf, sizeof(buf), "%.2f", solar_savings_ytd);
        lv_label_set_text(ui_widgets.label_solar_cost_ytd, buf);
    }

    if (ui_widgets.label_gas_ytd) {
        snprintf(buf, sizeof(buf), "%.1f", data->gas_ytd);
        lv_label_set_text(ui_widgets.label_gas_ytd, buf);
    }
    if (ui_widgets.label_gas_cost_ytd) {
        float gas_cost_ytd = data->gas_ytd * 2.20f;
        snprintf(buf, sizeof(buf), "%.2f", gas_cost_ytd);
        lv_label_set_text(ui_widgets.label_gas_cost_ytd, buf);
    }

    if (ui_widgets.label_water_ytd) {
        snprintf(buf, sizeof(buf), "%.0f", data->water_ytd);
        lv_label_set_text(ui_widgets.label_water_ytd, buf);
    }
    if (ui_widgets.label_water_cost_ytd) {
        float water_cost_ytd = data->water_ytd / 1000.0f * 5.00f;
        snprintf(buf, sizeof(buf), "%.2f", water_cost_ytd);
        lv_label_set_text(ui_widgets.label_water_cost_ytd, buf);
    }

    // Sync YTD screen status bar
    if (ui_widgets.label_date_ytd && ui_widgets.label_date) {
        lv_label_set_text(ui_widgets.label_date_ytd, lv_label_get_text(ui_widgets.label_date));
    }
    if (ui_widgets.label_time_ytd && ui_widgets.label_time) {
        lv_label_set_text(ui_widgets.label_time_ytd, lv_label_get_text(ui_widgets.label_time));
    }

    // Forecast screen
    if (ui_widgets.label_forecast_temp) {
        snprintf(buf, sizeof(buf), "%.1f", data->temp_outdoor);
        lv_label_set_text(ui_widgets.label_forecast_temp, buf);
    }
    if (ui_widgets.label_forecast_condition && data->weather_condition[0] != '\0') {
        lv_label_set_text(ui_widgets.label_forecast_condition, data->weather_condition);
    }
    if (ui_widgets.label_forecast_humidity) {
        snprintf(buf, sizeof(buf), "%.0f %%", data->humidity_outdoor);
        lv_label_set_text(ui_widgets.label_forecast_humidity, buf);
    }
    if (ui_widgets.label_forecast_pressure) {
        snprintf(buf, sizeof(buf), "%.0f", data->pressure);
        lv_label_set_text(ui_widgets.label_forecast_pressure, buf);
    }
    if (ui_widgets.label_forecast_wind) {
        snprintf(buf, sizeof(buf), "%.0f km/h", data->wind_speed);
        lv_label_set_text(ui_widgets.label_forecast_wind, buf);
    }
    if (ui_widgets.label_forecast_sunrise && data->sunrise[0] != '\0') {
        lv_label_set_text(ui_widgets.label_forecast_sunrise, data->sunrise);
    }
    if (ui_widgets.label_forecast_sunset && data->sunset[0] != '\0') {
        lv_label_set_text(ui_widgets.label_forecast_sunset, data->sunset);
    }

    // 7-day forecast
    for (int i = 0; i < 7; i++) {
        if (ui_widgets.label_day_high[i]) {
            snprintf(buf, sizeof(buf), "%.0f", data->forecast[i].temp_high);
            lv_label_set_text(ui_widgets.label_day_high[i], buf);
        }
        if (ui_widgets.label_day_low[i]) {
            snprintf(buf, sizeof(buf), "%.0f", data->forecast[i].temp_low);
            lv_label_set_text(ui_widgets.label_day_low[i], buf);
        }
        if (ui_widgets.bar_precip[i]) {
            int precip = (int)data->forecast[i].precipitation;
            if (precip > 20) precip = 20;
            lv_bar_set_value(ui_widgets.bar_precip[i], precip, LV_ANIM_OFF);
        }
    }
}

//=============================================================================
// Update WiFi status
//=============================================================================
void ui_update_wifi_status(bool connected, int rssi)
{
    lv_color_t wifi_color;

    if (!connected) {
        wifi_color = lv_color_hex(0xff5555);
    } else {
        if (rssi >= -50) {
            wifi_color = lv_color_hex(0x55ff55);
        } else if (rssi >= -60) {
            wifi_color = lv_color_hex(0x88ff88);
        } else if (rssi >= -70) {
            wifi_color = lv_color_hex(0xffff55);
        } else {
            wifi_color = lv_color_hex(0xffaa55);
        }
    }

    if (ui_widgets.label_wifi_status != NULL) {
        lv_label_set_text(ui_widgets.label_wifi_status, LV_SYMBOL_WIFI);
        lv_obj_set_style_text_color(ui_widgets.label_wifi_status, wifi_color, 0);
    }

    if (ui_widgets.label_wifi_status_ytd != NULL) {
        lv_label_set_text(ui_widgets.label_wifi_status_ytd, LV_SYMBOL_WIFI);
        lv_obj_set_style_text_color(ui_widgets.label_wifi_status_ytd, wifi_color, 0);
    }
}
