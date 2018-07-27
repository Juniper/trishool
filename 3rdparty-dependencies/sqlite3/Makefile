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

ROOT_PATTERN		= /trishool/
DEPOT_ROOT		= $(word 1,$(subst $(ROOT_PATTERN), ,$(CURDIR)))
ROOT_PATH		= $(DEPOT_ROOT)$(ROOT_PATTERN)

include $(ROOT_PATH)/Makefile.globals

SQLITE_VERSION		= 3.23.1
SQLITE_VERSION_ALT	= 3230100
TAR_ZARG			= -z
DIST_PKG_SUFFIX     = .zip

SQLITE_BUILD_ROOT	= $(VENDOR_BUILD_ROOT)/sqlite
SQLITE_SRC_DIR		= $(SQLITE_BUILD_ROOT)/src
PATCH_DIR			= $(CURDIR)/patches

SQLITE_TAR_FILE		= $(CURDIR)/sqlite-amalgamation-$(SQLITE_VERSION_ALT)$(DIST_PKG_SUFFIX)

LIB_TARGETS			= $(VENDOR_LIB_DIR)/libsqlite3.dll
INCLUDE_TARGETS		= $(VENDOR_INC_DIR)/sqlite3.h

TRISHOOL_DO_DEBUG	?=

#ifeq ($(TRISHOOL_DO_DEBUG),)
	WIN_ARCH		= Win32 Release
	NUM_REPEAT		= 1
#else
#	WIN_ARCH		= Win32 Debug
#	NUM_REPEAT		= 1
#endif


default : setup $(LIB_TARGETS) $(INCLUDE_TARGETS)

setup : $(VENDOR_LIB_DIR) $(VENDOR_INC_DIR) $(VENDOR_BIN_DIR)

$(LIB_TARGETS) : $(SQLITE_SRC_DIR)/Makefile $(SQLITE_BUILD_ROOT)/Makefile.globals $(SQLITE_BUILD_ROOT)/Makefile.build
	$(MAKE) -C $(SQLITE_SRC_DIR) TRISHOOL_BUILD_ROOT=$(TRISHOOL_BUILD_ROOT) default

$(INCLUDE_TARGETS) : $(SQLITE_SRC_DIR)/sqlite3.h
	cp $? $@

$(SQLITE_BUILD_ROOT)/Makefile.globals : $(ROOT_PATH)/Makefile.globals
	cp $(ROOT_PATH)/Makefile.globals $@

$(SQLITE_BUILD_ROOT)/Makefile.build : $(ROOT_PATH)/Makefile.build
	cp $(ROOT_PATH)/Makefile.build $@



$(SQLITE_SRC_DIR)/Makefile : $(SQLITE_SRC_DIR) Makefile.sqlite
	cp -f Makefile.sqlite $(SQLITE_SRC_DIR)/Makefile

$(SQLITE_SRC_DIR) : $(SQLITE_BUILD_ROOT) $(SQLITE_TAR_FILE)
	unzip	-d $(SQLITE_BUILD_ROOT) -q $(SQLITE_TAR_FILE)
	mv $(SQLITE_BUILD_ROOT)/sqlite-amalgamation-$(SQLITE_VERSION_ALT) $(SQLITE_SRC_DIR)

#$(SQLITE_SRC_DIR) : $(SQLITE_BUILD_ROOT) $(SQLITE_TAR_FILE)
#ifeq ($(PLATFORM_BUILD_NAME),win)
#	unzip	-d $(SQLITE_BUILD_ROOT) -q $(SQLITE_TAR_FILE)
#else
#	tar		-C $(SQLITE_BUILD_ROOT) $(TAR_ZARG) -xf $(SQLITE_TAR_FILE)
#endif
#	mv $(SQLITE_BUILD_ROOT)/sqlite-amalgamation-$(SQLITE_VERSION_ALT) $(SQLITE_SRC_DIR)
#	#cd $(SQLITE_SRC_DIR) && patch -g0 -p1 < $(SQLITE_PATCH)

$(SQLITE_BUILD_ROOT) :
	mkdir -p $@

$(VENDOR_LIB_DIR) :
	mkdir -p $@

$(VENDOR_INC_DIR) :
	mkdir -p $@

$(VENDOR_BIN_DIR) :
	mkdir -p $@

clean :
	if [ -d $(SQLITE_SRC_DIR) ] ; then $(MAKE) -C $(SQLITE_SRC_DIR) TRISHOOL_BUILD_ROOT=$(TRISHOOL_BUILD_ROOT) clean ; fi
	rm -rf $(SQLITE_BUILD_ROOT)
	rm -rf $(VENDOR_OBJ_DIR)/sqlite
	rm -rf $(VENDOR_INC_DIR)/*sqlite*
	rm -rf $(VENDOR_LIB_DIR)/*sqlite*
