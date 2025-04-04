// filepath: /home/roylasgrey/Documents/GitHub/3D_Printing_Enviromental_Sensing/3D_Printing_Enviromental_Sensor/main/components/SSD1306/i2c_oled_example_main.h
/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#ifndef SSD1306DISPLAY_H
#define SSD1306DISPLAY_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "esp_lcd_panel_vendor.h"

extern SemaphoreHandle_t i2c_mutex;  // Declare the semaphore as extern
extern QueueHandle_t OLEDQueue;

typedef struct {
    uint16_t VOC;
    uint16_t C02;
} CS811_data;

/**
 * @brief 
 *
 * This function configures the I2C bus, installs the SSD1306 panel driver,
 * initializes the LVGL library, and sets up the display for rendering.
 */
void oled_display_setup(void *param);

#endif // SSD1306DISPLAY_H