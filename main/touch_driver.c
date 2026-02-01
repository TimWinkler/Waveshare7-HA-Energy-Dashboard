/**
 * Touch Driver - GT911 capacitive touch controller
 * Direct I2C implementation (like ESPHome)
 */

#include "touch_driver.h"
#include "config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "lvgl.h"

static const char *TAG = "touch";

static lv_indev_t *s_indev = NULL;

// Direct GT911 I2C communication (like ESPHome)
#define GT911_ADDR 0x5D

static esp_err_t gt911_write_reg(uint16_t reg, const uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (GT911_ADDR << 1) | I2C_MASTER_WRITE, true);
    uint8_t reg_buf[2] = {reg >> 8, reg & 0xFF};
    i2c_master_write(cmd, reg_buf, 2, true);
    if (data && len > 0) {
        i2c_master_write(cmd, data, len, true);
    }
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t gt911_read_reg(uint16_t reg, uint8_t *data, size_t len)
{
    uint8_t reg_buf[2] = {reg >> 8, reg & 0xFF};
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (GT911_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, reg_buf, 2, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (GT911_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Direct touch read like ESPHome does
static bool gt911_read_touch_direct(uint16_t *x, uint16_t *y)
{
    uint8_t status;
    if (gt911_read_reg(0x814E, &status, 1) != ESP_OK) {
        return false;
    }

    // Bit 7 = buffer status, bits 0-3 = number of touches
    if ((status & 0x80) == 0) {
        return false;  // No valid data
    }

    uint8_t num_touches = status & 0x0F;

    // Clear the status (required for GT911)
    uint8_t zero = 0;
    gt911_write_reg(0x814E, &zero, 1);

    if (num_touches == 0 || num_touches > 5) {
        return false;
    }

    // Read first touch point from 0x814F
    uint8_t touch_data[8];
    if (gt911_read_reg(0x814F, touch_data, 8) != ESP_OK) {
        return false;
    }

    *x = touch_data[1] | (touch_data[2] << 8);
    *y = touch_data[3] | (touch_data[4] << 8);

    return true;
}

static void touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t x = 0, y = 0;

    // Direct GT911 read (like ESPHome)
    bool touched = gt911_read_touch_direct(&x, &y);

    if (touched) {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

esp_err_t touch_init(void)
{
    ESP_LOGI(TAG, "Initializing GT911 touch panel");

    // Verify GT911 communication by reading product ID
    uint8_t product_id[4] = {0};
    if (gt911_read_reg(0x8140, product_id, 4) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to communicate with GT911");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "GT911 detected (ID: %c%c%c%c)",
             product_id[0], product_id[1], product_id[2], product_id[3]);

    // Register with LVGL
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_cb;
    s_indev = lv_indev_drv_register(&indev_drv);

    if (s_indev == NULL) {
        ESP_LOGE(TAG, "Failed to register LVGL input device");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Touch panel initialized");
    return ESP_OK;
}
