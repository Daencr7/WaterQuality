################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Inc/driver/Oled/oled.c 

OBJS += \
./Inc/driver/Oled/oled.o 

C_DEPS += \
./Inc/driver/Oled/oled.d 


# Each subdirectory must supply rules for building sources it contributes
Inc/driver/Oled/%.o Inc/driver/Oled/%.su Inc/driver/Oled/%.cyclo: ../Inc/driver/Oled/%.c Inc/driver/Oled/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F411VETx -DSTM32 -DSTM32F4 -DSTM32F411E_DISCO -DSTM32F411xE -c -I../Inc -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/CMSIS/Include" -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/HAL/Device/ST/STM32F4xx/Include" -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/HAL/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Inc-2f-driver-2f-Oled

clean-Inc-2f-driver-2f-Oled:
	-$(RM) ./Inc/driver/Oled/oled.cyclo ./Inc/driver/Oled/oled.d ./Inc/driver/Oled/oled.o ./Inc/driver/Oled/oled.su

.PHONY: clean-Inc-2f-driver-2f-Oled

