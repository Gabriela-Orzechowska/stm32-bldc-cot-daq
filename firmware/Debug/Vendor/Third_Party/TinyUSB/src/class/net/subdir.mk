################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Vendor/Third_Party/TinyUSB/src/class/net/ecm_rndis_device.c \
../Vendor/Third_Party/TinyUSB/src/class/net/ncm_device.c 

OBJS += \
./Vendor/Third_Party/TinyUSB/src/class/net/ecm_rndis_device.o \
./Vendor/Third_Party/TinyUSB/src/class/net/ncm_device.o 

C_DEPS += \
./Vendor/Third_Party/TinyUSB/src/class/net/ecm_rndis_device.d \
./Vendor/Third_Party/TinyUSB/src/class/net/ncm_device.d 


# Each subdirectory must supply rules for building sources it contributes
Vendor/Third_Party/TinyUSB/src/class/net/%.o Vendor/Third_Party/TinyUSB/src/class/net/%.su Vendor/Third_Party/TinyUSB/src/class/net/%.cyclo: ../Vendor/Third_Party/TinyUSB/src/class/net/%.c Vendor/Third_Party/TinyUSB/src/class/net/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu17 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H753xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I.././Vendor/Third_Party/TinyUSB/src -I../App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src-2f-class-2f-net

clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src-2f-class-2f-net:
	-$(RM) ./Vendor/Third_Party/TinyUSB/src/class/net/ecm_rndis_device.cyclo ./Vendor/Third_Party/TinyUSB/src/class/net/ecm_rndis_device.d ./Vendor/Third_Party/TinyUSB/src/class/net/ecm_rndis_device.o ./Vendor/Third_Party/TinyUSB/src/class/net/ecm_rndis_device.su ./Vendor/Third_Party/TinyUSB/src/class/net/ncm_device.cyclo ./Vendor/Third_Party/TinyUSB/src/class/net/ncm_device.d ./Vendor/Third_Party/TinyUSB/src/class/net/ncm_device.o ./Vendor/Third_Party/TinyUSB/src/class/net/ncm_device.su

.PHONY: clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src-2f-class-2f-net

