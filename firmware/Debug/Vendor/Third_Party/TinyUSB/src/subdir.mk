################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Vendor/Third_Party/TinyUSB/src/tusb.c 

OBJS += \
./Vendor/Third_Party/TinyUSB/src/tusb.o 

C_DEPS += \
./Vendor/Third_Party/TinyUSB/src/tusb.d 


# Each subdirectory must supply rules for building sources it contributes
Vendor/Third_Party/TinyUSB/src/%.o Vendor/Third_Party/TinyUSB/src/%.su Vendor/Third_Party/TinyUSB/src/%.cyclo: ../Vendor/Third_Party/TinyUSB/src/%.c Vendor/Third_Party/TinyUSB/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu17 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H753xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I.././Vendor/Third_Party/TinyUSB/src -I../App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src

clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src:
	-$(RM) ./Vendor/Third_Party/TinyUSB/src/tusb.cyclo ./Vendor/Third_Party/TinyUSB/src/tusb.d ./Vendor/Third_Party/TinyUSB/src/tusb.o ./Vendor/Third_Party/TinyUSB/src/tusb.su

.PHONY: clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src

