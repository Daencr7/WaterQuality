#include "stm32f4xx.h"
#include "i2c3.h"

#define I2C3_TIMEOUT   100000U

static void I2C3_GpioInit(void);
static void I2C3_CoreInit(void);

void I2C3_Init(void)
{
    I2C3_GpioInit();
    I2C3_CoreInit();
}

/* ========== GPIO PA8(SCL), PC9(SDA) – AF4, OD, Pull-up ========== */
static void I2C3_GpioInit(void)
{
    /* Bật clock GPIOA, GPIOC */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

    /* PA8: AF, OD, Pull-up, High speed */
    GPIOA->MODER &= ~(3U << (8 * 2));
    GPIOA->MODER |=  (2U << (8 * 2));  // AF
    GPIOA->OTYPER |= (1U << 8);        // OD
    GPIOA->PUPDR &= ~(3U << (8 * 2));
    GPIOA->PUPDR |=  (1U << (8 * 2));  // PU
    GPIOA->OSPEEDR |= (3U << (8 * 2));
    /* AF4 cho PA8 */
    GPIOA->AFR[1] &= ~(0xFU << ((8 - 8) * 4));
    GPIOA->AFR[1] |=  (4U   << ((8 - 8) * 4));

    /* PC9: AF, OD, Pull-up, High speed */
    GPIOC->MODER &= ~(3U << (9 * 2));
    GPIOC->MODER |=  (2U << (9 * 2));
    GPIOC->OTYPER |= (1U << 9);
    GPIOC->PUPDR &= ~(3U << (9 * 2));
    GPIOC->PUPDR |=  (1U << (9 * 2));
    GPIOC->OSPEEDR |= (3U << (9 * 2));
    /* AF4 cho PC9 */
    GPIOC->AFR[1] &= ~(0xFU << ((9 - 8) * 4));
    GPIOC->AFR[1] |=  (4U   << ((9 - 8) * 4));
}

/* ========== Core I2C3 – 100kHz @ APB1 = 42MHz ========== */
static void I2C3_CoreInit(void)
{
    /* Bật clock I2C3 */
    RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;

    /* Reset I2C3 */
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C3RST;

    /* Disable I2C3 trước khi config */
    I2C3->CR1 &= ~I2C_CR1_PE;

    /* Freq = 42MHz */
    I2C3->CR2 &= ~I2C_CR2_FREQ;
    I2C3->CR2 |= 42U;

    /* 100kHz */
    I2C3->CCR = 210U;
    I2C3->TRISE = 43U;

    /* ACK */
    I2C3->CR1 |= I2C_CR1_ACK;

    /* Enable I2C3 */
    I2C3->CR1 |= I2C_CR1_PE;
}

/* ========== Helper thấp mức ========== */

static int I2C3_WaitFlagSet(volatile uint32_t *reg, uint32_t flag)
{
    uint32_t timeout = I2C3_TIMEOUT;
    while (((*reg) & flag) == 0U) {
        if (--timeout == 0U) return -1;
    }
    return 0;
}

static int I2C3_WaitFlagClear(volatile uint32_t *reg, uint32_t flag)
{
    uint32_t timeout = I2C3_TIMEOUT;
    while (((*reg) & flag) != 0U) {
        if (--timeout == 0U) return -1;
    }
    return 0;
}

static int I2C3_StartWrite(uint8_t devAddr7bit)
{
    I2C3->CR1 |= I2C_CR1_START;
    if (I2C3_WaitFlagSet(&I2C3->SR1, I2C_SR1_SB) < 0) return -1;

    I2C3->DR = (uint8_t)(devAddr7bit << 1);
    if (I2C3_WaitFlagSet(&I2C3->SR1, I2C_SR1_ADDR) < 0) return -1;

    (void)I2C3->SR1;
    (void)I2C3->SR2;
    return 0;
}

static void I2C3_Stop(void)
{
    I2C3->CR1 |= I2C_CR1_STOP;
}

/* ========== API mức cao – đủ xài cho SSD1306 ========== */

/* Ghi 1 byte – thường dùng cho command (0x00 + cmd) */
void I2C3_WriteByte(uint8_t devAddr7bit, uint8_t data)
{
    if (I2C3_StartWrite(devAddr7bit) < 0) return;

    if (I2C3_WaitFlagSet(&I2C3->SR1, I2C_SR1_TXE) < 0) return;
    I2C3->DR = data;

    (void)I2C3_WaitFlagSet(&I2C3->SR1, I2C_SR1_BTF);
    I2C3_Stop();
}

/* Ghi liên tục nhiều byte – dùng đổ buffer lên OLED */
void I2C3_WriteMulti(uint8_t devAddr7bit, const uint8_t *data, uint16_t len)
{
    if (I2C3_StartWrite(devAddr7bit) < 0) return;

    for (uint16_t i = 0; i < len; i++) {
        if (I2C3_WaitFlagSet(&I2C3->SR1, I2C_SR1_TXE) < 0) break;
        I2C3->DR = data[i];
    }

    (void)I2C3_WaitFlagSet(&I2C3->SR1, I2C_SR1_BTF);
    I2C3_Stop();
}
