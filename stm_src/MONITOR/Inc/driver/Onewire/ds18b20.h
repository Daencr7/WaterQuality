/*
 * ds18b20.h
 *
 *  Created on: Nov 20, 2025
 *      Author: edna
 */

#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f4xx.h"
#include <stdint.h>

/**
 * @brief Khởi tạo GPIO và kiểm tra có DS18B20 trên bus không.
 * @return 1 nếu phát hiện DS18B20, 0 nếu không.
 */
uint8_t DS18B20_Init(void);

/**
 * @brief Đọc nhiệt độ bằng độ C.
 * @return Nhiệt độ float, hoặc -1000.0f nếu lỗi (không thấy DS18B20).
 */
int32_t DS18B20_ReadTemp_fp(void);

#endif /* __DS18B20_H */
