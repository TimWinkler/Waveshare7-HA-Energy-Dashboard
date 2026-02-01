idf_component_register(
    SRCS "main.c" "display_driver.c" "touch_driver.c" "ui.c" "ui_screens.c" "ui_styles.c" "mqtt_handler.c"
    INCLUDE_DIRS "."
    REQUIRES esp_lcd esp_lcd_touch driver nvs_flash esp_wifi esp_netif mqtt esp_timer
)
