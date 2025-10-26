################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Inc/driver/Uart/uart.c 

OBJS += \
./Inc/driver/Uart/uart.o 

C_DEPS += \
./Inc/driver/Uart/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Inc/driver/Uart/%.o Inc/driver/Uart/%.su Inc/driver/Uart/%.cyclo: ../Inc/driver/Uart/%.c Inc/driver/Uart/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F411VETx -DSTM32 -DSTM32F4 -DSTM32F411E_DISCO -DSTM32F411xE -c -I../Inc -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/CMSIS/Include" -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/HAL/Device/ST/STM32F4xx/Include" -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/HAL/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Inc-2f-driver-2f-Uart

clean-Inc-2f-driver-2f-Uart:
	-$(RM) ./Inc/driver/Uart/uart.cyclo ./Inc/driver/Uart/uart.d ./Inc/driver/Uart/uart.o ./Inc/driver/Uart/uart.su

.PHONY: clean-Inc-2f-driver-2f-Uart

