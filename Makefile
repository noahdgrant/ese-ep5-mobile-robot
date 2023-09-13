# -.- Makefile -.-

DEVICE = STM32F303xE
FLASH  = 0x08000000

USE_ST_CMSIS = true

SERIES_CPU = cortex-m4
SERIES_ARCH = armv7e-m+fp

# Standard values for (linked) STM32-base folders
STM32_BASE_PATH   ?= ./stm32-base
STM32_CUBE_PATH   ?= ./stm32-base

# STM32-base sub-folders
BASE_LINKER   = $(STM32_BASE_PATH)/linker
BASE_MAKE     = $(STM32_BASE_PATH)/make
BASE_STARTUP  = $(STM32_BASE_PATH)/startup

# Standard values for project folders
BIN_FOLDER ?= ./bin
OBJ_FOLDER ?= ./obj
SRC_FOLDER ?= ./src
INC_FOLDER ?= ./inc

SERIES_FOLDER = STM32F3xx

MAPPED_DEVICE ?= $(DEVICE)

# Default flashing tool is st-flash
FLASHING_TOOL ?= st-flash

# The toolchain path, defaults to using the globally installed toolchain
ifdef TOOLCHAIN_PATH
    TOOLCHAIN_SEPARATOR = /
endif

TOOLCHAIN_PATH      ?= /opt/gcc-arm-none-eabi/bin/
TOOLCHAIN_SEPARATOR ?=
TOOLCHAIN_PREFIX    ?= arm-none-eabi-

CC = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)$(TOOLCHAIN_PREFIX)gcc
OBJCOPY = $(TOOLCHAIN_PATH)$(TOOLCHAIN_SEPARATOR)$(TOOLCHAIN_PREFIX)objcopy

# Flags - Overall Options
CPPFLAGS += -specs=nosys.specs

# Flags - C Language Options
CFLAGS += -ffreestanding

# Flags - Warning Options
CPPFLAGS += -Wall
CPPFLAGS += -Wextra

# Flags - Debugging Options
CPPFLAGS += -g

# Flags - Optimization Options
CPPFLAGS += -ffunction-sections
CPPFLAGS += -fdata-sections

# Flags - Preprocessor options
CPPFLAGS += -D $(MAPPED_DEVICE)

# Flags - Assembler Options
ifneq (,$(or USE_ST_CMSIS, USE_ST_HAL))
    CPPFLAGS += -Wa,--defsym,CALL_ARM_SYSTEM_INIT=1
endif

# Flags - Linker Options
# CPPFLAGS += -nostdlib
CPPFLAGS += -Wl,-L$(BASE_LINKER),-T$(BASE_LINKER)/$(DEVICE).ld

# Flags - Directory Options
CPPFLAGS += -I$(INC_FOLDER)
CPPFLAGS += -I$(BASE_STARTUP)

# Flags - Machine-dependant options
CPPFLAGS += -mcpu=$(SERIES_CPU)
CPPFLAGS += -march=$(SERIES_ARCH)
CPPFLAGS += -mlittle-endian
CPPFLAGS += -mthumb
CPPFLAGS += -masm-syntax-unified

# Output files
ELF_FILE_NAME ?= stm32_executable.elf
BIN_FILE_NAME ?= stm32_bin_image.bin
OBJ_FILE_NAME ?= startup_$(MAPPED_DEVICE).o

ELF_FILE_PATH = $(BIN_FOLDER)/$(ELF_FILE_NAME)
BIN_FILE_PATH = $(BIN_FOLDER)/$(BIN_FILE_NAME)
OBJ_FILE_PATH = $(OBJ_FOLDER)/$(OBJ_FILE_NAME)

# Input files
SRC ?=
SRC += $(SRC_FOLDER)/*.c

# Startup file
DEVICE_STARTUP = $(BASE_STARTUP)/$(MAPPED_DEVICE).s

# Include the CMSIS files, using the HAL implies using the CMSIS
ifneq (,$(or USE_ST_CMSIS, USE_ST_HAL))
    CPPFLAGS += -I$(STM32_CUBE_PATH)/CMSIS/ARM/inc
    CPPFLAGS += -I$(STM32_CUBE_PATH)/CMSIS/inc

    SRC += $(STM32_CUBE_PATH)/CMSIS/src/*.c
endif

# Include the HAL files
ifdef USE_ST_HAL
    CPPFLAGS += -D USE_HAL_DRIVER
    CPPFLAGS += -I$(STM32_CUBE_PATH)/HAL/inc

    # A simply expanded variable is used here to perform the find command only once.
    HAL_SRC := $(shell find $(STM32_CUBE_PATH)/HAL/src/*.c ! -name '*_template.c')
    SRC += $(HAL_SRC)
endif

# Make all
all:$(BIN_FILE_PATH)

$(BIN_FILE_PATH): $(ELF_FILE_PATH)
	$(OBJCOPY) -O binary $^ $@

$(ELF_FILE_PATH): $(SRC) $(OBJ_FILE_PATH) | $(BIN_FOLDER)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(OBJ_FILE_PATH): $(DEVICE_STARTUP) | $(OBJ_FOLDER)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(BIN_FOLDER):
	mkdir $(BIN_FOLDER)

$(OBJ_FOLDER):
	mkdir $(OBJ_FOLDER)

# Make clean
clean:
	rm -f $(ELF_FILE_PATH)
	rm -f $(BIN_FILE_PATH)
	rm -f $(OBJ_FILE_PATH)

# Make flash
flash:
ifeq ($(FLASHING_TOOL), st-flash)
	st-flash write $(BIN_FOLDER)/$(BIN_FILE_NAME) $(FLASH)
else ifeq ($(FLASHING_TOOL), stm32flash)
	stm32flash -w $(BIN_FOLDER)/$(BIN_FILE_NAME) -v -R $(FLASHING_SERIAL_PORT)
else
	$(error $(FLASHING_TOOL) not recognized)
endif

.PHONY: all clean flash
