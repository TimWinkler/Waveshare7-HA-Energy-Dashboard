/**
 * MQTT Handler - Subscribes to Home Assistant sensors and updates UI
 */

#include "mqtt_handler.h"
#include "config.h"
#include "ui.h"

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "mqtt";

static esp_mqtt_client_handle_t s_client = NULL;
static bool s_is_connected = false;
static SemaphoreHandle_t s_data_mutex = NULL;
static sensor_data_t s_sensor_data = {0};

// Topic subscription list
static const char *topics[] = {
    TOPIC_CURRENT_POWER,
    TOPIC_TOTAL_POWER,
    TOPIC_SOLAR_POWER,
    TOPIC_GAS_CONSUMPTION,
    TOPIC_GAS_COST,
    TOPIC_SOLAR_DAILY,
    TOPIC_TEMP_INDOOR,
    TOPIC_WATER_DAILY,
    TOPIC_WATER_TOTAL,
    TOPIC_GRID_DAILY,
    TOPIC_WEATHER_CONDITION,
    TOPIC_WEATHER_TEMP,
    TOPIC_WEATHER_HUMIDITY,
    TOPIC_HUMIDITY_INDOOR,
    TOPIC_WEATHER_PRESSURE,
    TOPIC_WEATHER_WIND_SPEED,
    TOPIC_WEATHER_WIND_BEARING,
    TOPIC_SUN_RISE,
    TOPIC_SUN_SET,
    TOPIC_GRID_YTD,
    TOPIC_GAS_YTD,
    TOPIC_SOLAR_YTD,
    TOPIC_WATER_YTD,
    TOPIC_FORECAST_0_HIGH, TOPIC_FORECAST_0_LOW, TOPIC_FORECAST_0_PRECIP, TOPIC_FORECAST_0_COND,
    TOPIC_FORECAST_1_HIGH, TOPIC_FORECAST_1_LOW, TOPIC_FORECAST_1_PRECIP, TOPIC_FORECAST_1_COND,
    TOPIC_FORECAST_2_HIGH, TOPIC_FORECAST_2_LOW, TOPIC_FORECAST_2_PRECIP, TOPIC_FORECAST_2_COND,
    TOPIC_FORECAST_3_HIGH, TOPIC_FORECAST_3_LOW, TOPIC_FORECAST_3_PRECIP, TOPIC_FORECAST_3_COND,
    TOPIC_FORECAST_4_HIGH, TOPIC_FORECAST_4_LOW, TOPIC_FORECAST_4_PRECIP, TOPIC_FORECAST_4_COND,
    TOPIC_FORECAST_5_HIGH, TOPIC_FORECAST_5_LOW, TOPIC_FORECAST_5_PRECIP, TOPIC_FORECAST_5_COND,
    TOPIC_FORECAST_6_HIGH, TOPIC_FORECAST_6_LOW, TOPIC_FORECAST_6_PRECIP, TOPIC_FORECAST_6_COND,
};

static const int topic_count = sizeof(topics) / sizeof(topics[0]);

static float parse_float(const char *data, int data_len)
{
    char buf[32];
    int len = data_len < 31 ? data_len : 31;
    memcpy(buf, data, len);
    buf[len] = '\0';
    return atof(buf);
}

static bool topic_matches(const char *topic, int topic_len, const char *pattern)
{
    return (strncmp(topic, pattern, topic_len) == 0 && pattern[topic_len] == '\0');
}

