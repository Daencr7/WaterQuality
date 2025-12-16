// #pragma once
// #include <stdint.h>
// #include "driver/i2c_master.h"


// #ifdef __cplusplus
// extern "C" {
// #endif

// typedef enum {
//     AT24CX_OK = 0,
//     AT24CX_FAIL,
//     AT24CX_INVALID_ADDRESS,
//     AT24CX_INVALID_PAGEWRITE_ADDRESS,
// } at24cx_err_t;

// /* Gắn thiết bị EEPROM (0x50..0x57) vào bus NG và giữ device handle nội bộ */
// at24cx_err_t at24cx_i2c_hal_attach(uint8_t dev_addr, i2c_master_bus_handle_t bus);

// /* Ghi/đọc từ địa chỉ nhớ 16-bit (big-endian AH/AL) */
// at24cx_err_t at24cx_i2c_hal_write_mem(uint16_t mem_addr, const uint8_t *data, uint16_t len);
// at24cx_err_t at24cx_i2c_hal_read_mem (uint16_t mem_addr,       uint8_t *data, uint16_t len);

// void at24cx_i2c_hal_ms_delay(uint32_t ms);

// #ifdef __cplusplus
// }
// #endif
