#include "rtc3231.h"
#include "../I2c/i2c1.h"     // dùng các hàm I2C1_ReadMulti đã viết

static uint8_t BCD_To_Dec(uint8_t bcd) {
    return (uint8_t)(((bcd >> 4) * 10U) + (bcd & 0x0FU));
}

static uint8_t Dec_To_BCD(uint8_t dec) {
    return (uint8_t)(((dec / 10U) << 4) | (dec % 10U));
}


void RTC3231_Init(void)
{
    /* DS3231 gần như không cần init gì đặc biệt để đọc giờ.
       Nếu sau này cần tắt alarm, config SQW... thì thêm ở đây. */
}

int RTC3231_ReadTime(DS3231_Time_t *t)
{
    if (!t) return -1;

    uint8_t buf[3];
    I2C1_ReadMulti(DS3231_ADDR_7BIT, 0x00, buf, 3);  // sec, min, hour

    uint8_t raw_sec  = buf[0];
    uint8_t raw_min  = buf[1];
    uint8_t raw_hour = buf[2];

    t->seconds = BCD_To_Dec(raw_sec & 0x7F);
    t->minutes = BCD_To_Dec(raw_min & 0x7F);

    /* Kiểm tra bit6 để xem đang 12h hay 24h */
    if (raw_hour & 0x40) {
        /* 12-hour mode */
        uint8_t hour12 = BCD_To_Dec(raw_hour & 0x1F);  // 1..12
        uint8_t pm     = (raw_hour & 0x20) ? 1 : 0;    // bit5 = PM

        if (hour12 == 12) {
            hour12 = 0;             // 12AM → 0h, 12PM xử lý dưới
        }
        if (pm) {
            hour12 += 12;           // PM → +12
        }
        t->hours = hour12;          // 0..23
    } else {
        /* 24-hour mode */
        t->hours = BCD_To_Dec(raw_hour & 0x3F);
    }

    return 0;
}
int RTC3231_SetTime24(uint8_t hour, uint8_t minute, uint8_t second)
{
    if (hour > 23 || minute > 59 || second > 59) {
        return -1;
    }

    uint8_t sec_bcd  = Dec_To_BCD(second);
    uint8_t min_bcd  = Dec_To_BCD(minute);
    uint8_t hour_bcd = Dec_To_BCD(hour);   // LƯU Ý: KHÔNG set bit 6 → 24h mode

    /* Ghi lần lượt 0x00(sec), 0x01(min), 0x02(hour) */
    I2C1_WriteRegister(DS3231_ADDR_7BIT, 0x00, sec_bcd);
    I2C1_WriteRegister(DS3231_ADDR_7BIT, 0x01, min_bcd);
    I2C1_WriteRegister(DS3231_ADDR_7BIT, 0x02, hour_bcd);  // bit6=0 => 24h

    return 0;
}
void RTC3231_Force24hMode(void)
{
    uint8_t raw_hour;

    /* Đọc đúng 1 byte thanh ghi giờ (0x02) */
    raw_hour = I2C1_ReadRegister(DS3231_ADDR_7BIT, 0x02);

    uint8_t hour24;

    if (raw_hour & 0x40) {
        /* Đang 12h mode -> convert sang 24h */
        uint8_t hour12 = BCD_To_Dec(raw_hour & 0x1F); // 1..12
        uint8_t pm     = (raw_hour & 0x20) ? 1 : 0;

        if (hour12 == 12) {
            hour12 = 0;
        }
        if (pm) {
            hour12 += 12;
        }
        hour24 = hour12;
    } else {
        /* Đang 24h rồi -> cứ đọc như bình thường */
        hour24 = BCD_To_Dec(raw_hour & 0x3F);
    }

    /* Ghi ngược lại dạng 24h thuần */
    uint8_t hour_bcd = Dec_To_BCD(hour24);  // bit6 = 0 -> 24h
    I2C1_WriteRegister(DS3231_ADDR_7BIT, 0x02, hour_bcd);
}
