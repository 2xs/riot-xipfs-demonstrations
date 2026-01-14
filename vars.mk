# Copyright (C) 2025 Université de Lille
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

DIRECTORIES_MK_PATH   = $(abspath ../directories.mk)

ifeq ("$(wildcard $(DIRECTORIES_MK_PATH))","")
    $(error "Please create $(DIRECTORIES_MK_PATH) and populate the file with FAE_DIRECTORY_PATH")
endif

include ../directories.mk

ifeq ("$(FAE_DIRECTORY_PATH)","")
    $(error "FAE_DIRECTORY_PATH has not been defined in $(DIRECTORIES_MK_PATH)")
endif

FAE_ABSOLUTE_DIRECTORY_PATH=$(abspath $(FAE_DIRECTORY_PATH))
ifeq ("$(wildcard $(FAE_ABSOLUTE_DIRECTORY_PATH))","")
    $(error "$(FAE_ABSOLUTE_DIRECTORY_PATH) does not exist." )
endif

FAE_UTILS_DIRECTORY_PATH=$(FAE_ABSOLUTE_DIRECTORY_PATH)/fae_utils
FAE_BUILDER=$(FAE_UTILS_DIRECTORY_PATH)/build_fae.py
ifeq ("$(wildcard $(FAE_BUILDER))","")
    $(error "Missing build_fae.py in $(FAE_UTILS_DIRECTORY_PATH) path. Please check this path.")
endif

FAE_INCLUDE_DIRECTORY_PATH=$(FAE_ABSOLUTE_DIRECTORY_PATH)/src/include

FAE_LIBRARY_DIRECTORY_PATH=$(FAE_ABSOLUTE_DIRECTORY_PATH)/build
FAE_LIBRARY_PATH=$(FAE_LIBRARY_DIRECTORY_PATH)/libfae.a
ifeq ("$(wildcard $(FAE_LIBRARY_PATH))","")
    $(error "Missing libfae.a. Please make $(FAE_LIBRARY_PATH) first.")
endif

FAE_CRT0_DIRECTORY_PATH=$(FAE_ABSOLUTE_DIRECTORY_PATH)/build
FAE_CRT0_FAE_PATH=$(FAE_CRT0_DIRECTORY_PATH)/crt0.fae
FAE_CRT0_ELF_PATH=$(FAE_CRT0_DIRECTORY_PATH)/crt0.elf
ifeq ("$(wildcard $(FAE_CRT0_FAE_PATH))","")
    $(error "Missing crt0.fae. Please make $(FAE_CRT0_FAE_PATH) first.")
endif
ifeq ("$(wildcard $(FAE_CRT0_ELF_PATH))","")
    $(error "Missing crt0.elf. Please make $(FAE_CRT0_ELF_PATH) first.")
endif

# To build the binary in debug mode, set the DEBUG variable
#DEBUG           = 1

PREFIX          = arm-none-eabi-
CC              = $(PREFIX)gcc
LD              = $(PREFIX)gcc

CFLAGS          = -Wall
CFLAGS         += -Wextra
CFLAGS         += -Werror
CFLAGS         += -mthumb
CFLAGS         += -I$(FAE_INCLUDE_DIRECTORY_PATH)
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

LDFLAGS         = -nostartfiles
LDFLAGS        += -nodefaultlibs
LDFLAGS        += -nolibc
LDFLAGS        += -nostdlib
LDFLAGS        += -Tlink.ld
LDFLAGS        += -Wl,-q
LDFLAGS        += -L$(FAE_LIBRARY_DIRECTORY_PATH) -lfae
# Disable the new linker warning '--warn-rwx-segments' introduced by
# Binutils 2.39, which causes the following message: "warning:
# $(TARGET).elf has a LOAD segment with RWX permissions".
ifeq ($(shell $(PREFIX)ld --help | grep -q 'warn-rwx-segments'; echo $$?), 0)
LDFLAGS        += -Wl,--no-warn-rwx-segments
endif
