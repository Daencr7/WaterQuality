#include "ds18b20.h"
#include "core_cm4.h"
// ================= CẤU HÌNH CHÂN DS18B20 (PA4) =================
#define OW_PORT        GPIOA
#define OW_PIN_NUM     4
#define OW_PIN_MASK    (1U << OW_PIN_NUM)
uint32_t SystemCoreClock = 16000000;

// Hàm delay ms do bạn đã có
extern void systick_msec_delay(uint32_t ms);

// ======== Delay nội bộ: dùng busy-wait + systick_msec_delay ========

static void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks) { }
}



static void delay_ms(uint32_t ms)
{
    systick_msec_delay(ms);  // dùng hàm của bạn
}

// ================= HÀM PHỤ GPIO =================

static inline void ow_pin_low(void)
{
    OW_PORT->BSRR = (uint32_t)OW_PIN_MASK << 16;   // reset bit -> kéo xuống 0
}

static inline void ow_pin_high(void)
{
    OW_PORT->BSRR = OW_PIN_MASK;                  // set bit -> thả chân (pull-up ngoài kéo lên)
}

static inline void ow_pin_output(void)
{
    OW_PORT->MODER &= ~(0x3U << (OW_PIN_NUM * 2));
    OW_PORT->MODER |=  (0x1U << (OW_PIN_NUM * 2));   // 01 = output
}

static inline void ow_pin_input(void)
{
    OW_PORT->MODER &= ~(0x3U << (OW_PIN_NUM * 2));   // 00 = input (thả nổi)
}

static inline uint8_t ow_pin_read(void)
{
    return (OW_PORT->IDR & OW_PIN_MASK) ? 1U : 0U;
}

// ================= INIT GPIO PA4 =================

static void OW_GPIO_Init(void)
{
    // Bật clock GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA4 output open-drain, speed medium, không pull nội
    ow_pin_output();

    // Output type open-drain
    OW_PORT->OTYPER |= OW_PIN_MASK;

    // No pull-up/down (dùng pull-up ngoài 4.7k)
    OW_PORT->PUPDR &= ~(0x3U << (OW_PIN_NUM * 2));

    // Speed medium
    OW_PORT->OSPEEDR &= ~(0x3U << (OW_PIN_NUM * 2));
    OW_PORT->OSPEEDR |=  (0x1U << (OW_PIN_NUM * 2));

    ow_pin_high();    // thả chân về mức 1
}

// ================= 1-Wire CORE =================
static void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static uint8_t OW_ResetPulse(void)
{
    uint8_t presence;

    ow_pin_output();
    ow_pin_low();
    delay_us(480);            // giữ thấp ~480us

    ow_pin_input();           // thả chân, pull-up ngoài kéo lên
    delay_us(70);             // chờ 60–70us

    presence = ow_pin_read(); // 0 = có thiết bị (presence)
    delay_us(410);            // đủ 480us tổng

    return (presence == 0U) ? 1U : 0U;
}

static void OW_WriteBit(uint8_t bit)
{
    ow_pin_output();
    ow_pin_low();

    if (bit)
    {
        // ghi '1': kéo thấp ngắn rồi thả
        delay_us(5);
        ow_pin_input();
        delay_us(60);
    }
    else
    {
        // ghi '0': giữ thấp lâu hơn
        delay_us(60);
        ow_pin_input();
        delay_us(5);
    }
}

static uint8_t OW_ReadBit(void)
{
    uint8_t bit;

    ow_pin_output();
    ow_pin_low();
    delay_us(3);          // bắt đầu slot

    ow_pin_input();       // thả chân

    delay_us(10);         // đợi 10us rồi đọc
    bit = ow_pin_read();

    delay_us(50);         // chờ hết slot ~60us
    return bit;
}

static void OW_WriteByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        OW_WriteBit(data & 0x01U);
        data >>= 1;
    }
}

static uint8_t OW_ReadByte(void)
{
    uint8_t data = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t bit = OW_ReadBit();
        data |= (bit << i);   // LSB first
    }
    return data;
}


// ================= DS18B20 COMMANDS =================

#define DS18B20_CMD_SKIP_ROM      0xCC
#define DS18B20_CMD_CONVERT_T     0x44
#define DS18B20_CMD_READ_SCRATCH  0xBE

// ================= API =================

uint8_t DS18B20_Init(void)
{
	dwt_init();
    OW_GPIO_Init();

    if (!OW_ResetPulse())
    {
        return 0;
    }
    return 1;
}

int32_t DS18B20_ReadTemp_fp(void)
{
    uint8_t scratchpad[9];
    int16_t raw;

    // Reset + Convert T
    if (!OW_ResetPulse()) return -1000000;

    OW_WriteByte(DS18B20_CMD_SKIP_ROM);
    OW_WriteByte(DS18B20_CMD_CONVERT_T);

    delay_ms(750);

    // Reset + Read Scratchpad
    if (!OW_ResetPulse()) return -1000000;

    OW_WriteByte(DS18B20_CMD_SKIP_ROM);
    OW_WriteByte(DS18B20_CMD_READ_SCRATCH);

    for (uint8_t i = 0; i < 9; i++)
        scratchpad[i] = OW_ReadByte();

    raw = (int16_t)((scratchpad[1] << 8) | scratchpad[0]);

    // temp (°C*1000) = raw*(1000/16)
    return ((int32_t)raw * 1000) / 16;
}

