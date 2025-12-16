#ifndef __OLED_H__
#define __OLED_H__

#include <stdint.h>
#include "../Rtc/rtc3231.h"

/* Khởi tạo SSD1306 128x32 (I2C3) */
void SSD1306_Init(void);

/* Xoá buffer (màn hình) */
void SSD1306_Clear(void);

/* Gửi buffer ra màn hình */
void SSD1306_Update(void);

/* Hiển thị HH:MM to, quay dọc, căn giữa */
void SSD1306_DisplayTimeVerticalBig(const DS3231_Time_t *t);

#endif
