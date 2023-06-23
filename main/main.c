#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include "aht.h"
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#define AHT_TYPE AHT_TYPE_AHT1x
//#define AHT_TYPE AHT_TYPE_AHT20

#if defined(CONFIG_IDF_TARGET_ESP8266)
#define SDA_GPIO 4
#define SCL_GPIO 5
#else
// Define the pins that will be used by the AHT device
#define SDA_GPIO 27
#define SCL_GPIO 33
#endif

#ifndef APP_CPU_NUM
// Trusting old forums (https://www.esp32.com/viewtopic.php?t=8558); This just refers to the CPU we will execute the process on. (0 or 1).
#define APP_CPU_NUM PRO_CPU_NUM
#endif

// CString printed with every ESP_LOG 
static const char *TAG = "aht-example";

//********************************************************
// Function: Read temp, and humidity from the AHT device. 
// Print the results to the serial output.
// Arguments: void* of parameters
// Return Value: none
//********************************************************
void task(void *pvParameters)
{
    // Initialize AHT Device
    aht_t dev = { 0 };
    dev.mode = AHT_MODE_NORMAL;
    dev.type = AHT_TYPE;

    ESP_ERROR_CHECK(aht_init_desc(&dev, AHT_I2C_ADDRESS_GND, (i2c_port_t)0, SDA_GPIO, SCL_GPIO));
    ESP_ERROR_CHECK(aht_init(&dev));

    // Calibrate device, log to serial results
    bool calibrated;
    ESP_ERROR_CHECK(aht_get_status(&dev, NULL, &calibrated));
    if (calibrated)
        ESP_LOGI(TAG, "Sensor calibrated");
    else
        ESP_LOGW(TAG, "Sensor not calibrated!");

    float temperature, humidity;

    // Forever, read and print the results from the AHT
    while (1)
    {
        esp_err_t res = aht_get_data(&dev, &temperature, &humidity);
        if (res == ESP_OK)
            ESP_LOGI(TAG, "\nTemperature: %.1f°C, Humidity: %.2f%%\nTemperature: %.1f°F, Humidity: %.2f%%", temperature, humidity, (temperature * (9.0/5.0) + 32.0), humidity);
        else
            ESP_LOGE(TAG, "Error reading data: %d (%s)", res, esp_err_to_name(res));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main()
{
    // Initialize I2C device 
    ESP_ERROR_CHECK(i2cdev_init());
    // Create a task and pin it to the APPLICATION CPU (Just one of the two cores)
    xTaskCreatePinnedToCore(task, TAG, configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}
