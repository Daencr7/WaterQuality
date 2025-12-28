#ifndef __ADC_H
#define __ADC_H

#include <stdint.h>
#include <stm32f4xx.h>

#define ADC_NUM_CHANNELS 3

extern __IO uint16_t adc_values[ADC_NUM_CHANNELS];

void ADC_Init_Scan_DMA(void);

uint16_t ADC_GetValue(uint8_t channel_rank);

#endif // __ADC_DRIVER_H
