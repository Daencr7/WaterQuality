#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "cJSON.h"
#include "freertos/semphr.h"
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "driver/gpio.h"
#include "wifi_connect.h"
#include "http_send.h"

#include "email_send.h"

#define UART_PORT_NUM   UART_NUM_2
#define UART_TX_PIN     17
#define UART_RX_PIN     16
#define BUF_SIZE        1024

#define SDA_PIN 21
#define SCL_PIN 22

#define LED_LEVEL1  2
#define LED_LEVEL2  4
#define LED_LEVEL3  5

// Ngưỡng cơ bản
#define TEMP_LIMIT  30.0
#define PH_LIMIT    7.0
#define TDS_LIMIT   1000.0
#define NTU_LIMIT   100.0



static const char *TAG = "GateIoT";
static const char *TAGG = "Wifi";
static const char *TAG_MAIL = "MAIL";


float sensor_values[4];
SemaphoreHandle_t data_mutex;
void uart_task(void *pvParameters);
void uart_init(void);
void oled_task(void *pvParameters);

void send_task(void *pvParameters);
void led_alert_task(void *pvParameters);
void led_init(void) {
    const gpio_config_t io = {
        .pin_bit_mask = (1ULL<<LED_LEVEL1) | (1ULL<<LED_LEVEL2) | (1ULL<<LED_LEVEL3),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);
    gpio_set_level(LED_LEVEL1, 0);
    gpio_set_level(LED_LEVEL2, 0);
    gpio_set_level(LED_LEVEL3, 0);
}

void app_main(void)
{
    led_init();
    wifi_init_sta();
    uart_init();

    data_mutex = xSemaphoreCreateMutex();
    if (data_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create mutex!");
        return;
    }
    xTaskCreate(led_alert_task, "led_alert_task", 4096, NULL, 3, NULL);
    xTaskCreate(uart_task, "uart_task", 4096, NULL, 5, NULL);
    xTaskCreate(oled_task, "oled_task", 4096, NULL, 4, NULL);
    xTaskCreate(send_task, "send_task", 8192, NULL, 2, NULL);
    while (!wifi_is_connected()) {
        ESP_LOGI(TAGG, "Wifi is connecting!...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_LOGI(TAGG, "Ready to start other task!");
    
    // uart_init();

    // data_mutex = xSemaphoreCreateMutex();
    // if (data_mutex == NULL) {
    //     ESP_LOGE(TAG, "Failed to create mutex!");
    //     return;
    // }
    // xTaskCreate(led_alert_task, "led_alert_task", 4096, NULL, 3, NULL);
    // xTaskCreate(uart_task, "uart_task", 4096, NULL, 5, NULL);
    // xTaskCreate(oled_task, "oled_task", 4096, NULL, 4, NULL);
    // xTaskCreate(send_task, "send_task", 8192, NULL, 2, NULL);
}
void send_task(void *pvParameters)
{
    float temp = 0, ph = 0, tds = 0, ntu = 0;
    while (1)
    {
        if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(200)) == pdTRUE)
        {
            temp = sensor_values[0];
            ph   = sensor_values[1];
            tds  = sensor_values[2];
            ntu  = sensor_values[3];
            xSemaphoreGive(data_mutex);
        }

        // Gửi lên webserver
        send_data_to_server(temp, ph, tds, ntu);
        vTaskDelay(pdMS_TO_TICKS(60000));  
    }
}

