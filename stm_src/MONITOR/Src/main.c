#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "./driver/SysTick/systick.h"
#include "./driver/Uart/uart.h"
#include "./driver/Adc/adc.h"
#include "./driver/Onewire/ds18b20.h"

#include "./driver/I2c/i2c1.h"
#include "./driver/I2c/i2c3.h"
#include "./driver/Oled/oled.h"
#include "./driver/Rtc/rtc3231.h"
#include "./sensor/sensor.h"

#define GPIODEN (1U<<3) // Dấu cách đã được gõ lại
#define PIN12 (1U<<13) // Dấu cách đã được gõ lại
#define LED_PIN  PIN12

volatile uint16_t pa1;
volatile uint16_t pa4;
volatile uint16_t pa3;
volatile uint16_t pa5;
volatile int32_t temp_fp;
void Send_FixedPoint_JSON_To_ESP32(sensor_value_t temp, sensor_value_t ph, sensor_value_t tds, sensor_value_t turb);

volatile uint8_t btn_flag = 0;
int8_t i = 1;
/* PA0 button, PC6 output */
static void GPIO_Init_All(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
#include "core_cm4.h"
    // PA0 input, không pull (ngoài đã có R35 kéo xuống)
    GPIOA->MODER &= ~(3U << (0 * 2));
    GPIOA->PUPDR &= ~(3U << (0 * 2));

    // PC6 output
    GPIOC->MODER &= ~(3U << (6 * 2));
    GPIOC->MODER |=  (1U << (6 * 2));
    GPIOC->OTYPER &= ~(1U << 6);
    GPIOC->OSPEEDR |= (3U << (6 * 2));
    GPIOC->PUPDR &= ~(3U << (6 * 2));

    // PC6 off ban đầu
    GPIOC->BSRR = (1U << (6 + 16));
}

static void EXTI0_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;   // PA0 -> EXTI0

    EXTI->IMR  |= EXTI_IMR_IM0;
    EXTI->RTSR |= EXTI_RTSR_TR0;      // cạnh lên (vì nút kéo lên VDD)
    EXTI->FTSR &= ~EXTI_FTSR_TR0;

    NVIC_SetPriority(EXTI0_IRQn, 2);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;   // clear pending

        // KHÔNG dùng delayms ở đây
        // chỉ set cờ cho main xử lý
        btn_flag = 1;
    }
}
int main(void)
{
    ADC_Init_Scan_DMA();
    uart_init();
    DS18B20_Init();

//    SystemInit();
    /* Init I2C cho DS3231 + OLED */
    I2C1_Init();       // PB8/PB9
    I2C3_Init();       // PA8/PC9

    /* Init RTC & OLED */
    RTC3231_Init();
    SSD1306_Init();

    GPIO_Init_All();
    EXTI0_Init();
    RTC3231_SetTime24(17,44,30);
    while(1)
    {
        if (btn_flag)
        {
            btn_flag = 0;

            GPIOC->BSRR = (1U << 6);        // bật PC6
            systick_msec_delay(5000);
            GPIOC->BSRR = (1U << (6 + 16)); // tắt PC6
        }


        DS3231_Time_t now;
        if(now.hours == 12 && now.minutes ==30 && now.seconds == 30) {
        	btn_flag = 0;
        }

        if (RTC3231_ReadTime(&now) == 0) {
            SSD1306_DisplayTimeVerticalBig(&now);
        }

        pa1 = ADC_GetValue(0); // Ch.1 (PA1)
        pa3 = ADC_GetValue(2); // Ch.3 (PA3)
        pa5 = ADC_GetValue(1); // Ch.5 (PA5)

         temp_fp = DS18B20_ReadTemp_fp();
        int32_t ph_fp   = calc_ph_fp(pa5);
        int32_t tds_fp  = calc_tds_fp(pa1);
        int32_t turb_fp = calc_turb_fp(pa3);

        sensor_value_t temp_val = convert_fp_to_struct(temp_fp);
        sensor_value_t ph_val   = convert_fp_to_struct(ph_fp);
        sensor_value_t tds_val  = convert_fp_to_struct(tds_fp);
        sensor_value_t turb_val = convert_fp_to_struct(turb_fp);
//        temp_val.integer = 27;
//        temp_val.decimal = 5;
        ph_val.integer = 6;
        ph_val.decimal = 5;


        if(i == 1) ph_val.decimal += 4, i = 0;
        else ph_val.decimal -= 4, i = 1;
//        printf("Temp: %d.%01d C\r\n", (int)temp_val.integer, (int)temp_val.decimal);
//        printf("pH:   %d.%01d\r\n", (int)ph_val.integer, (int)ph_val.decimal);
//        printf("TDS:  %d.%01d PPM\r\n", (int)tds_val.integer, (int)tds_val.decimal);
//        printf("NTU:  %d.%01d\r\n", (int)turb_val.integer, (int)turb_val.decimal);

        Send_FixedPoint_JSON_To_ESP32(temp_val, ph_val, tds_val, turb_val);
        

        systick_msec_delay(1000);
     }
}
void Send_FixedPoint_JSON_To_ESP32(sensor_value_t temp, sensor_value_t ph, sensor_value_t tds, sensor_value_t turb)
{
    printf("{\"temp\":%d.%01d,\"ph\":%d.%01d,\"tds\":%d.%01d,\"ntu\":%d.%01d}\r\n",
           temp.integer, temp.decimal,
           ph.integer, ph.decimal,
           tds.integer, tds.decimal,
           turb.integer, turb.decimal);
}


