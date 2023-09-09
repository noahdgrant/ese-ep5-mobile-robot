# -*- Makefile -*-

# Binaries will be generated with this name (.elf, .bin, .hex)
PROJ_NAME=ep5_mobile_robot

# STM standard peripheral library
STM_COMMON=./STM32F30x_DSP_StdPeriph_Lib_V1.3.0


# Compiler settings. Only edit CFLAGS to include other header files.
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

# Compiler flags
CFLAGS  = -g -O2 -Wall -Tstm32_flash.ld
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -I.

# Include files from STM libraries
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Device/ST/STM32F30x/Include
CFLAGS += -I$(STM_COMMON)/Libraries/STM32F30x_StdPeriph_Driver/inc
CFLAGS += -I$(STM_COMMON)/Utilities/STM32_EVAL

# Variable declaration
BIN=/main
BUILD_DIR=build
SRC=src
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(SRCS:.c=.o)

# Add startup file to build
SRCS += $(SRC)/startup_stm32f303xe.s

vpath %.c $(STM_COMMON)/Libraries/STM32F30x_StdPeriph_Driver/src \

.PHONY: proj

# MAKE PROJECT
all: proj

proj: $(PROJ_NAME).elf

# Output .elf .hex .bin files to build/
$(PROJ_NAME).elf: $(SRCS)
		$(CC) $(CFLAGS) $^ -o $@
		$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
		$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
		rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

# Flash the STM32F303RE
burn: proj
		st-flash write $(PROJ_NAME).bin 0x80000000
