#ifndef UI_SCREENS_H
#define UI_SCREENS_H

#include "lvgl.h"
#include "mqtt_handler.h"

// Screen objects
extern lv_obj_t *screen_today;
extern lv_obj_t *screen_ytd;
extern lv_obj_t *screen_forecast;

// Widget handles for updating values
typedef struct {
    // Status bar
    lv_obj_t *label_date;
    lv_obj_t *label_time;
    lv_obj_t *label_wifi_status;

    // Today screen - Power consumption card
    lv_obj_t *arc_power;
    lv_obj_t *label_power_value;
    lv_obj_t *label_power_unit;

    // Today screen - Solar power card
    lv_obj_t *arc_solar;
    lv_obj_t *label_solar_power_value;
    lv_obj_t *label_solar_power_unit;

    // Today screen - Solar/Grid daily summary
    lv_obj_t *label_solar_value;
    lv_obj_t *label_grid_value;
    lv_obj_t *label_grid_cost;

    // Today screen - Gas card
    lv_obj_t *arc_gas;
    lv_obj_t *label_gas_value;
    lv_obj_t *label_gas_cost;

    // Today screen - Water card
    lv_obj_t *arc_water;
    lv_obj_t *label_water_value;

    // Today screen - Outdoor card
    lv_obj_t *label_outdoor_temp;
    lv_obj_t *label_humidity;
    lv_obj_t *label_weather_condition;

    // Today screen - Indoor card
    lv_obj_t *label_indoor_temp;
    lv_obj_t *label_humidity_indoor;

    // YTD screen - 4 column layout
    lv_obj_t *label_grid_ytd;
    lv_obj_t *label_grid_cost_ytd;
    lv_obj_t *label_solar_ytd;
    lv_obj_t *label_solar_cost_ytd;
    lv_obj_t *label_gas_ytd;
    lv_obj_t *label_gas_cost_ytd;
    lv_obj_t *label_water_ytd;
    lv_obj_t *label_water_cost_ytd;

    // YTD screen status bar
    lv_obj_t *label_date_ytd;
    lv_obj_t *label_time_ytd;
    lv_obj_t *label_wifi_status_ytd;

    // Navigation
    lv_obj_t *label_page_title;
    lv_obj_t *page_indicator_0;
    lv_obj_t *page_indicator_1;
    lv_obj_t *page_indicator_2;

    // Forecast screen
    lv_obj_t *label_forecast_temp;
    lv_obj_t *label_forecast_condition;
    lv_obj_t *label_forecast_humidity;
    lv_obj_t *label_forecast_pressure;
    lv_obj_t *label_forecast_wind;
    lv_obj_t *label_forecast_sunrise;
    lv_obj_t *label_forecast_sunset;
    lv_obj_t *label_day[7];
    lv_obj_t *label_day_high[7];
    lv_obj_t *label_day_low[7];
    lv_obj_t *bar_precip[7];
} ui_widgets_t;

extern ui_widgets_t ui_widgets;

void ui_create_screen_today(void);
void ui_create_screen_ytd(void);
void ui_create_screen_forecast(void);
void ui_screens_update(const sensor_data_t *data);
void ui_update_wifi_status(bool connected, int rssi);

#endif // UI_SCREENS_H
