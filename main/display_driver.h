#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include "esp_err.h"
#include "esp_lcd_types.h"

/**
 * Initialize the LCD display and LVGL
 */
esp_err_t display_init(void);

/**
 * Get the display handle
 */
esp_lcd_panel_handle_t display_get_panel(void);

#endif // DISPLAY_DRIVER_H
