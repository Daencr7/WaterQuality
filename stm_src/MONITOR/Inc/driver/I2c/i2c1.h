#ifndef __I2C1_H__
#define __I2C1_H__

#include <stdint.h>

void I2C1_Init(void);

/* Ghi 1 byte vào 1 thanh ghi của slave */
void I2C1_WriteRegister(uint8_t devAddr7bit, uint8_t regAddr, uint8_t data);

/* Ghi nhiều byte, auto regAddr + i */
void I2C1_WriteMulti(uint8_t devAddr7bit, uint8_t regAddr, const uint8_t *data, uint8_t len);

/* Đọc 1 byte từ 1 thanh ghi */
uint8_t I2C1_ReadRegister(uint8_t devAddr7bit, uint8_t regAddr);

/* Đọc nhiều byte, auto regAddr + i */
void I2C1_ReadMulti(uint8_t devAddr7bit, uint8_t regAddr, uint8_t *data, uint8_t len);

#endif /* __I2C1_H__ */
