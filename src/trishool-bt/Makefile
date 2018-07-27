#
# Copyright(C) 2018, Juniper Networks, Inc.
# All rights reserved
#
# This SOFTWARE is licensed under the license provided in the LICENSE.txt
# file.  By downloading, installing, copying, or otherwise using the
# SOFTWARE, you agree to be bound by the terms of that license.  This
# SOFTWARE is not an official Juniper product.
#
# Third-Party Code: This SOFTWARE may depend on other components under
# separate copyright notice and license terms.  Your use of the source
# code for those components is subject to the term and conditions of
# the respective license as noted in the Third-Party source code.
#

ROOT_PATTERN	= /trishool/
DEPOT_ROOT		= $(word 1,$(subst $(ROOT_PATTERN), ,$(CURDIR)))
ROOT_PATH		= $(DEPOT_ROOT)$(ROOT_PATTERN)

include $(ROOT_PATH)/Makefile.globals

VPATH			= $(CURDIR)
LINK_LIBS		= sqlite3

ifeq ($(BT_PLATFORM), pin)
	PIN_INC  =
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/source/include/pin)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/source/include/pin/gen)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/stlport/include)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/libstdc++/include)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/crt/include)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/crt/)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/crt/include/arch-x86)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/crt/include/kernel/uapi)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/crt/include/kernel/uapi/asm-x86)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/components/include)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/xed-ia32/include/xed)
	PIN_INC	+= -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/source/tools/InstLib)
	PIN_INC	+= /FIinclude/msvc_compat.h

	PIN_BIN_DIRS	= $(shell cygpath -m $(VENDOR_BUILD_ROOT)/bin)
	PIN_LIBPATHS    =
	PIN_LIBPATHS    += -LIBPATH:$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/ia32/lib)
	PIN_LIBPATHS    += -LIBPATH:$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/ia32/lib-ext)
	PIN_LIBPATHS    += -LIBPATH:$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/ia32/runtime/pincrt)
	PIN_LIBPATHS    += -LIBPATH:$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/extras/xed-ia32/lib)
	PIN_LIBPATHS    += -LIBPATH:$(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/)
else ifeq ($(BT_PLATFORM), pin)
	LINK_LIBS		+=

	DRIO_INC =
	DRIO_INC += -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/drio/include)
	DRIO_INC += -I$(shell cygpath -m $(VENDOR_BUILD_ROOT)/drio/ext/include)

	DRIO_LIBPATHS	=
	DRIO_LIBPATHS	+= -LIBPATH:$(shell cygpath -m $(VENDOR_BUILD_ROOT)/drio/lib32)
	DRIO_LIBPATHS	+= -LIBPATH:$(shell cygpath -m $(VENDOR_BUILD_ROOT)/drio/lib32/release)
	DRIO_LIBPATHS	+= -LIBPATH:$(shell cygpath -m $(VENDOR_BUILD_ROOT)/drio/ext/lib32/release)
endif

ifeq ($(BT_PLATFORM), pin)
	WINDOWS_H_PATH ?= $(patsubst %\,%,$(strip $(shell cmd /c 'for %i in (Windows.h) do @echo.   %~dp$$INCLUDE:i')))
    WINDOWS_H_PATH := $(subst \,/,$(WINDOWS_H_PATH))
    export WINDOWS_H_PATH
endif

ifeq ($(BT_PLATFORM), pin)
	CL_FLAGS		+= /MT /EHs- /EHa- /wd4530 /DTARGET_WINDOWS \
						/nologo /Gy /Oi- /GR- /GS- /D__PIN__=1 \
						/DPIN_CRT=1 /D__i386__ /DTARGET_IA32 \
						/DHOST_IA32 /TP /w \
						/D_WINDOWS_H_PATH_="$(WINDOWS_H_PATH)"
else ifeq ($(BT_PLATFORM), drio)
	CL_FLAGS		+= /GS- /DWINDOWS /DX86_32
else
	CL_FLAGS		+= /GS- -MD -EHsc -nologo
endif

INCLUDES			+= -I $(TRISHOOL_INC_DIR)
ifeq ($(BT_PLATFORM), pin)
	INCLUDES		+= $(PIN_INC)
else ifeq ($(BT_PLATFORM), drio)
	INCLUDES		+= $(DRIO_INC)
endif

ifeq ($(BT_PLATFORM), pin)
OBJECTS_PREBUILT = $(shell cygpath -m $(VENDOR_BUILD_ROOT)/pin/ia32/runtime/pincrt/crtbeginS.obj)
endif

ifeq ($(BT_PLATFORM), pin)
LINK_LIBS       += pin xed pinvm \
                stlport-static m-static \
                c-static os-apis ntdll-32
else ifeq ($(BT_PLATFORM), drio)
LINK_LIBS		+= dynamorio drmgr drx
endif

LINK_ARGS		=

DEPENDENCIES_HEADERS		= ../include/util-log.h ../include/status.h

SOURCES			= instrumentor.c db.c \
					mmap.c image.c block.c \
					scan-engine.c \
					../libtrishool/util-log.c \
					../libtrishool/avl.c \
					../libtrishool/radix.c \
					../libtrishool/util-token.c

ifeq ($(BT_PLATFORM), pin)
	SOURCES		+= trishool-pin.c util-platform-pin.c util-string.c
else ifeq ($(BT_PLATFORM), drio)
	SOURCES		+= trishool-drio.c util-platform-drio.c
endif

INCLUDES		+= -I$(TRISHOOL_INC_DIR)

DEPENDENCIES_HEADERS =

EXECUTABLE		= $(BUILD_LIB_DIR)/trishool.dll

ifeq ($(BT_PLATFORM), pin)
	EXTRA_LD_FLAGS  += /DLL /EXPORT:main /NODEFAULTLIB /NOLOGO /INCREMENTAL:NO \
					/IGNORE:4210 /IGNORE:4049 /FORCE:MULTIPLE /DYNAMICBASE \
					/ENTRY:Ptrace_DllMainCRTStartup@12 /BASE:0x55000000 \
					/NXCOMPAT /MACHINE:x86
	EXTRA_LD_FLAGS  += $(PIN_LIBPATHS)
	CL_FLAGS  += /DBT_PLATFORM_PIN
else ifeq ($(BT_PLATFORM), drio)
	EXTRA_LD_FLAGS  += /DLL
	EXTRA_LD_FLAGS  += $(DRIO_LIBPATHS)
	CL_FLAGS  += /DWINDOWS /DBT_PLATFORM_DRIO
endif

default: setup $(EXECUTABLE)

clean::
	rm -rf $(BUILD_LIB_DIR)/trishool.*
	rm -rf $(BUILD_BIN_DIR)/trishool.*
	make rmtargets

$(DESTDIR)/usr/bin :
	mkdir -p $@

include $(ROOT_PATH)/Makefile.build
