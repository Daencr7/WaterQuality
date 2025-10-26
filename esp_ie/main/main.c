#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "cJSON.h"
#include "freertos/semphr.h"
#include "ssd1306.h"
#include "font8x8_basic.h"

#include "wifi_connect.h"
#include "http_send.h"
#define UART_PORT_NUM   UART_NUM_2
#define UART_TX_PIN     17
#define UART_RX_PIN     16
#define BUF_SIZE        1024

#define SDA_PIN 21
#define SCL_PIN 22

static const char *TAG = "GateIoT";
static const char *TAGG = "Wifi";
float sensor_values[4];
SemaphoreHandle_t data_mutex;
void uart_task(void *pvParameters);
void uart_init(void);
void oled_task(void *pvParameters);

void send_task(void *pvParameters);


void app_main(void)
{
    wifi_init_sta();
    while (!wifi_is_connected()) {
        ESP_LOGI(TAGG, "Wifi is connecting!...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_LOGI(TAGG, "Ready to start other task!");
    
    uart_init();

    data_mutex = xSemaphoreCreateMutex();
    if (data_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create mutex!");
        return;
    }
    
    xTaskCreate(uart_task, "uart_task", 4096, NULL, 5, NULL);
    xTaskCreate(oled_task, "oled_task", 4096, NULL, 4, NULL);
    xTaskCreate(send_task, "send_task", 8192, NULL, 3, NULL);
}
void send_task(void *pvParameters)
{
    float temp = 0, ph = 0, tds = 0, ntu = 0;
    while (1)
    {
        // 🔒 Lấy dữ liệu an toàn
        if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(200)) == pdTRUE)
        {
            temp = sensor_values[0];
            ph   = sensor_values[1];
            tds  = sensor_values[2];
            ntu  = sensor_values[3];
            xSemaphoreGive(data_mutex); // 🔓
        }

        // Gửi lên webserver
        send_data_to_server(temp, ph, tds, ntu);
        vTaskDelay(pdMS_TO_TICKS(10000));  // gửi mỗi 10s
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

            xSemaphoreGive(data_mutex);  // 🔓 Mở khóa mutex
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
