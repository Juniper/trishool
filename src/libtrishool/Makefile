ROOT_PATTERN			= /trishool/
DEPOT_ROOT              = $(word 1,$(subst $(ROOT_PATTERN), ,$(CURDIR)))
ROOT_PATH               = $(DEPOT_ROOT)$(ROOT_PATTERN)

include $(ROOT_PATH)/Makefile.globals

LINK_LIBS               =
LINK_ARGS               =

#
# 3-9-2012: currently Makefiles only support one executable per
# to build has-debug, use: make MYTARGET=has-debug
#
LIBRARY					= $(BUILD_LIB_DIR)/libtrishool
INCLUDES				+= -I $(TRISHOOL_INC_DIR) -I $(TRISHOOL_INC_DIR)/trishool
SOURCES					= util-log.c avl.c radix.c

ifeq ($(BT_PLATFORM), pin)
	CL_FLAGS  += /GS- /DBT_PLATFORM_PIN
else ifeq ($(BT_PLATFORM), drio)
	CL_FLAGS  += /GS- /DBT_PLATFORM_DRIO
endif

default: setup $(LIBRARY).$(LIBEXT)

include $(ROOT_PATH)/Makefile.build
