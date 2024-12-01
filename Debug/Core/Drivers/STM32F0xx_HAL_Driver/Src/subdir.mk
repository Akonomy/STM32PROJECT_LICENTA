################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_adc.c \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.c \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_exti.c \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.c \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_i2c.c \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_pwr.c \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.c \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.c \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.c \
../Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.c 

OBJS += \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_adc.o \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.o \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_exti.o \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.o \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_i2c.o \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_pwr.o \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.o \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.o \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.o \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.o 

C_DEPS += \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_adc.d \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.d \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_exti.d \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.d \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_i2c.d \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_pwr.d \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.d \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.d \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.d \
./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Drivers/STM32F0xx_HAL_Driver/Src/%.o Core/Drivers/STM32F0xx_HAL_Driver/Src/%.su Core/Drivers/STM32F0xx_HAL_Driver/Src/%.cyclo: ../Core/Drivers/STM32F0xx_HAL_Driver/Src/%.c Core/Drivers/STM32F0xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32F030x6 -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DHSI_VALUE=8000000 -DLSI_VALUE=40000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=0 -DDATA_CACHE_ENABLE=0 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Drivers-2f-STM32F0xx_HAL_Driver-2f-Src

clean-Core-2f-Drivers-2f-STM32F0xx_HAL_Driver-2f-Src:
	-$(RM) ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_adc.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_adc.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_adc.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_adc.su ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.su ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_exti.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_exti.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_exti.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_exti.su ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.su ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_i2c.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_i2c.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_i2c.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_i2c.su ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_pwr.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_pwr.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_pwr.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_pwr.su ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.su ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.su ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.su ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.cyclo ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.d ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.o ./Core/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.su

.PHONY: clean-Core-2f-Drivers-2f-STM32F0xx_HAL_Driver-2f-Src

