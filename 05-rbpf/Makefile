# Copyright (C) 2024 Université de Lille
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
include ../vars.mk

CFLAGS         += -Isrc/RIOT/sys/include
CFLAGS         += -Isrc/RIOT/sys/include/rbpf
CFLAGS         += -Isrc/RIOT/core/lib/include

TARGET          = rbpf

BUILD_DIRECTORY = build

C_SOURCES       = main.c
C_SOURCES      += $(shell find src -type f -name '*.c')
S_SOURCES       = $(shell find src -type f -name '*.S')

C_OBJECTS       = $(addprefix $(BUILD_DIRECTORY)/, $(C_SOURCES:.c=.o))
S_OBJECTS       = $(addprefix $(BUILD_DIRECTORY)/, $(S_SOURCES:.S=.o))

all: $(BUILD_DIRECTORY)/$(TARGET).fae

$(BUILD_DIRECTORY):
	mkdir -p ${BUILD_DIRECTORY}

$(BUILD_DIRECTORY)/$(TARGET).fae: $(BUILD_DIRECTORY)/$(TARGET).elf
	$(FAE_BUILDER) --crt0-path $(FAE_CRT0_DIRECTORY_PATH) $<
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

realclean: clean
	$(RM) -rf $(BUILD_DIRECTORY)

.PHONY: all clean realclean
