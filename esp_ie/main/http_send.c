#include "http_send.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "wifi_connect.h"

#define SERVER_URL "http://edna.io.vn/api/receive.php"

static const char *TAG_HTTP = "HTTP";

void send_data_to_server(float temp, float ph, float tds, float ntu)
{
    if (!wifi_is_connected()) {
        ESP_LOGW(TAG_HTTP, "Wi-Fi not connected");
        return;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "temp", temp);
    cJSON_AddNumberToObject(root, "ph", ph);
    cJSON_AddNumberToObject(root, "tds", tds);
    cJSON_AddNumberToObject(root, "ntu", ntu);
    char *post_data = cJSON_PrintUnformatted(root);
    ESP_LOGI(TAG_HTTP, "Sending JSON: %s", post_data);


    esp_http_client_config_t config = {
        .url = SERVER_URL,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 10000,
        .skip_cert_common_name_check = true,
        // .transport_type = HTTP_TRANSPORT_OVER_SSL,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    if (client == NULL) {
        ESP_LOGE(TAG_HTTP, "Failed to init HTTP client");
        cJSON_Delete(root);
        free(post_data);
        return;
    }


    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));


    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);
        
        ESP_LOGI(TAG_HTTP, "POST success!");
        ESP_LOGI(TAG_HTTP, "Status: %d", status);
        ESP_LOGI(TAG_HTTP, "Content length: %d", content_length);
        
        // Đọc response
        char response[256] = {0};
        int read_len = esp_http_client_read(client, response, sizeof(response) - 1);
        if (read_len > 0) {
            ESP_LOGI(TAG_HTTP, "Response: %s", response);
        }
    } else {
        ESP_LOGE(TAG_HTTP, "POST failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    cJSON_Delete(root);
    free(post_data);
}