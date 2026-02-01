#ifndef UI_H
#define UI_H

#include "mqtt_handler.h"

/**
 * Initialize the UI system
 */
void ui_init(void);

/**
 * Update time display
 */
void ui_update_time(int hour, int min, int day, int month, const char *weekday);

/**
 * Update sensor data display
 */
void ui_update_sensors(const sensor_data_t *data);

/**
 * Switch to a specific screen (0=Today, 1=YTD, 2=Forecast)
 */
void ui_switch_screen(int screen_index);

#endif // UI_H
