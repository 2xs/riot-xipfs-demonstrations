# Copyright (C) 2024 Université de Lille
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
include ../vars.mk

TARGET = integer-echo

all: build/$(TARGET).fae

build :
	mkdir -p build

build/$(TARGET).fae: build/$(TARGET).elf
	$(FAE_BUILDER) --crt0-path $(FAE_CRT0_DIRECTORY_PATH) $<
	@chmod 644 $@

build/$(TARGET).elf: build/main.o
	$(LD) $(LDFLAGS) $^ -o $@

build/main.o: main.c | build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) build/main.o

realclean: clean
	$(RM) -rf build

.PHONY: all clean realclean
