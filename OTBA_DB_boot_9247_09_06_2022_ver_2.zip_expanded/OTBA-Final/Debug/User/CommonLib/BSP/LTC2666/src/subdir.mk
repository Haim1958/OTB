################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/CommonLib/BSP/LTC2666/src/ltc2666.c \
../User/CommonLib/BSP/LTC2666/src/ltc2666_reg.c 

OBJS += \
./User/CommonLib/BSP/LTC2666/src/ltc2666.o \
./User/CommonLib/BSP/LTC2666/src/ltc2666_reg.o 

C_DEPS += \
./User/CommonLib/BSP/LTC2666/src/ltc2666.d \
./User/CommonLib/BSP/LTC2666/src/ltc2666_reg.d 


# Each subdirectory must supply rules for building sources it contributes
User/CommonLib/BSP/LTC2666/src/ltc2666.o: ../User/CommonLib/BSP/LTC2666/src/ltc2666.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F746xx -DDEBUG -c -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Middlewares/Third_Party/LwIP/system/arch -I../Middlewares/Third_Party/LwIP/src/include -I../LWIP/App -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Core/Inc -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Drivers/CMSIS/Include -I../LWIP/Target -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I"C:/Users/haim_rozenboim/Dropbox/emroz/EMROZ_customer/MICROKIM/Git_WORK/OTBA_DB_boot_9247_09_06_2022_ver_2.zip_expanded/OTBA-Final/User/App/inc" -I../Middlewares/Third_Party/LwIP/src/include/netif -I"C:/Users/haim_rozenboim/Dropbox/emroz/EMROZ_customer/MICROKIM/Git_WORK/OTBA_DB_boot_9247_09_06_2022_ver_2.zip_expanded/OTBA-Final/User/CommonLib/inc" -I/OTBA-Final/User/App/SM/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"User/CommonLib/BSP/LTC2666/src/ltc2666.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
User/CommonLib/BSP/LTC2666/src/ltc2666_reg.o: ../User/CommonLib/BSP/LTC2666/src/ltc2666_reg.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F746xx -DDEBUG -c -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Middlewares/Third_Party/LwIP/system/arch -I../Middlewares/Third_Party/LwIP/src/include -I../LWIP/App -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Core/Inc -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Drivers/CMSIS/Include -I../LWIP/Target -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I"C:/Users/haim_rozenboim/Dropbox/emroz/EMROZ_customer/MICROKIM/Git_WORK/OTBA_DB_boot_9247_09_06_2022_ver_2.zip_expanded/OTBA-Final/User/App/inc" -I../Middlewares/Third_Party/LwIP/src/include/netif -I"C:/Users/haim_rozenboim/Dropbox/emroz/EMROZ_customer/MICROKIM/Git_WORK/OTBA_DB_boot_9247_09_06_2022_ver_2.zip_expanded/OTBA-Final/User/CommonLib/inc" -I/OTBA-Final/User/App/SM/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"User/CommonLib/BSP/LTC2666/src/ltc2666_reg.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

