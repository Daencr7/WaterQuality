#include "adc.h"

__IO uint16_t adc_values[ADC_NUM_CHANNELS];


void ADC_Init_Scan_DMA(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    GPIOA->MODER &= ~((0x03 << (2 * 1)) |
                      (0x03 << (2 * 3)) |
                      (0x03 << (2 * 5)));
    // Đặt PA1, PA3, PA4, PA5 vào chế độ Analog (0b11)
    GPIOA->MODER |= (0x03 << (2 * 1)) | // PA1 (Ch.1)
                    (0x03 << (2 * 3)) | // PA3 (Ch.3)
                    (0x03 << (2 * 5));  // PA5 (Ch.5)


    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    
    DMA2_Stream0->CR &= ~DMA_SxCR_EN; 
    while ((DMA2_Stream0->CR & DMA_SxCR_EN) != 0); 
    
    DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL; 

    DMA2_Stream0->CR |= DMA_SxCR_CIRC | DMA_SxCR_PSIZE_0 | 
                        DMA_SxCR_MSIZE_0 | DMA_SxCR_MINC;
    
    DMA2_Stream0->NDTR = ADC_NUM_CHANNELS;
    
    DMA2_Stream0->PAR = (uint32_t)&(ADC1->DR);
    
    DMA2_Stream0->M0AR = (uint32_t)adc_values;
    
    DMA2_Stream0->CR |= DMA_SxCR_EN;


    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    
    ADC1->CR1 |= ADC_CR1_SCAN;          // Scan mode ON
    ADC1->CR2 |= ADC_CR2_CONT;          // Continuous mode ON
    ADC1->CR2 |= ADC_CR2_DMA;           // Enable DMA for ADC
    ADC1->CR2 |= ADC_CR2_DDS;           // Continuous DMA requests
    
    ADC1->SQR1 &= ~ADC_SQR1_L;          
    ADC1->SQR1 |= ((ADC_NUM_CHANNELS - 1) << 20); 

    ADC1->SQR3 = 0;
    ADC1->SQR3 |= (1 << 0);     // SQ1 = Channel 1 (PA1)
    ADC1->SQR3 |= (5 << 5);     // SQ2 = Channel 5 (PA5)
    ADC1->SQR3 |= (3 << 10);    // SQ3 = Channel 3 (PA3)
    uint32_t sampling_time = 0x04; 
    ADC1->SMPR2 = 0;
    ADC1->SMPR2 |= (sampling_time << (3 * 1)); // Ch.1
    ADC1->SMPR2 |= (sampling_time << (3 * 3)); // Ch.3
    ADC1->SMPR2 |= (sampling_time << (3 * 5)); // Ch.5
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint16_t ADC_GetValue(uint8_t channel_rank)
{
    if (channel_rank < ADC_NUM_CHANNELS)
    {
        return adc_values[channel_rank];
    }
    return 0;
}
