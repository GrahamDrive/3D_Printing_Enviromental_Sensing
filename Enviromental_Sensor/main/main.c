/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "ccs811.h"
#include "SSD1306Display.h"
#include "CCS811Sensor.h"


#include "lwip/err.h"
#include "lwip/sys.h"

static const char *TAGMain = "App Main";
SemaphoreHandle_t i2c_mutex;
QueueHandle_t OLEDQueue;


void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGD(TAGMain, "Initializing I2C");

    i2c_master_bus_config_t i2c_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t i2c_bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_config, &i2c_bus));
    i2c_mutex = xSemaphoreCreateMutex();
    
    TaskHandle_t xCCS811Task = NULL;
    TaskHandle_t xOLEDTask = NULL;
    OLEDQueue = xQueueCreate(4, 2 * sizeof(uint16_t));

    xTaskCreate(CCS811_task, "CCS811 Task", 2048, (void *)i2c_bus, 3, &xCCS811Task);
    xTaskCreate(oled_display_setup, "OLED Task", 3 * 2048, (void *)i2c_bus,3, &xOLEDTask);
    
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100ms
    }
}