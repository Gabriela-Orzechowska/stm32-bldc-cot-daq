################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Vendor/Third_Party/TinyUSB/src/typec/usbc.c 

OBJS += \
./Vendor/Third_Party/TinyUSB/src/typec/usbc.o 

C_DEPS += \
./Vendor/Third_Party/TinyUSB/src/typec/usbc.d 


# Each subdirectory must supply rules for building sources it contributes
Vendor/Third_Party/TinyUSB/src/typec/%.o Vendor/Third_Party/TinyUSB/src/typec/%.su Vendor/Third_Party/TinyUSB/src/typec/%.cyclo: ../Vendor/Third_Party/TinyUSB/src/typec/%.c Vendor/Third_Party/TinyUSB/src/typec/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu17 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H753xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I.././Vendor/Third_Party/TinyUSB/src -I../App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src-2f-typec

clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src-2f-typec:
	-$(RM) ./Vendor/Third_Party/TinyUSB/src/typec/usbc.cyclo ./Vendor/Third_Party/TinyUSB/src/typec/usbc.d ./Vendor/Third_Party/TinyUSB/src/typec/usbc.o ./Vendor/Third_Party/TinyUSB/src/typec/usbc.su

.PHONY: clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src-2f-typec

