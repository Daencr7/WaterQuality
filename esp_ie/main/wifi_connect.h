#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

void wifi_init_sta(void);
bool wifi_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_CONNECT_H
