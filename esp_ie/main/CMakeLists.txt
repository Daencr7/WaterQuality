idf_component_register(
  SRCS
    "main.c"
    "wifi_connect.c"
    "http_send.c"
    "email_send.c"
  INCLUDE_DIRS "."
  PRIV_REQUIRES
    esp_driver_i2c   
    esp_driver_uart    
    esp_driver_gpio     
    esp_wifi
    esp_http_client
    nvs_flash
    spi_flash
    cJSON
    
    ssd1306
)
