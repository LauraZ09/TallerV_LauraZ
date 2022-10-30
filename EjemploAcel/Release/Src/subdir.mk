################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/MainADC.c \
../Src/MainMCADC.c \
../Src/MainTimerEvent.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/MainADC.o \
./Src/MainMCADC.o \
./Src/MainTimerEvent.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/MainADC.d \
./Src/MainMCADC.d \
./Src/MainTimerEvent.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DSTM32 -DSTM32F4 -DSTM32F411RETx -c -I../Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

