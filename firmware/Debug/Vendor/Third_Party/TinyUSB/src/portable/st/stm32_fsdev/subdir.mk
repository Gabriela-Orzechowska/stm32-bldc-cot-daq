################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c \
../Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/fsdev_common.c \
../Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/hcd_stm32_fsdev.c 

OBJS += \
./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.o \
./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/fsdev_common.o \
./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/hcd_stm32_fsdev.o 

C_DEPS += \
./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.d \
./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/fsdev_common.d \
./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/hcd_stm32_fsdev.d 


# Each subdirectory must supply rules for building sources it contributes
Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/%.o Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/%.su Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/%.cyclo: ../Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/%.c Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu17 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H753xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I.././Vendor/Third_Party/TinyUSB/src -I../App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src-2f-portable-2f-st-2f-stm32_fsdev

clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src-2f-portable-2f-st-2f-stm32_fsdev:
	-$(RM) ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.cyclo ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.d ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.o ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.su ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/fsdev_common.cyclo ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/fsdev_common.d ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/fsdev_common.o ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/fsdev_common.su ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/hcd_stm32_fsdev.cyclo ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/hcd_stm32_fsdev.d ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/hcd_stm32_fsdev.o ./Vendor/Third_Party/TinyUSB/src/portable/st/stm32_fsdev/hcd_stm32_fsdev.su

.PHONY: clean-Vendor-2f-Third_Party-2f-TinyUSB-2f-src-2f-portable-2f-st-2f-stm32_fsdev

