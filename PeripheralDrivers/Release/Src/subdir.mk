################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/ADXL345.c \
../Src/AdcDriver.c \
../Src/AdcMCDriver.c \
../Src/BasicTimer.c \
../Src/ExtiDriver.c \
../Src/GPIOxDriver.c \
../Src/I2CDriver.c \
../Src/LCD2004A.c \
../Src/PwmDriver.c \
../Src/USARTxDriver.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/ADXL345.o \
./Src/AdcDriver.o \
./Src/AdcMCDriver.o \
./Src/BasicTimer.o \
./Src/ExtiDriver.o \
./Src/GPIOxDriver.o \
./Src/I2CDriver.o \
./Src/LCD2004A.o \
./Src/PwmDriver.o \
./Src/USARTxDriver.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/ADXL345.d \
./Src/AdcDriver.d \
./Src/AdcMCDriver.d \
./Src/BasicTimer.d \
./Src/ExtiDriver.d \
./Src/GPIOxDriver.d \
./Src/I2CDriver.d \
./Src/LCD2004A.d \
./Src/PwmDriver.d \
./Src/USARTxDriver.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DSTM32 -DSTM32F4 -DSTM32F411RETx -c -I../Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

