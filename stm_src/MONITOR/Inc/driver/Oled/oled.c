#include "oled.h"
#include "../I2c/i2c3.h"
#include <string.h>

#define SSD1306_I2C_ADDR   0x3C

/* Kích thước thật của OLED: 128x32 (ngang x dọc) */
#define HW_WIDTH   128
#define HW_HEIGHT  32
#define HW_PAGES   (HW_HEIGHT / 8)

/* Kích thước "ảo" sau khi xoay: 32 ngang x 128 cao */
#define V_WIDTH    32
#define V_HEIGHT   128

static uint8_t Buffer[HW_WIDTH * HW_PAGES];

/* ---------- Gửi command/data qua I2C3 ---------- */

static void ssd1306_SendCommand(uint8_t cmd)
{
    uint8_t buf[2] = {0x00, cmd}; // control=command
    I2C3_WriteMulti(SSD1306_I2C_ADDR, buf, 2);
}

static void ssd1306_SendData(const uint8_t *data, uint16_t len)
{
    uint8_t frame[1 + 16];
    frame[0] = 0x40;  // control=data

    uint16_t sent = 0;
    while (sent < len) {
        uint16_t chunk = (len - sent > 16) ? 16 : (len - sent);
        memcpy(&frame[1], &data[sent], chunk);
        I2C3_WriteMulti(SSD1306_I2C_ADDR, frame, (uint16_t)(1 + chunk));
        sent += chunk;
    }
}

/* ---------- SetPixel với toạ độ XOAY ---------- */
/*
 * Toạ độ ảo (vx,vy):
 *   vx: 0..31  (ngang)
 *   vy: 0..127 (dọc – chiều cao)
 *
 * Map sang toạ độ thật (xh,yh) 128x32, xoay 90°:
 *   xh = vy
 *   yh = 31 - vx
 */
static void SetPixelRot(uint8_t vx, uint8_t vy, uint8_t color)
{
    if (vx >= V_WIDTH || vy >= V_HEIGHT) return;

    uint8_t xh = vy;
    uint8_t yh = (uint8_t)(31 - vx);

    uint16_t index = (uint16_t)(xh + (yh / 8) * HW_WIDTH);
    uint8_t  bit   = (uint8_t)(1U << (yh % 8));

    if (color)
        Buffer[index] |= bit;
    else
        Buffer[index] &= (uint8_t)~bit;
}

/* Vẽ 1 hình chữ nhật đầy (toạ độ ảo) */
static void FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
    for (uint8_t dx = 0; dx < w; dx++) {
        for (uint8_t dy = 0; dy < h; dy++) {
            SetPixelRot((uint8_t)(x + dx), (uint8_t)(y + dy), color);
        }
    }
}

/* ---------- Vẽ digit lớn kiểu 7 đoạn, ~14x27 px ---------- */

static void DrawBigDigit(uint8_t x, uint8_t y, char ch)
{
    if (ch < '0' || ch > '9') return;
    uint8_t d = (uint8_t)(ch - '0');

    /* Cấu hình khung digit */
    const uint8_t seg_thick = 3;   // độ dày thanh
    const uint8_t seg_len_h = 10;  // chiều dài thanh ngang
    const uint8_t seg_len_v = 10;  // chiều dài thanh dọc

    /* Vị trí tương đối các thanh (a..g) trong khung khoảng 14x27 */
    // a: top
    uint8_t ax = x + 2;
    uint8_t ay = y + 0;
    // g: middle
    uint8_t gx = x + 2;
    uint8_t gy = y + 12;
    // d: bottom
    uint8_t dx = x + 2;
    uint8_t dy = y + 24;
    // f: upper-left
    uint8_t fx = x + 0;
    uint8_t fy = y + 2;
    // e: lower-left
    uint8_t ex = x + 0;
    uint8_t ey = y + 14;
    // b: upper-right
    uint8_t bx = x + 11;
    uint8_t by = y + 2;
    // c: lower-right
    uint8_t cx = x + 11;
    uint8_t cy = y + 14;

    /* Bật tắt segments theo digit (7-seg classic) */

    /* a (top) */
    if (d == 0 || d == 2 || d == 3 || d == 5 || d == 6 || d == 7 || d == 8 || d == 9)
        FillRect(ax, ay, seg_len_h, seg_thick, 1);

    /* d (bottom) */
    if (d == 0 || d == 2 || d == 3 || d == 5 || d == 6 || d == 8 || d == 9)
        FillRect(dx, dy, seg_len_h, seg_thick, 1);

    /* g (middle) */
    if (d == 2 || d == 3 || d == 4 || d == 5 || d == 6 || d == 8 || d == 9)
        FillRect(gx, gy, seg_len_h, seg_thick, 1);

    /* f (upper-left) */
    if (d == 0 || d == 4 || d == 5 || d == 6 || d == 8 || d == 9)
        FillRect(fx, fy, seg_thick, seg_len_v, 1);

    /* e (lower-left) */
    if (d == 0 || d == 2 || d == 6 || d == 8)
        FillRect(ex, ey, seg_thick, seg_len_v, 1);

    /* b (upper-right) */
    if (d == 0 || d == 1 || d == 2 || d == 3 || d == 4 || d == 7 || d == 8 || d == 9)
        FillRect(bx, by, seg_thick, seg_len_v, 1);

    /* c (lower-right) */
    if (d == 0 || d == 1 || d == 3 || d == 4 || d == 5 || d == 6 || d == 7 || d == 8 || d == 9)
        FillRect(cx, cy, seg_thick, seg_len_v, 1);
}

