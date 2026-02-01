#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#include "esp_err.h"

/**
 * Initialize the touch panel (GT911)
 */
esp_err_t touch_init(void);

#endif // TOUCH_DRIVER_H
