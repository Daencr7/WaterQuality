#ifndef __RTC3231_H__
#define __RTC3231_H__

#include <stdint.h>

#define DS3231_ADDR_7BIT   0x68

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} DS3231_Time_t;

void RTC3231_Init(void);
int  RTC3231_ReadTime(DS3231_Time_t *t);
int RTC3231_SetTime24(uint8_t hour, uint8_t minute, uint8_t second);
#endif
