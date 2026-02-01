/**
 * Touch Driver - GT911 capacitive touch controller
 */

#include "touch_driver.h"
#include "config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "lvgl.h"
#include "esp_lcd_touch_gt911.h"

static const char *TAG = "touch";

static esp_lcd_touch_handle_t s_touch = NULL;
static lv_indev_t *s_indev = NULL;

static void touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t x[1];
    uint16_t y[1];
    uint16_t strength[1];
    uint8_t count = 0;

    // Read fresh touch data from the controller
    esp_lcd_touch_read_data(s_touch);

    // Get touch coordinates
    bool touched = esp_lcd_touch_get_coordinates(s_touch, x, y, strength, &count, 1);

    if (touched && count > 0) {
        data->point.x = x[0];
        data->point.y = y[0];
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

esp_err_t touch_init(void)
{
    ESP_LOGI(TAG, "Initializing touch panel");

    // Configure INT pin
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << TOUCH_INT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&io_conf);

    // Configure GT911
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = 0x5D,  // GT911 address
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 16,
        .lcd_param_bits = 0,
        .dc_bit_offset = 0,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(I2C_NUM_0, &io_config, &io_handle));

    esp_lcd_touch_config_t touch_config = {
        .x_max = LCD_WIDTH,
        .y_max = LCD_HEIGHT,
        .rst_gpio_num = -1,
        .int_gpio_num = TOUCH_INT_PIN,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };

    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_gt911(io_handle, &touch_config, &s_touch));

    // Register with LVGL
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_cb;
    s_indev = lv_indev_drv_register(&indev_drv);

    ESP_LOGI(TAG, "Touch panel initialized");
    return ESP_OK;
}
