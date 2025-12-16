// #pragma once
// #include <stdint.h>
// #include "driver/i2c_master.h"

// #include "at24cx_i2c_hal.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

// typedef struct {
//     uint8_t  status : 1;
//     uint32_t byte_size;        // tổng dung lượng (32768)
//     uint16_t page_write_size;  // 64
//     uint8_t  i2c_addres;       // 0x50
//     uint16_t dev_chip;         // alias
// } at24cx_dev_t;

// at24cx_err_t at24cx_init_ng(at24cx_dev_t *dev, uint16_t chip_size, uint8_t i2c_addr,
//                             i2c_master_bus_handle_t bus);

// /* Ghi/đọc tiện lợi: tự chia trang khi ghi */
// at24cx_err_t at24cx_write(at24cx_dev_t dev, uint16_t mem_addr, const uint8_t *data, uint16_t len);
// at24cx_err_t at24cx_read (at24cx_dev_t dev, uint16_t mem_addr,       uint8_t *data, uint16_t len);

// #ifdef __cplusplus
// }
// #endif