static void process_message(const char *topic, int topic_len, const char *data, int data_len)
{
    float value = 0;

    if (xSemaphoreTake(s_data_mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return;
    }

    // Handle weather condition (string value)
    if (topic_matches(topic, topic_len, TOPIC_WEATHER_CONDITION)) {
        int len = data_len < 31 ? data_len : 31;
        memcpy(s_sensor_data.weather_condition, data, len);
        s_sensor_data.weather_condition[len] = '\0';
        xSemaphoreGive(s_data_mutex);
        ui_update_sensors(&s_sensor_data);
        return;
    }

    value = parse_float(data, data_len);

    if (topic_matches(topic, topic_len, TOPIC_CURRENT_POWER)) {
        s_sensor_data.current_power = value;
    } else if (topic_matches(topic, topic_len, TOPIC_TOTAL_POWER)) {
        s_sensor_data.total_power = value / 1000.0f;
    } else if (topic_matches(topic, topic_len, TOPIC_SOLAR_POWER)) {
        s_sensor_data.solar_power = value;
    } else if (topic_matches(topic, topic_len, TOPIC_SOLAR_DAILY)) {
        s_sensor_data.solar_daily = value / 1000.0f;
    } else if (topic_matches(topic, topic_len, TOPIC_GRID_DAILY)) {
        s_sensor_data.grid_daily = value;
    } else if (topic_matches(topic, topic_len, TOPIC_GAS_CONSUMPTION)) {
        s_sensor_data.gas_consumption = value;
    } else if (topic_matches(topic, topic_len, TOPIC_GAS_COST)) {
        s_sensor_data.gas_cost = value;
    } else if (topic_matches(topic, topic_len, TOPIC_WATER_DAILY)) {
        s_sensor_data.water_daily = value;
    } else if (topic_matches(topic, topic_len, TOPIC_WATER_TOTAL)) {
        s_sensor_data.water_total = value;
    } else if (topic_matches(topic, topic_len, TOPIC_WEATHER_TEMP)) {
        s_sensor_data.temp_outdoor = value;
    } else if (topic_matches(topic, topic_len, TOPIC_WEATHER_HUMIDITY)) {
        s_sensor_data.humidity_outdoor = value;
    } else if (topic_matches(topic, topic_len, TOPIC_TEMP_INDOOR)) {
        s_sensor_data.temp_indoor = value;
    } else if (topic_matches(topic, topic_len, TOPIC_HUMIDITY_INDOOR)) {
        s_sensor_data.humidity_indoor = value;
    } else if (topic_matches(topic, topic_len, TOPIC_WEATHER_PRESSURE)) {
        s_sensor_data.pressure = value;
    } else if (topic_matches(topic, topic_len, TOPIC_WEATHER_WIND_SPEED)) {
        s_sensor_data.wind_speed = value;
    } else if (topic_matches(topic, topic_len, TOPIC_WEATHER_WIND_BEARING)) {
        s_sensor_data.wind_bearing = (int)value;
    } else if (topic_matches(topic, topic_len, TOPIC_SUN_RISE)) {
        int len = data_len < 7 ? data_len : 7;
        memcpy(s_sensor_data.sunrise, data, len);
        s_sensor_data.sunrise[len] = '\0';
    } else if (topic_matches(topic, topic_len, TOPIC_SUN_SET)) {
        int len = data_len < 7 ? data_len : 7;
        memcpy(s_sensor_data.sunset, data, len);
        s_sensor_data.sunset[len] = '\0';
    }
    // YTD values
    else if (topic_matches(topic, topic_len, TOPIC_GRID_YTD)) {
        s_sensor_data.grid_ytd = value;
        ESP_LOGI(TAG, "Grid YTD: %.2f kWh", value);
    } else if (topic_matches(topic, topic_len, TOPIC_GAS_YTD)) {
        s_sensor_data.gas_ytd = value;
        ESP_LOGI(TAG, "Gas YTD: %.2f m3", value);
    } else if (topic_matches(topic, topic_len, TOPIC_SOLAR_YTD)) {
        s_sensor_data.solar_ytd = value;
        ESP_LOGI(TAG, "Solar YTD: %.2f kWh", value);
    } else if (topic_matches(topic, topic_len, TOPIC_WATER_YTD)) {
        s_sensor_data.water_ytd = value;
        ESP_LOGI(TAG, "Water YTD: %.2f L", value);
    }
    // 7-day forecast
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_0_HIGH)) { s_sensor_data.forecast[0].temp_high = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_0_LOW)) { s_sensor_data.forecast[0].temp_low = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_0_PRECIP)) { s_sensor_data.forecast[0].precipitation = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_1_HIGH)) { s_sensor_data.forecast[1].temp_high = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_1_LOW)) { s_sensor_data.forecast[1].temp_low = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_1_PRECIP)) { s_sensor_data.forecast[1].precipitation = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_2_HIGH)) { s_sensor_data.forecast[2].temp_high = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_2_LOW)) { s_sensor_data.forecast[2].temp_low = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_2_PRECIP)) { s_sensor_data.forecast[2].precipitation = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_3_HIGH)) { s_sensor_data.forecast[3].temp_high = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_3_LOW)) { s_sensor_data.forecast[3].temp_low = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_3_PRECIP)) { s_sensor_data.forecast[3].precipitation = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_4_HIGH)) { s_sensor_data.forecast[4].temp_high = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_4_LOW)) { s_sensor_data.forecast[4].temp_low = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_4_PRECIP)) { s_sensor_data.forecast[4].precipitation = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_5_HIGH)) { s_sensor_data.forecast[5].temp_high = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_5_LOW)) { s_sensor_data.forecast[5].temp_low = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_5_PRECIP)) { s_sensor_data.forecast[5].precipitation = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_6_HIGH)) { s_sensor_data.forecast[6].temp_high = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_6_LOW)) { s_sensor_data.forecast[6].temp_low = value; }
    else if (topic_matches(topic, topic_len, TOPIC_FORECAST_6_PRECIP)) { s_sensor_data.forecast[6].precipitation = value; }

    xSemaphoreGive(s_data_mutex);
    ui_update_sensors(&s_sensor_data);
}

static void subscribe_all(void)
{
    for (int i = 0; i < topic_count; i++) {
        esp_mqtt_client_subscribe(s_client, topics[i], 0);
    }
    ESP_LOGI(TAG, "Subscribed to %d topics", topic_count);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT connected");
        s_is_connected = true;
        subscribe_all();
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT disconnected");
        s_is_connected = false;
        break;

    case MQTT_EVENT_DATA:
        process_message(event->topic, event->topic_len,
                       event->data, event->data_len);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT error");
        break;

    default:
        break;
    }
}

esp_err_t mqtt_init(void)
{
    s_data_mutex = xSemaphoreCreateMutex();
    if (s_data_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create mutex");
        return ESP_FAIL;
    }

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .uri = MQTT_BROKER_URI,
            },
        },
        .credentials = {
            .username = MQTT_USERNAME,
            .authentication = {
                .password = MQTT_PASSWORD,
            },
        },
        .session = {
            .keepalive = 60,
        },
    };

    s_client = esp_mqtt_client_init(&mqtt_cfg);
    if (s_client == NULL) {
        ESP_LOGE(TAG, "Failed to init MQTT client");
        return ESP_FAIL;
    }

    ESP_ERROR_CHECK(esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID,
                                                   mqtt_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(s_client));

    ESP_LOGI(TAG, "MQTT client started, connecting to %s", MQTT_BROKER_URI);
    return ESP_OK;
}

bool mqtt_is_connected(void)
{
    return s_is_connected;
}

void mqtt_get_sensor_data(sensor_data_t *data)
{
    if (xSemaphoreTake(s_data_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        memcpy(data, &s_sensor_data, sizeof(sensor_data_t));
        xSemaphoreGive(s_data_mutex);
    }
}
