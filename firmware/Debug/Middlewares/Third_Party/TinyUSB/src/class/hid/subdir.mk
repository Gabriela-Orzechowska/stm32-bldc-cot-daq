################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/TinyUSB/src/class/hid/hid_device.c \
../Middlewares/Third_Party/TinyUSB/src/class/hid/hid_host.c 

OBJS += \
./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_device.o \
./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_host.o 

C_DEPS += \
./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_device.d \
./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_host.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/TinyUSB/src/class/hid/%.o Middlewares/Third_Party/TinyUSB/src/class/hid/%.su Middlewares/Third_Party/TinyUSB/src/class/hid/%.cyclo: ../Middlewares/Third_Party/TinyUSB/src/class/hid/%.c Middlewares/Third_Party/TinyUSB/src/class/hid/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"/home/gabi/Projects/stm32-bldc-cot-daq/firmware/App" -I"/home/gabi/Projects/stm32-bldc-cot-daq/firmware/Middlewares/Third_Party/TinyUSB" -I"/home/gabi/Projects/stm32-bldc-cot-daq/firmware/Middlewares/Third_Party/TinyUSB/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-TinyUSB-2f-src-2f-class-2f-hid

clean-Middlewares-2f-Third_Party-2f-TinyUSB-2f-src-2f-class-2f-hid:
	-$(RM) ./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_device.cyclo ./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_device.d ./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_device.o ./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_device.su ./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_host.cyclo ./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_host.d ./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_host.o ./Middlewares/Third_Party/TinyUSB/src/class/hid/hid_host.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-TinyUSB-2f-src-2f-class-2f-hid

