/**
 * Waveshare Energy Dashboard - Main Entry Point
 *
 * ESP32-S3-Touch-LCD-7 Home Assistant Energy Dashboard
 */

#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_sntp.h"
#include "nvs_flash.h"
#include "lwip/apps/sntp.h"
#include "lvgl.h"

#include "config.h"
#include "display_driver.h"
#include "touch_driver.h"
#include "ui.h"
#include "ui_screens.h"
#include "mqtt_handler.h"

static const char *TAG = "main";

static bool s_wifi_connected = false;

// WiFi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        s_wifi_connected = false;
        ui_update_wifi_status(false, 0);
        ESP_LOGW(TAG, "WiFi disconnected, reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_wifi_connected = true;

        wifi_ap_record_t ap_info;
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            ui_update_wifi_status(true, ap_info.rssi);
        }
    }
}

static void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi initialization complete");
}

static void sntp_init_time(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();

    // Set timezone to CET/CEST (Central European Time)
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
}

// LVGL task
static void lvgl_task(void *pvParameters)
{
    ESP_LOGI(TAG, "LVGL task started");

    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Time update task
static void time_update_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Time update task started");
    static const char *weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    while (1) {
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);

        if (timeinfo.tm_year > (2020 - 1900)) {
            ui_update_time(timeinfo.tm_hour, timeinfo.tm_min,
                          timeinfo.tm_mday, timeinfo.tm_mon + 1,
                          weekdays[timeinfo.tm_wday]);
        }

        // Update WiFi signal strength
        if (s_wifi_connected) {
            wifi_ap_record_t ap_info;
            if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
                ui_update_wifi_status(true, ap_info.rssi);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "=== Waveshare Energy Dashboard ===");
    ESP_LOGI(TAG, "Free heap: %lu bytes", esp_get_free_heap_size());

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize display and LVGL
    ESP_ERROR_CHECK(display_init());
    ESP_LOGI(TAG, "Display initialized");

    // Initialize touch
    ESP_ERROR_CHECK(touch_init());
    ESP_LOGI(TAG, "Touch initialized");

    // Initialize UI
    ui_init();
    ESP_LOGI(TAG, "UI initialized");

    // Start LVGL task
    xTaskCreatePinnedToCore(lvgl_task, "lvgl", 8192, NULL, 5, NULL, 1);

    // Initialize WiFi
    wifi_init();

    // Wait for WiFi connection
    int wifi_timeout = 0;
    while (!s_wifi_connected && wifi_timeout < 100) {
        vTaskDelay(pdMS_TO_TICKS(100));
        wifi_timeout++;
    }

    // Initialize SNTP
    sntp_init_time();

    // Initialize MQTT
    ESP_ERROR_CHECK(mqtt_init());
    ESP_LOGI(TAG, "MQTT initialized");

    // Start time update task
    xTaskCreate(time_update_task, "time", 4096, NULL, 3, NULL);

    ESP_LOGI(TAG, "Startup complete, free heap: %lu bytes", esp_get_free_heap_size());

    // Main loop - just keep watchdog happy
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
