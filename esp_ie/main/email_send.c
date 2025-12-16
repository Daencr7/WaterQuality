#include "esp_http_client.h"
#include "esp_log.h"
#include <string.h>
#include "wifi_connect.h"

#define MAIL_URL   "http://edna.io.vn/api/send_mail.php"

static const char *TAG_MAIL = "MAIL";

// Hàm encode đơn giản cho form POST (chỉ encode ký tự lạ)
static void url_encode(const char *src, char *dst, size_t dst_size)
{
    const char *hex = "0123456789ABCDEF";
    size_t i = 0;

    while (*src && i + 4 < dst_size) {
        char c = *src;
        if (('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9') ||
            c == '-' || c == '_' || c == '.' ) {
            dst[i++] = c;
        } else {
            dst[i++] = '%';
            dst[i++] = hex[(c >> 4) & 0x0F];
            dst[i++] = hex[c & 0x0F];
        }
        src++;
    }
    dst[i] = 0;
}

void send_email_alert(int level, float temp, float ph, float tds, float ntu)
{
    if (!wifi_is_connected()) {
        ESP_LOGW(TAG_MAIL, "Wi-Fi not connected, skip email");
        return;
    }

    char subject_raw[64];
    char message_raw[256];

    if (level == 0) {
        sprintf(subject_raw, "He thong tro lai binh thuong");
        sprintf(message_raw,
                "He thong da tro lai binh thuong.\n"
                "Temp: %.1f\npH: %.1f\nTDS: %.1f\nNTU: %.1f",
                temp, ph, tds, ntu);
    } else {
        sprintf(subject_raw, "CANH BAO MUC %d", level);
        sprintf(message_raw,
                "CANH BAO MUC %d !!!\n"
                "Temp: %.1f\npH: %.1f\nTDS: %.1f\nNTU: %.1f",
                level, temp, ph, tds, ntu);
    }

    char subject[128];
    char message[512];
    url_encode(subject_raw, subject, sizeof(subject));
    url_encode(message_raw, message, sizeof(message));

    char post_data[700];
    sprintf(post_data, "subject=%s&message=%s", subject, message);

    esp_http_client_config_t config = {
        .url = MAIL_URL,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG_MAIL, "Failed to init HTTP client");
        return;
    }

    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG_MAIL, "Mail POST success, status=%d", status);
    } else {
        ESP_LOGE(TAG_MAIL, "Mail POST failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}
