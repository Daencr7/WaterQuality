#include "stm32f4xx.h"
#include "i2c1.h"

#define I2C1_TIMEOUT   100000U

static void I2C1_GpioInit(void);
static void I2C1_CoreInit(void);

void I2C1_Init(void)
{
    I2C1_GpioInit();
    I2C1_CoreInit();
}

/* ========== GPIO PB8(SCL), PB9(SDA) – AF4, OD, Pull-up ========== */
static void I2C1_GpioInit(void)
{
    /* Bật clock GPIOB */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* PB8, PB9: Alternate function (10) */
    GPIOB->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
    GPIOB->MODER |=  (2U << (8 * 2)) | (2U << (9 * 2));

    /* Open-drain */
    GPIOB->OTYPER |= (1U << 8) | (1U << 9);

    /* Pull-up */
    GPIOB->PUPDR &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
    GPIOB->PUPDR |=  (1U << (8 * 2)) | (1U << (9 * 2));

    /* High speed */
    GPIOB->OSPEEDR |= (3U << (8 * 2)) | (3U << (9 * 2));

    /* AF4 cho PB8, PB9 */
    GPIOB->AFR[1] &= ~((0xFU << ((8 - 8) * 4)) | (0xFU << ((9 - 8) * 4)));
    GPIOB->AFR[1] |=  (4U   << ((8 - 8) * 4)) | (4U   << ((9 - 8) * 4));
}

/* ========== Core I2C1 – 100kHz @ APB1 = 42MHz ========== */
static void I2C1_CoreInit(void)
{
    /* Bật clock I2C1 */
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    /* Reset I2C1 */
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    /* Disable I2C1 trước khi config */
    I2C1->CR1 &= ~I2C_CR1_PE;

    /* Freq = 42MHz trên APB1 */
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= 42U;  // 42 MHz

    /* 100kHz standard mode: CCR = Fpclk / (2*Fscl) = 42MHz/(2*100k) = 210 */
    I2C1->CCR = 210U;
    I2C1->TRISE = 43U;  // Freq + 1

    /* Enable ACK */
    I2C1->CR1 |= I2C_CR1_ACK;

    /* Enable I2C1 */
    I2C1->CR1 |= I2C_CR1_PE;
}

/* ========== Helper thấp mức ========== */

static int I2C1_WaitFlagSet(volatile uint32_t *reg, uint32_t flag)
{
    uint32_t timeout = I2C1_TIMEOUT;
    while (((*reg) & flag) == 0U) {
        if (--timeout == 0U) return -1;
    }
    return 0;
}

static int I2C1_WaitFlagClear(volatile uint32_t *reg, uint32_t flag)
{
    uint32_t timeout = I2C1_TIMEOUT;
    while (((*reg) & flag) != 0U) {
        if (--timeout == 0U) return -1;
    }
    return 0;
}

static int I2C1_StartWrite(uint8_t devAddr7bit)
{
    /* Generate START */
    I2C1->CR1 |= I2C_CR1_START;
    if (I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_SB) < 0) return -1;

    /* Gửi address + write(0) */
    I2C1->DR = (uint8_t)(devAddr7bit << 1);
    if (I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_ADDR) < 0) return -1;

    /* Clear ADDR bằng việc đọc SR1 rồi SR2 */
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    return 0;
}

static int I2C1_StartRead(uint8_t devAddr7bit)
{
    /* Generate START */
    I2C1->CR1 |= I2C_CR1_START;
    if (I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_SB) < 0) return -1;

    /* Gửi address + read(1) */
    I2C1->DR = (uint8_t)((devAddr7bit << 1) | 1U);
    if (I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_ADDR) < 0) return -1;

    /* Clear ADDR */
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    return 0;
}

static void I2C1_Stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
}

/* ========== API mức cao ========== */

void I2C1_WriteRegister(uint8_t devAddr7bit, uint8_t regAddr, uint8_t data)
{
    /* Bắt đầu write */
    if (I2C1_StartWrite(devAddr7bit) < 0) return;

    /* Gửi regAddr */
    if (I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_TXE) < 0) return;
    I2C1->DR = regAddr;

    /* Gửi data */
    if (I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_TXE) < 0) return;
    I2C1->DR = data;

    /* Chờ BTF rồi STOP */
    (void)I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_BTF);
    I2C1_Stop();
}

void I2C1_WriteMulti(uint8_t devAddr7bit, uint8_t regAddr, const uint8_t *data, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        I2C1_WriteRegister(devAddr7bit, (uint8_t)(regAddr + i), data[i]);
    }
}

uint8_t I2C1_ReadRegister(uint8_t devAddr7bit, uint8_t regAddr)
{
    uint8_t value = 0;

    /* Bước 1: ghi regAddr (write) */
    if (I2C1_StartWrite(devAddr7bit) < 0) return 0;

    /* Gửi regAddr */
    if (I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_TXE) < 0) return 0;
    I2C1->DR = regAddr;
    (void)I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_BTF);

    /* Bước 2: RESTART để đọc 1 byte */
    /* Disable ACK cho single-byte */
    I2C1->CR1 &= ~I2C_CR1_ACK;

    if (I2C1_StartRead(devAddr7bit) < 0) return 0;

    /* Chuẩn bị STOP ngay sau khi nhận byte */
    I2C1_Stop();

    /* Chờ RXNE */
    if (I2C1_WaitFlagSet(&I2C1->SR1, I2C_SR1_RXNE) < 0) return 0;
    value = (uint8_t)I2C1->DR;

    /* Bật lại ACK cho lần đọc sau */
    I2C1->CR1 |= I2C_CR1_ACK;

    return value;
}

void I2C1_ReadMulti(uint8_t devAddr7bit, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        data[i] = I2C1_ReadRegister(devAddr7bit, (uint8_t)(regAddr + i));
    }
}
