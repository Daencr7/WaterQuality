// #include "at24cx_i2c_hal.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include <string.h>

// static i2c_device_handle_t s_ee_dev = NULL;

// at24cx_err_t at24cx_i2c_hal_attach(uint8_t dev_addr, i2c_master_bus_handle_t bus)
// {
//     i2c_device_config_t cfg = {
//         .dev_addr_length = I2C_ADDR_BIT_LEN_7,
//         .device_address  = dev_addr,     // 0x50
//         .scl_speed_hz    = 100000        // 100 kHz an toàn cho 24C256
//     };
//     return (i2c_master_bus_add_device(bus, &cfg, &s_ee_dev) == ESP_OK) ? AT24CX_OK : AT24CX_FAIL;
// }

// void at24cx_i2c_hal_ms_delay(uint32_t ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }

// /* Ghi: [AH][AL] + payload */
// at24cx_err_t at24cx_i2c_hal_write_mem(uint16_t mem_addr, const uint8_t *data, uint16_t len)
// {
//     if (!s_ee_dev) return AT24CX_FAIL;
//     if (len > 66) return AT24CX_INVALID_PAGEWRITE_ADDRESS; // buffer tạm >64B + 2B addr

//     uint8_t buf[2 + 64];
//     buf[0] = (uint8_t)(mem_addr >> 8);
//     buf[1] = (uint8_t)(mem_addr & 0xFF);
//     memcpy(&buf[2], data, len);

//     esp_err_t err = i2c_master_transmit(s_ee_dev, buf, 2 + len, 1000);
//     at24cx_i2c_hal_ms_delay(5); // write-cycle 5ms ~ 10ms
//     return (err == ESP_OK) ? AT24CX_OK : AT24CX_FAIL;
// }

// /* Read: repeated-start → gửi 2 byte địa chỉ, sau đó nhận dữ liệu */
// at24cx_err_t at24cx_i2c_hal_read_mem(uint16_t mem_addr, uint8_t *data, uint16_t len)
// {
//     if (!s_ee_dev) return AT24CX_FAIL;
//     uint8_t addr_buf[2] = { (uint8_t)(mem_addr >> 8), (uint8_t)(mem_addr & 0xFF) };
//     esp_err_t err = i2c_master_transmit_receive(s_ee_dev, addr_buf, 2, data, len, 1000);
//     return (err == ESP_OK) ? AT24CX_OK : AT24CX_FAIL;
// }
