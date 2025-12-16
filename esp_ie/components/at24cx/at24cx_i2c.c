// #include "at24cx_i2c.h"
// #include <string.h>

// at24cx_err_t at24cx_init_ng(at24cx_dev_t *dev, uint16_t chip_size, uint8_t i2c_addr,
//                             i2c_master_bus_handle_t bus)
// {
//     dev->status = 1;
//     dev->byte_size = chip_size;           // 32768
//     dev->page_write_size = 64;            // AT24C256
//     dev->i2c_addres = i2c_addr;           // 0x50
//     dev->dev_chip = chip_size;
//     return at24cx_i2c_hal_attach(i2c_addr, bus);
// }

// /* Ghi an toàn: không băng qua ranh giới page 64B */
// at24cx_err_t at24cx_write(at24cx_dev_t dev, uint16_t mem_addr, const uint8_t *data, uint16_t len)
// {
//     uint16_t pos = mem_addr;
//     uint16_t left = len;
//     while (left > 0) {
//         uint16_t page_off   = pos % dev.page_write_size;
//         uint16_t room_in_pg = dev.page_write_size - page_off;
//         uint16_t chunk      = (left < room_in_pg) ? left : room_in_pg;
//         if (chunk > dev.page_write_size) chunk = dev.page_write_size;

//         at24cx_err_t e = at24cx_i2c_hal_write_mem(pos, data, chunk);
//         if (e != AT24CX_OK) return e;

//         pos  += chunk;
//         data += chunk;
//         left -= chunk;
//     }
//     return AT24CX_OK;
// }

// at24cx_err_t at24cx_read(at24cx_dev_t dev, uint16_t mem_addr, uint8_t *data, uint16_t len)
// {
//     return at24cx_i2c_hal_read_mem(mem_addr, data, len);
// }
