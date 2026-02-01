/**
 * UI Main - Coordinates LVGL and screen management
 */

#include "ui.h"
#include "ui_styles.h"
#include "ui_screens.h"
#include "config.h"
#include "lvgl.h"
#include "esp_log.h"

static const char *TAG = "ui";

// Current screen pointers from ui_screens.c
extern lv_obj_t *screen_today;
extern lv_obj_t *screen_ytd;
extern lv_obj_t *screen_forecast;

void ui_init(void)
{
    ESP_LOGI(TAG, "Initializing UI");

    // Initialize styles
    ui_styles_init();

    // Create screens
    ui_create_screen_today();
    ui_create_screen_ytd();
    ui_create_screen_forecast();

    // Load the first screen
    lv_scr_load(screen_today);

    ESP_LOGI(TAG, "UI initialized");
}

void ui_update_time(int hour, int min, int day, int month, const char *weekday)
{
    char time_buf[16];
    char date_buf[32];

    snprintf(time_buf, sizeof(time_buf), "%02d:%02d", hour, min);
    snprintf(date_buf, sizeof(date_buf), "%s %d. %s",
             weekday, day,
             (const char*[]){"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}[month - 1]);

    if (ui_widgets.label_time) {
        lv_label_set_text(ui_widgets.label_time, time_buf);
    }
    if (ui_widgets.label_date) {
        lv_label_set_text(ui_widgets.label_date, date_buf);
    }
    // Also update YTD screen time
    if (ui_widgets.label_time_ytd) {
        lv_label_set_text(ui_widgets.label_time_ytd, time_buf);
    }
    if (ui_widgets.label_date_ytd) {
        lv_label_set_text(ui_widgets.label_date_ytd, date_buf);
    }
}

void ui_update_sensors(const sensor_data_t *data)
{
    ui_screens_update(data);
}

void ui_switch_screen(int screen_index)
{
    lv_obj_t *screens[] = {screen_today, screen_ytd, screen_forecast};

    if (screen_index >= 0 && screen_index < 3 && screens[screen_index]) {
        lv_scr_load_anim(screens[screen_index], LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    }
}
