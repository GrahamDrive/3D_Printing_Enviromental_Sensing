#include "CCS811Sensor.h"

static const char *TAGCCS811Task = "CCS811 Task";
extern SemaphoreHandle_t i2c_mutex;
extern QueueHandle_t OLEDQueue;


void CCS811_task(void *param) {
    i2c_master_bus_handle_t bus_handle = (i2c_master_bus_handle_t)param;
    ccs811_config_t dev_cfg = I2C_CCS811_CONFIG_DEFAULT;
    ccs811_handle_t dev_hdl;
    TickType_t last_wake_time = xTaskGetTickCount();

    ccs811_init(bus_handle, &dev_cfg, &dev_hdl);
    
    if (dev_hdl == NULL) {
        ESP_LOGE(TAGCCS811Task, "ccs811 handle init failed");
        assert(dev_hdl);
    }

    CS811_data data;
    for (;;) {
        ESP_LOGI(TAGCCS811Task, "######################## CCS811 - START #########################");
        uint16_t eco2, etvoc;
        esp_err_t result;

        // Lock the mutex before accessing I2C
        if (xSemaphoreTake(i2c_mutex, portMAX_DELAY)) {
            result = ccs811_get_measurement(dev_hdl, &eco2, &etvoc);
            if (result != ESP_OK) {
                ESP_LOGE(TAGCCS811Task, "i2c0 i2c_ccs811_get_measurement failed: %s", esp_err_to_name(result));
            } else {
                data.C02 = eco2;
                data.VOC = etvoc;
                xQueueSend(OLEDQueue, (void *) &data, 0);
                ESP_LOGI(TAGCCS811Task, "eCO2  value: %d ppm", eco2);
                ESP_LOGI(TAGCCS811Task, "eTVOC value: %d ppb", etvoc);
            }
            xSemaphoreGive(i2c_mutex);  // Unlock the mutex
        }
        ESP_LOGI(TAGCCS811Task, "######################## CCS811 - END ###########################");
        xTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}