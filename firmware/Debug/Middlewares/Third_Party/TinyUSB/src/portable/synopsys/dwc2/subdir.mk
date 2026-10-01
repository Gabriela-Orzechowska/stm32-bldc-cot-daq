################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.c \
../Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.c \
../Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.c 

OBJS += \
./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.o \
./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.o \
./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.o 

C_DEPS += \
./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.d \
./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.d \
./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/%.o Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/%.su Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/%.cyclo: ../Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/%.c Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"/home/gabi/Projects/stm32-bldc-cot-daq/firmware/App" -I"/home/gabi/Projects/stm32-bldc-cot-daq/firmware/Middlewares/Third_Party/TinyUSB" -I"/home/gabi/Projects/stm32-bldc-cot-daq/firmware/Middlewares/Third_Party/TinyUSB/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-TinyUSB-2f-src-2f-portable-2f-synopsys-2f-dwc2

clean-Middlewares-2f-Third_Party-2f-TinyUSB-2f-src-2f-portable-2f-synopsys-2f-dwc2:
	-$(RM) ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.cyclo ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.d ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.o ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dcd_dwc2.su ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.cyclo ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.d ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.o ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/dwc2_common.su ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.cyclo ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.d ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.o ./Middlewares/Third_Party/TinyUSB/src/portable/synopsys/dwc2/hcd_dwc2.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-TinyUSB-2f-src-2f-portable-2f-synopsys-2f-dwc2

