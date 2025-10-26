#include "stm32f4xx.h"  // Thư viện CMSIS cho phép truy cập thanh ghi (RCC, GPIOA, USART2)
#include <stdint.h>     // Dùng cho kiểu dữ liệu chuẩn
// #include "uart.h"    // (Giữ lại nếu bạn có tệp header riêng)

// **********************************************
// Định nghĩa Macro
// **********************************************

// Enable bit cho clock GPIOA (Bus AHB1, bit 0)
#define GPIOAEN             (1U<<0)
// Enable bit cho clock USART2 (Bus APB1, bit 17)
#define UART2EN             (1U<<17)

// Tốc độ Baudrate
#define DBG_UART_BAUDRATE   115200
// Tần số xung clock hệ thống/ Bus APB1 (giả định)
#define SYS_FREQ            16000000
#define APB1_CLK            SYS_FREQ

// USART Control Register 1 (CR1) bits
#define CR1_TE              (1U<<3)     // Transmitter Enable (TX)
#define CR1_UE              (1U<<13)    // USART Enable

// Status Register (SR) bit
#define SR_TXE              (1U<<7)     // Transmit data register Empty flag

// **********************************************
// Định nghĩa và Triển khai các Hàm Static
// **********************************************

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate)
{
    /* Công thức tính USARTDIV cho chế độ Over-sampling by 16, sử dụng kỹ thuật làm tròn (Rounding) */
    return ((periph_clk + (baudrate / 2U)) / baudrate);
}

static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate)
{
    // Ghi giá trị USARTDIV đã tính toán vào thanh ghi BRR (Baud Rate Register)
    USART2->BRR = compute_uart_bd(periph_clk, baudrate);
}

static void uart_write(int ch)
{
    /* Chờ cho đến khi cờ TXE (Transmit data register Empty) được bật */
    while(!(USART2->SR & SR_TXE)) {}

    /* Ghi dữ liệu vào thanh ghi DR (Data Register) */
    USART2->DR = (uint8_t)(ch & 0xFF);
}

// **********************************************
// Hàm Retargeting (cho phép dùng printf)
// **********************************************

int __io_putchar(int ch)
{
    uart_write(ch);
    return ch;
}

// **********************************************
// Hàm Khởi tạo UART2
// **********************************************

void uart_init(void)
{
    /* 1. Kích hoạt và cấu hình GPIOA pin 2 (TX) */

    // Bật xung clock cho GPIOA (Bus AHB1)
    RCC->AHB1ENR |= GPIOAEN;

    // Đặt chế độ PA2 thành Alternate Function mode (10)
    GPIOA->MODER &= ~(1U<<4);
    GPIOA->MODER |= (1U<<5);

    // Đặt kiểu Alternate Function cho PA2 là AF7 (USART2)
    // Pin 2 là AFR2 (bit 8-11)
    // Giá trị 0111 (7)
    GPIOA->AFR[0] |= (1U<<8);
    GPIOA->AFR[0] |= (1U<<9);
    GPIOA->AFR[0] |= (1U<<10);
    GPIOA->AFR[0] &= ~(1U<<11); // Xóa bit 11 để đảm bảo giá trị là 7

    /* 2. Kích hoạt và cấu hình USART2 */

    // Bật xung clock cho USART2 (Bus APB1)
    RCC->APB1ENR |= UART2EN;

    // Cấu hình Baudrate (BRR)
    uart_set_baudrate(APB1_CLK, DBG_UART_BAUDRATE);

    // Cấu hình hướng truyền: Chỉ bật Transmitter (CR1_TE)
    USART2->CR1 = CR1_TE;

    // Kích hoạt Module UART (CR1_UE)
    USART2->CR1 |= CR1_UE;
}