/* Vẽ dấu ':' to */
static void DrawBigColon(uint8_t x, uint8_t y)
{
    /* 2 chấm vuông */
//    FillRect(x,     (uint8_t)(y + 4),  3, 3, 1);
//    FillRect(x,     (uint8_t)(y + 18), 3, 3, 1);
}

/* ---------- API công khai ---------- */

void SSD1306_Init(void)
{
    /* Init sequence cơ bản cho SSD1306 128x32 */
    ssd1306_SendCommand(0xAE); // display off
    ssd1306_SendCommand(0x20); // Memory addressing mode
    ssd1306_SendCommand(0x00); // Horizontal
    ssd1306_SendCommand(0xB0);
    ssd1306_SendCommand(0xC8);
    ssd1306_SendCommand(0x00);
    ssd1306_SendCommand(0x10);
    ssd1306_SendCommand(0x40);
    ssd1306_SendCommand(0x81);
    ssd1306_SendCommand(0x8F);
    ssd1306_SendCommand(0xA1);
    ssd1306_SendCommand(0xA6);
    ssd1306_SendCommand(0xA8);
    ssd1306_SendCommand(0x1F);   // 32-1
    ssd1306_SendCommand(0xD3);
    ssd1306_SendCommand(0x00);
    ssd1306_SendCommand(0xD5);
    ssd1306_SendCommand(0x80);
    ssd1306_SendCommand(0xD9);
    ssd1306_SendCommand(0xF1);
    ssd1306_SendCommand(0xDA);
    ssd1306_SendCommand(0x02);
    ssd1306_SendCommand(0xDB);
    ssd1306_SendCommand(0x40);
    ssd1306_SendCommand(0x8D);
    ssd1306_SendCommand(0x14);
    ssd1306_SendCommand(0xAF);   // display on

    SSD1306_Clear();
    SSD1306_Update();
}

void SSD1306_Clear(void)
{
    memset(Buffer, 0, sizeof(Buffer));
}

void SSD1306_Update(void)
{
    for (uint8_t page = 0; page < HW_PAGES; page++) {
        ssd1306_SendCommand((uint8_t)(0xB0 | page));
        ssd1306_SendCommand(0x00);
        ssd1306_SendCommand(0x10);
        ssd1306_SendData(&Buffer[page * HW_WIDTH], HW_WIDTH);
    }
}

/* Hiển thị HH:MM to, vertical, căn giữa */
void SSD1306_DisplayTimeVerticalBig(const DS3231_Time_t *t)
{
    char h1 = (char)('0' + (t->hours   / 10));
    char h2 = (char)('0' + (t->hours   % 10));
    char m1 = (char)('0' + (t->minutes / 10));
    char m2 = (char)('0' + (t->minutes % 10));

    /* Kích thước digit ~ 14 (ngang) x 27 (dọc) */
    const uint8_t digit_w = 14;
    const uint8_t digit_h = 27;

    /* Hai số chiếm 2*digit_w + gap; mình chọn gap=2 → ~30 px ngang */
    const uint8_t gap_x   = 2;
    const uint8_t total_w = (uint8_t)(digit_w * 2 + gap_x);  // ~30
    const uint8_t start_x = (uint8_t)((V_WIDTH - total_w) / 2); // căn giữa ngang

    /* Tổng chiều cao khối:
       HH: digit_h
       khoảng trống: 6
       ':' khoảng ~10
       khoảng trống: 6
       MM: digit_h
       → total_h ≈ 2*digit_h + 22 ≈ 76
       căn giữa trong 128
    */
    const uint8_t colon_h  = 10;
    const uint8_t gap1     = 6;
    const uint8_t gap2     = 6;
    const uint8_t total_h  = (uint8_t)(digit_h + gap1 + colon_h + gap2 + digit_h);
    const uint8_t start_y  = (uint8_t)((V_HEIGHT - total_h) / 2);

    uint8_t y_HH    = start_y;
    uint8_t y_colon = (uint8_t)(y_HH + digit_h + gap1);
    uint8_t y_MM    = (uint8_t)(y_colon + colon_h + gap2);

    uint8_t x_H1 = start_x;
    uint8_t x_H2 = (uint8_t)(start_x + digit_w + gap_x);

    uint8_t x_M1 = start_x;
    uint8_t x_M2 = (uint8_t)(start_x + digit_w + gap_x);

    uint8_t x_colon = (uint8_t)(start_x + total_w/2 - 1);

    SSD1306_Clear();

    /* Vẽ HH */
    DrawBigDigit(x_H1, y_HH, h1);
    DrawBigDigit(x_H2, y_HH, h2);

    /* Vẽ ':' */
    DrawBigColon(x_colon, y_colon);

    /* Vẽ MM */
    DrawBigDigit(x_M1, y_MM, m1);
    DrawBigDigit(x_M2, y_MM, m2);

    SSD1306_Update();
}
