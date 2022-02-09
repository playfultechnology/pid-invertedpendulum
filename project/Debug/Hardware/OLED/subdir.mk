################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardware/OLED/oled.c 

OBJS += \
./Hardware/OLED/oled.o 

C_DEPS += \
./Hardware/OLED/oled.d 


# Each subdirectory must supply rules for building sources it contributes
Hardware/OLED/%.o: ../Hardware/OLED/%.c Hardware/OLED/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/alast/Documents/GitHub/pid-invertedpendulum/project/Hardware/OLED" -I"C:/Users/alast/Documents/GitHub/pid-invertedpendulum/project/System/delay" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Hardware-2f-OLED

clean-Hardware-2f-OLED:
	-$(RM) ./Hardware/OLED/oled.d ./Hardware/OLED/oled.o

.PHONY: clean-Hardware-2f-OLED

