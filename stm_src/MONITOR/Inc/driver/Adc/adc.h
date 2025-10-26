#ifndef __ADC_H
#define __ADC_H

#include <stdint.h>
#include <stm32f4xx.h>

// Định nghĩa số lượng kênh được lấy mẫu
#define ADC_NUM_CHANNELS 4

// Biến toàn cục (được đánh dấu là volatile vì nó được DMA ghi vào)
// Mảng này sẽ chứa 4 giá trị ADC theo thứ tự: Ch1 (PA1), Ch5 (PA5), Ch3 (PA3), Ch4 (PA4)
extern __IO uint16_t adc_values[ADC_NUM_CHANNELS];

// Khai báo hàm cấu hình
void ADC_Init_Scan_DMA(void);

// Khai báo hàm lấy giá trị đã chuyển đổi
uint16_t ADC_GetValue(uint8_t channel_rank);

#endif // __ADC_DRIVER_H