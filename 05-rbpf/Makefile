# Copyright (C) 2024 Université de Lille
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

PREFIX          = arm-none-eabi-
CC              = $(PREFIX)gcc
LD              = $(PREFIX)gcc
OBJCOPY         = $(PREFIX)objcopy

CFLAGS          = -Wall
CFLAGS         += -Wextra
CFLAGS         += -Werror
CFLAGS         += -mthumb
CFLAGS         += -mcpu=cortex-m4
CFLAGS         += -mfloat-abi=hard
CFLAGS         += -mfpu=fpv4-sp-d16
CFLAGS         += -msingle-pic-base
CFLAGS         += -mpic-register=sl
CFLAGS         += -mno-pic-data-is-text-relative
CFLAGS         += -fPIC
CFLAGS         += -ffreestanding
ifdef DEBUG
CFLAGS         += -Og
CFLAGS         += -ggdb
else
CFLAGS         += -Os
endif
CFLAGS         += -Wno-unused-parameter
CFLAGS         += -Istdriot
CFLAGS         += -Isrc/RIOT/sys/include
CFLAGS         += -Isrc/RIOT/sys/include/rbpf
CFLAGS         += -Isrc/RIOT/core/lib/include

LDFLAGS         = -nostartfiles
LDFLAGS        += -nodefaultlibs
LDFLAGS        += -nolibc
LDFLAGS        += -nostdlib
LDFLAGS        += -Tlink.ld
LDFLAGS        += -Wl,-q
# Disable the new linker warning '--warn-rwx-segments' introduced by
# Binutils 2.39, which causes the following message: "warning:
# $(TARGET).elf has a LOAD segment with RWX permissions".
ifeq ($(shell $(PREFIX)ld --help | grep -q 'warn-rwx-segments'; echo $$?), 0)
LDFLAGS        += -Wl,--no-warn-rwx-segments
endif

OBJCOPYFLAGS    = --input-target=elf32-littlearm
OBJCOPYFLAGS   += --output-target=binary

TARGET          = rbpf

BUILD_DIRECTORY = build

C_SOURCES       = main.c
C_SOURCES      += $(shell find src stdriot -type f -name '*.c')
S_SOURCES       = $(shell find src stdriot -type f -name '*.S')

C_OBJECTS       = $(addprefix $(BUILD_DIRECTORY)/, $(C_SOURCES:.c=.o))
S_OBJECTS       = $(addprefix $(BUILD_DIRECTORY)/, $(S_SOURCES:.S=.o))

all: $(BUILD_DIRECTORY)/$(TARGET).fae

$(BUILD_DIRECTORY):
	mkdir -p ${BUILD_DIRECTORY}

$(BUILD_DIRECTORY)/$(TARGET).fae: $(BUILD_DIRECTORY)/$(TARGET).elf
	./fae_utils/build_fae.py $<
	@chmod 644 $@

$(BUILD_DIRECTORY)/$(TARGET).elf: $(C_OBJECTS) $(S_OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

$(BUILD_DIRECTORY)/%.o: %.c | $(BUILD_DIRECTORY)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIRECTORY)/%.o: %.S | $(BUILD_DIRECTORY)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM)\
            $(C_OBJECTS)\
            $(S_OBJECTS)
	make -C crt0 clean

realclean: clean
	$(RM) -rf $(BUILD_DIRECTORY)

.PHONY: all clean realclean
