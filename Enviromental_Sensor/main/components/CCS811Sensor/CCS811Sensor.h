#ifndef CCS811SENSOR_H
#define CCS811SENSOR_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "ccs811.h"
#include "SSD1306Display.h"

void CCS811_task(void *param);

#endif // CCS811SENSOR_H