#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include "esp_err.h"
#include <stdbool.h>

// Forecast data for one day
typedef struct {
    float temp_high;
    float temp_low;
    float precipitation;
    char condition[16];
} forecast_day_t;

typedef struct {
    float current_power;      // Current power consumption (W)
    float solar_power;        // Current solar/PV power (W)
    float total_power;        // Total power consumption (kWh) - lifetime
    float solar_daily;        // Daily solar production (kWh)
    float grid_daily;         // Daily grid consumption (kWh)
    float gas_consumption;    // Daily gas consumption (m3)
    float gas_cost;           // Gas cost (EUR) - legacy field
    float water_daily;        // Daily water consumption (L)
    float water_total;        // Total water consumption (m3)
    float temp_outdoor;       // Outdoor temperature (C)
    float temp_indoor;        // Indoor temperature (C)
    float humidity_outdoor;   // Outdoor humidity (%)
    float humidity_indoor;    // Indoor humidity (%)
    char weather_condition[32];
    // Extended weather data
    float pressure;           // Atmospheric pressure (hPa)
    float wind_speed;         // Wind speed (km/h)
    int wind_bearing;         // Wind direction (degrees)
    char sunrise[8];          // Sunrise time HH:MM
    char sunset[8];           // Sunset time HH:MM
    // Year-to-Date (YTD) values from utility_meter
    float grid_ytd;           // YTD grid consumption (kWh)
    float solar_ytd;          // YTD solar production (kWh)
    float gas_ytd;            // YTD gas consumption (m3)
    float water_ytd;          // YTD water consumption (L)
    // 7-day forecast
    forecast_day_t forecast[7];
} sensor_data_t;

esp_err_t mqtt_init(void);
bool mqtt_is_connected(void);
void mqtt_get_sensor_data(sensor_data_t *data);

#endif // MQTT_HANDLER_H
