
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#define TAG "LIQUID_SENSOR"
#include "driver/gpio.h"
// GPIO define
#define SENSOR_GPIO   14   // DATA cảm biến mực
#define OUTPUT_GPIO   13   // LED / Relay

void app_main(void)
{
    /* =============================
       CẤU HÌNH GPIO
       ============================= */

    // GPIO 14: INPUT (cảm biến)
    gpio_config_t io_conf_input = {
        .pin_bit_mask = (1ULL << SENSOR_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_input);

    // GPIO 13: OUTPUT (LED / relay)
    gpio_config_t io_conf_output = {
        .pin_bit_mask = (1ULL << OUTPUT_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_output);

    while (1)
    {
        int sensor_state = gpio_get_level(SENSOR_GPIO);

        if (sensor_state == 1)
        {
            gpio_set_level(OUTPUT_GPIO, 1);
            ESP_LOGI(TAG,"MAY BOM ON");
        }
        else
        {
            gpio_set_level(OUTPUT_GPIO, 0);
            ESP_LOGI(TAG,"MAY BOM OFF");
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // đọc mỗi 500ms
    }
}
