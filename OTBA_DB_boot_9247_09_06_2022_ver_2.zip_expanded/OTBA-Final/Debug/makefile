################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/CommonLib/printf/src/subdir.mk
-include User/CommonLib/irq/src/subdir.mk
-include User/CommonLib/io/src/subdir.mk
-include User/CommonLib/fsm/src/subdir.mk
-include User/CommonLib/com/ip_allocation/src/subdir.mk
-include User/CommonLib/com/TELNET/src/subdir.mk
-include User/CommonLib/com/SerCom/src/subdir.mk
-include User/CommonLib/com/SPECTRA/src/subdir.mk
-include User/CommonLib/com/OTBBCOM/src/subdir.mk
-include User/CommonLib/com/CLI/src/subdir.mk
-include User/CommonLib/algo/RF/src/subdir.mk
-include User/CommonLib/algo/OPTMOD/src/subdir.mk
-include User/CommonLib/Middlewares/SETTINGS/src/subdir.mk
-include User/CommonLib/BSP/UART_IO/src/subdir.mk
-include User/CommonLib/BSP/SPI_IO/src/subdir.mk
-include User/CommonLib/BSP/OPT_ADC/src/subdir.mk
-include User/CommonLib/BSP/M24512/src/subdir.mk
-include User/CommonLib/BSP/LTC2666/src/subdir.mk
-include User/CommonLib/BSP/I2C_IO/src/subdir.mk
-include User/CommonLib/BSP/FPGA_OTBC/src/subdir.mk
-include User/CommonLib/BSP/FPGA_OTBA/src/subdir.mk
-include User/CommonLib/BSP/DAC7311/src/subdir.mk
-include User/CommonLib/BSP/DAC61408/src/subdir.mk
-include User/CommonLib/BSP/CPUTEMP/src/subdir.mk
-include User/CommonLib/BSP/ADS8681/src/subdir.mk
-include User/App/src/subdir.mk
-include User/App/Tasks/src/subdir.mk
-include User/App/TCPCOM/subdir.mk
-include User/App/SM/src/subdir.mk
-include User/App/RF/src/subdir.mk
-include User/App/OTBBCOM/subdir.mk
-include User/App/IBIT/src/subdir.mk
-include User/App/CLI/src/subdir.mk
-include User/App/CBIT/src/subdir.mk
-include Middlewares/Third_Party/LwIP/system/OS/subdir.mk
-include Middlewares/Third_Party/LwIP/src/netif/ppp/subdir.mk
-include Middlewares/Third_Party/LwIP/src/netif/subdir.mk
-include Middlewares/Third_Party/LwIP/src/core/ipv6/subdir.mk
-include Middlewares/Third_Party/LwIP/src/core/ipv4/subdir.mk
-include Middlewares/Third_Party/LwIP/src/core/subdir.mk
-include Middlewares/Third_Party/LwIP/src/apps/mqtt/subdir.mk
-include Middlewares/Third_Party/LwIP/src/api/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include LWIP/Target/subdir.mk
-include LWIP/App/subdir.mk
-include Drivers/STM32F7xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

BUILD_ARTIFACT_NAME := OTBA-Final
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
OTBA-Final.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
OTBA-Final.list \

OBJCOPY_BIN += \
OTBA-Final.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: OTBA-Final.elf secondary-outputs

# Tool invocations
OTBA-Final.elf: $(OBJS) $(USER_OBJS) C:\Users\haim_rozenboim\Dropbox\emroz\EMROZ_customer\MICROKIM\Git_WORK\OTBA_DB_boot_9247_09_06_2022_ver_2.zip_expanded\OTBA-Final\STM32F746IGKX_FLASH.ld
	arm-none-eabi-gcc -o "OTBA-Final.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"C:\Users\haim_rozenboim\Dropbox\emroz\EMROZ_customer\MICROKIM\Git_WORK\OTBA_DB_boot_9247_09_06_2022_ver_2.zip_expanded\OTBA-Final\STM32F746IGKX_FLASH.ld" --specs=nosys.specs -Wl,-Map="OTBA-Final.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

OTBA-Final.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "OTBA-Final.list"
	@echo 'Finished building: $@'
	@echo ' '

OTBA-Final.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "OTBA-Final.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	../add_crc_end_binary/add_crc_end_binary.exe -f OTBA-Final.bin
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY: post-build

-include ../makefile.targets
