#ifndef __I2C3_H__
#define __I2C3_H__

#include <stdint.h>

void I2C3_Init(void);

/* Ghi 1 byte (thường dùng cho command của OLED) */
void I2C3_WriteByte(uint8_t devAddr7bit, uint8_t data);

/* Ghi nhiều byte – dùng để đổ buffer frame cho OLED */
void I2C3_WriteMulti(uint8_t devAddr7bit, const uint8_t *data, uint16_t len);

#endif /* __I2C3_H__ */
