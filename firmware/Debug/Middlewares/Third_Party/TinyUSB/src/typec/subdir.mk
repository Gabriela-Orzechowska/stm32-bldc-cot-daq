################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/TinyUSB/src/typec/usbc.c 

OBJS += \
./Middlewares/Third_Party/TinyUSB/src/typec/usbc.o 

C_DEPS += \
./Middlewares/Third_Party/TinyUSB/src/typec/usbc.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/TinyUSB/src/typec/%.o Middlewares/Third_Party/TinyUSB/src/typec/%.su Middlewares/Third_Party/TinyUSB/src/typec/%.cyclo: ../Middlewares/Third_Party/TinyUSB/src/typec/%.c Middlewares/Third_Party/TinyUSB/src/typec/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"/home/gabi/Projects/stm32-bldc-cot-daq/firmware/App" -I"/home/gabi/Projects/stm32-bldc-cot-daq/firmware/Middlewares/Third_Party/TinyUSB" -I"/home/gabi/Projects/stm32-bldc-cot-daq/firmware/Middlewares/Third_Party/TinyUSB/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-TinyUSB-2f-src-2f-typec

clean-Middlewares-2f-Third_Party-2f-TinyUSB-2f-src-2f-typec:
	-$(RM) ./Middlewares/Third_Party/TinyUSB/src/typec/usbc.cyclo ./Middlewares/Third_Party/TinyUSB/src/typec/usbc.d ./Middlewares/Third_Party/TinyUSB/src/typec/usbc.o ./Middlewares/Third_Party/TinyUSB/src/typec/usbc.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-TinyUSB-2f-src-2f-typec

