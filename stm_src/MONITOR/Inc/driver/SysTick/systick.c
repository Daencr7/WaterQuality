#include "stm32f4xx.h" // Thêm header của STM32F4 để sử dụng các thanh ghi
#include "systick.h"   // Thư viện SysTick nếu có, hoặc để trống nếu không có tệp cụ thể này

// **********************************************
// Định nghĩa SysTick Control Register (CTRL) bits
// **********************************************

// Bit 0: ENABLE - Bật SysTick
#define CTRL_ENABLE     (1U<<0) 
// Bit 2: CLCKSOURCE - Chọn nguồn xung clock (0: External, 1: Processor Clock)
#define CTRL_CLCKSRC    (1U<<2) 
// Bit 16: COUNTFLAG - Cờ báo hiệu đếm xong (Read-only)
#define CTRL_COUNTFLAG  (1U<<16)

// **********************************************
// Cấu hình thời gian
// **********************************************

/* Giả định: Tần số xung clock của MCU là 16MHz */
/* Để có 1ms (1 mili giây), cần 16,000 chu kỳ clock */
#define ONE_MSEC_LOAD   16000

// **********************************************
// Hàm Delay (Phương pháp Blocking Polling)
// **********************************************

void systick_msec_delay(uint32_t delay)
{
    // 1. Tải số chu kỳ clock cho 1 mili giây. SysTick sẽ đếm từ giá trị này về 0.
    SysTick->LOAD = ONE_MSEC_LOAD - 1; 

    // 2. Xóa thanh ghi giá trị hiện tại (Bắt đầu đếm từ đầu)
    SysTick->VAL = 0;

    // 3. Chọn nguồn xung clock (sử dụng xung clock nội của hệ thống)
    //    Cũng có thể bao gồm CTRL_INTEN (1U<<1) để bật ngắt, nhưng ở đây chỉ dùng polling
    SysTick->CTRL = CTRL_CLCKSRC;

    // 4. Bật SysTick (SysTick bắt đầu đếm ngược)
    SysTick->CTRL |= CTRL_ENABLE;

    // Vòng lặp chính: Chờ đợi số mili giây (delay)
    for(uint32_t i = 0; i < delay; i++)
    {
        // Chờ cho đến khi cờ COUNTFLAG được bật (nghĩa là đếm xong 1ms)
        // Đây là cơ chế Polling (CPU bị chặn)
        while((SysTick->CTRL & CTRL_COUNTFLAG) == 0)
        {
            // Do nothing
        }
    }

    // 5. Tắt SysTick khi hoàn thành
    SysTick->CTRL = 0;
}