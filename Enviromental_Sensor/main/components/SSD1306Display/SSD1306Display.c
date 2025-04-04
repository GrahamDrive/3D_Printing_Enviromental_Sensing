/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "SSD1306Display.h"

static const char *TAG = "OLED";

#define I2C_BUS_PORT  0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ    (400 * 1000)
#define EXAMPLE_PIN_NUM_SDA           3
#define EXAMPLE_PIN_NUM_SCL           4
#define EXAMPLE_PIN_NUM_RST           -1
#define EXAMPLE_I2C_HW_ADDR           0x3C

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              128
#define EXAMPLE_LCD_V_RES              64
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8

lv_disp_t *disp;

void show_sensor_data(lv_disp_t *disp, CS811_data data);

void oled_display_setup(void *param)
{
    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_master_bus_handle_t i2c_bus = (i2c_master_bus_handle_t) param;

    // Lock the mutex to ensure exclusive access to the I2C bus
    if (xSemaphoreTake(i2c_mutex, portMAX_DELAY)) {
        ESP_LOGI(TAG, "Install panel IO");
        esp_lcd_panel_io_handle_t io_handle = NULL;
        esp_lcd_panel_io_i2c_config_t io_config = {
            .dev_addr = EXAMPLE_I2C_HW_ADDR,
            .scl_speed_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .control_phase_bytes = 1,               // According to SSD1306 datasheet
            .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,   // According to SSD1306 datasheet
            .lcd_param_bits = EXAMPLE_LCD_CMD_BITS, // According to SSD1306 datasheet
            .dc_bit_offset = 6,                     // According to SSD1306 datasheet
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

        ESP_LOGI(TAG, "Install SSD1306 panel driver");
        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_panel_dev_config_t panel_config = {
            .bits_per_pixel = 1,
            .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        };
        esp_lcd_panel_ssd1306_config_t ssd1306_config = {
            .height = EXAMPLE_LCD_V_RES,
        };
        panel_config.vendor_config = &ssd1306_config;
        ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
        ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

        ESP_LOGI(TAG, "Initialize LVGL");
        const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
        lvgl_port_init(&lvgl_cfg);

        const lvgl_port_display_cfg_t disp_cfg = {
            .io_handle = io_handle,
            .panel_handle = panel_handle,
            .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES,
            .double_buffer = true,
            .hres = EXAMPLE_LCD_H_RES,
            .vres = EXAMPLE_LCD_V_RES,
            .monochrome = true,
            .rotation = {
                .swap_xy = false,
                .mirror_x = false,
                .mirror_y = false,
            }
        };
        
        disp = lvgl_port_add_disp(&disp_cfg);
    }
    xSemaphoreGive(i2c_mutex);

    CS811_data data;
    data.C02 = 0;
    data.VOC = 0;
    for(;;){
    
    xQueueReceive(OLEDQueue, &data, portMAX_DELAY);
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(0) & xSemaphoreTake(i2c_mutex, portMAX_DELAY)) {
        show_sensor_data(disp, data);
        // Release the mutex
        lvgl_port_unlock();
    }

    // Unlock the mutex after the OLED setup is complete
    xSemaphoreGive(i2c_mutex);
    }
    
    vTaskDelete(NULL);
}


void show_sensor_data(lv_disp_t *disp, CS811_data data)
{   
    char buffer[100];
    sprintf(buffer, "eTVOC: %.2d ppb\neC02 %.2d ppm", data.VOC, data.C02);

    lv_disp_set_rotation(disp, LV_DISPLAY_ROTATION_180);
    lv_obj_t *scr = lv_disp_get_scr_act(disp);

    // Clear the screen by deleting all child objects
    lv_obj_clean(scr);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, buffer);
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(label, 128);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);
}