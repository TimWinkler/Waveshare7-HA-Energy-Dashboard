/**
 * Display Driver - Waveshare ESP32-S3-Touch-LCD-7
 */

#include "display_driver.h"
#include "config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "lvgl.h"

static const char *TAG = "display";

// CH422G I2C addresses
#define CH422G_I2C_ADDR_SYS     0x24
#define CH422G_I2C_ADDR_OUT     0x38

// CH422G output bits
#define CH422G_BIT_BACKLIGHT    0x01
#define CH422G_BIT_LCD_RST      0x02
#define CH422G_BIT_TOUCH_RST    0x04
#define CH422G_BIT_LCD_EN       0x08

static esp_lcd_panel_handle_t s_panel = NULL;
static lv_disp_t *s_disp = NULL;
static lv_disp_draw_buf_t s_disp_buf;
static lv_disp_drv_t s_disp_drv;

// Double buffering
#define LVGL_BUF_HEIGHT 40
static lv_color_t *s_buf1 = NULL;
static lv_color_t *s_buf2 = NULL;

static void ch422g_write(uint8_t addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
}

static void ch422g_init(void)
{
    // Configure CH422G as output mode
    ch422g_write(CH422G_I2C_ADDR_SYS, 0x01);

    // Enable backlight, LCD reset high, touch reset high, LCD enable
    ch422g_write(CH422G_I2C_ADDR_OUT,
                 CH422G_BIT_BACKLIGHT | CH422G_BIT_LCD_RST |
                 CH422G_BIT_TOUCH_RST | CH422G_BIT_LCD_EN);
}

static void i2c_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = TOUCH_I2C_SDA,
        .scl_io_num = TOUCH_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
}

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel = (esp_lcd_panel_handle_t)drv->user_data;
    int x1 = area->x1;
    int y1 = area->y1;
    int x2 = area->x2;
    int y2 = area->y2;

    esp_lcd_panel_draw_bitmap(panel, x1, y1, x2 + 1, y2 + 1, color_map);
    lv_disp_flush_ready(drv);
}

static void lvgl_tick_timer_cb(void *arg)
{
    lv_tick_inc(2);
}

esp_err_t display_init(void)
{
    ESP_LOGI(TAG, "Initializing display");

    // Initialize I2C for CH422G and touch
    i2c_init();

    // Initialize CH422G GPIO expander
    ch422g_init();
    vTaskDelay(pdMS_TO_TICKS(100));

    // Allocate LVGL draw buffers
    s_buf1 = heap_caps_malloc(LCD_WIDTH * LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_DMA);
    s_buf2 = heap_caps_malloc(LCD_WIDTH * LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_DMA);

    if (!s_buf1 || !s_buf2) {
        ESP_LOGE(TAG, "Failed to allocate LVGL buffers");
        return ESP_FAIL;
    }

    // Configure RGB LCD panel
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings = {
            .pclk_hz = LCD_PCLK_HZ,
            .h_res = LCD_WIDTH,
            .v_res = LCD_HEIGHT,
            .hsync_pulse_width = LCD_HSYNC_PULSE_WIDTH,
            .hsync_back_porch = LCD_HSYNC_BACK_PORCH,
            .hsync_front_porch = LCD_HSYNC_FRONT_PORCH,
            .vsync_pulse_width = LCD_VSYNC_PULSE_WIDTH,
            .vsync_back_porch = LCD_VSYNC_BACK_PORCH,
            .vsync_front_porch = LCD_VSYNC_FRONT_PORCH,
            .flags.pclk_active_neg = true,
        },
        .data_width = 16,
        .bits_per_pixel = LCD_BITS_PER_PIXEL,
        .num_fbs = 1,
        .bounce_buffer_size_px = LCD_WIDTH * 10,
        .psram_trans_align = 64,
        .hsync_gpio_num = LCD_PIN_HSYNC,
        .vsync_gpio_num = LCD_PIN_VSYNC,
        .de_gpio_num = LCD_PIN_DE,
        .pclk_gpio_num = LCD_PIN_PCLK,
        .disp_gpio_num = LCD_PIN_DISP,
        .data_gpio_nums = {
            LCD_PIN_DATA0, LCD_PIN_DATA1, LCD_PIN_DATA2, LCD_PIN_DATA3,
            LCD_PIN_DATA4, LCD_PIN_DATA5, LCD_PIN_DATA6, LCD_PIN_DATA7,
            LCD_PIN_DATA8, LCD_PIN_DATA9, LCD_PIN_DATA10, LCD_PIN_DATA11,
            LCD_PIN_DATA12, LCD_PIN_DATA13, LCD_PIN_DATA14, LCD_PIN_DATA15,
        },
        .flags.fb_in_psram = true,
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &s_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(s_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(s_panel));

    // Initialize LVGL
    lv_init();

    // Initialize display buffer
    lv_disp_draw_buf_init(&s_disp_buf, s_buf1, s_buf2, LCD_WIDTH * LVGL_BUF_HEIGHT);

    // Initialize display driver
    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.hor_res = LCD_WIDTH;
    s_disp_drv.ver_res = LCD_HEIGHT;
    s_disp_drv.flush_cb = lvgl_flush_cb;
    s_disp_drv.draw_buf = &s_disp_buf;
    s_disp_drv.user_data = s_panel;
    s_disp = lv_disp_drv_register(&s_disp_drv);

    // Create LVGL tick timer
    const esp_timer_create_args_t timer_args = {
        .callback = lvgl_tick_timer_cb,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t tick_timer;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(tick_timer, 2000));  // 2ms

    ESP_LOGI(TAG, "Display initialized successfully");
    return ESP_OK;
}

esp_lcd_panel_handle_t display_get_panel(void)
{
    return s_panel;
}
