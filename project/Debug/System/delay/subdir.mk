################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../System/delay/delay.c 

OBJS += \
./System/delay/delay.o 

C_DEPS += \
./System/delay/delay.d 


# Each subdirectory must supply rules for building sources it contributes
System/delay/%.o: ../System/delay/%.c System/delay/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/alast/Documents/GitHub/pid-invertedpendulum/project/Hardware/OLED" -I"C:/Users/alast/Documents/GitHub/pid-invertedpendulum/project/System/delay" -I"C:/Users/alast/Documents/GitHub/pid-invertedpendulum/project/User/show" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-System-2f-delay

clean-System-2f-delay:
	-$(RM) ./System/delay/delay.d ./System/delay/delay.o

.PHONY: clean-System-2f-delay

