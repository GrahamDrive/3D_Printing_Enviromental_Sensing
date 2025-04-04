idf_component_register(SRCS "main.c" "components/CCS811Sensor/CCS811Sensor.c" "components/SSD1306Display/SSD1306Display.c"
                    PRIV_REQUIRES esp_wifi nvs_flash esp_driver_i2c esp_lcd
                    INCLUDE_DIRS "." "components/CCS811Sensor" "components/SSD1306Display")