void oled_task(void *pvParameters)
{
    SSD1306_t dev;

    i2c_master_init(&dev, SDA_PIN, SCL_PIN, -1);
    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
    float temp, ph, tds, ntu;
    while (1)
    {
        if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            temp = sensor_values[0];
            ph   = sensor_values[1];
            tds  = sensor_values[2];
            ntu  = sensor_values[3];

            xSemaphoreGive(data_mutex);  //
        }
        else
        {
            ESP_LOGW("OLED", "Failed to lock mutex");
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        char line1[32], line2[32], line3[32], line4[32];

        snprintf(line1, sizeof(line1), "\tTEMP: %3.1f", temp);
        snprintf(line2, sizeof(line2), "\tPH  : %3.1f", ph);
        snprintf(line3, sizeof(line3), "\tTDS : %3.1f", tds);
        snprintf(line4, sizeof(line4), "\tNTU : %3.1f", ntu);

        ssd1306_clear_screen(&dev, false);
        ssd1306_display_text(&dev, 0, "[--------------]", 16, false);
        ssd1306_display_text(&dev, 1, line1, strlen(line1), false);
        ssd1306_display_text(&dev, 2, "", 0, false);
        ssd1306_display_text(&dev, 3, line2, strlen(line2), false);
        ssd1306_display_text(&dev, 4, "", 0, false);
        ssd1306_display_text(&dev, 5, line3, strlen(line3), false);
        ssd1306_display_text(&dev, 6, "", 0, false);
        ssd1306_display_text(&dev, 7, line4, strlen(line4), false);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void uart_task(void *pvParameters)
{
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1)
    {
        int len = uart_read_bytes(UART_PORT_NUM, data, BUF_SIZE - 1, 100 / portTICK_PERIOD_MS);
        if (len > 0)
        {
            data[len] = '\0';
            // ESP_LOGI(TAG, "Raw: %s", (char *)data);

            cJSON *json = cJSON_Parse((char *)data);
            if (json)
            {
                cJSON *temp = cJSON_GetObjectItem(json, "temp");
                cJSON *ph   = cJSON_GetObjectItem(json, "ph");
                cJSON *tds  = cJSON_GetObjectItem(json, "tds");
                cJSON *ntu  = cJSON_GetObjectItem(json, "ntu");

                if (temp && ph && tds && ntu)
                {
                    if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(100)) == pdTRUE){

                    
                        sensor_values[0] = (float) temp->valuedouble;
                        sensor_values[1] = (float) ph->valuedouble;
                        sensor_values[2] = (float) tds->valuedouble;
                        sensor_values[3] = (float) ntu->valuedouble;

                        ESP_LOGI(TAG, "Temp=%.1f -- pH=%.1f -- TDS=%.1f -- NTU=%.1f",
                                sensor_values[0],
                                sensor_values[1],
                                sensor_values[2],
                                sensor_values[3]);
                        xSemaphoreGive(data_mutex);
                    }
                }
                cJSON_Delete(json);
            }
            else
            {
                ESP_LOGW(TAG, "Invalid JSON: %s", (char *)data);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay nhẹ cho task
    }
}

void uart_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
}

void led_alert_task(void *pvParameters)
{
    int last_level = -1;
    while (1)
    {
        float temp = 0, ph = 0, tds = 0, ntu = 0;

        if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            temp = sensor_values[0];
            ph   = sensor_values[1];
            tds  = sensor_values[2];
            ntu  = sensor_values[3];
            xSemaphoreGive(data_mutex);
        }

        int level = 0;

        // if (temp > TEMP_LIMIT * 1.3 || ph > PH_LIMIT * 1.3 ||
        //     tds  > TDS_LIMIT  * 1.3 || ntu > NTU_LIMIT  * 1.3) level = 3;
        // else if (temp > TEMP_LIMIT * 1.2 || ph > PH_LIMIT * 1.2 ||
        //          tds  > TDS_LIMIT  * 1.2 || ntu > NTU_LIMIT  * 1.2) level = 2;
        // else if (temp > TEMP_LIMIT * 1.1 || ph > PH_LIMIT * 1.1 ||
        //          tds  > TDS_LIMIT  * 1.1 || ntu > NTU_LIMIT  * 1.1) level = 1;
        // else level = 0;
        
        if(ph >= 11.1 || ph <= 4.2 || tds >= 1950 || ntu >= 130 || temp  >= 52.0) level = 3;
        else if(ph >= 10.2 || ph <= 4.8 || tds >= 1800 || ntu >= 120 || temp  >= 48.0) level = 2;
        else if(ph >= 9.3 || ph <= 5.4 || tds >= 1650 || ntu >= 110 || temp  >= 44.0) level = 1;
        else level = 0;
        gpio_set_level(LED_LEVEL1, (level == 1));
        gpio_set_level(LED_LEVEL2, (level == 2));
        gpio_set_level(LED_LEVEL3, (level == 3));

        if (level > 0){
            ESP_LOGW("ALERT", "Muc canh bao %d! (temp=%.1f, ph=%.1f, tds=%.1f, ntu=%.1f)",
                     level, temp, ph, tds, ntu);
            send_email_alert(level, temp, ph, tds, ntu);
        }
        if (level != last_level){
            send_email_alert(level, temp, ph, tds, ntu);
            last_level = level;
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

