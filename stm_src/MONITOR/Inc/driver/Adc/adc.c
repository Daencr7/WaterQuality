#include "adc.h"

// Định nghĩa biến toàn cục từ file .h
// Mảng này phải là volatile vì nó được DMA ghi, không phải CPU.
__IO uint16_t adc_values[ADC_NUM_CHANNELS];

// Chú ý: Thứ tự kênh được đặt trong SQR3 là: Ch.1, Ch.5, Ch.3, Ch.4

/**
 * @brief Cấu hình ADC1 để hoạt động ở chế độ Quét, Liên tục, và dùng DMA
 * để đọc 4 kênh PA1, PA5, PA3, PA4.
 */
void ADC_Init_Scan_DMA(void)
{
    // --- 1. Cấu hình GPIO (PA1, PA3, PA4, PA5 -> Analog Mode) ---
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    // Đặt PA1, PA3, PA4, PA5 vào chế độ Analog (0b11)
    GPIOA->MODER |= (0x03 << (2 * 1)) | // PA1 (Ch.1)
                    (0x03 << (2 * 3)) | // PA3 (Ch.3)
                    (0x03 << (2 * 4)) | // PA4 (Ch.4)
                    (0x03 << (2 * 5));  // PA5 (Ch.5)


    // --- 2. Cấu hình DMA2 Stream 0 (cho ADC1) ---
    // Bật clock cho DMA2
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    
    // Tắt Stream 0 trước khi cấu hình
    DMA2_Stream0->CR &= ~DMA_SxCR_EN; 
    while ((DMA2_Stream0->CR & DMA_SxCR_EN) != 0); 
    
    // Đặt Channel 0 (ADC1)
    DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL; 

    // Chế độ Vòng tròn (CIRC), P2M, Data size 16-bit, Memory Increment (MINC)
    DMA2_Stream0->CR |= DMA_SxCR_CIRC | DMA_SxCR_PSIZE_0 | 
                        DMA_SxCR_MSIZE_0 | DMA_SxCR_MINC;
    
    // Số lượng dữ liệu cần truyền (4 kênh)
    DMA2_Stream0->NDTR = ADC_NUM_CHANNELS;
    
    // Địa chỉ nguồn (Thanh ghi dữ liệu ADC)
    DMA2_Stream0->PAR = (uint32_t)&(ADC1->DR);
    
    // Địa chỉ đích (Mảng trong RAM)
    DMA2_Stream0->M0AR = (uint32_t)adc_values;
    
    // Kích hoạt DMA Stream
    DMA2_Stream0->CR |= DMA_SxCR_EN;


    // --- 3. Cấu hình ADC1 ---
    // Bật clock cho ADC1
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    
    // Cấu hình CR1 & CR2: Kích hoạt SCAN, CONT, DMA, DDS
    ADC1->CR1 |= ADC_CR1_SCAN;          // Scan mode ON
    ADC1->CR2 |= ADC_CR2_CONT;          // Continuous mode ON
    ADC1->CR2 |= ADC_CR2_DMA;           // Enable DMA for ADC
    ADC1->CR2 |= ADC_CR2_DDS;           // Continuous DMA requests
    
    // Thiết lập tổng số chuyển đổi là 4 (L = 3)
    ADC1->SQR1 &= ~ADC_SQR1_L;          
    ADC1->SQR1 |= ((ADC_NUM_CHANNELS - 1) << 20); 

    // Đặt thứ tự các kênh trong SQR3
    // Rank 1: Ch.1, Rank 2: Ch.5, Rank 3: Ch.3, Rank 4: Ch.4
    ADC1->SQR3 = 0;
    ADC1->SQR3 |= (1 << 0);     // SQ1 = Channel 1 (PA1)
    ADC1->SQR3 |= (5 << 5);     // SQ2 = Channel 5 (PA5)
    ADC1->SQR3 |= (3 << 10);    // SQ3 = Channel 3 (PA3)
    ADC1->SQR3 |= (4 << 15);    // SQ4 = Channel 4 (PA4)

    // Cấu hình thời gian lấy mẫu (Ví dụ: 144 chu kỳ = 0b100)
    uint32_t sampling_time = 0x04; 
    ADC1->SMPR2 = 0;
    ADC1->SMPR2 |= (sampling_time << (3 * 1)); // Ch.1
    ADC1->SMPR2 |= (sampling_time << (3 * 3)); // Ch.3
    ADC1->SMPR2 |= (sampling_time << (3 * 4)); // Ch.4
    ADC1->SMPR2 |= (sampling_time << (3 * 5)); // Ch.5

    // --- 4. Bắt đầu Chuyển đổi ---
    // Bật ADC1 (ADON)
    ADC1->CR2 |= ADC_CR2_ADON;

    // Bắt đầu chuyển đổi Regular Group (SWSTART)
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

/**
 * @brief Lấy giá trị ADC từ một vị trí (Rank) cụ thể trong mảng DMA.
 * @param channel_rank: Vị trí (0-3) trong mảng DMA.
 * 0: Ch.1 (PA1), 1: Ch.5 (PA5), 2: Ch.3 (PA3), 3: Ch.4 (PA4)
 * @return Giá trị ADC 12-bit (0-4095).
 */
uint16_t ADC_GetValue(uint8_t channel_rank)
{
    if (channel_rank < ADC_NUM_CHANNELS)
    {
        return adc_values[channel_rank];
    }
    return 0; // Trả về 0 nếu Rank không hợp lệ
}