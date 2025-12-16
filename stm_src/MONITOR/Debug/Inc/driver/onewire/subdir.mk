################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Inc/driver/onewire/ds18b20.c 

OBJS += \
./Inc/driver/onewire/ds18b20.o 

C_DEPS += \
./Inc/driver/onewire/ds18b20.d 


# Each subdirectory must supply rules for building sources it contributes
Inc/driver/onewire/%.o Inc/driver/onewire/%.su Inc/driver/onewire/%.cyclo: ../Inc/driver/onewire/%.c Inc/driver/onewire/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F411VETx -DSTM32 -DSTM32F4 -DSTM32F411E_DISCO -DSTM32F411xE -c -I../Inc -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/CMSIS/Include" -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/HAL/Device/ST/STM32F4xx/Include" -I"/home/edna/Souce/STM_src/MONITOR/chip_headers/HAL/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Inc-2f-driver-2f-onewire

clean-Inc-2f-driver-2f-onewire:
	-$(RM) ./Inc/driver/onewire/ds18b20.cyclo ./Inc/driver/onewire/ds18b20.d ./Inc/driver/onewire/ds18b20.o ./Inc/driver/onewire/ds18b20.su

.PHONY: clean-Inc-2f-driver-2f-onewire

