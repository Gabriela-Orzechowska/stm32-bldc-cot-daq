################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/application.c \
../App/config.c \
../App/packet.c \
../App/usb_descriptors.c 

OBJS += \
./App/application.o \
./App/config.o \
./App/packet.o \
./App/usb_descriptors.o 

C_DEPS += \
./App/application.d \
./App/config.d \
./App/packet.d \
./App/usb_descriptors.d 


# Each subdirectory must supply rules for building sources it contributes
App/%.o App/%.su App/%.cyclo: ../App/%.c App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu17 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H753xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I.././Vendor/Third_Party/TinyUSB/src -I../App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App

clean-App:
	-$(RM) ./App/application.cyclo ./App/application.d ./App/application.o ./App/application.su ./App/config.cyclo ./App/config.d ./App/config.o ./App/config.su ./App/packet.cyclo ./App/packet.d ./App/packet.o ./App/packet.su ./App/usb_descriptors.cyclo ./App/usb_descriptors.d ./App/usb_descriptors.o ./App/usb_descriptors.su

.PHONY: clean-App

