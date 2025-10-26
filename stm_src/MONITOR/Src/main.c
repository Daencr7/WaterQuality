#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "./driver/SysTick/systick.h"
#include "./driver/Uart/uart.h"
#include "./driver/Adc/adc.h"
#include "./sensor/sensor.h"
#define GPIODEN (1U<<3) // Dấu cách đã được gõ lại
#define PIN12 (1U<<13) // Dấu cách đã được gõ lại
#define LED_PIN  PIN12

volatile uint16_t pa1;
volatile uint16_t pa4;
volatile uint16_t pa3;
volatile uint16_t pa5;

void Send_FixedPoint_JSON_To_ESP32(sensor_value_t temp, sensor_value_t ph, sensor_value_t tds, sensor_value_t turb);

int main(void)
{

    ADC_Init_Scan_DMA();
    uart_init();
//    USART3_Init_CMSIS();
    while(1)
    {
        pa1 = ADC_GetValue(0); // Ch.1 (PA1)
        pa3 = ADC_GetValue(2); // Ch.3 (PA3)
        pa4 = ADC_GetValue(3); // Ch.4 (PA4)
        pa5 = ADC_GetValue(1); // Ch.5 (PA5)
        int32_t temp_fp = calc_temp_fp(pa4);
        int32_t ph_fp   = calc_ph_fp(pa1);
        int32_t tds_fp  = calc_tds_fp(pa5);
        int32_t turb_fp = calc_turb_fp(pa3);

        sensor_value_t temp_val = convert_fp_to_struct(temp_fp);
        sensor_value_t ph_val   = convert_fp_to_struct(ph_fp);
        sensor_value_t tds_val  = convert_fp_to_struct(tds_fp);
        sensor_value_t turb_val = convert_fp_to_struct(turb_fp);

//        printf("Temp: %d.%01d C\r\n", (int)temp_val.integer, (int)temp_val.decimal);
//        printf("pH:   %d.%01d\r\n", (int)ph_val.integer, (int)ph_val.decimal);
//        printf("TDS:  %d.%01d PPM\r\n", (int)tds_val.integer, (int)tds_val.decimal);
//        printf("NTU:  %d.%01d\r\n", (int)turb_val.integer, (int)turb_val.decimal);
        Send_FixedPoint_JSON_To_ESP32(temp_val, ph_val, tds_val, turb_val);

        systick_msec_delay(2000);       
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

