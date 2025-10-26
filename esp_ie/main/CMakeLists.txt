idf_component_register(SRCS "main.c" "wifi_connect.c" "http_send.c"
                       PRIV_REQUIRES spi_flash esp_wifi esp_http_client 
                       nvs_flash esp_driver_uart json ssd1306 esp_driver_i2c
                       
                       
                       INCLUDE_DIRS ".")
